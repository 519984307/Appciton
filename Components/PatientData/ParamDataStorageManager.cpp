/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/26
 **/



#include "../Utility/DataStorage/StorageManager_p.h"
#include "ParamDataStorageManager.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "TimeManager.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "TrendCache.h"
#include "ParamManager.h"
#include "Alarm.h"
#include "PatientManager.h"
#include "Framework/Utility/Utility.h"
#include "RESPAlarm.h"
#include "CO2Alarm.h"
#include "ECGDupParam.h"
#include "RESPDupParam.h"
#include "CO2Param.h"
#include "TimeDate.h"
#include <QDateTime>
#include <SystemManager.h>
#include "AlarmSourceManager.h"

#define NIBP_MAX_PENDING_TIME 3

class ParamDataStorageManagerPrivate: public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(ParamDataStorageManager)

    explicit ParamDataStorageManagerPrivate(ParamDataStorageManager *q_ptr)
        : StorageManagerPrivate(q_ptr),
          forceSave(true),
          lastAlarmTimestamp(0),
          nibpPendingCounter(NIBP_MAX_PENDING_TIME)
    {}

    void updateAdditionInfo();
    void addData(ParamDataStorageManager::ParamBuf &paramBuf);  /* NOLINT */
    ParamDataDescription dataDesc;  // 数据描述
    ParamDataStorageManager::ParamBuf lastParamBuf;
    bool forceSave;
    QMutex paramBufMutex;
    unsigned lastAlarmTimestamp;  // recored the timestamp of last alarm happens

    /*
     *  When a NIBP measurement happened, it will store after alarm check (3 seconds delay).
     *  If NIBP trigger alarm, update the alarm info in previous nibp record instead of
     *  storing this alarm record. If the NIBP stored timestamp can be divided by 30 seconds,
     *  we don't store the 30 seconds interval record
     */

    QList<ParamDataStorageManager::ParamBuf *> nibpPendingList;
    int nibpPendingCounter;
    void flushNibpPendingList();
};

/**************************************************************************************************
 * 更新附加信息
 *************************************************************************************************/
void ParamDataStorageManagerPrivate::updateAdditionInfo()
{
    // starttime,deviceid,serialNum保持不变

    bool changed = false;
    if (memcmp(dataDesc.patientName, patientManager.getName(), MAX_PATIENT_NAME_LEN))
    {
        Util::strlcpy(dataDesc.patientName, patientManager.getName(), MAX_PATIENT_NAME_LEN);
        changed = true;
    }
    if (memcmp(dataDesc.PatientID, patientManager.getPatID(), MAX_PATIENT_ID_LEN))
    {
        Util::strlcpy(dataDesc.PatientID, patientManager.getPatID(), MAX_PATIENT_ID_LEN);
        changed = true;
    }

    if (!changed && !forceSave)
    {
        return;
    }

    // fdfileName

    dataDesc.checkSum = dataDesc.sum();
    backend->writeAdditionalData(reinterpret_cast<char *>(&dataDesc), sizeof(dataDesc));
    forceSave = false;
}

/**************************************************************************************************
 * 添加数据项
 *************************************************************************************************/
void ParamDataStorageManagerPrivate::addData(ParamDataStorageManager::ParamBuf &paramBuf)
{
    Q_Q(ParamDataStorageManager);

    unsigned int dataType = 0;
    if (paramBuf.item.isAlarm)
    {
        dataType |= TREND_FLAG_ALARM;
    }
    else if (paramBuf.item.isNibpShowMeasureValue)
    {
        dataType |= TREND_FLAG_NIBP;
    }
    else
    {
        dataType |= TREND_FLAG_NORMAL;
    }
    q->addData(dataType, reinterpret_cast<char *>(&paramBuf), sizeof(ParamDataStorageManager::ParamBuf),
               paramBuf.item.t);

    paramBufMutex.lock();
    memcpy(reinterpret_cast<char *>(&lastParamBuf), reinterpret_cast<char *>(&paramBuf),
           sizeof(ParamDataStorageManager::ParamBuf));
    paramBufMutex.unlock();
}

// flush the nibpPendingList
void ParamDataStorageManagerPrivate::flushNibpPendingList()
{
    while (nibpPendingList.count())
    {
        ParamDataStorageManager::ParamBuf *paramBuf = nibpPendingList.takeFirst();
        addData(*paramBuf);
        delete paramBuf;
    }
    nibpPendingCounter = NIBP_MAX_PENDING_TIME;
}

