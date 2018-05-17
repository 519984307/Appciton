#include "EventStorageItem.h"
#include <QList>
#include "TrendCache.h"
#include "WaveformCache.h"
#include "TimeManager.h"
#include "ECGDupParam.h"
#include "RESPDupParam.h"
#include "ParamManager.h"
#include "WaveformCache.h"
#include "ConfigManager.h"
#include "Utility.h"
#include <QMap>
#include <QBuffer>
#include "Debug.h"
#include "ConfigManager.h"
#include "OxyCRGEventWaveWidget.h"

class EventStorageItemPrivate
{
public:
    EventStorageItemPrivate(EventType type, const QList<WaveformID> &waveforms) :
        waveforms(waveforms),
        startCollect(false),
        trendCacheComplete(false),
        almInfo(NULL),
        codeMarkerInfo(NULL),
        oxyCRGInfo(NULL)
    {
        int duration_after_event = 8;
        int duration_before_event = 8;
        Config &conf =  configManager.getCurConfig();
        conf.getNumValue("Event|WaveLengthBefore", duration_before_event);
        conf.getNumValue("Event|WaveLengthAfter", duration_after_event);
        eventInfo.type = type;
        eventInfo.duration_after = duration_after_event;
        eventInfo.duration_before = duration_before_event;
    }

    ~EventStorageItemPrivate()
    {
        trendCache.unregisterTrendRecorder(this);

        foreach (TrendDataSegment *trendSeg, trendSegments)
        {
            qFree(trendSeg);
        }

        foreach (WaveformDataSegment *waveSeg, waveSegments)
        {
            if(!waveComplete[waveSeg->waveID])
            {
                waveformCache.unRegisterWaveformRecorder(waveSeg->waveID, this);
            }
            qFree(waveSeg);
        }


        if(almInfo)
        {
            delete almInfo;
        }

        if(codeMarkerInfo)
        {
            delete codeMarkerInfo;
        }
    }

    void saveTrendData(unsigned timestamp, const TrendCacheData &data, const TrendAlarmStatus &almStatus);

    static void waveCacheCompleteCallback(WaveformID id, void *obj);

    static void trendCacheCompleteCallback(unsigned timestamp, const TrendCacheData &data, const TrendAlarmStatus &almStatus, void *obj);

    QList<TrendDataSegment*> trendSegments;
    QList<WaveformDataSegment *> waveSegments;
    EventInfoSegment eventInfo;
    QList<WaveformID> waveforms;
    QMap<WaveformID, bool> waveComplete;
    bool startCollect;
    bool trendCacheComplete;

    AlarmInfoSegment *almInfo;
    CodeMarkerSegment *codeMarkerInfo;
    OxyCRGSegment *oxyCRGInfo;
};

void EventStorageItemPrivate::saveTrendData(unsigned timestamp, const TrendCacheData &data, const TrendAlarmStatus &almStatus)
{
    QVector<TrendValueSegment> valueSegments;

    QList<ParamID> idList;
    paramManager.getParams(idList);

    bool hasAlarm = false;
    ParamID paramId = PARAM_NONE;
    //collect sub param that have valid value
    for(int i = 0; i < SUB_PARAM_NR; i++)
    {
        paramId = paramInfo.getParamID((SubParamID) i);
        if(-1 == idList.indexOf(paramId))
        {
            continue;
        }

        if(data.values.value((SubParamID)i, InvData()) == InvData())
        {
            continue;
        }

        TrendValueSegment valueSegment;
        valueSegment.subParamId = i;
        valueSegment.value = data.values.value((SubParamID)i);
        valueSegment.alarmFlag = almStatus.alarms.value((SubParamID)i);
        if(!hasAlarm)
        {
            hasAlarm = valueSegment.alarmFlag;
        }

        valueSegments.append(valueSegment);
    }

    TrendDataSegment *trendSeg = (TrendDataSegment *) qMalloc(sizeof(TrendDataSegment) + valueSegments.size() * sizeof(TrendValueSegment));

    qMemSet(trendSeg, 0, sizeof(TrendDataSegment));

    trendSeg->timestamp = timestamp;
    trendSeg->co2Baro = data.co2baro;

    trendSeg->trendValueNum = valueSegments.size();

    //copy the trend values
    qMemCopy((char *)trendSeg+ sizeof(TrendDataSegment), valueSegments.constData(), sizeof(TrendValueSegment) * valueSegments.size());

    trendSegments.append(trendSeg);
}

void EventStorageItemPrivate::waveCacheCompleteCallback(WaveformID id, void *obj)
{
    EventStorageItemPrivate *p = reinterpret_cast<EventStorageItemPrivate *>(obj);
    Q_ASSERT(p != NULL);
    if(p)
    {
        p->waveComplete[id] = true;
    }
}

