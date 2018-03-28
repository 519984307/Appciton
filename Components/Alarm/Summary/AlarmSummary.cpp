#include "Alarm.h"
#include "TimeDate.h"
#include "WaveformCache.h"
#include "AlarmSummary.h"
#include "SystemManager.h"
#include "ParamManager.h"
#include "TimeManager.h"
#include "DataStorageDirManager.h"
#include "SummaryStorageManager.h"
#include "RescueDataDefine.h"
#include "ECGParam.h"

/***************************************************************************************************
 * create : create an instance
 **************************************************************************************************/
OneShotAlarmSummaryItem *OneShotAlarmSummaryItem::create(unsigned alarmTime)
{
    SummaryCheckPatient *item = new SummaryCheckPatient();
    item->itemDesc.time = alarmTime;
    item->itemDesc.type = SUMMARY_CHECK_PATIENT_ALARM;
    OneShotAlarmSummaryItem *summaryItem = new OneShotAlarmSummaryItem(item);
    summaryItem->startWaveCache();
    return summaryItem;
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
OneShotAlarmSummaryItem::OneShotAlarmSummaryItem(SummaryCheckPatient *summarydata)
    :CommonSummaryItem((char*) summarydata), _summary(summarydata)
{
}

/***************************************************************************************************
 * isCompleted : check whether the summary is finish collecting data
 **************************************************************************************************/
bool PhyAlarmSummaryItem::isCompleted() const
{
    bool ecgWaveComplete = CommonSummaryItem::isCompleted();
    if (_summary->waveID == (char) WAVE_NONE)
    {
        return ecgWaveComplete;
    }
    else
    {
        bool otherWaveComplete = waveformCache.isSyncCacheCompleted((WaveformID) _summary->waveID,
                                                                    (long)_summary);
        //time exceed more than 1 seconds, force complete
        if(!otherWaveComplete && timeManager.getCurTime() - _summary->itemDesc.time > SYNC_CACHE_TIME + 1)
        {
            otherWaveComplete = true;
        }

        return otherWaveComplete && ecgWaveComplete;
    }

}


/***************************************************************************************************
 * getCacheWaveLength : get current wave cache length
 **************************************************************************************************/
int PhyAlarmSummaryItem::getCacheWaveLength(WaveformID id) const
{
    if (id == (WaveformID) _summary->commonInfo.ECGLeadName)
    {
        return _summary->commonInfo.waveNum;
    }
    else if( id == (WaveformID) _summary->waveID)
    {
        return _summary->waveNum;
    }

    return 0;
}

/***************************************************************************************************
 * create : create an instance
 **************************************************************************************************/
PhyAlarmSummaryItem *PhyAlarmSummaryItem::create(unsigned alarmTime, ParamID paramID, int alarmType, bool oneshot, WaveformID id)
{
    SummaryPhyAlarm *item = new SummaryPhyAlarm();
    item->itemDesc.time = alarmTime;
    item->itemDesc.type = SUMMARY_PHY_ALARM;
    item->waveID  = id > WAVE_ECG_V6 ? id : WAVE_NONE;
    item->curAlarmInfo.paramid = paramID;
    item->curAlarmInfo.isOneshot = oneshot;
    item->curAlarmInfo.alarmType = alarmType;

    PhyAlarmSummaryItem *summaryItem = new PhyAlarmSummaryItem(item);
    summaryItem->startWaveCache();
    return summaryItem;
}


/***************************************************************************************************
 * phyCacheCallback : callback to update the wave wave num
 **************************************************************************************************/
static void phyCacheCallback (WaveformID id, long cacheid, int cachelen)
{
    SummaryPhyAlarm *item = reinterpret_cast<SummaryPhyAlarm *> (cacheid);
    int rate = waveformCache.getSampleRate(id);
    WaveformID calcLeadWaveID = ecgParam.leadToWaveID((ECGLead)(item->commonInfo.ECGLeadName));

    if (id == item->waveID)
    {
        item->waveNum = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate
                + cachelen;
    }
    else if(id == calcLeadWaveID)
    {
        item->commonInfo.waveNum = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate
                + cachelen;
    }
}

/***************************************************************************************************
 * startWaveCache : start wave cache
 **************************************************************************************************/
void PhyAlarmSummaryItem::startWaveCache()
{
    CommonSummaryItem::startWaveCache();
    if(_summary->waveID != (char) WAVE_NONE)
    {
        int rate = waveformCache.getSampleRate((WaveformID) _summary->waveID);
        waveformCache.readStorageChannel((WaveformID) _summary->waveID, _summary->waveBuf,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary->waveNum = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary->waveID, (long) _summary,
                _summary->waveBuf + _summary->waveNum, SYNC_CACHE_TIME * rate, phyCacheCallback);
    }

}

/***************************************************************************************************
 * stopWaveCache : stop wave cache
 **************************************************************************************************/
void PhyAlarmSummaryItem::stopWaveCache()
{
    CommonSummaryItem::stopWaveCache();
    if(_summary->waveID != (char) WAVE_NONE)
    {
        int rate = waveformCache.getSampleRate((WaveformID) _summary->waveID);
        waveformCache.unRegisterSyncCache((WaveformID) _summary->waveID, (long) _summary);
        if(DATA_STORAGE_WAVE_TIME * rate > _summary->waveNum)
        {
            WaveDataType *start =  _summary->waveBuf + _summary->waveNum;
            WaveDataType *end = _summary->waveBuf + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary->waveNum = DATA_STORAGE_WAVE_TIME * rate;
        }
   }
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
PhyAlarmSummaryItem::PhyAlarmSummaryItem(SummaryPhyAlarm *summaryData)
    :CommonSummaryItem((char *) summaryData), _summary(summaryData)
{
    memset(&_summary->curActiveAlarmInfos, 0, sizeof(_summary->curActiveAlarmInfos));
    _summary->waveGain = summaryStorageManager.getWaveGain((WaveformID)_summary->waveID);

    //其它active参数ID

    QList<Alarm::AlarmInfo> almInfoList = alertor.getCurrentPhyAlarmInfo();
    int size = sizeof(_summary->curActiveAlarmInfos) / sizeof(_summary->curActiveAlarmInfos[0]);
    int i = 0;

    while(almInfoList.size())
    {
        Alarm::AlarmInfo alminfo = almInfoList.takeLast();
        if(alminfo.paramid == _summary->curAlarmInfo.paramid
                && alminfo.alarmType == _summary->curAlarmInfo.alarmType
                && alminfo.isOneshot == _summary->curAlarmInfo.isOneshot)
        {
            continue;
        }

        _summary->curActiveAlarmInfos[i].paramid = alminfo.paramid;
        _summary->curActiveAlarmInfos[i].isOneshot = alminfo.isOneshot;
        _summary->curActiveAlarmInfos[i].alarmType = alminfo.alarmType;
        i++;
        if(i>=size)
        {
            qdebug("Too much physical alarm info.\n");
            break;
        }
    }

    _summary->curActiveAlarmNum = i;
}
