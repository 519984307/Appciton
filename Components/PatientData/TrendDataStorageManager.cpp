/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/25
 **/

#include "StorageManager_p.h"
#include "TrendDataStorageManager.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "TimeManager.h"
#include "IConfig.h"
#include "Utility.h"
#include "PatientManager.h"
#include "SystemManager.h"
#include "TrendCache.h"
#include "ParamManager.h"
#include <QVector>
#include "Alarm.h"
#include "ECGDupParam.h"
#include "RESPDupParam.h"
#include "TimeDate.h"
#include "EventStorageManager.h"
#include <QMap>
#include "RingBuff.h"

class ShortTrendStorage
{
public:
    explicit ShortTrendStorage(int dataSize)
    {
        for (int i = 0; i < SHORT_TREND_INTERVAL_NR; i++)
        {
            trendBuffer[i] = new RingBuff<TrendDataType>(dataSize);
        }
    }

    ~ShortTrendStorage()
    {
        for (int i = 0; i < SHORT_TREND_INTERVAL_NR; i++)
        {
            delete trendBuffer[i];
        }
    }

    RingBuff<TrendDataType> *trendBuffer[SHORT_TREND_INTERVAL_NR];
};

class TrendDataStorageManagerPrivate: public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(TrendDataStorageManager)
    explicit TrendDataStorageManagerPrivate(TrendDataStorageManager *q_ptr)
        : StorageManagerPrivate(q_ptr), firstSave(false), lastStoreTimestamp(0)
    {
        memset(&dataDesc, 0, sizeof(TrendDataDescription));
    }


    // check and update addition info
    void updateAdditionInfo();

    void storeShortTrendData(SubParamID subParamID, unsigned timestamp, TrendDataType data, bool forceSave = false);

    void emitShortTrendSingals(unsigned timestamp);

    TrendDataDescription dataDesc;
    bool firstSave;  // first time save after initialize
    unsigned lastStoreTimestamp;
    QMap<SubParamID, ShortTrendStorage *> shortTrends;
};

void TrendDataStorageManagerPrivate::updateAdditionInfo()
{
    bool changed = false;

    if (strcmp(dataDesc.patientName, patientManager.getName()))
    {
        Util::strlcpy(dataDesc.patientName, patientManager.getName(), MAX_PATIENT_NAME_LEN);
        changed = true;
    }

    if (strcmp(dataDesc.patientID, patientManager.getPatID()))
    {
        Util::strlcpy(dataDesc.patientID, patientManager.getName(), MAX_PATIENT_ID_LEN);
        changed = true;
    }

    if (dataDesc.bornDate != patientManager.getBornDate())
    {
        dataDesc.bornDate = patientManager.getBornDate();
        changed = true;
    }

    if (dataDesc.patientGender != patientManager.getSex())
    {
        dataDesc.patientGender = patientManager.getSex();
        changed = true;
    }

    if (dataDesc.patientHeight != patientManager.getHeight())
    {
        dataDesc.patientHeight = patientManager.getHeight();
        changed = true;
    }

    if (dataDesc.patientType != patientManager.getType())
    {
        dataDesc.patientType = patientManager.getType();
        changed = true;
    }

    if (dataDesc.patientWeight != patientManager.getWeight())
    {
        dataDesc.patientWeight = patientManager.getWeight();
    }

    if (!changed && !firstSave)
    {
        return;
    }

    backend->writeAdditionalData(reinterpret_cast<char *>(&dataDesc), sizeof(dataDesc));
    firstSave = false;
}

void TrendDataStorageManagerPrivate::storeShortTrendData(SubParamID subParamID, unsigned timeStamp, TrendDataType data, bool forceSave)
{
    QMap<SubParamID, ShortTrendStorage *>::Iterator iter = shortTrends.find(subParamID);
    if (iter != shortTrends.end())
    {
        if (timeStamp % 10 == 0)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_10S]->push(data);
        }
        else if (forceSave)
        {
            // remove last data
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_10S]->takeHead();
            // add current data
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_10S]->push(data);
        }

        if (timeStamp % 20 == 0)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_20S]->push(data);
        }
        else if (forceSave)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_20S]->takeHead();
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_20S]->push(data);
        }

        if (timeStamp % 30 == 0)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_30S]->push(data);
        }
        else if (forceSave)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_30S]->takeHead();
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_30S]->push(data);
        }

        if (timeStamp % 60 == 0 || forceSave)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_60S]->push(data);
        }
        else if (forceSave)
        {
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_60S]->takeHead();
            (*iter)->trendBuffer[SHORT_TREND_INTERVAL_60S]->push(data);
        }
    }
}

