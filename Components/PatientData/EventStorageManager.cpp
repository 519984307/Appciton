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
#include "ECGDupParam.h"
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
          generator(NULL),
          item(NULL)
    {
    }


    QList<WaveformID> getStoreWaveList(WaveformID paramWave,  int subID = -1);

    QList<EventStorageItem *> eventItemList;
    QMutex mutex;
    bool _eventTriggerFlag;

    int printTimerId;
    int waitTimerId;
    bool isWait;
    int timeoutNum;
    RecordPageGenerator *generator;
    EventStorageItem *item;
};

QList<WaveformID> EventStorageManagerPrivate::getStoreWaveList(WaveformID paramWave, int subID)
{
    QList<int> displaywaves = layoutManager.getDisplayedWaveformIDs();


    WaveformID calcLeadWave = (WaveformID)ecgParam.getCalcLead();


    QList<WaveformID> storeWaves;

    if (paramWave != WAVE_NONE)
    {
        if (subID == SUB_PARAM_HR_PR && ecgDupParam.getCurHRSource() == HR_SOURCE_SPO2)
        {
            // 如果时PR时，第一条波形为pleth
            paramWave = WAVE_SPO2;
        }
        storeWaves.append(paramWave);
    }

    if (paramWave != calcLeadWave)
    {
        storeWaves.append(calcLeadWave);
    }

    // HR报警且来源为ECG,体温报警,只打印计算导联
    if ((subID == SUB_PARAM_HR_PR && (ecgDupParam.getCurHRSource() == HR_SOURCE_ECG)) ||
            subID == SUB_PARAM_T1 ||
            subID == SUB_PARAM_T2 ||
            subID == SUB_PARAM_TD)
    {
        return storeWaves;
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
        EventStorageManagerInterface *old = registerEventStorageManager(instance);
        if (old)
        {
            delete old;
        }
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
            d->getStoreWaveList(paramWave, almInfo.subParamID),
            almInfo);
    item->startCollectTrendAndWaveformData(t);

    int index = 0;
    systemConfig.getNumValue("Print|PhysiologicalAlarm", index);
    if (index && recorderManager.isConnected())   // don't start trigger printing when the printer is not connected
    {
        RecordPageGenerator *generator = new TriggerPageGenerator(item);
        // 是否处于正在打印且不处于等待状态
        if (recorderManager.isPrinting() && !d->isWait)
        {
            // 打印优先级小于当前打印时不处理
            if (generator->getPriority() <= recorderManager.getCurPrintPriority())
            {
                generator->deleteLater();
            }
            // 打印优先级大于当前打印优先级时停止当前打印,等待2000ms打印此刻打印任务
            else
            {
                recorderManager.stopPrint();
                d->generator = generator;
                d->waitTimerId = startTimer(2000); // 等待2000ms
                d->isWait = true;
                d->item = item;
            }
        }
        // 当前无打印任务
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
    if (index && recorderManager.isConnected())  // don't start trigger printing when the printer is not connected
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
                d->item = item;
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

void EventStorageManager::triggerNIBPMeasurementEvent(unsigned t, NIBPOneShotType result)
{
    Q_D(EventStorageManager);

    EventStorageItem *item = new EventStorageItem(EventNIBPMeasurement,
            d->getStoreWaveList(WAVE_NONE), result);
    item->startCollectTrendAndWaveformData(t);

    int index = 0;
    systemConfig.getNumValue("Print|NIBPReading", index);
    if (index && recorderManager.isConnected())  // don't start trigger printing when the printer is not connected
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
                d->item = item;
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
    if (index && recorderManager.isConnected())     // don't start trigger printing when the printer is not connected
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
                d->item = item;
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

            // 事件发生时刻的文件路径与存储时刻的文件路径是否一致
            if (item->getEventFolderName() == dataStorageDirManager.getCurFolder())
            {
                addData(item->getType(), item->getStorageData(), item->getExtraData());
            }
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

void EventStorageManager::clearEventItemList()
{
    Q_D(EventStorageManager);
    if (!d->eventItemList.isEmpty())
    {
        d->eventItemList.clear();
    }
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
            // 没有打印任务时打印当前任务
            if (!recorderManager.isPrinting())
            {
                recorderManager.addPageGenerator(d->generator);
                if (d->item)
                {
                    d->item->setWaitForTriggerPrintFlag(true);
                }
            }
            // 超时时不处理当前打印任务
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