ParamDataStorageManager &ParamDataStorageManager::construction()
{
    static ParamDataStorageManager *_intance = NULL;

    if (_intance == NULL)
    {
        _intance =  new ParamDataStorageManager();
    }

    return *_intance;
}


ParamDataStorageManager::ParamDataStorageManager()
    : StorageManager(new ParamDataStorageManagerPrivate(this), new StorageFile())
{
    createDir();
}

/**************************************************************************************************
 * 新建文件夹
 *************************************************************************************************/
void ParamDataStorageManager::createDir()
{
    Q_D(ParamDataStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + PARAM_DATA_FILE_NAME,
                       QIODevice::ReadWrite);
    d->backend->setReservedSize(sizeof(ParamDataDescription));
    d->backend->readAdditionalData(reinterpret_cast<char *>(&d->dataDesc), sizeof(ParamDataDescription));
    if (d->dataDesc.checkSum == d->dataDesc.sum())
    {
        return;
    }

    memset(&d->dataDesc, 0, sizeof(ParamDataDescription));
    d->dataDesc.startTime = timeManager.getStartTime();

    QString tmpStr;
    currentConfig.getStrValue("General|DeviceID", tmpStr);
    if (tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->dataDesc.deviceID, tmpStr.toLocal8Bit().constData(),
                  sizeof(d->dataDesc.deviceID));

    // serial
    machineConfig.getStrValue("SerialNumber", tmpStr);
    if (tmpStr.isNull())
    {
        tmpStr = QString();
    }
    Util::strlcpy(d->dataDesc.serialNum, tmpStr.toLocal8Bit().constData(),
                  sizeof(d->dataDesc.serialNum));

    // incident id number
    QDateTime dt = QDateTime::fromTime_t(d->dataDesc.startTime);
    QString idStr = QString("ZE%1_%2_%3").arg(tmpStr).arg(dt.toString("yyyyMMdd")).arg(dt.toString("hhmmss"));
    Util::strlcpy(d->dataDesc.fdFileName, qPrintable(idStr), sizeof(d->dataDesc.fdFileName));


    // update module config
    d->dataDesc.moduleConfig = systemManager.getModuleConfig();
}

ParamDataStorageManager::~ParamDataStorageManager()
{
}

/**************************************************************************************************
 * 主运行函数,存储趋势数据
 * 参数:
 *     t:时间戳
 *     data:趋势数据
 *************************************************************************************************/
void ParamDataStorageManager::mainRun(unsigned t)
{
    Q_D(ParamDataStorageManager);


    if (!d->nibpPendingList.isEmpty())
    {
        ParamBuf *nibpParamBuf = d->nibpPendingList.first();
        if (nibpParamBuf->item.t == t)
        {
            return;
        }
        if ((d->nibpPendingCounter--) == 0)
        {
            d->flushNibpPendingList();
        }
    }

    if (dataStorageDirManager.isCurRescueFolderFull())
    {
        return;
    }

    // store data every 30 seconds
    if (0 != timeDate.getTimeSenonds(t) % 30)
    {
        return;
    }

    if (d->lastAlarmTimestamp == t)
    {
        // if alarm happens at the same time, skip this storage. Only store one recored for the same second
        return;
    }

    TrendCacheData data;
    if (!trendCache.getTrendData(t, data))
    {
        // no trend data, return
        return;
    }

    ParamBuf *paramBuf = new ParamBuf();
    paramBuf->item.t = t;
    paramBuf->item.co2Baro = data.co2baro;
    AlarmOneShotIFace *respAlarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
    AlarmOneShotIFace *co2AlarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO2);
    paramBuf->item.isApneaAlarm = alertor.getOneShotAlarmStatus(respAlarmSource, RESP_ONESHOT_ALARM_APNEA)
                                  || alertor.getOneShotAlarmStatus(co2AlarmSource, CO2_ONESHOT_ALARM_APNEA);
    paramBuf->item.hrSource = ecgDupParam.getCurHRSource();
    paramBuf->item.brSource = respDupParam.getBrSource();
    paramBuf->item.isFico2Display = co2Param.getFICO2Display();
    paramBuf->item.dataLen = SUB_PARAM_NR * sizeof(ParamItem);
    paramBuf->item.makeCheckSum();
    QList<ParamID> idList;
    paramManager.getParams(idList);

    ParamID paramID = PARAM_NONE;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        paramID = paramInfo.getParamID((SubParamID)i);

        if (-1 == idList.indexOf(paramID))
        {
            continue;
        }

        paramBuf->paramItem[i].paramID = i;
        paramBuf->paramItem[i].value = data.values.value((SubParamID)i, InvData());
        paramBuf->paramItem[i].alarmFlag =
            alertor.getAlarmSourceStatus(paramInfo.getParamName(paramID), (SubParamID)i);
        paramBuf->item.checkSum += paramBuf->paramItem[i].calcSum();
    }

    if (d->nibpPendingList.isEmpty())
    {
        d->addData(*paramBuf);
        delete paramBuf;
    }
    else
    {
        d->nibpPendingList.append(paramBuf);
    }
}


