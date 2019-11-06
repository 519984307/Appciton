/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/7/18
 **/


#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "serializer.h"
#include "parser.h"
#include "Framework/Storage/StorageFile.h"
#include <QMutex>
#include <QFile>
#include <QDir>
#include "Debug.h"
#include <QMutexLocker>
#include <unistd.h>
#include <QApplication>
#include "IConfig.h"
#include <QDateTime>
#include <QHash>

#define ERRORLOGNEW "/usr/local/nPM/data/errorlog_new"
#define ERRORLOGOLD "/usr/local/nPM/data/errorlog_old"

class ErrorLogPrivate
{
public:
    explicit ErrorLogPrivate(ErrorLog *const q_ptr) : q_ptr(q_ptr), partNew(new StorageFile()),
        partOld(new StorageFile()), errorLogNewPath(ERRORLOGNEW), errorLogOldPath(ERRORLOGOLD)
    {
    }

    ErrorLogPrivate(const ErrorLogPrivate &c)
    {
        q_ptr = c.q_ptr;
        partNew = new StorageFile();
        partOld = new StorageFile();
    }
    ~ErrorLogPrivate()
    {
        delete partNew;
        delete partOld;
    }

    void loadfile();
    void clear();
    void switchFile();

    bool checkErrorTimeStamp(unsigned logTitleHashVal, unsigned timestamp);

    ErrorLog *q_ptr;
    QJson::Serializer serializer;
    QJson::Parser parser;
    StorageFile *partNew;
    StorageFile *partOld;
    QString errorLogNewPath;
    QString errorLogOldPath;
    QList<ErrorLogItemBase *> storeItem;
    QMap<unsigned, unsigned> errorItemTimeStamp;
    QMutex filemutex;
    QMutex listMutex;
};

/***************************************************************************************************
 * loadfile : load the storage file
 **************************************************************************************************/
void ErrorLogPrivate::loadfile()
{
    bool broken = false;
    partNew->reload(errorLogNewPath, QIODevice::ReadWrite);
    if (!partNew->isValid())
    {
        broken = true;
        StorageFile::remove(partNew);
        partNew->reload(errorLogNewPath, QIODevice::ReadWrite);
    }

    partOld->reload(errorLogOldPath, QIODevice::ReadWrite);
    if (!partOld->isValid())
    {
        broken = true;
        StorageFile::remove(partOld);
        partOld->reload(errorLogOldPath, QIODevice::ReadWrite);
    }

    if (broken)
    {
        qdebug("Error log is broken.");
    }
}

/***************************************************************************************************
 * clear : clear all the error log
 **************************************************************************************************/
void ErrorLogPrivate::clear()
{
    StorageFile::remove(partNew);
    StorageFile::remove(partOld);
    sync();
    loadfile();
}

/***************************************************************************************************
 * switchFile: switch file when the block number is larger than MaximumLogNum in one storage file.
 **************************************************************************************************/
void ErrorLogPrivate::switchFile()
{
    StorageFile::remove(partOld);
    StorageFile::rename(partNew, errorLogOldPath);
    sync();
    loadfile();
}

/***************************************************************************************************
 *   check the error log timestamp, if two identical error happens in less than 2 second, return false,
 *   otherwise, return true
 **************************************************************************************************/
bool ErrorLogPrivate::checkErrorTimeStamp(unsigned logTitleHashVal, unsigned timestamp)
{
    bool ret = false;

    QMap<unsigned, unsigned>::iterator iter = errorItemTimeStamp.find(logTitleHashVal);
    if (iter != errorItemTimeStamp.end())
    {
        if (timestamp - iter.value() <= 1)
        {
            ret = true;
        }
    }

    errorItemTimeStamp.insert(logTitleHashVal, timestamp);

    return ret;
}

/***************************************************************************************************
 * count : get the items number. Although the actual number might be larger than MaximumLogNum,
 *         we return MaximumLogNum at most
 **************************************************************************************************/
