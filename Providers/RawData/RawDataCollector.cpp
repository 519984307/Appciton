/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/15
 **/

#include "RawDataCollector.h"
#include <QDateTime>
#include "USBManager.h"
#include "IConfig.h"
#include <QLinkedList>
#include <QTextStream>
#include <QDir>
#include <QMutex>
#include <QDebug>
#include <unistd.h>
#include <QTimerEvent>
#include "USBManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include <QTimer>

struct StoreDataType
{
    StoreDataType(RawDataCollector::CollectDataType type, const QByteArray &data, bool stop)
        : type(type), data(data), stop(stop) {}
    RawDataCollector::CollectDataType type;
    QByteArray data;
    bool stop;
};

class RawDataCollectorPrivate
{
public:
    RawDataCollectorPrivate()
        : timerId(-1)
    {
        for (int i = RawDataCollector::ECG_DATA; i < RawDataCollector::DATA_TYPE_NR; ++i)
        {
            files[i] = NULL;
            leadOffStatus[i] = true;
            startCollection[i] = false;
            leadOffTimer[i] = new QTimer();
            leadOffTimer[i]->setSingleShot(true);
            leadOffTimer[i]->setInterval(10 * 1000);   // 10 s
            fileTimer[i] = new QTimer();
            fileTimer[i]->setSingleShot(true);
            fileTimer[i]->setInterval(24 * 3600 * 1000);    // 24 hour
        }
        int v = 0;
        machineConfig.getNumValue("Record|ECG", v);
        collectionStatus[RawDataCollector::ECG_DATA] = v;
        v = 0;
        machineConfig.getNumValue("Record|SPO2", v);
        collectionStatus[RawDataCollector::SPO2_DATA] = v;
        v = 0;
        machineConfig.getNumValue("Record|NIBP", v);
        collectionStatus[RawDataCollector::NIBP_DATA] = v;
        startCollection[RawDataCollector::NIBP_DATA] = true;
        v = 0;
        machineConfig.getNumValue("Record|TEMP", v);
        collectionStatus[RawDataCollector::TEMP_DATA] = v;
        v = 0;
        machineConfig.getNumValue("Record|CO2", v);
        collectionStatus[RawDataCollector::CO2_DATA] = v;
        startCollection[RawDataCollector::CO2_DATA] = true;
    }

    /**
     * @brief handleECGRawData handle the ECG raw data
     * @param data pointer to the data
     * @param len the data length
     */
    void handleECGRawData(const unsigned char *data,  int len, bool stop);

    /**
     * @brief handleSPO2RawData handle the SPO2 raw data
     * @param data pointer to the data
     * @param len the data length
     * @param stop
     */
    void handleSPO2RawData(const unsigned char *data, int len, bool stop);

    /**
     * @brief handleNIBPRawData handle the NIBP raw data
     * @param data pointer to the data
     * @param len the data length
     */
    void handleNIBPRawData(const unsigned char *data, int len, bool stop);

    /**
     * @brief handleCO2RawData handle the CO2 raw data
     * @param data pointer to the data
     * @param len the data length
     */
    void handleCO2RawData(const unsigned char *data, int len, bool stop);

    /**
     * @brief handleTempRawData  handle the temp raw data
     * @param data  pointer to the data
     * @param len the data length
     * @param stop stop collectting data
     */
    void handleTempRawData(const unsigned char *data, int len, bool stop,
                           RawDataCollector::CollectDataType type);

    /**
     * @brief saveParamRawData  save the param raw data to file
     * @param data  the data struct contain data need to store
     */
    void saveParamRawData(const StoreDataType *data);

    /**
     * @brief createNewFile  create new file by CollectDataType
     * @param type  Collect Data Type
     */
    QFile *createNewFile(RawDataCollector::CollectDataType type);

    bool collectionStatus[RawDataCollector::DATA_TYPE_NR];
    bool leadOffStatus[RawDataCollector::DATA_TYPE_NR];    // lead status
    bool startCollection[RawDataCollector::DATA_TYPE_NR];  // true :start collectting raw data

    QLinkedList<StoreDataType *> dataBuffer;

