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

struct StoreDataType
{
    StoreDataType(RawDataCollector::CollectDataType type, const QByteArray &data)
        : type(type), data(data) {}
    RawDataCollector::CollectDataType type;
    QByteArray data;
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
    void handleECGRawData(const unsigned char *data,  int len);

    /**
     * @brief saveEcgRawData and the ecg raw data to file
     * @param content the file content
     */
    void saveEcgRawData(const QByteArray &content);

    bool collectionStatus[RawDataCollector::DATA_TYPE_NR];

    QLinkedList<StoreDataType *> dataBuffer;

    QFile *files[RawDataCollector::DATA_TYPE_NR];

    QMutex mutex;
};

void RawDataCollectorPrivate::handleECGRawData(const unsigned char *data, int len)
{
    Q_UNUSED(len)
    Q_ASSERT(len == 524);

    QByteArray content;
    QTextStream stream(&content);

    // skip the first 4 SN bytes
    data += 4;

    for (int n = 0; n < 20; n++)
    {

        unsigned short marksAndLeadOffs = (data[0] << 8) | data[1];
        data += 2;
        stream << marksAndLeadOffs;

        // 12 lead data
        for (int i = 0; i < 12; i++)
        {
            short v = data[0] | (data[1] << 8);
            data += 2;
            stream << ',' << v;
        }
        stream << endl;
    }
    stream.flush();

    mutex.lock();
    dataBuffer.append(new StoreDataType(RawDataCollector::ECG_DATA, content));
    mutex.unlock();
}

void RawDataCollectorPrivate::saveEcgRawData(const QByteArray &content)
{
    QFile *f = files[RawDataCollector::ECG_DATA];
    if (f == NULL)
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
        }
        files[RawDataCollector::ECG_DATA] = f;
    }

    f->write(content);

    if (f->size() > 10 * 1024 * 1024)
    {
        // store 10 M data in each file
        f->close();
        delete f;
        files[RawDataCollector::ECG_DATA] = NULL;
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
            d_ptr->saveEcgRawData(data->data);
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

void RawDataCollector::collectData(RawDataCollector::CollectDataType type, const unsigned char *data, int len)
{
    if (!d_ptr->collectionStatus[type])
    {
        // not enable yet
        return;
    }

    switch (type)
    {
    case RawDataCollector::ECG_DATA:
        d_ptr->handleECGRawData(data, len);
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