/**************************************************************************************************
 * NIBP测量完成时添加趋势数据
 *************************************************************************************************/
void ParamDataStorageManager::addNIBPData(unsigned t)
{
    Q_D(ParamDataStorageManager);
    if (dataStorageDirManager.isCurRescueFolderFull())
    {
        return;
    }

    TrendCacheData data;
    if (!trendCache.getTrendData(t, data))
    {
        // no trend data, return
        return;
    }
    if (t != data.lastNibpMeasureSuccessTime)
    {
        return;
    }

    if (!d->nibpPendingList.isEmpty())
    {
        // bug, nibp measurement happend too fast
        qWarning() << Q_FUNC_INFO << "Nibp measurement happens to fast, result abandoned.";
        return;
    }

    // nibp measurement record need to pending for 3 seconds
    ParamBuf *paramBuf = new ParamBuf();
    paramBuf->item.t = t;
    paramBuf->item.co2Baro = data.co2baro;
    AlarmOneShotIFace *respAlarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
    AlarmOneShotIFace *co2AlarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO2);
    paramBuf->item.isApneaAlarm = alertor.getOneShotAlarmStatus(respAlarmSource, RESP_ONESHOT_ALARM_APNEA)
                                  || alertor.getOneShotAlarmStatus(co2AlarmSource, CO2_ONESHOT_ALARM_APNEA);
    paramBuf->item.hrSource = ecgDupParam.getCurHRSource();
    paramBuf->item.brSource = respDupParam.getBrSource();
    paramBuf->item.isFico2Display = co2Param.getFICO2Display();
    paramBuf->item.isNibpShowMeasureValue = 1;
    paramBuf->item.dataLen = SUB_PARAM_NR * sizeof(ParamItem);
    paramBuf->item.makeCheckSum();

    ParamID paramID = PARAM_NONE;
    QList<ParamID> idList;
    paramManager.getParams(idList);

    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        paramID = paramInfo.getParamID((SubParamID)i);

        if (-1 == idList.indexOf(paramID))
        {
            continue;
        }

        paramBuf->paramItem[i].paramID = i;
        paramBuf->paramItem[i].value = data.values.value((SubParamID) i, InvData());
        paramBuf->paramItem[i].alarmFlag =
            alertor.getAlarmSourceStatus(paramInfo.getParamName(paramID), (SubParamID)i);
        paramBuf->item.checkSum += paramBuf->paramItem[i].calcSum();
    }

    d->nibpPendingList.append(paramBuf);
}

/**************************************************************************************************
 * 参数超限报警时添加趋势数据
 *************************************************************************************************/
