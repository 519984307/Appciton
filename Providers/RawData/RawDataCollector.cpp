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
    {
        int v = 0;
        machineConfig.getNumValue("Record|ECG", v);
        collectionStatus[RawDataCollector::ECG_DATA] = v;
        v = 0;
        machineConfig.getNumValue("Record|SPO2", v);
        collectionStatus[RawDataCollector::SPO2_DATA] = v;
        v = 0;
        machineConfig.getNumValue("Record|NIBP", v);
        collectionStatus[RawDataCollector::NIBP_DATA] = v;
        v = 0;
        machineConfig.getNumValue("Record|TEMP", v);
        collectionStatus[RawDataCollector::TEMP_DATA] = v;

        for (int i = RawDataCollector::ECG_DATA; i < RawDataCollector::DATA_TYPE_NR; ++i)
        {
            files[i] = 0;
        }
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
     * @brief saveEcgRawData and the ecg raw data to file
     * @param data the data struct contain data need to store
     */
    void saveEcgRawData(const StoreDataType *data);

    /**
     * @brief saveSPO2RawData and the spo2 raw data to file
     * @param data the data struct contain data need to store
     */
    void saveSPO2RawData(const StoreDataType *data);

    /**
     * @brief saveNIBPRawData and the NIBP raw data to file
     * @param data the data struct contain data need to store
     */
    void saveNIBPRawData(const StoreDataType *data);

    bool collectionStatus[RawDataCollector::DATA_TYPE_NR];

    QLinkedList<StoreDataType *> dataBuffer;

    QFile *files[RawDataCollector::DATA_TYPE_NR];

    QMutex mutex;
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
        Q_UNUSED(len)
        Q_ASSERT(len == 90);
        QTextStream stream(&content);
        for (int i = 0; i < 10; i++)
        {
            // 4 Byte IPACE
            unsigned int ipace = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
            // 3 Byte ecg
            data += 4;
            unsigned int rawEcgData = (data[0] << 8) | (data[1] << 16) | (data[2] << 24);
            int ecgData = rawEcgData;
            ecgData >>= 8;
            stream << ipace << ',';
            stream << 0 << ',';
            stream << 256 << ',';
            stream << ecgData << ',';
            stream << endl;
            // 2 byte lead status
            data += 5;
        }

//        // skip the first 4 SN bytes
//        data += 4;

//        for (int n = 0; n < 20; n++)
//        {

//            unsigned short marksAndLeadOffs = (data[0] << 8) | data[1];
//            data += 2;
//            stream << marksAndLeadOffs;

//            // 12 lead data
//            for (int i = 0; i < 12; i++)
//            {
//                short v = data[0] | (data[1] << 8);
//                data += 2;
//                stream << ',' << v;
//            }
//            stream << endl;
//        }
        stream.flush();

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
        Q_UNUSED(len)
        Q_ASSERT(len == 16);
        QTextStream stream(&content);

        data += 1;
        for (int n = 0; n < 5; n++)
        {

            int v = data[0] | (data[1] << 8) | (data[2] << 16);
            data += 3;
            stream << v;
            if (n != 4)
            {
                stream << ",";
            }
        }
        stream << endl;
        stream.flush();

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
        Q_UNUSED(len)
        Q_ASSERT(len == 50);
        QTextStream stream(&content);
        // 10个AD值,每个占3个字节
        for (int n = 0; n < 10; n ++)
        {
            unsigned int adVal = (data[n * 3]) | (data[n * 3 + 1] << 8) | (data[n * 3 + 2] << 16);
            stream << adVal << endl;
        }

        data += 10 * 3;

        // 10个压力值, 每个占2个字节
        for (int n = 0; n < 10; n++)
        {
            unsigned short pressureVal = (data[n * 2]) | (data[n * 2 + 1] << 8);
            stream << pressureVal << endl;
        }
        stream.flush();

        mutex.lock();
        dataBuffer.append(new StoreDataType(RawDataCollector::NIBP_DATA, content, stop));
        mutex.unlock();
    }
}

