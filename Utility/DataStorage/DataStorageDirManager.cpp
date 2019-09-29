/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/21
 **/


#include <QDir>
#include "DataStorageDirManager.h"
#include "IConfig.h"
#include "TimeManagerInterface.h"
#include <QMutex>
#include "WaveformCache.h"
#include "StorageManager.h"
#include <QDateTime>
#include "SystemManagerInterface.h"
#include "AlarmSourceManager.h"
#include "SystemDefine.h"
#include "AlarmInterface.h"

static QString _lastFolder;
static QString _lastFDFileName;

/***************************************************************************************************
 * dirSize : calculate the size of specific directory
 **************************************************************************************************/
quint64 DataStorageDirManager::dirSize(const QString &dir)
{
    quint64 sizex = 0;
    QFileInfo dirInfo(dir);
    if (dirInfo.isDir())
    {
        QDir d(dir);
        QFileInfoList list = d.entryInfoList(QDir::Files | QDir::Dirs | QDir::Hidden
                                             | QDir::NoSymLinks | QDir::NoDotAndDotDot);
        for (int i = 0; i < list.size(); i++)
        {
            QFileInfo  info = list.at(i);
            if (info.isDir())
            {
                sizex += dirSize(info.absoluteFilePath());
            }
            else
            {
                sizex += info.size();
            }
        }
    }
    else
    {
        sizex = dirInfo.size();
    }

    return sizex;
}

