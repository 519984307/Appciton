#include "PrinterActionSummary.h"
#include "DataStorageDirManager.h"
#include "WindowManager.h"
#include "WaveformCache.h"
#include "IConfig.h"
#include "ECGParam.h"
#include "TimeManager.h"
#include "PrintManager.h"

/***************************************************************************************************
 * create : create a instance
 **************************************************************************************************/
PrinterActionSummaryItem *PrinterActionSummaryItem::create(unsigned time)
{
    QList<int> waveID;
    UserFaceType type = windowManager.getUFaceType();
    if(UFACE_MONITOR_12LEAD == type)
    {
        //print 3 trace in 12 lead mode
        int lead = ECG_LEAD_I;
        currentConfig.getNumValue("ECG12L|RealtimePrintTopLead", lead);
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        currentConfig.getNumValue("ECG12L|RealtimePrintMiddleLead", lead);
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
        currentConfig.getNumValue("ECG12L|RealtimePrintBottomLead", lead);
        waveID.append((WaveformID)ecgParam.leadToWaveID((ECGLead)lead));
    }
    else
    {
        windowManager.getDisplayedWaveform(waveID);
        int printTraceNum = printManager.getPrintWaveNum();
        while(waveID.size() > printTraceNum && !waveID.isEmpty())
        {
            waveID.removeLast();
        }
    }

    PrinterActionSummaryItem *summaryItem = NULL;

    if(waveID.length() == 4)
    {
        SummaryPrinterAction4Trace *item = new SummaryPrinterAction4Trace();
        item->itemDesc.time = time;
        item->itemDesc.type = SUMMARY_PRINTER_ACTION_4_TRACE;
        item->waveID1 = waveID.at(1);
        item->waveID2 = waveID.at(2);
        item->waveID3 = waveID.at(3);
        summaryItem = new PrinterActionSummaryItem(item);
    }
    else if(waveID.length() == 3)
    {
        SummaryPrinterAction3Trace *item = new SummaryPrinterAction3Trace();
        item->itemDesc.time = time;
        item->itemDesc.type = SUMMARY_PRINTER_ACTION_3_TRACE;
        item->waveID1 = waveID.at(1);
        item->waveID2 = waveID.at(2);
        summaryItem = new PrinterActionSummaryItem(item);
    }
    else if(waveID.length() == 2)
    {
        SummaryPrinterAction2Trace *item = new SummaryPrinterAction2Trace();
        item->itemDesc.time = time;
        item->itemDesc.type = SUMMARY_PRINTER_ACTION_2_TRACE;
        item->waveID1 = waveID.at(1);
        summaryItem = new PrinterActionSummaryItem(item);
    }
    else
    {
        SummaryPrinterAction *item = new SummaryPrinterAction();
        item->itemDesc.time = time;
        item->itemDesc.type = SUMMARY_PRINTER_ACTION;
        summaryItem = new PrinterActionSummaryItem(item);
    }

    summaryItem->startWaveCache();
    return summaryItem;
}

static void cacheCallback(WaveformID id, long cacheid, int cachelen)
{
    Q_UNUSED(id);
    Q_UNUSED(cachelen);
    int *waveNum = (int *)cacheid;
    *waveNum += 1;
}

/***************************************************************************************************
 * start wave cache
 **************************************************************************************************/
