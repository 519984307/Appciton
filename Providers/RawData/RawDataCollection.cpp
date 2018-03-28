#include "Debug.h"
#include "RawDataCollection.h"
#include <QMutexLocker>

#include "TimeManager.h"
#include "TimeDate.h"
#include "USBManager.h"
#include <QVector>
#include <QDateTime>

#if defined(Q_WS_QWS)

RawDataCollection *RawDataCollection::_selfObj = NULL;
/**************************************************************************************************
 * 向缓冲区中写入数据。
 *************************************************************************************************/
void RawDataCollection::pushData(QString name, const unsigned char *data, int len)
{
    //检查U盘
    if (!usbManager.isUSBExist())
    {
        return;
    }

    if (!_mutex.tryLock())
    {
        ProviderBuffBk::iterator Bk = _providerBuffBk.find(name);
        // 已经存在。
        if (Bk == _providerBuffBk.end())
        {
            _providerBuffBk.insert(name, QByteArray() );
            Bk = _providerBuffBk.find(name);
        }

        QByteArray &providerBuffBk = Bk.value();

        for (int i = 0; i < len; i++)
        {
            providerBuffBk.append(data[i]);
        }
//        providerBuffBk.append(datanum);
//        datanum++;
    }
    else
    {
        ProviderBuff::iterator it = _providerBuff.find(name);
        // 已经存在。
        if (it == _providerBuff.end())
        {
            _providerBuff.insert(name, QByteArray());
            _providerFile.insert(name, new QFile() );
            it = _providerBuff.find(name);
        }

        QByteArray &providerBuff = it.value();

        //判断备份缓冲区是否存在，不存在则跳过，存在则判断是否为空
        ProviderBuffBk::iterator Bk = _providerBuffBk.find(name);
        if (Bk != _providerBuffBk.end())
        {
            QByteArray &providerBuffBk = Bk.value();
            if (!providerBuffBk.isEmpty())
            {
                for (int j = 0; j < providerBuffBk.size(); ++j)
                {
                    providerBuff.append(providerBuffBk[j]);
                }
                providerBuffBk.clear();
            }
        }

        for (int i = 0; i < len; i++)
        {
            providerBuff.append(data[i]);
        }
//        providerBuff.append(datanum);
//        datanum++;

        _mutex.unlock();
    }
}

/**************************************************************************************************
 * 关闭txt文件。
 *************************************************************************************************/
void RawDataCollection::_closeFile(QString name)
{
    QFile *file = _providerFile.value(name);
    if (file == NULL)
    {
        return;
    }
    if (!file->isOpen())
    {
        return;
    }
    file->close();
    delete file;
    _providerFile.insert(name,NULL);
    debug("%s File Close!",qPrintable(name));
//    debug("%s File Close! %d",qPrintable(name),datanum);
    //    datanum = 0;
}

/**************************************************************************************************
 * 文件夹的创建。
 *************************************************************************************************/
bool RawDataCollection::_folderCreate(QString name)
{
    QString _folderPath;
    _folderPath = usbManager.getUdiskMountPoint();
    _folderPath += "/";
    _folderPath += name;

    QDir _folder(_folderPath);

    if(_folder.exists())
    {
        return _openTxt(name, _folderPath);
    }
    else
    {
        bool folderOK = _folder.mkdir(_folderPath);
        if (folderOK)
        {
            debug("%s Folder mkdir is OK!",qPrintable(name));
            return _openTxt(name, _folderPath);
        }
        else
        {
            debug("%s Folder mkdir is Failed!",qPrintable(name));
            return false;
        }
    }
}

/**************************************************************************************************
 * 打开U盘文件。
 *************************************************************************************************/
