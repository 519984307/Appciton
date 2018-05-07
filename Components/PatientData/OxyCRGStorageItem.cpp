#include "OxyCRGStorageItem.h"
#include "TrendCache.h"
#include "WaveformCache.h"
#include "Debug.h"
#include "TimeManager.h"

class OxyCRGStorageItemPrivate
{
public:
    OxyCRGStorageItemPrivate() :
        startCollect(false)
    {
        waveforms.append(WAVE_RESP);
        waveforms.append(WAVE_CO2);
    }

    ~OxyCRGStorageItemPrivate()
    {}

    static void trendCacheCompleteCallback(unsigned timestamp, const TrendCacheData &trendData, const TrendAlarmStatus &alarmStatus, void *obj);
    static void waveCacheCompleteCallback(WaveformID id, void *obj);

    OxyCRGEvent *oxyCRGSegment;
    QList<WaveformID> waveforms;
    QMap<WaveformID, bool> waveComplete;
    bool startCollect;
};

OxyCRGStorageItem::OxyCRGStorageItem()
{

}

OxyCRGStorageItem::~OxyCRGStorageItem()
{

}

bool OxyCRGStorageItem::startCollectTrendAndWaveformData()
{
    if (d_ptr->startCollect)
    {
        qdebug("Already start collect data");
        return false;
    }
    d_ptr->startCollect = true;

    unsigned currentTime = timeManager.getCurTime();
    d_ptr->oxyCRGSegment->storageInfo.timestamp = currentTime;

    //store the trend data before current timestamp
    TrendRecorder trendRec;
    trendRec.toTimestamp = currentTime;
    trendRec.obj = d_ptr.data();
    trendRec.record = OxyCRGStorageItemPrivate::trendCacheCompleteCallback;
    trendCache.registerTrendRecorder(trendRec);

    foreach (WaveformID waveid, d_ptr->waveforms)
    {
        int sampleRate = waveformCache.getSampleRate(waveid);
        int waveNum = (d_ptr->oxyCRGSegment->storageInfo.duration_after +
                d_ptr->oxyCRGSegment->storageInfo.duration_before) * sampleRate;

        WaveformDataSegment *waveSegment = (WaveformDataSegment *) qMalloc(sizeof(WaveformDataSegment) +
                                                                           sizeof(WaveDataType) * waveNum);

        if (waveSegment == NULL)
        {
            qdebug("Out of memory!");
            break;
        }

        d_ptr->oxyCRGSegment->waveSegments.append(waveSegment);

        waveSegment->waveID = waveid;
        waveSegment->sampleRate = sampleRate;
        waveSegment->waveNum = waveNum;

        waveformCache.readStorageChannel(waveid, waveSegment->waveData, d_ptr->oxyCRGSegment->storageInfo.duration_before, false);

        WaveformRecorder waveRec;
        waveRec.buf = waveSegment->waveData;
        waveRec.curRecWaveNum = d_ptr->oxyCRGSegment->storageInfo.duration_before * sampleRate;
        waveRec.totalRecWaveNum = waveSegment->waveNum * sampleRate;
        waveRec.recObj = d_ptr.data();
        waveRec.recordCompleteCallback = OxyCRGStorageItemPrivate::waveCacheCompleteCallback;
        waveformCache.registerWaveformRecorder(waveid, waveRec);
    }
    return true;
}

void OxyCRGStorageItemPrivate::trendCacheCompleteCallback(unsigned /*timestamp*/, const TrendCacheData &/*trendData*/, const TrendAlarmStatus &/*alarmStatus*/, void */*obj*/)
{

}

void OxyCRGStorageItemPrivate::waveCacheCompleteCallback(WaveformID id, void *obj)
{
    OxyCRGStorageItemPrivate *p = reinterpret_cast<OxyCRGStorageItemPrivate *>(obj);
    Q_ASSERT(p != NULL);
    if(p)
    {
        p->waveComplete[id] = true;
    }
}
