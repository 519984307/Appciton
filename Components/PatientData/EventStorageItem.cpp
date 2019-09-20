/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/22
 **/

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
#include "Alarm.h"
#include "AlarmConfig.h"
#include "DataStorageDirManager.h"
#include "LanguageManager.h"

class EventStorageItemPrivate
{
public:
    EventStorageItemPrivate(EventStorageItem *const q_ptr, EventType type, const QList<WaveformID> &waveforms) :
        q_ptr(q_ptr),
        waveforms(waveforms),
        startCollect(false),
        trendCacheComplete(false),
        waitForTriggerPrintFlag(false),
        triggerPrintStopped(false),
        extraData(0),
        almInfo(NULL),
        codeMarkerInfo(NULL),
        oxyCRGInfo(NULL),
        measureInfo(NULL),
        eventFolderName(dataStorageDirManager.getCurFolder())
    {
        int duration_after_event;
        int duration_before_event;
        Config &conf =  configManager.getCurConfig();
        conf.getNumValue("Event|WaveLength", duration_before_event);
        duration_after_event = duration_before_event;
        eventInfo.type = type;
        eventInfo.duration_after = duration_after_event;
        eventInfo.duration_before = duration_before_event;
        eventTypeAndPrio = type | (ALARM_PRIO_PROMPT << 8);
    }

    ~EventStorageItemPrivate()
    {
        trendCache.unregisterTrendRecorder(this);

        foreach(TrendDataSegment *trendSeg, trendSegments)
        {
            qFree(trendSeg);
        }

        foreach(WaveformDataSegment *waveSeg, waveSegments)
        {
            if (!waveComplete[waveSeg->waveID])
            {
                waveformCache.unRegisterWaveformRecorder(waveSeg->waveID, this);
            }
            qFree(waveSeg);
        }


        if (almInfo)
        {
            delete almInfo;
        }

        if (codeMarkerInfo)
        {
            delete codeMarkerInfo;
        }

        if (oxyCRGInfo)
        {
            delete oxyCRGInfo;
        }

        if (measureInfo)
        {
            delete measureInfo;
        }
    }

    void saveTrendData(unsigned timestamp, const TrendCacheData &data, const TrendAlarmStatus &almStatus);

    static void waveCacheDurationIncrease(WaveformID id, void *obj);

    static void waveCacheCompleteCallback(WaveformID id, void *obj);

    static void trendCacheCompleteCallback(unsigned timestamp, const TrendCacheData &data,
                                           const TrendAlarmStatus &almStatus, void *obj);

    EventStorageItem *const q_ptr;
    QList<TrendDataSegment *> trendSegments;
    QList<WaveformDataSegment *> waveSegments;
    EventInfoSegment eventInfo;
    QList<WaveformID> waveforms;
    QMap<WaveformID, bool> waveComplete;
    QMap<WaveformID, int> waveCacheDuration;
    bool startCollect;
    bool trendCacheComplete;
    bool waitForTriggerPrintFlag;
    bool triggerPrintStopped;
    quint32 extraData;          // 事件产生时间

    AlarmInfoSegment *almInfo;
    CodeMarkerSegment *codeMarkerInfo;
    OxyCRGSegment *oxyCRGInfo;
    NIBPMeasureSegment *measureInfo;

    QString eventFolderName;
    quint32 eventTypeAndPrio;          // 包括事件类型和事件报警等级
};