void PrinterActionSummaryItem::startWaveCache()
{
    //claculate lead
    CommonSummaryItem::startWaveCache();

    if(_summary4Trace)
    {
        //second trace
        int rate = waveformCache.getSampleRate((WaveformID) _summary4Trace->waveID1);
        waveformCache.readStorageChannel((WaveformID) _summary4Trace->waveID1, _summary4Trace->waveBuf1,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary4Trace->waveNum1 = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary4Trace->waveID1, (long) (&_summary4Trace->waveNum1),
                _summary4Trace->waveBuf1 + _summary4Trace->waveNum1, SYNC_CACHE_TIME * rate, cacheCallback);

        //third trace
        rate = waveformCache.getSampleRate((WaveformID) _summary4Trace->waveID2);
        waveformCache.readStorageChannel((WaveformID) _summary4Trace->waveID2, _summary4Trace->waveBuf2,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary4Trace->waveNum2 = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary4Trace->waveID2, (long) (&_summary4Trace->waveNum2),
                _summary4Trace->waveBuf2 + _summary4Trace->waveNum2, SYNC_CACHE_TIME * rate, cacheCallback);

        //forth trace
        rate = waveformCache.getSampleRate((WaveformID) _summary4Trace->waveID3);
        waveformCache.readStorageChannel((WaveformID) _summary4Trace->waveID3, _summary4Trace->waveBuf3,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary4Trace->waveNum3 = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary4Trace->waveID3, (long) (&_summary4Trace->waveNum3),
                _summary4Trace->waveBuf3 + _summary4Trace->waveNum3, SYNC_CACHE_TIME * rate, cacheCallback);
    }
    else if(_summary3Trace)
    {
        //second trace
        int rate = waveformCache.getSampleRate((WaveformID) _summary3Trace->waveID1);
        waveformCache.readStorageChannel((WaveformID) _summary3Trace->waveID1, _summary3Trace->waveBuf1,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary3Trace->waveNum1 = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary3Trace->waveID1, (long) (&_summary3Trace->waveNum1),
                _summary3Trace->waveBuf1 + _summary3Trace->waveNum1, SYNC_CACHE_TIME * rate, cacheCallback);

        //third trace
        rate = waveformCache.getSampleRate((WaveformID) _summary3Trace->waveID2);
        waveformCache.readStorageChannel((WaveformID) _summary3Trace->waveID2, _summary3Trace->waveBuf2,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary3Trace->waveNum2 = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary3Trace->waveID2, (long) (&_summary3Trace->waveNum2),
                _summary3Trace->waveBuf2 + _summary3Trace->waveNum2, SYNC_CACHE_TIME * rate, cacheCallback);
    }
    else if(_summary2Trace)
    {
        //second trace
        int rate = waveformCache.getSampleRate((WaveformID) _summary2Trace->waveID1);
        waveformCache.readStorageChannel((WaveformID) _summary2Trace->waveID1, _summary2Trace->waveBuf1,
                DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME, false);

        _summary2Trace->waveNum1 = (DATA_STORAGE_WAVE_TIME - SYNC_CACHE_TIME) * rate;
        waveformCache.registerSyncCache((WaveformID) _summary2Trace->waveID1, (long) (&_summary2Trace->waveNum1),
                _summary2Trace->waveBuf1 + _summary2Trace->waveNum1, SYNC_CACHE_TIME * rate, cacheCallback);
    }

}

/***************************************************************************************************
 * stop wave cache
 **************************************************************************************************/