void EventStorageItemPrivate::trendCacheCompleteCallback(unsigned timestamp, const TrendCacheData &data, const TrendAlarmStatus &almStatus, void *obj)
{
    EventStorageItemPrivate *p = reinterpret_cast<EventStorageItemPrivate *>(obj);
    p->saveTrendData(timestamp, data, almStatus);
    Q_ASSERT(p != NULL);
    if(p)
    {
        p->trendCacheComplete = true;
    }
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms)
    :d_ptr(new EventStorageItemPrivate(type, storeWaveforms))
{
    qDebug()<<"Create Event Stroage Item:"<<this<<" type: "<<type;
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, const AlarmInfoSegment &almInfo)
    :d_ptr(new EventStorageItemPrivate(type, storeWaveforms))
{
    d_ptr->almInfo = new AlarmInfoSegment();
    d_ptr->almInfo->alarmLimit = almInfo.alarmLimit;
    d_ptr->almInfo->alarmType = almInfo.alarmType;
    d_ptr->almInfo->alarmInfo = almInfo.alarmInfo;
    d_ptr->almInfo->subParamID = almInfo.subParamID;
    qDebug()<<"Create Event Stroage Item:"<<this<<" type: "<<type;
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, const char *codeName)
    :d_ptr(new EventStorageItemPrivate(type, storeWaveforms))
{
    d_ptr->codeMarkerInfo = new CodeMarkerSegment;
    Util::strlcpy(d_ptr->codeMarkerInfo->codeName, codeName, sizeof(d_ptr->codeMarkerInfo->codeName));
    qDebug()<<"Create Event Stroage Item:"<<this<<" type: "<<type;
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, OxyCRGEventType oxyCRGtype, const AlarmInfoSegment &almInfo)
    :d_ptr(new EventStorageItemPrivate(type, storeWaveforms))
{
    d_ptr->almInfo = new AlarmInfoSegment();
    d_ptr->almInfo->alarmLimit = almInfo.alarmLimit;
    d_ptr->almInfo->alarmType = almInfo.alarmType;
    d_ptr->almInfo->alarmInfo = almInfo.alarmInfo;
    d_ptr->almInfo->subParamID = almInfo.subParamID;

    d_ptr->oxyCRGInfo = new OxyCRGSegment;
    d_ptr->oxyCRGInfo->type = oxyCRGtype;

    int durationType;
    if (currentConfig.getNumValue("OxyCRG|EventStorageSetup", durationType))
    {
        if ((OxyCRGEventStorageDuration)durationType == OxyCRG_EVENT_DURATION_1_3MIN)
        {
            d_ptr->eventInfo.duration_before = 60;
            d_ptr->eventInfo.duration_after = 180;
        }
        else if ((OxyCRGEventStorageDuration)durationType == OxyCRG_EVENT_DURATION_2_2MIN)
        {
            d_ptr->eventInfo.duration_before = 120;
            d_ptr->eventInfo.duration_after = 120;
        }
        else if ((OxyCRGEventStorageDuration)durationType == OxyCRG_EVENT_DURATION_3_1MIN)
        {
            d_ptr->eventInfo.duration_before = 180;
            d_ptr->eventInfo.duration_after = 60;
        }
    }
    else
    {
        d_ptr->eventInfo.duration_after = 120;
        d_ptr->eventInfo.duration_before = 120;
    }

    qDebug()<<"Create Event Stroage Item:"<<this<<" type: "<<type;
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, OxyCRGEventType oxyCRGtype)
    :d_ptr(new EventStorageItemPrivate(type, storeWaveforms))
{
    d_ptr->oxyCRGInfo = new OxyCRGSegment;
    d_ptr->oxyCRGInfo->type = oxyCRGtype;
    qDebug()<<"Create Event Stroage Item:"<<this<<" type: "<<type;
}

EventStorageItem::~EventStorageItem()
{
    qDebug()<<"Destroy Event Stroage Item:"<<this;
}

EventType EventStorageItem::getType() const
{
    return d_ptr->eventInfo.type;
}

bool EventStorageItem::checkCompleted()
{
    if(timeManager.getCurTime() -  d_ptr->eventInfo.timestamp  > (unsigned) (d_ptr->eventInfo.duration_after + 2))
    {
        //time expired
        qdebug("cache time expired");

        //unregister recorder
        foreach (WaveformDataSegment *seg, d_ptr->waveSegments)
        {
            waveformCache.unRegisterWaveformRecorder(seg->waveID, d_ptr.data());
            d_ptr->waveComplete[seg->waveID] = true;
        }

        return true;
    }

    foreach(WaveformID waveid, d_ptr->waveforms)
    {
        if(!d_ptr->waveComplete[waveid])
        {
            return false;
        }
    }

    if(d_ptr->eventInfo.type == EventOxyCRG && !d_ptr->trendCacheComplete)
    {
        return false;
    }

    return true;
}