void TrendDataStorageManagerPrivate::emitShortTrendSingals(unsigned timestamp)
{
    Q_Q(TrendDataStorageManager);
    if (timestamp % 10 == 0)
    {
        emit q->newTrendDataArrived(SHORT_TREND_INTERVAL_10S);
    }

    if (timestamp % 20 == 0)
    {
        emit q->newTrendDataArrived(SHORT_TREND_INTERVAL_20S);
    }

    if (timestamp % 30 == 0)
    {
        emit q->newTrendDataArrived(SHORT_TREND_INTERVAL_30S);
    }

    if (timestamp % 60 == 0)
    {
        emit q->newTrendDataArrived(SHORT_TREND_INTERVAL_60S);
    }
}

TrendDataStorageManager &TrendDataStorageManager::getInstance()
{
    static TrendDataStorageManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new TrendDataStorageManager();
        TrendDataStorageManagerInterface *old = registerTrendDataStorageManager(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

TrendDataStorageManager::~TrendDataStorageManager()
{
}

void TrendDataStorageManager::periodRun(unsigned t)
{
    Q_D(TrendDataStorageManager);

    if (d->lastStoreTimestamp == t)
    {
        return;
    }

    TrendDataFlags status = CollectStatusNone;
    if (0 == t % 5)
    {
        status |= CollectStatusPeriod;
        storeData(t, status);
    }
}

void TrendDataStorageManager::storeData(unsigned t, TrendDataFlags dataStatus)
{
    Q_D(TrendDataStorageManager);
    if (dataStorageDirManager.isCurRescueFolderFull() || d->lastStoreTimestamp == t)
    {
        return;
    }

    TrendCacheData data;
    if (!trendCache.getTrendData(t, data))
    {
        // no trend data
        return;
    }

    d->lastStoreTimestamp = t;


    QList<ParamID> idList;
    paramManager.getParams(idList);

    QVector<TrendValueSegment> valueSegments;

    bool hasAlarm = false;
    ParamID paramId = PARAM_NONE;
    bool newNibpData = false;

    for (int i = 0; i < SUB_PARAM_NR; i++)
    {
        SubParamID subParamID = static_cast<SubParamID>(i);
        paramId = paramInfo.getParamID(subParamID);
        if (-1 == idList.indexOf(paramId))
        {
            continue;
        }

        if (!data.values.contains(subParamID))
        {
            d->storeShortTrendData(subParamID, t, InvData());
            continue;
        }


        TrendValueSegment valueSegment;
        valueSegment.subParamId = i;
        valueSegment.value = data.values.value(subParamID);
        valueSegment.alarmFlag = alertor.getAlarmSourceStatus(paramInfo.getParamName(paramId), subParamID);
        if (!hasAlarm)
        {
            hasAlarm = valueSegment.alarmFlag;
        }

        valueSegments.append(valueSegment);

        if (subParamID == SUB_PARAM_NIBP_SYS || subParamID == SUB_PARAM_NIBP_DIA || subParamID == SUB_PARAM_NIBP_MAP)
        {
            // should record the nibp value when the current timestamp is equal to the nibp measurement complete timestamp
            if (t == data.lastNibpMeasureSuccessTime)
            {
                d->storeShortTrendData(subParamID, t, valueSegment.value, true);
                newNibpData = true;
            }
            else
            {
                // current time is not the nibp measure complete time, save as invalid value
                d->storeShortTrendData(subParamID, t, InvData());
            }
        }
        else
        {
            d->storeShortTrendData(subParamID, t, valueSegment.value);
        }
    }

    // emit signals
    d->emitShortTrendSingals(t);

    if (newNibpData)
    {
        emit newNibpDataReceived();
    }

    int dataSize = sizeof(TrendDataSegment) + valueSegments.size() * sizeof(TrendValueSegment);
    QByteArray content(dataSize, 0);

    TrendDataSegment *dataSegment = reinterpret_cast<TrendDataSegment *>(content.data());

    dataSegment->timestamp = t;
    dataSegment->co2Baro = data.co2baro;
    dataSegment->status = dataStatus;
    dataSegment->eventFlag = eventStorageManager.getEventTriggerFlag();
    eventStorageManager.clearEventTriggerFlag();

    if (ecgDupParam.getCurHRSource() == HR_SOURCE_SPO2)
    {
        dataSegment->status |= HRSourceIsSpo2;
    }

    if (respDupParam.getBrSource() == RESPDupParam::BR_SOURCE_ECG)
    {
        dataSegment->status |= BRSourceIsResp;
    }

    if (hasAlarm)
    {
        dataSegment->status |= HasAlarm;
    }

    dataSegment->trendValueNum = valueSegments.size();

    // copy the trend values
    qMemCopy(reinterpret_cast<char *>(dataSegment) + sizeof(TrendDataSegment), valueSegments.constData(),
             sizeof(TrendValueSegment) * valueSegments.size());

    addData(0, content);
}


/* move data from memory to flash */
void TrendDataStorageManager::run()
{
    Q_D(TrendDataStorageManager);
    d->updateAdditionInfo();

    if (dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        return;
    }

    dataStorageDirManager.addDataSize(saveData());
}

void TrendDataStorageManager::registerShortTrend(SubParamID subParamID)
{
    Q_D(TrendDataStorageManager);
    if (d->shortTrends.contains(subParamID))
    {
        return;
    }

    d->shortTrends[subParamID] = new ShortTrendStorage(SHORT_TREND_DATA_NUM);
}

void TrendDataStorageManager::unRegisterShortTrend(SubParamID subParamID)
{
    Q_D(TrendDataStorageManager);
    QMap<SubParamID, ShortTrendStorage *>::Iterator iter = d->shortTrends.find(subParamID);
    if (iter != d->shortTrends.end())
    {
        ShortTrendStorage *s = iter.value();
        d->shortTrends.erase(iter);
        delete s;
    }
}

int TrendDataStorageManager::getShortTrendData(SubParamID subParam, TrendDataType *dataBuf, int count,
        ShortTrendInterval interval) const
{
    Q_D(const TrendDataStorageManager);
    if (dataBuf == NULL)
    {
        return 0;
    }

    ShortTrendStorage *s = d->shortTrends.value(subParam, NULL);
    if (s == NULL)
    {
        return 0;
    }

    RingBuff<TrendDataType> *ringBuf = s->trendBuffer[interval];

    int index = 0;
    int bufSize = ringBuf->dataSize();
    if (bufSize > count)
    {
        index  = bufSize - count;
    }

    return ringBuf->copy(index, dataBuf, count);
}

TrendDataType TrendDataStorageManager::getLatestShortTrendData(SubParamID subParam, ShortTrendInterval interval)
{
    Q_D(const TrendDataStorageManager);
    ShortTrendStorage *s = d->shortTrends.value(subParam, NULL);
    if (s)
    {
        bool ok = false;
        TrendDataType d = s->trendBuffer[interval]->head(ok);
        if (ok)
        {
            return d;
        }
    }

    return InvData();
}

void TrendDataStorageManager::newPatientHandle()
{
    Q_D(TrendDataStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + TREND_DATA_FILE_NAME,
                       QIODevice::ReadWrite);
}

TrendDataStorageManager::TrendDataStorageManager()
    : StorageManager(new TrendDataStorageManagerPrivate(this), new StorageFile())
{
    Q_D(TrendDataStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + TREND_DATA_FILE_NAME,
                       QIODevice::ReadWrite);
    d->backend->setReservedSize(sizeof(TrendDataDescription));
    d->backend->readAdditionalData(reinterpret_cast<char *>(&d->dataDesc), sizeof(TrendDataDescription));

    if (d->dataDesc.startTime != 0)
    {
        // read a exist file, no need to initialize
        return;
    }

    // initialize the file
    d->dataDesc.startTime = timeManager.getStartTime();

    d->dataDesc.moduleConfig = systemManager.getModuleConfig();

    // device id;
    QString tmpStr = QString();
    systemConfig.getStrValue("General|DeviceID", tmpStr);
    if (tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->dataDesc.deviceID, tmpStr.toLocal8Bit().constData(),
                  sizeof(d->dataDesc.deviceID));

    // serial number
    machineConfig.getStrValue("SerialNumber",  tmpStr);
    if (tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->dataDesc.serialNum, tmpStr.toLocal8Bit().constData(),
                  sizeof(d->dataDesc.serialNum));

    // other field will initialize when write date to flash
    d->firstSave = true;
}