void PrinterActionSummaryItem::stopWaveCache()
{
    CommonSummaryItem::stopWaveCache();
    if(_summary4Trace)
    {
        //second trace
        int rate = waveformCache.getSampleRate((WaveformID) _summary4Trace->waveID1);
        waveformCache.unRegisterSyncCache((WaveformID) _summary4Trace->waveID1, (long) (&_summary4Trace->waveNum1));
        if(DATA_STORAGE_WAVE_TIME * rate > _summary4Trace->waveNum1)
        {
            WaveDataType *start =  _summary4Trace->waveBuf1 + _summary4Trace->waveNum1;
            WaveDataType *end = _summary4Trace->waveBuf1 + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary4Trace->waveNum1 = DATA_STORAGE_WAVE_TIME * rate;
        }

        //third trace
        rate = waveformCache.getSampleRate((WaveformID) _summary4Trace->waveID2);
        waveformCache.unRegisterSyncCache((WaveformID) _summary4Trace->waveID2, (long) (&_summary4Trace->waveNum2));
        if(DATA_STORAGE_WAVE_TIME * rate > _summary4Trace->waveNum2)
        {
            WaveDataType *start =  _summary4Trace->waveBuf2 + _summary4Trace->waveNum2;
            WaveDataType *end = _summary4Trace->waveBuf2 + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary4Trace->waveNum2 = DATA_STORAGE_WAVE_TIME * rate;
        }

        //forth trace
        rate = waveformCache.getSampleRate((WaveformID) _summary4Trace->waveID3);
        waveformCache.unRegisterSyncCache((WaveformID) _summary4Trace->waveID3, (long) (&_summary4Trace->waveNum3));
        if(DATA_STORAGE_WAVE_TIME * rate > _summary4Trace->waveNum3)
        {
            WaveDataType *start =  _summary4Trace->waveBuf3 + _summary4Trace->waveNum3;
            WaveDataType *end = _summary4Trace->waveBuf3 + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary4Trace->waveNum3 = DATA_STORAGE_WAVE_TIME * rate;
        }
    }
    else if(_summary3Trace)
    {
        //second trace
        int rate = waveformCache.getSampleRate((WaveformID) _summary3Trace->waveID1);
        waveformCache.unRegisterSyncCache((WaveformID) _summary3Trace->waveID1, (long) (&_summary3Trace->waveNum1));
        if(DATA_STORAGE_WAVE_TIME * rate > _summary3Trace->waveNum1)
        {
            WaveDataType *start =  _summary3Trace->waveBuf1 + _summary3Trace->waveNum1;
            WaveDataType *end = _summary3Trace->waveBuf1 + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary3Trace->waveNum1 = DATA_STORAGE_WAVE_TIME * rate;
        }

        //third trace
        rate = waveformCache.getSampleRate((WaveformID) _summary3Trace->waveID2);
        waveformCache.unRegisterSyncCache((WaveformID) _summary3Trace->waveID2, (long) (&_summary3Trace->waveNum2));
        if(DATA_STORAGE_WAVE_TIME * rate > _summary3Trace->waveNum2)
        {
            WaveDataType *start =  _summary3Trace->waveBuf2 + _summary3Trace->waveNum2;
            WaveDataType *end = _summary3Trace->waveBuf2 + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary3Trace->waveNum2 = DATA_STORAGE_WAVE_TIME * rate;
        }
    }
    else if(_summary2Trace)
    {
        //second trace
        int rate = waveformCache.getSampleRate((WaveformID) _summary2Trace->waveID1);
        waveformCache.unRegisterSyncCache((WaveformID) _summary2Trace->waveID1, (long) (&_summary2Trace->waveNum1));
        if(DATA_STORAGE_WAVE_TIME * rate > _summary2Trace->waveNum1)
        {
            WaveDataType *start =  _summary2Trace->waveBuf1 + _summary2Trace->waveNum1;
            WaveDataType *end = _summary2Trace->waveBuf1 + DATA_STORAGE_WAVE_TIME * rate;
            qFill(start, end, 0x40000000);
            _summary2Trace->waveNum1 = DATA_STORAGE_WAVE_TIME * rate;
        }
    }
}


/***************************************************************************************************
 * update checksum
 **************************************************************************************************/
void PrinterActionSummaryItem::updateChecksum()
{
    if(_summary)
    {
        _summary->sum();
    }

    if(_summary2Trace)
    {
        _summary2Trace->sum();
    }

    if(_summary3Trace)
    {
        _summary3Trace->sum();
    }

    if(_summary4Trace)
    {
        _summary4Trace->sum();
    }
}

/***************************************************************************************************
 * return the size of current summary data
 **************************************************************************************************/
int PrinterActionSummaryItem::summaryDataLength() const {
    if(_summary2Trace)
    {
        return sizeof(SummaryPrinterAction2Trace);
    }
    else if(_summary3Trace)
    {
        return sizeof(SummaryPrinterAction3Trace);
    }
    else if(_summary4Trace)
    {
        return sizeof(SummaryPrinterAction4Trace);
    }
    else
    {
        return sizeof(SummaryPrinterAction);
    }
}

/***************************************************************************************************
 * check wheter cache complete
 **************************************************************************************************/
