#include "ECGSummary.h"
#include "DataStorageDirManager.h"
#include "TimeManager.h"
#include "SummaryStorageManager.h"
#include "TimeDate.h"
#include "WaveformCache.h"
#include "ECGParam.h"

/***************************************************************************************************
 * create : create an instance
 **************************************************************************************************/
PresentingRhythmSummaryItem *PresentingRhythmSummaryItem::create(unsigned time)
{
    SummaryPresentRhythm * item = new SummaryPresentRhythm();
    item->itemDesc.time = time;
    item->itemDesc.type = SUMMARY_ECG_RHYTHM;
    PresentingRhythmSummaryItem *summaryItem = new PresentingRhythmSummaryItem(item);
    summaryItem->startWaveCache();
    return summaryItem;
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
PresentingRhythmSummaryItem::PresentingRhythmSummaryItem(SummaryPresentRhythm *summarydata)
    :CommonSummaryItem((char *) summarydata), _summary(summarydata)
{

}


static void cacheWaveCallback(WaveformID waveid, long cacheid, int cachelen)
{
    Q_UNUSED(cachelen)
    SummaryDiagnosticECG *summary = reinterpret_cast<SummaryDiagnosticECG*>(cacheid);
    if(waveid == summary->commonInfo.ECGLeadName)
    {
        summary->commonInfo.waveNum += 1;
    }
}

/***************************************************************************************************
 * Diagnostic ecg wave cache, store 6 seconds of primary ecg data before the snapshot and 12 seconds
 * of diagnostic ecg data
 **************************************************************************************************/
void DiagnosticEcgSummaryItem::startWaveCache()
{
    WaveformID waveid = (WaveformID)_summary->commonInfo.ECGLeadName;
    int rate = waveformCache.getSampleRate((WaveformID) _summary->commonInfo.ECGLeadName);
    _summary->commonInfo.waveNum = 6 * rate;
    memset(_summary->commonInfo.ECGWave, 0, sizeof(_summary->commonInfo.ECGWave));
    //store 6 seconds of the primary ecg data
    waveformCache.readStorageChannel(waveid, _summary->commonInfo.ECGWave,
                              6, false);

    int baseLine = 0;
    waveformCache.getBaseline(waveid, baseLine);
    int *buff = _summary->commonInfo.ECGWave;
    buff += 6 * rate;
    //store 12 seconds of the diagnostic ecg data
    waveformCache.registerSyncCache(waveid, (long)_summary,
                                    buff, SYNC_CACHE_TIME * rate, cacheWaveCallback);
}

void DiagnosticEcgSummaryItem::stopWaveCache()
{
    WaveformID waveid = (WaveformID)_summary->commonInfo.ECGLeadName;
    int rate = waveformCache.getSampleRate(waveid);
    waveformCache.unRegisterSyncCache(waveid, (long)_summary);
    //cache length not match, fill the end with invalid data
    if (DATA_STORAGE_WAVE_TIME * rate > _summary->commonInfo.waveNum)
    {
        WaveDataType *start =  _summary->commonInfo.ECGWave + _summary->commonInfo.waveNum;
        WaveDataType *end = _summary->commonInfo.ECGWave + DATA_STORAGE_WAVE_TIME * rate;
        qFill(start, end, 0x40000000);
        _summary->commonInfo.waveNum = DATA_STORAGE_WAVE_TIME * rate;
    }
}

/***************************************************************************************************
 * check complete or not
 **************************************************************************************************/
bool DiagnosticEcgSummaryItem::isCompleted() const
{
    bool cacheComplete = waveformCache.isSyncCacheCompleted((WaveformID) _summary->commonInfo.ECGLeadName, (long)_summary);
    if (cacheComplete)
    {
        return true;
    }

    //exceed time range, force complete;
    if(timeManager.getCurTime() - _summary->itemDesc.time > SYNC_CACHE_TIME)
    {
        return true;
    }

    return false;
}

/***************************************************************************************************
 * create : create a instance
 **************************************************************************************************/
DiagnosticEcgSummaryItem *DiagnosticEcgSummaryItem::create(unsigned time, ECGBandwidth diagBandwidth)
{
    SummaryDiagnosticECG *item = new SummaryDiagnosticECG();
    item->itemDesc.time = time;
    item->itemDesc.type = SUMMARY_DIAG_ECG;
    item->diagBandWidth = diagBandwidth;
    DiagnosticEcgSummaryItem *summaryItem = new DiagnosticEcgSummaryItem(item);

    if(time - ecgParam.lastDiagModeTimestamp() < 6)
    {
        // a diag mode was performed less than 6 seconds ago, the case 6 seconds wave data was multiple bandwidth
        item->commonInfo.ECGBandwidth = ECG_BANDWIDTH_MULTIPLE;
    }

    summaryItem->startWaveCache();
    return summaryItem;
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
DiagnosticEcgSummaryItem::DiagnosticEcgSummaryItem(SummaryDiagnosticECG *summarydata)
    :CommonSummaryItem((char *) summarydata), _summary(summarydata)
{

}