    QFile *files[RawDataCollector::DATA_TYPE_NR];
    QTimer *leadOffTimer[RawDataCollector::DATA_TYPE_NR];  // ECG,SPO2,TEMP lead off timer
    QTimer *fileTimer[RawDataCollector::DATA_TYPE_NR];     // Close raw files every 24 hours.
    QMutex mutex;

    int timerId;
};

void RawDataCollectorPrivate::handleECGRawData(const unsigned char *data, int len, bool stop)
{
    QByteArray content;
    if (stop)
    {
        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::ECG_DATA, content, stop));
        mutex.unlock();
    }
    else
    {
        /*
         * 1 Byte                                        + (3 Byte)         + (3 Byte)
         * IPACE(1bit) + RESP Calc(2bit)+ ECG Calc(4bit) + ECG data(3 Byte) + RESP data(3 Byte)
         */
        QDataStream stream(&content, QIODevice::WriteOnly);
        for (int i = 0; i < len; ++i)
        {
            stream << data[i];
        }

        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::ECG_DATA, content, stop));
        mutex.unlock();
    }
}

void RawDataCollectorPrivate::handleSPO2RawData(const unsigned char *data, int len, bool stop)
{
    QByteArray content;
    if (stop)
    {
        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::SPO2_DATA, content, stop));
        mutex.unlock();
    }
    else
    {
        /*
         * 红光(3Byte) + 红外光(3Byte) + 背景光(3Byte )
         */
        QDataStream stream(&content, QIODevice::WriteOnly);
        for (int i = 0; i < len; ++i)
        {
            stream << data[i];
        }

        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::SPO2_DATA, content, stop));
        mutex.unlock();
    }
}

void RawDataCollectorPrivate::handleNIBPRawData(const unsigned char *data, int len, bool stop)
{
    QByteArray content;

    if (stop)
    {
        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::NIBP_DATA, content, stop));
        mutex.unlock();
    }
    else
    {
        /*
         * 10个AD值,每个占3个字节  + 10个压力值, 每个占2个字节
         */
        QDataStream stream(&content, QIODevice::WriteOnly);
        for (int i = 0; i < len; ++i)
        {
            stream << data[i];
        }
        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::NIBP_DATA, content, stop));
        mutex.unlock();
    }
}

void RawDataCollectorPrivate::handleCO2RawData(const unsigned char *data, int len, bool stop)
{
    QByteArray content;

    if (stop)
    {
        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::CO2_DATA, content, stop));
        mutex.unlock();
    }
    else
    {
        QDataStream stream(&content, QIODevice::WriteOnly);
        for (int i = 0; i < len; ++i)
        {
            stream << data[i];
        }

        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::CO2_DATA, content, stop));
        mutex.unlock();
    }
}

void RawDataCollectorPrivate::handleTempRawData(const unsigned char *data, int len, bool stop,
                                                RawDataCollector::CollectDataType type)
{
    QByteArray content;

    if (stop)
    {
        mutex.lock();
        dataBuffer.append(new StoreDataType(type, content, stop));
        mutex.unlock();
    }
    else
    {
        // collect temp raw data
        QDataStream stream(&content, QIODevice::WriteOnly);
        for (int i = 0; i < len; ++i)
        {
            stream << data[i];
        }

        mutex.lock();
        dataBuffer.append(new StoreDataType(type, content, stop));
        mutex.unlock();
    }
}

void RawDataCollectorPrivate::saveParamRawData(const StoreDataType *data)
{
    if (data == NULL)
    {
        return;
    }
    RawDataCollector::CollectDataType type = data->type;
    if (type >= RawDataCollector::DATA_TYPE_NR)
    {
        return;
    }

    QFile *f = files[type];
    if (f == NULL && data->stop)
    {
        // do nothing
        return;
    }
    else if (f && data->stop)
    {
        // close the file and delete the file descriptor
        fsync(f->handle());
        f->close();
        delete f;
        files[type] = NULL;
        return;
    }
    else if (f == NULL)
    {
        f = createNewFile(type);
        if (f == NULL)
        {
            return;
        }
    }

    f->write(data->data);

    if (f->size() > 10 * 1024 * 1024)
    {
        // store 10 M data in each file
        fsync(f->handle());
        f->close();
        delete f;
        files[type] = NULL;
    }
}