void EventStorageItemPrivate::saveTrendData(unsigned timestamp, const TrendCacheData &data,
        const TrendAlarmStatus &almStatus)
{
    QVector<TrendValueSegment> valueSegments;

    QList<ParamID> idList;
    paramManager.getParams(idList);

    bool hasAlarm = false;
    ParamID paramId = PARAM_NONE;

    for (int i = 0; i < SUB_PARAM_NR; i++)
    {
        paramId = paramInfo.getParamID((SubParamID) i);
        if (static_cast<SubParamID>(i) == SUB_PARAM_NIBP_PR)
        {
            paramId = PARAM_NIBP;
        }
        if (-1 == idList.indexOf(paramId))
        {
            continue;
        }

        if (!data.values.contains((SubParamID)i))
        {
            continue;
        }

        TrendValueSegment valueSegment;
        valueSegment.subParamId = i;
        valueSegment.value = data.values.value((SubParamID)i);
        valueSegment.alarmFlag = almStatus.alarms.value((SubParamID)i);
        if (!hasAlarm)
        {
            hasAlarm = valueSegment.alarmFlag;
        }

        valueSegments.append(valueSegment);
    }

    TrendDataSegment *trendSeg = reinterpret_cast<TrendDataSegment *>(qMalloc(sizeof(TrendDataSegment) + valueSegments.size() * sizeof(
                                     TrendValueSegment)));

    qMemSet(trendSeg, 0, sizeof(TrendDataSegment));

    trendSeg->timestamp = timestamp;
    trendSeg->co2Baro = data.co2baro;

    trendSeg->trendValueNum = valueSegments.size();

    // copy the trend values
    qMemCopy(reinterpret_cast<char *>(trendSeg) + sizeof(TrendDataSegment), valueSegments.constData(),
             sizeof(TrendValueSegment) * valueSegments.size());

    trendSegments.append(trendSeg);
}

void EventStorageItemPrivate::waveCacheDurationIncrease(WaveformID id, void *obj)
{
    EventStorageItemPrivate *p = reinterpret_cast<EventStorageItemPrivate *>(obj);
    Q_ASSERT(p != NULL);
    p->waveCacheDuration[id]  += 1;
}

void EventStorageItemPrivate::waveCacheCompleteCallback(WaveformID id, void *obj)
{
    EventStorageItemPrivate *p = reinterpret_cast<EventStorageItemPrivate *>(obj);
    Q_ASSERT(p != NULL);
    p->waveComplete[id] = true;
}

void EventStorageItemPrivate::trendCacheCompleteCallback(unsigned timestamp, const TrendCacheData &data,
        const TrendAlarmStatus &almStatus, void *obj)
{
    qDebug("timestamp = %d\n", timestamp);
    EventStorageItemPrivate *p = reinterpret_cast<EventStorageItemPrivate *>(obj);
    p->saveTrendData(timestamp, data, almStatus);
    Q_ASSERT(p != NULL);
    if (p)
    {
        p->trendCacheComplete = true;
    }
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms)
    : d_ptr(new EventStorageItemPrivate(this, type, storeWaveforms))
{
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms,
                                   const AlarmInfoSegment &almInfo)
    : d_ptr(new EventStorageItemPrivate(this, type, storeWaveforms))
{
    d_ptr->almInfo = new AlarmInfoSegment();
    d_ptr->almInfo->alarmLimit = almInfo.alarmLimit;
    d_ptr->almInfo->alarmType = almInfo.alarmType;
    d_ptr->almInfo->alarmInfo = almInfo.alarmInfo;
    d_ptr->almInfo->subParamID = almInfo.subParamID;
    AlarmPriority priority = ALARM_PRIO_PROMPT;
    if (almInfo.alarmInfo & 0x01)   // oneshot 报警事件
    {
        AlarmOneShotIFace *alarmOneShot = NULL;
        alarmOneShot = alertor.getAlarmOneShotIFace(static_cast<SubParamID>(almInfo.subParamID));
        if (alarmOneShot)
        {
            priority = alarmOneShot->getAlarmPriority(almInfo.subParamID);
        }
    }
    else
    {
        AlarmLimitIFace *alarmLimit = NULL;
        alarmLimit = alertor.getAlarmLimitIFace(static_cast<SubParamID>(almInfo.subParamID));
        if (alarmLimit)
        {
            priority = alarmLimit->getAlarmPriority(almInfo.subParamID);
        }
    }

    d_ptr->eventTypeAndPrio = d_ptr->eventInfo.type | (priority << 8);
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, const char *codeName)
    : d_ptr(new EventStorageItemPrivate(this, type, storeWaveforms))
{
    d_ptr->codeMarkerInfo = new CodeMarkerSegment;
    Util::strlcpy(d_ptr->codeMarkerInfo->codeName, codeName, sizeof(d_ptr->codeMarkerInfo->codeName));
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, OxyCRGEventType oxyCRGtype,
                                   const AlarmInfoSegment &almInfo)
    : d_ptr(new EventStorageItemPrivate(this, type, storeWaveforms))
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
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, OxyCRGEventType oxyCRGtype)
    : d_ptr(new EventStorageItemPrivate(this, type, storeWaveforms))
{
    d_ptr->oxyCRGInfo = new OxyCRGSegment;
    d_ptr->oxyCRGInfo->type = oxyCRGtype;
}