int ErrorLog::count()
{
    Q_D(ErrorLog);
    int count = 0;
    d->filemutex.lock();
    count = d->partNew->getBlockNR() + d->partOld->getBlockNR();
    d->filemutex.unlock();

    if (count > static_cast<int>(MaximumLogNum))
    {
        return  MaximumLogNum;
    }
    else
    {
        return count;
    }
}

/***************************************************************************************************
 * getLog : get a errorlogitem at specific index, return NULL on failure
 **************************************************************************************************/
ErrorLogItemBase *ErrorLog::getLog(int index)
{
    Q_D(ErrorLog);
    QByteArray data;
    quint32 type = 0;
    int realindex;
    ErrorLogItemBase *item = NULL;
    if (index >= this->count())
    {
        return item;
    }
    d->filemutex.lock();
    if (d->partNew->getBlockNR() > (unsigned int)index)
    {
        realindex = d->partNew->getBlockNR() - index - 1;
        data = d->partNew->getBlockData(realindex);
        type = d->partNew->getBlockType(realindex);
    }
    else
    {
        realindex = d->partOld->getBlockNR() - 1 - (index - d->partNew->getBlockNR());
        data = d->partOld->getBlockData(realindex);
        type = d->partOld->getBlockType(realindex);
    }
    d->filemutex.unlock();

    QMap<int, ErrorLogItemCreateFunc> &creatormap = ErrorLogItemBase::getItemCreatorMap();
    if (creatormap.contains(type))
    {
        item = creatormap[type]();
        item->content = d->parser.parse(data).toMap();
    }
    return item;
}


/***************************************************************************************************
 * append : append a error log item, we will delete the item on complete
 **************************************************************************************************/
void ErrorLog::append(ErrorLogItemBase *item)
{
    Q_D(ErrorLog);
    QMutexLocker locker(&d->listMutex);

    QDateTime dt = QDateTime::fromString(item->getTime(), "yyyy-MM-dd HH:mm:ss.zzz");

    if (d->checkErrorTimeStamp(qHash(item->name()), dt.toTime_t()))
    {
        // this log is recorded too fast, not add to errorlog
        qDebug() << "Discard errorlog: " << item->name();
        delete item;
        return;
    }
    d->storeItem.append(item);
}

/***************************************************************************************************
 * append : append items, we will delete the item on complete
 **************************************************************************************************/
void ErrorLog::append(const QList<ErrorLogItemBase *> &items)
{
    Q_D(ErrorLog);
    QDateTime dt;
    QMutexLocker locker(&d->listMutex);
    foreach(ErrorLogItemBase *item, items)
    {
        dt = QDateTime::fromString(item->getTime(), "yyyy-MM-dd HH:mm:ss.zzz");
        if (d->checkErrorTimeStamp(qHash(item->name()), dt.toTime_t()))
        {
            // this log is recorded too fast, not add to errorlog
            qDebug() << "Discard errorlog: " << item->name();
            delete item;
            continue;
        }
        d->storeItem.append(item);
    }
}

/***************************************************************************************************
 * clear : clear all the items
 **************************************************************************************************/
void ErrorLog::clear()
{
    Q_D(ErrorLog);
    d->listMutex.lock();
    qDeleteAll(d->storeItem);
    d->storeItem.clear();
    d->errorItemTimeStamp.clear();
    d->listMutex.unlock();

    d->filemutex.lock();
    d->clear();
    d->filemutex.unlock();
}

/***************************************************************************************************
 * get the number of specific log type
 **************************************************************************************************/
int ErrorLog::getTypeCount(unsigned int type)
{
    Q_D(ErrorLog);
    int total = this->count();
    int num = 0;
    d->filemutex.lock();
    int newblockNr = d->partNew->getBlockNR();
    for (int i = 0; i < newblockNr; i++)
    {
        if (d->partNew->getBlockType(i) == type)
        {
            num += 1;
        }
    }
    int oldblockNr = d->partOld->getBlockNR();
    int left = total - newblockNr;
    for (int i = oldblockNr - left; i < oldblockNr; i++)
    {
        if (d->partOld->getBlockType(i) == type)
        {
            num += 1;
        }
    }
    d->filemutex.unlock();
    return num;
}