bool RawDataCollection::_openTxt(QString name, QString folderPath)
{
    unsigned time = timeManager.getCurTime();

    //文件名，以时间命名
    QString _fileName;
    QString _filePath;

    QDateTime dt = QDateTime::fromTime_t(time);
    _fileName = dt.toString("yyyy.MM.dd-HH.mm.ss");

//    _fileName.sprintf("%02d.%02d.%02d", timeDate.getTimeHour(time), timeDate.getTimeMinute(time),
//                 timeDate.getTimeSenonds(time));

    _filePath = folderPath;
    _filePath += "/";
    _filePath += name + "_" + _fileName + ".bin";

    QFile *file = new QFile(_filePath);
    _providerFile.insert(name, file);


    if (file->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        return true;
    }
    else if (!file->open(QIODevice::WriteOnly))
    {
        //如果文件不存在就创建
        file->close();
        if (file->open(QIODevice::ReadWrite | QIODevice::Text))
        {
            return true;
        }
        else
        {
            debug("%s open wrong!", qPrintable(_filePath));
            return false;
        }
    }
    else
        return false;
}

/**************************************************************************************************
 * 保存数据到U盘。
 *************************************************************************************************/
void RawDataCollection::_saveData(void)
{
    _mutex.lock();
    if (_providerBuff.isEmpty())
    {
        _mutex.unlock();
        return;
    }

    ProviderBuff::iterator iters;
    for(iters = _providerBuff.begin(); iters != _providerBuff.end(); iters++)
    {
        QByteArray &providerBuff = iters.value();
        _exportUdisk(iters.key(), providerBuff);
        providerBuff.clear();
    }
    _mutex.unlock();
}

/**************************************************************************************************
 * 将数据写入TXT文件。
 *************************************************************************************************/
void RawDataCollection::_exportUdisk(QString name, const QByteArray &data)
{
    //检查U盘
    if (!usbManager.isUSBExist())
    {
        return;
    }

    ProviderFile::iterator it = _providerFile.find(name);
    if (it == _providerFile.end())
    {
        return;
    }

    QFile *file = it.value();

    //NIBP每次测量打一个包
    if (name == "BLM_TN3")
    {
        if (file == NULL)
        {
            if (!data.isEmpty())
            {
                _folderCreate(name);
                it = _providerFile.find(name);
                if (it == _providerFile.end())
                {
                    return;
                }
                file = it.value();
                if (file == NULL)
                {
                    return;
                }
            }
            else
            {
                return;
            }
        }

        if (data.isEmpty())
        {
            if (file->isOpen())
            {
                _closeFile(name);
            }
            return;
        }

        if (file->exists() && file->isOpen())
        {
            file->write(data.constData(), data.length());
            file->flush();
        }
        else
        {
            _folderCreate(name);
        }
    }
    //其他模块10K或100K打一个包
    else
    {
        if (file == NULL)
        {
            return;
        }

        if (file->exists() && file->isOpen())
        {
            file->write(data.constData(), data.length());
            file->flush();

            int size = 10240;
            if (name == "BLM_TE3" || name == "BLM_TS3")
            {
                size = 102400;
            }
            if (file->size()>= size)
            {
                _closeFile(name);
                _folderCreate(name);
            }
        }
        else
        {
            _folderCreate(name);
        }
    }
}

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
RawDataCollection::RawDataCollection()
{
    datanum = 0;
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
RawDataCollection::~RawDataCollection()
{
    _saveData();
    ProviderBuff::iterator iters;
    for(iters = _providerBuff.begin(); iters != _providerBuff.end(); iters++)
    {
        _closeFile(iters.key());
    }
    _providerBuff.clear();
    _providerFile.clear();

    if (!_providerBuffBk.isEmpty())
    {
        _providerBuffBk.clear();
    }
}

void RawDataCollection::run()
{
    _saveData();
}

#elif defined(Q_WS_X11)
RawDataCollection *RawDataCollection::_selfObj = NULL;
void RawDataCollection::pushData(QString /*name*/, const unsigned char */*data*/, int /*len*/) {}

void RawDataCollection::run() { }
RawDataCollection::RawDataCollection() { }
RawDataCollection::~RawDataCollection() { }

#endif