EventStorageItem::EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, NIBPOneShotType measureResult)
    : d_ptr(new EventStorageItemPrivate(this, type, storeWaveforms))
{
    d_ptr->measureInfo = new NIBPMeasureSegment;
    d_ptr->measureInfo->measureResult = measureResult;
}

EventStorageItem::~EventStorageItem()
{
}

quint32 EventStorageItem::getType() const
{
    return d_ptr->eventTypeAndPrio;
}

quint32 EventStorageItem::getExtraData() const
{
    return d_ptr->extraData;
}

bool EventStorageItem::checkCompleted()
{
    if (d_ptr->waitForTriggerPrintFlag && !d_ptr->triggerPrintStopped)
    {
        // have to wait until the page generator is stopped
        return false;
    }

    if (timeManager.getCurTime() -  d_ptr->eventInfo.timestamp  > (unsigned)(d_ptr->eventInfo.duration_after))
    {
        // time expired
        qdebug("cache time expired");

        // unregister recorder
        foreach(WaveformDataSegment *seg, d_ptr->waveSegments)
        {
            waveformCache.unRegisterWaveformRecorder(seg->waveID, d_ptr.data());
            d_ptr->waveComplete[seg->waveID] = true;
        }

        return true;
    }

    foreach(WaveformID waveid, d_ptr->waveforms)
    {
        if (!d_ptr->waveComplete[waveid])
        {
            return false;
        }
    }

    if (d_ptr->eventInfo.type == EventOxyCRG && !d_ptr->trendCacheComplete)
    {
        return false;
    }

    return true;
}