/***************************************************************************************************
 * get the latest log of specific type
 **************************************************************************************************/
ErrorLogItemBase *ErrorLog::getLatestLog(unsigned int type)
{
    Q_D(ErrorLog);
    int total = this->count();
    ErrorLogItemBase *item = NULL;
    QByteArray data;
    bool found = false;
    QMutexLocker locker(&d->filemutex);
    int newblockNr = d->partNew->getBlockNR();
    for (int i = newblockNr - 1; i >= 0; i--)
    {
        if (d->partNew->getBlockType(i) == type)
        {
            data = d->partNew->getBlockData(i);
            found = true;
            break;
        }
    }
    if (!found)
    {
        int oldBlockNr = d->partOld->getBlockNR();
        int left = total - newblockNr;
        for (int i = oldBlockNr - 1; i >= oldBlockNr - left; i--)
        {
            if (d->partOld->getBlockType(i) == type)
            {
                data = d->partOld->getBlockData(i);
                found = true;
            }
            break;
        }
    }
    if (found)
    {
        item = ErrorLogItemBase::getItemCreatorMap()[type]();
        item->content = d->parser.parse(data).toMap();
    }
    return item;
}

ErrorLog::Summary ErrorLog::getSummary()
{
    Summary summary;
    ErrorLogItemBase *item;
    summary.NumOfErrors = count();
    summary.numOfCriticalErrors = getTypeCount(CriticalFaultLogItem::Type);
    if ((item = getLog(0)))
    {
        summary.mostRecentErrorDate = item->getTime();
        delete item;
    }

    if ((item = getLatestLog(CriticalFaultLogItem::Type)))
    {
        summary.mostRecentCriticalErrorDate = item->getTime();
        delete item;
    }

    if ((item = getLog(count() - 1)))
    {
        summary.oldestErrorDate = item->getTime();
    }

    unsigned int num = 0;
    systemConfig.getNumValue("ErrorLogEraseTime", num);
    if (num > 0)
    {
        summary.lastEraseTimeDate = QDateTime::fromTime_t(num).toString("yyyy-MM-dd HH:mm:ss");
    }

    return summary;
}

void ErrorLog::run()
{
    Q_D(ErrorLog);

    d->listMutex.lock();
    while (!d->storeItem.isEmpty())
    {
        ErrorLogItemBase *item = d->storeItem.takeFirst();
        d->listMutex.unlock();

        QByteArray data = d->serializer.serialize(item->content);
        d->filemutex.lock();
        d->partNew->appendBlockData(item->type(), data.constData(), data.length());
        if (d->partNew->getBlockNR() >= ErrorLog::MaximumLogNum)
        {
            d->switchFile();
        }
        d->filemutex.unlock();
        delete item;
        d->listMutex.lock();
    }
    d->listMutex.unlock();
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
ErrorLog::ErrorLog()
    : d_ptr(new ErrorLogPrivate(this))
{
    Q_D(ErrorLog);
    d->loadfile();
    d->serializer.setIndentMode(QJson::IndentCompact);
}

/***************************************************************************************************
 * Destructor
 **************************************************************************************************/
ErrorLog &ErrorLog::getInstance()
{
    static ErrorLog *instance = NULL;
    if (instance == NULL)
    {
        instance = new ErrorLog;

        // register the new interface and delete the old one
        ErrorLogInterface *old = registerErrorLog(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

ErrorLog::~ErrorLog()
{
}

bool ErrorLog::setStorageDir(const QString &dir)
{
    Q_D(ErrorLog);

    if (!QDir(dir).exists()) {
        return false;
    }

    QString newDir = dir;

    d->filemutex.lock();

    if (!newDir.endsWith('/')) {
        newDir.append('/');
    }
    d->errorLogNewPath = newDir + "errorlog_new";
    d->errorLogOldPath = newDir + "errorlog_old";
    d->loadfile();

    d->filemutex.unlock();

    return true;
}