bool PrinterActionSummaryItem::isCompleted() const
{
    if(!CommonSummaryItem::isCompleted())
    {
        return false;
    }

    if(_summary4Trace)
    {

        //time exceed
        if(timeManager.getCurTime() - _summary4Trace->itemDesc.time > SYNC_CACHE_TIME + 1)
        {
            return true;
        }

        if(!waveformCache.isSyncCacheCompleted((WaveformID)_summary4Trace->waveID1,
                                              (long)(&_summary4Trace->waveNum1)))
        {
            return false;
        }

        if(!waveformCache.isSyncCacheCompleted((WaveformID)_summary4Trace->waveID2,
                                              (long)(&_summary4Trace->waveNum2)))
        {
            return false;
        }

        if(!waveformCache.isSyncCacheCompleted((WaveformID)_summary4Trace->waveID3,
                                              (long)(&_summary4Trace->waveNum3)))
        {
            return false;
        }

    }
    else if(_summary3Trace)
    {
        //time exceed
        if(timeManager.getCurTime() - _summary3Trace->itemDesc.time > SYNC_CACHE_TIME + 1)
        {
            return true;
        }

        if(!waveformCache.isSyncCacheCompleted((WaveformID)_summary3Trace->waveID1,
                                              (long)(&_summary3Trace->waveNum1)))
        {
            return false;
        }

        if(!waveformCache.isSyncCacheCompleted((WaveformID)_summary3Trace->waveID2,
                                              (long)(&_summary3Trace->waveNum2)))
        {
            return false;
        }
    }
    else if(_summary2Trace)
    {
        //time exceed
        if(timeManager.getCurTime() - _summary2Trace->itemDesc.time > SYNC_CACHE_TIME + 1)
        {
            return true;
        }

        if(!waveformCache.isSyncCacheCompleted((WaveformID)_summary2Trace->waveID1,
                                              (long)(&_summary2Trace->waveNum1)))
        {
            return false;
        }
    }

    return true;
}


/***************************************************************************************************
 * get current wave cache length
 **************************************************************************************************/
int PrinterActionSummaryItem::getCacheWaveLength(WaveformID id) const
{
    if(_summary4Trace)
    {
        if(_summary4Trace->commonInfo.ECGLeadName == id)
        {
            return _summary4Trace->commonInfo.waveNum;
        }

        if(_summary4Trace->waveID1 == id)
        {
            return _summary4Trace->waveNum1;
        }

        if(_summary4Trace->waveID2 == id)
        {
            return _summary4Trace->waveNum2;
        }

        if(_summary4Trace->waveID3 == id)
        {
            return _summary4Trace->waveNum3;
        }
    }
    else if(_summary3Trace)
    {
        if(_summary3Trace->commonInfo.ECGLeadName == id)
        {
            return _summary3Trace->commonInfo.waveNum;
        }

        if(_summary3Trace->waveID1 == id)
        {
            return _summary3Trace->waveNum1;
        }

        if(_summary3Trace->waveID2 == id)
        {
            return _summary3Trace->waveNum2;
        }
    }
    else if(_summary2Trace)
    {
        if(_summary2Trace->commonInfo.ECGLeadName == id)
        {
            return _summary2Trace->commonInfo.waveNum;
        }

        if(_summary2Trace->waveID1 == id)
        {
            return _summary2Trace->waveNum1;
        }
    }
    else if(_summary)
    {
        if(_summary->commonInfo.ECGLeadName == id)
        {
            return _summary->commonInfo.waveNum;
        }
    }

    return 0;
}


/***************************************************************************************************
 * constructor
 **************************************************************************************************/
PrinterActionSummaryItem::PrinterActionSummaryItem(SummaryPrinterAction *summarydata)
    :CommonSummaryItem((char*) summarydata), _summary(summarydata)
{
    _summary2Trace = NULL;
    _summary3Trace = NULL;
    _summary4Trace = NULL;
}

PrinterActionSummaryItem::PrinterActionSummaryItem(SummaryPrinterAction2Trace *summarydata)
    :CommonSummaryItem((char*) summarydata), _summary2Trace(summarydata)
{
    _summary = NULL;
    _summary3Trace = NULL;
    _summary4Trace = NULL;
}

PrinterActionSummaryItem::PrinterActionSummaryItem(SummaryPrinterAction3Trace *summarydata)
    :CommonSummaryItem((char*) summarydata), _summary3Trace(summarydata)
{
    _summary = NULL;
    _summary2Trace = NULL;
    _summary4Trace = NULL;
}

PrinterActionSummaryItem::PrinterActionSummaryItem(SummaryPrinterAction4Trace *summarydata)
    :CommonSummaryItem((char*) summarydata), _summary4Trace(summarydata)
{
    _summary = NULL;
    _summary2Trace = NULL;
    _summary3Trace = NULL;
}


/***************************************************************************************************
 * destructor
 **************************************************************************************************/
PrinterActionSummaryItem::~PrinterActionSummaryItem()
{
    if(_summary)
    {
        delete _summary;
    }

    if(_summary2Trace)
    {
        delete _summary2Trace;
    }

    if(_summary3Trace)
    {
        delete _summary3Trace;
    }

    if(_summary4Trace)
    {
        delete _summary4Trace;
    }
}