QFile *RawDataCollectorPrivate::createNewFile(RawDataCollector::CollectDataType type)
{
    QString dirName = usbManager.getUdiskMountPoint();
    QString fileName;
    QString curTime = QDateTime::currentDateTime().toString("yyyyMMddHHmmss");
    switch (type)
    {
    case RawDataCollector::ECG_DATA:
        dirName += "/ECG_DATA/";
        fileName = dirName + QString("ECG_%1.dat").arg(curTime);
        break;
    case RawDataCollector::SPO2_DATA:
        dirName += "/SPO2_DATA/";
        fileName = dirName + QString("SPO2_%1.dat").arg(curTime);
        break;
    case RawDataCollector::NIBP_DATA:
        dirName += "/NIBP_DATA/";
        fileName = dirName + QString("NIBP_%1.dat").arg(curTime);
        break;
    case RawDataCollector::TEMP_DATA:
        dirName += "/TEMP_DATA/";
        fileName = dirName + QString("TEMP1_%1.dat").arg(curTime);
        break;
    case RawDataCollector::TEMP2_DATA:
        dirName += "/TEMP_DATA/";
        fileName = dirName + QString("TEMP2_%1.dat").arg(curTime);
        break;
    case RawDataCollector::CO2_DATA:
        dirName += "/CO2_DATA/";
        fileName = dirName + QString("CO2_%1.dat").arg(curTime);
        break;
    default:
        qWarning("Raw data collect type error!");
        return NULL;
        break;
    }

    if (!QDir(dirName).exists())
    {
        if (!QDir().mkpath(dirName))
        {
            qDebug() << "Fail to create directory " << dirName;
            return NULL;
        }
    }

    // not open the file yet, open now
    QFile *f = new QFile(fileName);
    if (!f->open(QIODevice::WriteOnly))
    {
        qDebug() << "Fail to create file " << fileName;
        delete f;
        return NULL;
    }
    files[type] = f;
    if (fileTimer[type])
    {
        fileTimer[type]->start();
    }
    return f;
}

RawDataCollector &RawDataCollector::getInstance()
{
    static RawDataCollector *instance = NULL;
    if (instance == NULL)
    {
        instance = new RawDataCollector();
    }
    return *instance;
}

void RawDataCollector::run()
{
    if (!usbManager.isUSBExist())
    {
        for (int i = ECG_DATA; i < DATA_TYPE_NR; i++)
        {
            if (d_ptr->files[i])
            {
                d_ptr->files[i]->close();
                delete d_ptr->files[i];
                d_ptr->files[i] = NULL;
            }
        }

        d_ptr->mutex.lock();
        qDeleteAll(d_ptr->dataBuffer);
        d_ptr->dataBuffer.clear();
        d_ptr->mutex.unlock();
        stopCollectData();          // U盘被强制拔出停止定时器
        return;
    }

    d_ptr->mutex.lock();
    while (!d_ptr->dataBuffer.isEmpty())
    {
        StoreDataType *data = d_ptr->dataBuffer.takeFirst();
        d_ptr->mutex.unlock();

        // save param raw data to file
        d_ptr->saveParamRawData(data);

        delete data;

        d_ptr->mutex.lock();
    }
    d_ptr->mutex.unlock();
}

void RawDataCollector::startCollectData()
{
    if (d_ptr->timerId == -1)
    {
        d_ptr->timerId = startTimer(1000);  // 1s
    }
}

void RawDataCollector::stopCollectData()
{
    if (d_ptr->timerId != -1)
    {
        killTimer(d_ptr->timerId);
        d_ptr->timerId = -1;
    }
    // U盘卸载时首先应该关掉正在使用的文件
    for (int i = ECG_DATA; i < DATA_TYPE_NR; i++)
    {
        if (d_ptr->files[i])
        {
            fsync(d_ptr->files[i]->handle());
            d_ptr->files[i]->close();
            delete d_ptr->files[i];
            d_ptr->files[i] = NULL;
        }

        // stop timer
        if (d_ptr->fileTimer[i])
        {
            d_ptr->fileTimer[i]->stop();
        }
        if (d_ptr->leadOffTimer[i])
        {
            d_ptr->leadOffTimer[i]->stop();
        }
    }
    d_ptr->mutex.lock();
    qDeleteAll(d_ptr->dataBuffer);
    d_ptr->dataBuffer.clear();
    d_ptr->mutex.unlock();

    if (usbManager.isUSBExist())
    {
        usbManager.umountUDisk();     // 正常卸载U盘
    }
    else
    {
        usbManager.forceUmountDisk();  // 强制卸载U盘
    }
}