bool EventStorageItem::startCollectTrendAndWaveformData(unsigned t)
{
    if (d_ptr->startCollect)
    {
        qdebug("Already start collect data");
        return false;
    }
    d_ptr->startCollect = true;
    d_ptr->eventInfo.timestamp = t;
    d_ptr->extraData = t;


    // store the trend data before current timestamp
    TrendCacheData trendData;
    TrendAlarmStatus trendAlarmStatus;

    trendCache.collectTrendData(t);
    trendCache.collectTrendAlarmStatus(t);
    if (d_ptr->eventInfo.type == EventOxyCRG)
    {
        for (unsigned time = t - d_ptr->eventInfo.duration_before; time <= t; time ++)
        {
            TrendCacheData trendData;
            TrendAlarmStatus trendAlarmStatus;
            if (trendCache.getTrendData(time, trendData) && trendCache.getTrendAlarmStatus(time, trendAlarmStatus))
            {
                d_ptr->saveTrendData(time, trendData, trendAlarmStatus);
            }
        }

        TrendRecorder trendRecorder;
        trendRecorder.toTimestamp =  t + d_ptr->eventInfo.duration_after;
        trendRecorder.obj = d_ptr.data();
        trendRecorder.completeCallback = EventStorageItemPrivate::trendCacheCompleteCallback;
        trendCache.registerTrendRecorder(trendRecorder);
    }
    else
    {
        trendCache.getTrendData(t, trendData);
        trendCache.getTrendAlarmStatus(t, trendAlarmStatus);
        d_ptr->saveTrendData(t, trendData, trendAlarmStatus);
    }

    foreach(WaveformID waveid, d_ptr->waveforms)
    {
        int sampleRate = waveformCache.getSampleRate(waveid);
        int waveNum = (d_ptr->eventInfo.duration_after + d_ptr->eventInfo.duration_before) * sampleRate;

        QString remark = QString("%1 %2").arg(ECGSymbol::convert(ecgParam.getFilterMode()))
                                          .arg(ECGSymbol::convert(ecgParam.getNotchFilter()));
        std::string stdStr = remark.toStdString();
        const char *remarks = stdStr.c_str();
        WaveformDataSegment *waveSegment = reinterpret_cast<WaveformDataSegment *>(qMalloc(sizeof(WaveformDataSegment) +
                                           sizeof(WaveDataType) * waveNum));

        if (waveSegment == NULL)
        {
            qdebug("Out of memory!");
            break;
        }

        d_ptr->waveSegments.append(waveSegment);

        waveSegment->waveID = waveid;
        waveSegment->sampleRate = sampleRate;
        waveSegment->waveNum = waveNum;
        Util::strlcpy(waveSegment->remarks, remarks, MAX_WAVE_SEG_INFO_REMARK);

        d_ptr->waveComplete[waveid] = false;
        d_ptr->waveCacheDuration[waveid] = d_ptr->eventInfo.duration_before;

        waveformCache.readStorageChannel(waveid, waveSegment->waveData, d_ptr->eventInfo.duration_before, false);

        WaveformRecorder recorder;
        recorder.buf = waveSegment->waveData;
        recorder.curRecWaveNum = d_ptr->eventInfo.duration_before * sampleRate;
        recorder.sampleRate = sampleRate;
        recorder.totalRecWaveNum = waveNum;
        recorder.recObj = d_ptr.data();
        recorder.recordDurationIncreaseCallback = EventStorageItemPrivate::waveCacheDurationIncrease;
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

    // write event info
    type = EVENT_INFO_SEGMENT;
    buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
    buffer.write(reinterpret_cast<char *>(&d_ptr->eventInfo), sizeof(d_ptr->eventInfo));

    if (d_ptr->almInfo)
    {
        // store alarm info
        type = EVENT_ALARM_INFO_SEGMENT;
        buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
        buffer.write(reinterpret_cast<char *>(d_ptr->almInfo), sizeof(AlarmInfoSegment));
    }

    if (d_ptr->codeMarkerInfo)
    {
        // store codemarker info
        type = EVENT_CODEMARKER_SEGMENT;
        buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
        buffer.write(reinterpret_cast<char *>(d_ptr->codeMarkerInfo), sizeof(CodeMarkerSegment));
    }

    if (d_ptr->oxyCRGInfo)
    {
        // store the oxyCRG info
        type = EVENT_OXYCRG_SEGMENT;
        buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
        buffer.write(reinterpret_cast<char *>(d_ptr->oxyCRGInfo), sizeof(OxyCRGSegment));
    }

    if (d_ptr->measureInfo)
    {
        // store the nibp measure info
        type = EVENT_NIBPMEASURE_SEGMENT;
        buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
        buffer.write(reinterpret_cast<char *>(d_ptr->measureInfo), sizeof(NIBPMeasureSegment));
    }

    // write trend segments
    if (d_ptr->trendSegments.count())
    {
        type = EVENT_TRENDDATA_SEGMENT;
        foreach(TrendDataSegment *trendSeg, d_ptr->trendSegments)
        {
            buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
            buffer.write(reinterpret_cast<char *>(trendSeg), sizeof(TrendDataSegment) + sizeof(TrendValueSegment) * trendSeg->trendValueNum);
        }
    }

    // write wave segments
    if (d_ptr->waveSegments.count())
    {
        type = EVENT_WAVEFORM_SEGMENT;
        foreach(WaveformDataSegment *waveSeg, d_ptr->waveSegments)
        {
            buffer.write(reinterpret_cast<char *>(&type), sizeof(type));
            buffer.write(reinterpret_cast<char *>(waveSeg), sizeof(WaveformDataSegment) + sizeof(WaveDataType) * waveSeg->waveNum);
        }
    }

    return buffer.data();
}

QList<WaveformID> EventStorageItem::getStoreWaveforms() const
{
    return d_ptr->waveforms;
}

int EventStorageItem::getCurWaveCacheDuration(WaveformID waveId) const
{
    return d_ptr->waveCacheDuration.value(waveId, -1);
}

bool EventStorageItem::getOneSecondWaveform(WaveformID waveId, WaveDataType *waveBuf, int startSecond)
{
    if (d_ptr->waveCacheDuration.contains(waveId) && d_ptr->waveCacheDuration.value(waveId) > startSecond)
    {
        int waveIndex = d_ptr->waveforms.indexOf(waveId);
        WaveformDataSegment *waveSegment = d_ptr->waveSegments.at(waveIndex);
        int startIndex = startSecond * waveSegment->sampleRate;
        qMemCopy(waveBuf, waveSegment->waveData + startIndex, waveSegment->sampleRate * sizeof(WaveDataType));
        return true;
    }
    return false;
}

int EventStorageItem::getTotalWaveCacheDuration() const
{
    return d_ptr->eventInfo.duration_after + d_ptr->eventInfo.duration_before;
}

TrendDataPackage EventStorageItem::getTrendData() const
{
    return parseTrendSegment(d_ptr->trendSegments.at(0));
}

void EventStorageItem::setWaitForTriggerPrintFlag(bool flag)
{
    d_ptr->waitForTriggerPrintFlag = flag;
}

QString EventStorageItem::getEventTitle() const
{
    QString eventTitle;
    switch (d_ptr->eventInfo.type)
    {
    case EventPhysiologicalAlarm:
    {
        SubParamID subparamID = (SubParamID) d_ptr->almInfo->subParamID;
        unsigned char alarmId = d_ptr->almInfo->alarmType;
        unsigned char alarmInfo = d_ptr->almInfo->alarmInfo;
        AlarmPriority priority;
        if (alarmInfo & 0x01)   // oneshot 报警事件
        {
            AlarmOneShotIFace *alarmOneShot = alertor.getAlarmOneShotIFace(subparamID);
            if (alarmOneShot)
            {
                priority = alarmOneShot->getAlarmPriority(alarmId);
            }
        }
        else
        {
            AlarmLimitIFace *almIface = alertor.getAlarmLimitIFace(subparamID);
            if (almIface)
            {
                priority = almIface->getAlarmPriority(alarmId);
            }
        }

        QString titleStr;

        if (priority == ALARM_PRIO_LOW)
        {
            titleStr = "*";
        }
        else if (priority == ALARM_PRIO_MED)
        {
            titleStr = "**";
        }
        else if (priority == ALARM_PRIO_HIGH)
        {
            titleStr = "***";
        }

        ParamID paramId = paramInfo.getParamID(subparamID);
        // oneshot 报警
        if (alarmInfo & 0x01)
        {
            // 将参数ID转换为oneshot报警对应的参数ID
            if (paramId == PARAM_DUP_ECG)
            {
                paramId = PARAM_ECG;
            }
            else if (paramId == PARAM_DUP_RESP)
            {
                paramId = PARAM_RESP;
            }
        }
        titleStr += " ";
        titleStr += trs(Alarm::getPhyAlarmMessage(paramId,
                        d_ptr->almInfo->alarmType,
                        d_ptr->almInfo->alarmInfo & 0x01));

        if (!(d_ptr->almInfo->alarmInfo & 0x01))
        {
            if (d_ptr->almInfo->alarmInfo & 0x02)
            {
                titleStr += " > ";
            }
            else
            {
                titleStr += " < ";
            }

            UnitType unit = paramManager.getSubParamUnit(paramId, subparamID);
            LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subparamID, unit);

            titleStr += Util::convertToString(d_ptr->almInfo->alarmLimit, config.scale);
        }

        eventTitle = titleStr;
    }
    break;
    case EventCodeMarker:
    {
        eventTitle = QString(d_ptr->codeMarkerInfo->codeName);
    }
    break;
    case EventRealtimePrint:
    {
        eventTitle = trs("RealtimePrintSegment");
    }
    break;
    case EventNIBPMeasurement:
    {
        eventTitle = trs("NibpMeasurement");
    }
    break;
    case EventWaveFreeze:
    {
        eventTitle = trs("WaveFreeze");
    }
    break;
    default:
        break;
    }
    return eventTitle;
}

QString EventStorageItem::getEventFolderName() const
{
    return d_ptr->eventFolderName;
}

void EventStorageItem::onTriggerPrintStopped()
{
    d_ptr->triggerPrintStopped = true;
}