void ParamDataStorageManager::addAlarmData(unsigned t, ParamID id)
{
    Q_D(ParamDataStorageManager);
    if (dataStorageDirManager.isCurRescueFolderFull())
    {
        return;
    }

    if (d->nibpPendingList.isEmpty() && d->lastAlarmTimestamp == t)
    {
        // no exist NIBP alarm, the alarm timestamp is equal to last alarm, just return, because all the
        // alarm info has been collected.
        return;
    }

    d->lastAlarmTimestamp = t;

    TrendCacheData data;
    if (!trendCache.getTrendData(t, data))
    {
        // no trend data, return
        return;
    }

    bool mergeFlag = false;
    ParamBuf *paramBuf;
    if (!d->nibpPendingList.isEmpty())
    {
        ParamBuf *nibpParamBuf = d->nibpPendingList.first();
        if (nibpParamBuf->item.t == t)
        {
            /*
             * new alarm arrived and nibp measurement happend in the same time
             * store data in the previous nibp record
             */
            paramBuf = nibpParamBuf;
            mergeFlag = true;
        }
        else if (id == PARAM_NIBP)
        {
            // the nibp measurement finally trigger a alarm
            // get the alarm status and return;
            nibpParamBuf->item.makeCheckSum();
            ParamID paramID = PARAM_NONE;
            QList<ParamID> idList;
            paramManager.getParams(idList);

            for (int i = 0; i < SUB_PARAM_NR; ++i)
            {
                paramID = paramInfo.getParamID((SubParamID)i);

                if (-1 == idList.indexOf(paramID))
                {
                    continue;
                }

                if (i == SUB_PARAM_NIBP_DIA || i == SUB_PARAM_NIBP_MAP || i == SUB_PARAM_NIBP_SYS)
                {
                    nibpParamBuf->paramItem[i].alarmFlag =
                        alertor.getAlarmSourceStatus(paramInfo.getParamName(paramID), (SubParamID)i);
                }
                nibpParamBuf->item.checkSum += nibpParamBuf->paramItem[i].calcSum();
            }

            d->flushNibpPendingList();
            return;
        }
        else
        {
            paramBuf = new ParamBuf();
        }
    }
    else
    {
        paramBuf = new ParamBuf();
        if (id == PARAM_NIBP)
        {
            // at this case, the alarm is created after user change the nibp alarm limit or power on during 30 second,
            // the NIBP value of previous power cycle is store in the config file
            paramBuf->item.isNibpShowMeasureValue = 1;
        }
    }

    paramBuf->item.t = t;
    paramBuf->item.co2Baro = data.co2baro;
    AlarmOneShotIFace *respAlarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
    AlarmOneShotIFace *co2AlarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO2);
    paramBuf->item.isApneaAlarm = alertor.getOneShotAlarmStatus(respAlarmSource, RESP_ONESHOT_ALARM_APNEA)
                                  || alertor.getOneShotAlarmStatus(co2AlarmSource, CO2_ONESHOT_ALARM_APNEA);
    paramBuf->item.hrSource = ecgDupParam.getCurHRSource();
    paramBuf->item.brSource = respDupParam.getBrSource();
    paramBuf->item.isFico2Display = co2Param.getFICO2Display();
    paramBuf->item.isAlarm = 1;
    paramBuf->item.dataLen = SUB_PARAM_NR * sizeof(ParamItem);
    paramBuf->item.makeCheckSum();

    ParamID paramID = PARAM_NONE;
    QList<ParamID> idList;
    paramManager.getParams(idList);

    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        paramID = paramInfo.getParamID((SubParamID)i);

        if (-1 == idList.indexOf(paramID))
        {
            continue;
        }

        paramBuf->paramItem[i].paramID = i;
        paramBuf->paramItem[i].value = data.values.value((SubParamID)i, InvData());
        paramBuf->paramItem[i].alarmFlag =
            alertor.getAlarmSourceStatus(paramInfo.getParamName(paramID), (SubParamID)i);
        paramBuf->item.checkSum += paramBuf->paramItem[i].calcSum();
    }

    if (d->nibpPendingList.isEmpty())
    {
        d->addData(*paramBuf);
        delete paramBuf;
    }
    else
    {
        if (!mergeFlag)
        {
            d->nibpPendingList.append(paramBuf);
        }
    }
}

/**************************************************************************************************
 * 存储
 *************************************************************************************************/
void ParamDataStorageManager::run()
{
    Q_D(ParamDataStorageManager);

    d->updateAdditionInfo();

    if (dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        return;
    }

    dataStorageDirManager.addDataSize(saveData());
}

/***************************************************************************************************
 * getRecentParamData : get the last store param data, this data might haven't been stored  in the
 *                      storagebackend
 **************************************************************************************************/
void ParamDataStorageManager::getRecentParamData(ParamDataStorageManager::ParamBuf &parambuf)
{
    Q_D(ParamDataStorageManager);
    QMutexLocker locker(&d->paramBufMutex);
    memcpy(reinterpret_cast<char *>(&parambuf), reinterpret_cast<char *>(&d->lastParamBuf), sizeof(ParamBuf));
}

void ParamDataStorageManager::newPatientHandle()
{
    trendCache.clearTrendCache();
    trendCache.stopDataCollect(1);
}