void RawDataCollector::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == d_ptr->timerId)
    {
        run();
    }
}

void RawDataCollector::_onTimeOut()
{
    QTimer *timer = qobject_cast<QTimer *>(sender());
    if (timer == NULL)
    {
        return;
    }

    for (int i = RawDataCollector::ECG_DATA; i < RawDataCollector::DATA_TYPE_NR; ++i)
    {
        if (d_ptr->leadOffTimer[i] == timer)
        {
            d_ptr->startCollection[i] = false;
        }
        else if (d_ptr->fileTimer[i] == timer && d_ptr->files[i])
        {
            // The raw data file can store data for up to 24 hours.
            fsync(d_ptr->files[i]->handle());
            d_ptr->files[i]->close();
            delete d_ptr->files[i];
            d_ptr->files[i] = NULL;
        }
    }
}

RawDataCollector::RawDataCollector()
    : d_ptr(new RawDataCollectorPrivate())
{
    for (int i = RawDataCollector::ECG_DATA; i < RawDataCollector::DATA_TYPE_NR; ++i)
    {
        if (d_ptr->leadOffTimer[i])
        {
            connect(d_ptr->leadOffTimer[i], SIGNAL(timeout()), this, SLOT(_onTimeOut()));
        }
        if (d_ptr->fileTimer[i])
        {
            connect(d_ptr->fileTimer[i], SIGNAL(timeout()), this, SLOT(_onTimeOut()));
        }
    }
}

void RawDataCollector::collectData(RawDataCollector::CollectDataType type, const unsigned char *data, int len,
                                   bool stop)
{
    if (!d_ptr->collectionStatus[type] || !usbManager.isUSBExist() || !d_ptr->startCollection[type])
    {
        // not enable yet
        return;
    }

    switch (type)
    {
    case RawDataCollector::ECG_DATA:
        d_ptr->handleECGRawData(data, len, stop);
        break;
    case RawDataCollector::SPO2_DATA:
        d_ptr->handleSPO2RawData(data, len, stop);
        break;
    case RawDataCollector::NIBP_DATA:
        d_ptr->handleNIBPRawData(data, len, stop);
        break;
    case RawDataCollector::CO2_DATA:
        d_ptr->handleCO2RawData(data, len, stop);
        break;
    case RawDataCollector::TEMP_DATA:
    case RawDataCollector::TEMP2_DATA:
        d_ptr->handleTempRawData(data, len, stop, type);
        break;

    default:
        break;
    }
}

void RawDataCollector::setCollectStatus(RawDataCollector::CollectDataType type, bool enable)
{
    d_ptr->collectionStatus[type] = enable;
    if (type == RawDataCollector::TEMP_DATA)
    {
        d_ptr->collectionStatus[TEMP2_DATA] = enable;
    }
}

void RawDataCollector::setLeadOffStatus(RawDataCollector::CollectDataType type, bool status)
{
    if (d_ptr->leadOffStatus[type] == status || d_ptr->leadOffTimer[type] == NULL)
    {
        return;
    }

    d_ptr->leadOffStatus[type] = status;

    // After ECG, SPO2, TEMP lead off, the raw data were only stored for 10s.
    if (status)
    {
        // ECG, SPO2, TEMP lead off, start timer to stop collectting data.
        d_ptr->leadOffTimer[type]->start();
    }
    else
    {
        // ECG, SPO2, TEMP lead on, stop timer and start collect raw data.
        d_ptr->leadOffTimer[type]->stop();
        // start collectting raw data.
        d_ptr->startCollection[type] = true;
    }
}

RawDataCollector::~RawDataCollector()
{
    qDeleteAll(d_ptr->dataBuffer);
    for (int i = ECG_DATA; i < DATA_TYPE_NR; i++)
    {
        if (d_ptr->files[i])
        {
            delete d_ptr->files[i];
            d_ptr->files[i] = NULL;
        }
    }

    delete d_ptr;
}
