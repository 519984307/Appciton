#include "SummaryItem.h"
#include "WaveformCache.h"
#include "SummaryStorageManager.h"
#include "TimeManager.h"
#include "ECGParam.h"


/***************************************************************************************************
 * constructor
 **************************************************************************************************/
CommonSummaryItem::CommonSummaryItem(char *summaryData)
    :_data(summaryData)
{
    SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *> (_data + sizeof(SummaryItemDesc));
    summaryStorageManager.getSummaryCommonInfo(*commonInfo);
}

/***************************************************************************************************
 * destructor
 **************************************************************************************************/
CommonSummaryItem::~CommonSummaryItem()
{
}

/***************************************************************************************************
 * getCacheWaveLength : get the wave length has been cache
 **************************************************************************************************/
int CommonSummaryItem::getCacheWaveLength(WaveformID id) const
{
    SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *> (_data + sizeof(SummaryItemDesc));
    if(id == (WaveformID) commonInfo->ECGLeadName)
    {
        return commonInfo->waveNum;
    }
    return 0;
}

/***************************************************************************************************
 * isCompleted : check whether the summary is complete
 **************************************************************************************************/
bool CommonSummaryItem::isCompleted() const
{
    SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *> (_data + sizeof(SummaryItemDesc));
    WaveformID calcLeadWaveID = ecgParam.leadToWaveID((ECGLead)commonInfo->ECGLeadName);
    bool cacheComplete = waveformCache.isSyncCacheCompleted(calcLeadWaveID, (long)_data);
    if (cacheComplete)
    {
        return true;
    }

    SummaryItemDesc *itemDesc = reinterpret_cast<SummaryItemDesc *>(_data);
    //exceed time range, force complete;
    if(timeManager.getCurTime() - itemDesc->time > SYNC_CACHE_TIME + 1)
    {
        return true;
    }
    return false;
}

/***************************************************************************************************
 * time : get the time summary happened
 **************************************************************************************************/
unsigned CommonSummaryItem::time() const
{
    SummaryItemDesc *desc = reinterpret_cast<SummaryItemDesc *> (_data);
    return desc->time;
}

/***************************************************************************************************
 * type : get the type of the summary
 **************************************************************************************************/
SummaryType CommonSummaryItem::type() const
{
    SummaryItemDesc *desc = reinterpret_cast<SummaryItemDesc *> (_data);
    return (SummaryType)desc->type;
}

/***************************************************************************************************
 * getSummaryData : get the summary data pointer
 **************************************************************************************************/
char *CommonSummaryItem::getSummaryData() const
{
    return _data;
}

/***************************************************************************************************
 * commonCacheCallback : call back function to update wave num
 **************************************************************************************************/
static void commonCacheCallback (WaveformID id, long cacheid, int cachelen)
{
    SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *> (cacheid + sizeof(SummaryItemDesc));
    WaveformID calcLeadWaveID = ecgParam.leadToWaveID((ECGLead)commonInfo->ECGLeadName);

    int rate = waveformCache.getSampleRate(id);
    if(id == calcLeadWaveID)
    {
        commonInfo->waveNum = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate
                + cachelen;
    }
}

/***************************************************************************************************
 * startWaveCache : start wave cache, read 6 seconds cache data from wave cache and start 12 seconds
 *                  sync cache.
 **************************************************************************************************/
void CommonSummaryItem::startWaveCache()
{
    SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *> (_data + sizeof(SummaryItemDesc));
    WaveformID calcLeadWaveID = ecgParam.leadToWaveID((ECGLead)commonInfo->ECGLeadName);

    int rate = waveformCache.getSampleRate(calcLeadWaveID);
    commonInfo->waveNum = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
    waveformCache.readStorageChannel(calcLeadWaveID, commonInfo->ECGWave,
            DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);
    waveformCache.registerSyncCache(calcLeadWaveID, (long)_data,
                                    commonInfo->ECGWave + commonInfo->waveNum, SYNC_CACHE_TIME * rate, commonCacheCallback);
}

/***************************************************************************************************
 * stopWaveCache : stop the wave Cache
 **************************************************************************************************/
void CommonSummaryItem::stopWaveCache()
{
    SummaryCommonInfo *commonInfo = reinterpret_cast<SummaryCommonInfo *> (_data + sizeof(SummaryItemDesc));
    WaveformID calcLeadWaveID = ecgParam.leadToWaveID((ECGLead)commonInfo->ECGLeadName);

    int rate = waveformCache.getSampleRate(calcLeadWaveID);
    waveformCache.unRegisterSyncCache(calcLeadWaveID, (long)_data);
    //cache length not match, fill the end with invalid data
    if (DATA_STORAGE_WAVE_TIME * rate > commonInfo->waveNum)
    {
        WaveDataType *start =  commonInfo->ECGWave + commonInfo->waveNum;
        WaveDataType *end = commonInfo->ECGWave + DATA_STORAGE_WAVE_TIME * rate;
        qFill(start, end, 0x40000000);
        commonInfo->waveNum = DATA_STORAGE_WAVE_TIME * rate;
    }

}

void SummaryItem::increaseRef()
{
    ref.ref();
}

void SummaryItem::decreaseRef()
{
    if(!ref.deref())
    {
        //no more refrence
        delete this;
    }
}