void DataStorageDirManager::cleanCurData()
{
    // 删除当前文件夹中的数据，重新生成新的数据文件
    if (_folderNameList.size() <= 1)
    {
        return;
    }

    QString path = _curFolder;
    quint32 totalSize = 0;

    quint32 size = _cleanupDir(path);
    if (_previousDataSize > size)
    {
        _previousDataSize -= size;
    }
    else
    {
        _previousDataSize = 0;
    }


    totalSize = _previousDataSize + _curDataSize;

    systemConfig.setNumValue("DataStorage|DataSize", totalSize);
    emit newPatient();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
DataStorageDirManager::DataStorageDirManager()
{
    int folderSequenceNum = 0;
    systemConfig.getNumValue("DataStorage|FolderSequenceNum", folderSequenceNum);

    systemConfig.getStrValue("DataStorage|FolderName", _curFolder);

    systemConfig.getStrValue("DataStorage|FDFileName", _fdFileName);

    if (_curFolder.contains("nPMD"))
    {
        // delete demo data
        _deleteDir(_curFolder);
    }

    if (_curFolder.isEmpty() && _fdFileName.isEmpty() && folderSequenceNum == 1)
    {
        // check whether exist rescue data,
        // if exist, update the folder sequence number to a proper value
        QDir resuceDataStorageDir(dataStorageDir());
        resuceDataStorageDir.setNameFilters(QStringList() << "*nPM??????????????");
        resuceDataStorageDir.setSorting(QDir::Name);
        QStringList entrylist = resuceDataStorageDir.entryList();
        QList<int> seqlist;
        foreach(QString folder, entrylist)
        {
            // get the sequence number string
            folder.chop(17);
            seqlist.append(folder.toInt());
        }
        qSort(seqlist);
        if (!seqlist.isEmpty() && seqlist.last() > 0)
        {
            folderSequenceNum = seqlist.last() + 1;
        }
    }

    _lastFolder = _curFolder;
    _lastFDFileName = _fdFileName;
    _previousDataSize = 0;

    QDir dir(_curFolder);
    QString timeStr;
    bool newRescue  = false;
    TimeManagerInterface *timeManager = TimeManagerInterface::getTimeManager();
    bool powerOnSta = false;
    if (timeManager)
    {
        powerOnSta = timeManager->getPowerOnSession() == POWER_ON_SESSION_NEW ? true : false;
    }
    if (_curFolder.isEmpty() || !dir.exists() || powerOnSta)
    {
        _curFolder.clear();
        unsigned t = timeManager->getStartTime();
        QDateTime dt = QDateTime::fromTime_t(t);
        timeStr = dt.toString("yyyyMMddHHmmss");
        _curFolder += dataStorageDir();
        _curFolder += QString::number(folderSequenceNum) + "nPM" + timeStr;
        dir.setPath(_curFolder);
        if (dir.mkpath(_curFolder))
        {
            ++folderSequenceNum;
            systemConfig.setNumValue("DataStorage|FolderSequenceNum", folderSequenceNum);
            systemConfig.setStrValue("DataStorage|FolderName", _curFolder);
            systemConfig.setNumValue("DataStorage|SignalRescueSize", 0);
        }
        else
        {
            debug("create %s folder failure\n", qPrintable(_curFolder));
            _curFolder.clear();
        }

        newRescue = true;
    }

    _createFDFileName(_fdFileName, timeManager->getStartTime(), newRescue);

    QString curFolderName = QDir(_curFolder).dirName();
    dir.setPath(dataStorageDir());
    if (dir.exists())
    {
        struct FolderName folderName;
        foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            if (fileInfo.isDir())
            {
                folderName.name = fileInfo.fileName();
                folderName.getNum();

                // time string is same as the old folder's time string
                // remove the older one
                // 移除同名文件夹和移除演示数据文件夹
                if ((!timeStr.isEmpty() && folderName.getDatetimeStr() == timeStr && folderName.name != curFolderName)
                        || folderName.name.contains("nPMD"))
                {
                    _deleteDir(dataStorageDir() + folderName.name);
                    continue;
                }

                if (0 != folderName.sequenceNum)
                {
                    if (folderName.name != curFolderName)
                    {
                        _folderNameList.append(folderName);
                        _previousDataSize += dirSize(fileInfo.absoluteFilePath());
                    }
                }
            }
        }

        qSort(_folderNameList);
        // make sure the last item in the _folderNameList is current folder
        folderName.name = curFolderName;
        folderName.getNum();
        _folderNameList.append(folderName);
    }
    else
    {
        dir.mkpath(dataStorageDir());
    }

    _curDataSize  = dirSize(_curFolder);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
DataStorageDirManager &DataStorageDirManager::getInstance()
{
    static DataStorageDirManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new DataStorageDirManager;

        // register the new interface and delete the old one
        DataStorageDirManagerInterface *old = registerDataStorageDirManager(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

DataStorageDirManager::~DataStorageDirManager()
{
    _folderNameList.clear();
}

/**************************************************************************************************
 * 删除较早的数据。
 *************************************************************************************************/
void DataStorageDirManager::_deleteOldData(void)
{
    if (_folderNameList.size() <= 1)
    {
        return;
    }

    QString path = dataStorageDir() + _folderNameList.takeFirst().name;
    quint32 totalSize = 0;

    quint32 size = _deleteDir(path);
    if (_previousDataSize > size)
    {
        _previousDataSize -= size;
    }
    else
    {
        _previousDataSize = 0;
    }


    totalSize = _previousDataSize + _curDataSize;

    systemConfig.setNumValue("DataStorage|DataSize", totalSize);
}

/**************************************************************************************************
 * 创建FD文件名。
 *************************************************************************************************/
void DataStorageDirManager::_createFDFileName(QString &fileName, unsigned time, bool newRescue)
{
    int seqNum = 1;
    QString tmpStr;
    if (newRescue || fileName.isEmpty())
    {
        fileName.clear();
        fileName += "/ZE";

        machineConfig.getStrValue("SerialNumber", tmpStr);
        fileName += tmpStr;
        fileName += "_";

        QDateTime dt = QDateTime::fromTime_t(time);
        fileName += dt.toString("yyyyMMdd_hhmmss");

        tmpStr = "A";
    }
    else
    {
        systemConfig.getNumValue("DataStorage|FDFileSequenceNum", seqNum);
        if (seqNum > 26)
        {
            return;
        }
        else
        {
            fileName = fileName.left(fileName.size() - 6);  // remove "?.json"
            char a = seqNum + 64;
            tmpStr = QString("%1").arg(a);  // begin with 'A'
        }
    }

    ++seqNum;
    fileName += tmpStr;
    fileName += ".json";
    systemConfig.setNumValue("DataStorage|FDFileSequenceNum", seqNum);
    systemConfig.setStrValue("DataStorage|FDFileName", fileName);
}

/**************************************************************************************************
 * 获取营救事件时间列表。
 *************************************************************************************************/
void DataStorageDirManager::getRescueEvent(QStringList &timeList)
{
    if (_folderNameList.isEmpty())
    {
        return;
    }

    timeList.clear();

    QString tmpstr;
    int count = _folderNameList.count();
    for (int i = count - 1; i >= 0; --i)
    {
        tmpstr = _folderNameList.at(i).name.right(14);
        timeList << tmpstr;
        tmpstr.clear();
    }
}

/**************************************************************************************************
 * 获取营救数据目录。index = 0 means get the current folder.
 *************************************************************************************************/
QString DataStorageDirManager::getRescueDataDir(int index, const QString &time)
{
    if (_folderNameList.empty())
    {
        return QString("");
    }

    int count = _folderNameList.count() - 1;
    index = count - index;
    if (index < 0 || index > count)
    {
        return QString("");
    }

    if (_folderNameList.at(index).name.right(14) == time)
    {
        return _folderNameList.at(index).name;
    }

    return QString("");
}

/**************************************************************************************************
 * 获取营救数据目录。
 *************************************************************************************************/
QString DataStorageDirManager::getRescueDataDir(int index)
{
    if (_folderNameList.empty())
    {
        return QString("");
    }

    int count = _folderNameList.count() - 1;
    index = count - 1 - index;
    if (index < 0 || index > count)
    {
        return QString("");
    }

    return _folderNameList.at(index).name;
}

/**************************************************************************************************
 * 创建文件夹。
 *************************************************************************************************/
void DataStorageDirManager::createDir(bool createNew)
{
    _createNew = createNew;
    int folderSequenceNum = 0;
    systemConfig.getNumValue("DataStorage|FolderSequenceNum", folderSequenceNum);

    systemConfig.getStrValue("DataStorage|FolderName", _curFolder);

    _lastFolder = _curFolder;
    _previousDataSize = 0;

    QDir dir(_curFolder);
    QString timeStr;
    TimeManagerInterface *timeManager = TimeManagerInterface::getTimeManager();
    if (_curFolder.isEmpty() || !dir.exists() || _createNew)
    {
        _folderNameList.clear();
        _curFolder.clear();
        QDateTime dt = QDateTime::fromTime_t(timeManager->getCurTime());
        timeStr = dt.toString("yyyyMMddHHmmss");
        _curFolder += dataStorageDir();
        QString demoFlag = "";
        SystemManagerInterface *systemManager = SystemManagerInterface::getSystemManager();
        if (systemManager)
        {
            if (systemManager->getCurWorkMode() == WORK_MODE_DEMO)
            {
                demoFlag = "D";
            }
        }
        _curFolder += QString::number(folderSequenceNum) + "nPM" + demoFlag + timeStr;
        dir.setPath(_curFolder);
        if (dir.mkpath(_curFolder))
        {
            ++folderSequenceNum;
            systemConfig.setNumValue("DataStorage|FolderSequenceNum", folderSequenceNum);
            systemConfig.setStrValue("DataStorage|FolderName", _curFolder);
            systemConfig.setNumValue("DataStorage|SignalRescueSize", 0);
        }
        else
        {
            debug("create %s folder failure\n", qPrintable(_curFolder));
            _curFolder.clear();
        }
    }

    QString curFolderName = QDir(_curFolder).dirName();
    dir.setPath(dataStorageDir());
    // 删除日期同名的文件
    if (dir.exists())
    {
        struct FolderName folderName;
        foreach(QFileInfo fileInfo, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot))
        {
            if (fileInfo.isDir())
            {
                folderName.name = fileInfo.fileName();
                folderName.getNum();

                // time string is same as the old folder's time string
                // remove the older one
                if (!timeStr.isEmpty() && folderName.getDatetimeStr() == timeStr && folderName.name != curFolderName)
                {
                    _deleteDir(dataStorageDir() + folderName.name);
                    continue;
                }

                if (0 != folderName.sequenceNum)
                {
                    if (folderName.name != curFolderName)
                    {
                        _folderNameList.append(folderName);
                        _previousDataSize += dirSize(fileInfo.absoluteFilePath());
                    }
                }
            }
        }

        qSort(_folderNameList);
        // make sure the last item in the _folderNameList is current folder
        folderName.name = curFolderName;
        folderName.getNum();
        _folderNameList.append(folderName);
    }
    else
    {
        dir.mkpath(dataStorageDir());
    }

    _curDataSize  = dirSize(_curFolder);

    if (_storageList.count() > 0)
    {
        for (int i = 0; i < _storageList.count(); i++)
        {
            StorageManager *item = NULL;
            item = _storageList.at(i);
            item->createDir();
        }
    }

    if (_createNew)
    {
        // 新病人，删除上次删掉备份文件，设置时间，恢复主配置文件
        cleanUpLastIncidentDir();
        timeManager->setElapsedTime();
        Config systemDefCfg(systemConfig.getCurConfigName());
        systemConfig.setNodeValue("PrimaryCfg", systemDefCfg);
        emit newPatient();
        AlarmInterface *alertor = AlarmInterface::getAlarm();
        if (alertor)
        {
            alertor->removeAllLimitAlarm();
        }
    }
}

void DataStorageDirManager::addStorage(StorageManager *storage)
{
    _storageList.append(storage);
}

/**************************************************************************************************
 * 获取当前文件夹。
 *************************************************************************************************/
QString DataStorageDirManager::getCurFolder() const
{
    return _curFolder;
}

/**************************************************************************************************
 * 获取文件夹的个数。
 *************************************************************************************************/
int DataStorageDirManager::getDirNum() const
{
    return _folderNameList.count();
}

/**************************************************************************************************
 * isCurRescueFolderFull : check whether current rescure storage folder is full
 *************************************************************************************************/
bool DataStorageDirManager::isCurRescueFolderFull()
{
    AlarmOneShotIFace *systemAlarm = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SYSTEM);
    if (systemAlarm == NULL)
    {
        return false;
    }

    if (_curDataSize > (unsigned) SIGNAL_RESCUE_MAX_DATA_SIZE)
    {
        systemAlarm->setOneShotAlarm(STORAGE_SPACE_FULL, true);
        return true;
    }
    systemAlarm->setOneShotAlarm(STORAGE_SPACE_FULL, false);
    return false;
}

/**************************************************************************************************
 * 判断加上size大小的数据后，存储空间是否存满了
 *************************************************************************************************/
void DataStorageDirManager::addDataSize(unsigned size)
{
    if (!size)
    {
        return;
    }

    _curDataSize += size;
    systemConfig.setNumValue("DataStorage|SignalRescueSize", _curDataSize);
    quint32 totalSize = _previousDataSize + _curDataSize;
    systemConfig.setNumValue("DataStorage|DataSize", totalSize);

    if (totalSize > (unsigned)MAX_DATA_SIZE)
    {
        _deleteOldData();
    }
}

/**************************************************************************************************
 * 删除所有数据
 *************************************************************************************************/
void DataStorageDirManager::deleteAllData()
{
    if (_folderNameList.count() <= 1)
    {
        return;
    }

    while (_folderNameList.count() > 1)
    {
        _deleteDir(dataStorageDir()+ _folderNameList.takeAt(0).name);
    }

    _previousDataSize = 0;
    quint32 totalsize = _curDataSize;
    systemConfig.setNumValue("DataStorage|DataSize", totalsize);
}

/**************************************************************************************************
 * 删除数据
 *************************************************************************************************/
void DataStorageDirManager::deleteData(QString &path)
{
    if (path.isEmpty())
    {
        return;
    }

    int count = _folderNameList.count();
    if (count <= 1)
    {
        return;
    }

    int i = 0;
    for (; i < count; ++i)
    {
        if (_folderNameList.at(i).name == path)
        {
            break;
        }
    }

    if (i >= count - 1)  // not found or the last one
    {
        return;
    }

    _folderNameList.removeAt(i);

    quint32 size = _deleteDir(dataStorageDir() + path);
    if (_previousDataSize > size)
    {
        _previousDataSize -= size;
    }
    else
    {
        _previousDataSize = 0;
    }

    quint32 totalSize = _previousDataSize + _curDataSize;
    systemConfig.setNumValue("DataStorage|DataSize", totalSize);
}

/**************************************************************************************************
 * 删除数据
 *************************************************************************************************/
void DataStorageDirManager::deleteData(int index)
{
    int count = _folderNameList.count();
    if (count <= 1)
    {
        return;
    }

    if (index >= count - 1)  // not found or the last one
    {
        return;
    }

    quint32 size = _deleteDir(dataStorageDir() + _folderNameList.takeAt(index).name);
    if (_previousDataSize > size)
    {
        _previousDataSize -= size;
    }
    else
    {
        _previousDataSize = 0;
    }

    quint32 totalSize = _previousDataSize + _curDataSize;
    systemConfig.setNumValue("DataStorage|DataSize", totalSize);
}

/***************************************************************************************************
 * cleanUpLastIncidentDir: 整理上个存储目录中的文件，主要是删掉备份文件
 **************************************************************************************************/
bool DataStorageDirManager::cleanUpLastIncidentDir()
{
    if (_lastFolder.isEmpty() || !QDir(_lastFolder).exists())
    {
        return false;
    }

    bool ret = cleanUpIncidentDir(_lastFolder);

    _lastFolder.clear();

    return ret;
}

/***************************************************************************************************
 * cleanUpIncidentDir : clean up incident directory
 **************************************************************************************************/
bool DataStorageDirManager::cleanUpIncidentDir(const QString &dir)
{
    QStringList cleanupFilesList;
    TimeManagerInterface *timeManager = TimeManagerInterface::getTimeManager();
    if (timeManager->getPowerOnSession() == POWER_ON_SESSION_CONTINUE)
    {
        // If reboot and continue a session, the host will create a new full disclosure file,
        // need to clean up the old one
        if (_lastFDFileName.right(1) == "Z")
        {
            // index will no increase any more, the file will be used again, don't cleanup
            return false;
        }
        cleanupFilesList.append(_lastFDFileName);
    }
    else
    {
        cleanupFilesList.append(PARAM_DATA_FILE_NAME);
        cleanupFilesList.append(ECG12L_FILE_NAME);
        cleanupFilesList.append(SUMMARY_DATA_FILE_NAME);
        cleanupFilesList.append(_lastFDFileName);
    }

    IStorageBackend *backend = StorageManager::open(QString(), QIODevice::ReadOnly);
    for (int i = 0; i < cleanupFilesList.count(); i++)
    {
        QString filename = dir + cleanupFilesList[i];
        if (StorageManager::exist(filename))
        {
            backend->reload(filename, QIODevice::ReadWrite);
        }
    }

    delete backend;
    return true;
}

/**************************************************************************************************
 * 删除指定目录,返回删除目录的大小。
 *************************************************************************************************/
int DataStorageDirManager::_deleteDir(const QString &path)
{
    int size = 0;
    QDir dir(path);
    if (!dir.exists())
    {
        return size;
    }

    foreach(QFileInfo fileinfo, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden | QDir::NoSymLinks |
             QDir::Files))
    {
        if (fileinfo.isDir())
        {
            size += _deleteDir(fileinfo.filePath());
        }
        else
        {
            size += fileinfo.size();
            QFile::remove(fileinfo.filePath());
        }
    }

    dir.rmdir(path);

    return size;
}

int DataStorageDirManager::_cleanupDir(const QString &path)
{
    int size = 0;
    QDir cleanDir(path);
    foreach(QFileInfo file, cleanDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Dirs | QDir::Hidden | QDir::NoSymLinks
                                                    | QDir::Files))
    {
        if (file.isDir())
        {
            size += _deleteDir(file.filePath());
        }
        else
        {
            size += file.size();
            QFile::remove(file.filePath());
        }
    }
    return size;
}
