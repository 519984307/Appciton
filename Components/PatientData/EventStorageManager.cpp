/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/20
 **/

#include "EventStorageManager.h"
#include "StorageManager_p.h"
#include "StorageFile.h"
#include "DataStorageDirManager.h"
#include "EventStorageItem.h"
#include "ECGParam.h"
#include <QList>
#include <QMutex>
#include "Debug.h"
#include "RecorderManager.h"
#include "TriggerPageGenerator.h"
#include "LayoutManager.h"
#include "ConfigManager.h"
#include <QTimerEvent>
#include "IConfig.h"

#define MAX_STORE_WAVE_NUM 3
#define STOP_PRINT_TIMEOUT          (100)

class EventStorageManagerPrivate: public StorageManagerPrivate
{
public:
    Q_DECLARE_PUBLIC(EventStorageManager)

    explicit EventStorageManagerPrivate(EventStorageManager *q_ptr)
        : StorageManagerPrivate(q_ptr), _eventTriggerFlag(false),
          printTimerId(-1),
          waitTimerId(-1),
          isWait(false),
          timeoutNum(0),
          generator(NULL)
    {
    }


    QList<WaveformID> getStoreWaveList(WaveformID paramWave);

    QList<EventStorageItem *> eventItemList;
    QMutex mutex;
    bool _eventTriggerFlag;

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;
};

QList<WaveformID> EventStorageManagerPrivate::getStoreWaveList(WaveformID paramWave)
{
    QList<int> displaywaves = layoutManager.getDisplayedWaveformIDs();


    WaveformID calcLeadWave = (WaveformID)ecgParam.getCalcLead();


    QList<WaveformID> storeWaves;

    if (paramWave != WAVE_NONE)
    {
        storeWaves.append(paramWave);
    }

    if (paramWave != calcLeadWave)
    {
        storeWaves.append(calcLeadWave);
    }

    foreach(int id, displaywaves)
    {
        if (storeWaves.count() >= MAX_STORE_WAVE_NUM)
        {
            break;
        }

        if (id <= WAVE_ECG_V6 || id == paramWave)
        {
            continue;
        }

        storeWaves.append((WaveformID) id);
    }

    return storeWaves;
}

EventStorageManager &EventStorageManager::getInstance()
{
    static EventStorageManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new EventStorageManager();
    }
    return *instance;
}

EventStorageManager::~EventStorageManager()
{
}