bool EventStorageItem::startCollectTrendAndWaveformData()
{
    if(d_ptr->startCollect)
    {
        qdebug("Already start collect data");
        return false;
    }
    d_ptr->startCollect = true;
    unsigned currentTime = timeManager.getCurTime();
    d_ptr->eventInfo.timestamp = currentTime;


    //store the trend data before current timestamp
    TrendCacheData trendData;
    TrendAlarmStatus trendAlarmStatus;
    trendCache.collectTrendData(currentTime);
    trendCache.collectTrendAlarmStatus(currentTime);

    if(d_ptr->eventInfo.type == EventOxyCRG)
    {
        QList<TrendCacheData> trendDataList = trendCache.getTrendData(currentTime - d_ptr->eventInfo.duration_before,
                                                                      currentTime);
        QList<TrendAlarmStatus> alarmStatusList = trendCache.getTrendAlarmStatus(currentTime - d_ptr->eventInfo.duration_before,
                                                                                 currentTime);

        unsigned t = currentTime - d_ptr->eventInfo.duration_before;
        int i = 0;
        while(t <= currentTime)
        {
            d_ptr->saveTrendData(t, trendDataList.at(i), alarmStatusList.at(i));
            t++;
            i++;
        };


        TrendRecorder trendRecorder;
        trendRecorder.toTimestamp =  currentTime + d_ptr->eventInfo.duration_after;
        trendRecorder.obj = d_ptr.data();
        trendRecorder.completeCallback = EventStorageItemPrivate::trendCacheCompleteCallback;
        trendCache.registerTrendRecorder(trendRecorder);
    }
    else
    {
        trendCache.getTendData(currentTime, trendData);
        trendCache.getTrendAlarmStatus(currentTime, trendAlarmStatus);
        d_ptr->saveTrendData(currentTime, trendData, trendAlarmStatus);
    }

    foreach (WaveformID waveid, d_ptr->waveforms)
    {
        int sampleRate = waveformCache.getSampleRate(waveid);
        int waveNum = (d_ptr->eventInfo.duration_after + d_ptr->eventInfo.duration_before) * sampleRate;

        WaveformDataSegment *waveSegment = (WaveformDataSegment *) qMalloc(sizeof(WaveformDataSegment) +
                                                                           sizeof(WaveDataType) * waveNum);

        if(waveSegment == NULL)
        {
            qdebug("Out of memory!");
            break;
        }

        d_ptr->waveSegments.append(waveSegment);

        waveSegment->waveID = waveid;
        waveSegment->sampleRate = sampleRate;
        waveSegment->waveNum = waveNum;

        d_ptr->waveComplete[waveid] = false;

        waveformCache.readStorageChannel(waveid, waveSegment->waveData, d_ptr->eventInfo.duration_before, false);

        WaveformRecorder recorder;
        recorder.buf = waveSegment->waveData;
        recorder.curRecWaveNum = d_ptr->eventInfo.duration_before * sampleRate;
        recorder.totalRecWaveNum = waveSegment->waveNum;
        recorder.recObj = d_ptr.data();
        recorder.recordCompleteCallback = EventStorageItemPrivate::waveCacheCompleteCallback;
        waveformCache.registerWaveformRecorder(waveid, recorder);
    }

    return true;
}

QByteArray EventStorageItem::getStorageData() const
{
    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    EventSegmentType type;

    //write event info
    type = EVENT_INFO_SEGMENT;
    buffer.write((char *)&type, sizeof(type));
    buffer.write((char *)&d_ptr->eventInfo, sizeof(d_ptr->eventInfo));

    if(d_ptr->almInfo)
    {
        //store alarm info
        type = EVENT_ALARM_INFO_SEGMENT;
        buffer.write((char *)&type, sizeof(type));
        buffer.write((char *)d_ptr->almInfo, sizeof(AlarmInfoSegment));
    }

    if(d_ptr->codeMarkerInfo)
    {
        //store codemarker info
        type = EVENT_CODEMARKER_SEGMENT;
        buffer.write((char *)&type, sizeof(type));
        buffer.write((char *)d_ptr->codeMarkerInfo, sizeof(CodeMarkerSegment));
    }

    if(d_ptr->oxyCRGInfo)
    {
        //store the oxyCRG info
        type = EVENT_OXYCRG_SEGMENT;
        buffer.write((char *)&type, sizeof(type));
        buffer.write((char *)d_ptr->oxyCRGInfo, sizeof(OxyCRGSegment));
    }

    //write trend segments
    if(d_ptr->trendSegments.count())
    {
        type = EVENT_TRENDDATA_SEGMENT;
        foreach(TrendDataSegment *trendSeg, d_ptr->trendSegments)
        {
         buffer.write((char *)&type, sizeof(type));
         buffer.write((char *)trendSeg, sizeof(TrendDataSegment) + sizeof(TrendValueSegment) * trendSeg->trendValueNum);
        }
    }

    //write wave segments
    if(d_ptr->waveSegments.count())
    {
        type = EVENT_WAVEFORM_SEGMENT;
        foreach (WaveformDataSegment *waveSeg, d_ptr->waveSegments) {
            buffer.write((char *)&type, sizeof(type));
            buffer.write((char *)waveSeg, sizeof(WaveformDataSegment) + sizeof(WaveDataType) * waveSeg->waveNum);
        }
    }

    return buffer.data();
}