void RawDataCollectorPrivate::saveEcgRawData(const StoreDataType *data)
{
    QFile *f = files[RawDataCollector::ECG_DATA];

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
        files[RawDataCollector::ECG_DATA] = NULL;
        return;
    }
    else if (f == NULL)
    {
        QString dirname = usbManager.getUdiskMountPoint() + "/ECG_DATA/";
        if (!QDir(dirname).exists())
        {
            if (!QDir().mkpath(dirname))
            {
                qDebug() << "Fail to create directory " << dirname;
                return;
            }
        }
        QString name = dirname + QDateTime::currentDateTime().toString("yyyy.MM.dd-HH.mm.ss") + ".txt";

        // not open the file yet, open now
        f = new QFile(name);
        if (!f->open(QIODevice::WriteOnly))
        {
            qDebug() << "Fail to create file " << name;
            delete f;
            return;
        }
        files[RawDataCollector::ECG_DATA] = f;
    }

    f->write(data->data);

    if (f->size() > 10 * 1024 * 1024)
    {
        // store 10 M data in each file
        fsync(f->handle());
        f->close();
        delete f;
        files[RawDataCollector::ECG_DATA] = NULL;
    }
}

void RawDataCollectorPrivate::saveSPO2RawData(const StoreDataType *data)
{
    QFile *f = files[RawDataCollector::SPO2_DATA];

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
        files[RawDataCollector::SPO2_DATA] = NULL;
        return;
    }
    else if (f == NULL)
    {
        QString dirname = usbManager.getUdiskMountPoint() + "/SPO2_DATA/";
        if (!QDir(dirname).exists())
        {
            if (!QDir().mkpath(dirname))
            {
                qDebug() << "Fail to create directory " << dirname;
                return;
            }
        }
        QString name = dirname + QDateTime::currentDateTime().toString("yyyy.MM.dd-HH.mm.ss") + ".txt";

        // not open the file yet, open now
        f = new QFile(name);
        if (!f->open(QIODevice::WriteOnly))
        {
            qDebug() << "Fail to create file " << name;
            delete f;
            return;
        }
        files[RawDataCollector::SPO2_DATA] = f;
    }

    f->write(data->data);

    if (f->size() > 10 * 1024 * 1024)
    {
        // store 10 M data in each file
        fsync(f->handle());
        f->close();
        delete f;
        files[RawDataCollector::SPO2_DATA] = NULL;
    }
}

void RawDataCollectorPrivate::saveNIBPRawData(const StoreDataType *data)
{
    QFile *f = files[RawDataCollector::NIBP_DATA];
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
        files[RawDataCollector::NIBP_DATA] = NULL;
        return;
    }
    else if (f == NULL)
    {
        QString dirname = usbManager.getUdiskMountPoint() + "/BLM_N5/";
        if (!QDir(dirname).exists())
        {
            if (!QDir().mkpath(dirname))
            {
                qDebug() << "Fail to create directory " << dirname;
                return;
            }
        }
        QString name = dirname + QString("N5_%1.txt").arg(QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));

        // not open the file yet, open now
        f = new QFile(name);
        if (!f->open(QIODevice::WriteOnly))
        {
            qDebug() << "Fail to create file " << name;
            delete f;
            return;
        }
        files[RawDataCollector::NIBP_DATA] = f;
    }

    f->write(data->data);

    if (f->size() > 10 * 1024 * 1024)
    {
        // store 10 M data in each file
        fsync(f->handle());
        f->close();
        delete f;
        files[RawDataCollector::NIBP_DATA] = NULL;
    }
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
        return;
    }

    d_ptr->mutex.lock();
    while (!d_ptr->dataBuffer.isEmpty())
    {
        StoreDataType *data = d_ptr->dataBuffer.takeFirst();
        d_ptr->mutex.unlock();

        switch (data->type)
        {
        case ECG_DATA:
            d_ptr->saveEcgRawData(data);
            break;
        case SPO2_DATA:
            d_ptr->saveSPO2RawData(data);
            break;
        case NIBP_DATA:
            d_ptr->saveNIBPRawData(data);
            break;

        default:
            break;
        }
        delete data;

        d_ptr->mutex.lock();
    }
    d_ptr->mutex.unlock();
}

RawDataCollector::RawDataCollector()
    : d_ptr(new RawDataCollectorPrivate())
{
}

void RawDataCollector::collectData(RawDataCollector::CollectDataType type, const unsigned char *data, int len,
                                   bool stop)
{
    if (!d_ptr->collectionStatus[type])
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

    default:
        break;
    }
}

void RawDataCollector::setCollectStatus(RawDataCollector::CollectDataType type, bool enable)
{
    d_ptr->collectionStatus[type] = enable;
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