void EventStorageManager::triggerEvent(EventType type)
{
    Q_D(EventStorageManager);
    EventStorageItem *eventItem = NULL;
    switch (type)
    {
    case EventPhysiologicalAlarm:
        break;
    case EventCodeMarker:
        break;
    case EventRealtimePrint:
        break;
    case EventNIBPMeasurement:
        break;
    case EventWaveFreeze:
        break;
    default:
        qdebug("Unknown event type %d", type);
        return;
    }

    if (eventItem)
    {
        d->mutex.lock();
        d->eventItemList.append(eventItem);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerAlarmEvent(const AlarmInfoSegment &almInfo, WaveformID paramWave, unsigned t)
{
    Q_D(EventStorageManager);
    d->_eventTriggerFlag = true;

    EventStorageItem *item = new EventStorageItem(EventPhysiologicalAlarm,
            d->getStoreWaveList(paramWave),
            almInfo);
    item->startCollectTrendAndWaveformData(t);

    int index = 0;
    systemConfig.getNumValue("Print|PhysiologicalAlarm", index);
    if (index)
    {
        RecordPageGenerator *generator = new TriggerPageGenerator(item);
        if (recorderManager.isPrinting() && !d->isWait)
        {
            if (generator->getPriority() <= recorderManager.getCurPrintPriority())
            {
                generator->deleteLater();
            }
            else
            {
                recorderManager.stopPrint();
                d->generator = generator;
                d->waitTimerId = startTimer(2000); // 等待2000ms
                d->isWait = true;
                item->setWaitForTriggerPrintFlag(true);
            }
        }
        else if (!recorderManager.getPrintStatus())
        {
            recorderManager.addPageGenerator(generator);
            item->setWaitForTriggerPrintFlag(true);
        }
        else
        {
            generator->deleteLater();
        }
    }

    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerCodeMarkerEvent(const char *codeName, unsigned t)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventCodeMarker,
            d->getStoreWaveList(WAVE_NONE),
            codeName);
    item->startCollectTrendAndWaveformData(t);

    int index = 0;
    systemConfig.getNumValue("Print|CoderMarker", index);
    if (index)
    {
        RecordPageGenerator *generator = new TriggerPageGenerator(item);
        if (recorderManager.isPrinting() && !d->isWait)
        {
            if (generator->getPriority() <= recorderManager.getCurPrintPriority())
            {
                generator->deleteLater();
            }
            else
            {
                recorderManager.stopPrint();
                d->generator = generator;
                d->waitTimerId = startTimer(2000); // 等待2000ms
                d->isWait = true;
                item->setWaitForTriggerPrintFlag(false);
            }
        }
        else if (!recorderManager.getPrintStatus())
        {
            recorderManager.addPageGenerator(generator);
            item->setWaitForTriggerPrintFlag(true);
        }
        else
        {
            generator->deleteLater();
        }
    }

    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerRealtimePrintEvent(unsigned t)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventRealtimePrint,
            d->getStoreWaveList(WAVE_NONE));
    item->startCollectTrendAndWaveformData(t);
    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerNIBPMeasurementEvent(unsigned t)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventNIBPMeasurement,
            d->getStoreWaveList(WAVE_NONE));
    item->startCollectTrendAndWaveformData(t);

    int index = 0;
    systemConfig.getNumValue("Print|NIBPReading", index);
    if (index)
    {
        RecordPageGenerator *generator = new TriggerPageGenerator(item);
        if (recorderManager.isPrinting() && !d->isWait)
        {
            if (generator->getPriority() <= recorderManager.getCurPrintPriority())
            {
                generator->deleteLater();
            }
            else
            {
                recorderManager.stopPrint();
                d->generator = generator;
                d->waitTimerId = startTimer(2000); // 等待2000ms
                d->isWait = true;
                item->setWaitForTriggerPrintFlag(false);
            }
        }
        else if (!recorderManager.getPrintStatus())
        {
            recorderManager.addPageGenerator(generator);
            item->setWaitForTriggerPrintFlag(true);
        }
        else
        {
            generator->deleteLater();
        }
    }

    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerWaveFreezeEvent(unsigned t)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventWaveFreeze,
            d->getStoreWaveList(WAVE_NONE));
    item->startCollectTrendAndWaveformData(t);

    int index = 0;
    systemConfig.getNumValue("Print|WaveFreeze", index);
    if (index)
    {
        RecordPageGenerator *generator = new TriggerPageGenerator(item);
        if (recorderManager.isPrinting() && !d->isWait)
        {
            if (generator->getPriority() <= recorderManager.getCurPrintPriority())
            {
                generator->deleteLater();
            }
            else
            {
                recorderManager.stopPrint();
                d->generator = generator;
                d->waitTimerId = startTimer(2000);  // 等待2000ms
                d->isWait = true;
                item->setWaitForTriggerPrintFlag(false);
            }
        }
        else if (!recorderManager.getPrintStatus())
        {
            recorderManager.addPageGenerator(generator);
            item->setWaitForTriggerPrintFlag(true);
        }
        else
        {
            generator->deleteLater();
        }
    }

    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerAlarmOxyCRGEvent(const AlarmInfoSegment &almInfo, OxyCRGEventType type, unsigned t)
{
    Q_D(EventStorageManager);
    QList<WaveformID> waveList;
    waveList.append(WAVE_RESP);
    waveList.append(WAVE_CO2);
    EventStorageItem *item = new EventStorageItem(EventOxyCRG, waveList, type, almInfo);
    item->startCollectTrendAndWaveformData(t);
    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::triggerOxyCRGEvent(OxyCRGEventType type, unsigned t)
{
    Q_D(EventStorageManager);
    QList<WaveformID> waveList;
    waveList.append(WAVE_RESP);
    waveList.append(WAVE_CO2);
    EventStorageItem *item = new EventStorageItem(EventOxyCRG, waveList, type);
    item->startCollectTrendAndWaveformData(t);
    if (item)
    {
        d->mutex.lock();
        d->eventItemList.append(item);
        d->mutex.unlock();
    }
}

void EventStorageManager::checkCompletedEvent()
{
    Q_D(EventStorageManager);
    QMutexLocker locker(&d->mutex);
    while (!d->eventItemList.isEmpty())
    {
        EventStorageItem *item = d->eventItemList.first();

        if (item->checkCompleted())
        {
            d->eventItemList.takeFirst();

            addData(item->getType(), item->getStorageData());

            delete item;
        }
        else
        {
            break;
        }
    }
}

void EventStorageManager::run()
{
    if (dataStorageDirManager.isCurRescueFolderFull())
    {
        discardData();
        return;
    }

    dataStorageDirManager.addDataSize(saveData());
}

bool EventStorageManager::getEventTriggerFlag()
{
    Q_D(EventStorageManager);
    return d->_eventTriggerFlag;
}

void EventStorageManager::clearEventTriggerFlag()
{
    Q_D(EventStorageManager);
    d->_eventTriggerFlag = false;
}

void EventStorageManager::newPatientHandle()
{
    Q_D(EventStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + EVENT_DATA_FILE_NAME, QIODevice::ReadWrite);
}

void EventStorageManager::timerEvent(QTimerEvent *ev)
{
    Q_D(EventStorageManager);
    if (d->printTimerId == ev->timerId())
    {
        if (!recorderManager.isPrinting() || d->timeoutNum == 10) // 1000ms超时处理
        {
            if (!recorderManager.isPrinting())
            {
                recorderManager.addPageGenerator(d->generator);
            }
            else
            {
                d->generator->deleteLater();
                d->generator = NULL;
            }
            killTimer(d->printTimerId);
            d->printTimerId = -1;
            d->timeoutNum = 0;
        }
        d->timeoutNum++;
    }
    else if (d->waitTimerId == ev->timerId())
    {
        d->printTimerId = startTimer(STOP_PRINT_TIMEOUT);
        killTimer(d->waitTimerId);
        d->waitTimerId = -1;
        d->isWait = false;
    }
}

EventStorageManager::EventStorageManager()
    : StorageManager(new EventStorageManagerPrivate(this), new StorageFile())
{
    Q_D(EventStorageManager);
    d->backend->reload(dataStorageDirManager.getCurFolder() + EVENT_DATA_FILE_NAME, QIODevice::ReadWrite);
}
