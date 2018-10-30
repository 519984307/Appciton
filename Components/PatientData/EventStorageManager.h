/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/20
 **/

#pragma once
#include "StorageManager.h"
#include "EventDataDefine.h"

class EventStorageManagerPrivate;
class EventStorageManager : public StorageManager
{
    Q_OBJECT
public:
    static EventStorageManager &getInstance();
    ~EventStorageManager();

    void triggerEvent(EventType type);

    /**
     * @brief triggerAlarmEvent
     * @param almInfo alarm info
     * @param paramWave param relate wave
     */
    void triggerAlarmEvent(const AlarmInfoSegment &almInfo, WaveformID paramWave, unsigned t);

    // trigger an codemarker event
    void triggerCodeMarkerEvent(const char * codeName, unsigned t);

    // trigger and realtime print event
    void triggerRealtimePrintEvent(unsigned t);

    // trigger a nibp measurement event
    void triggerNIBPMeasurementEvent(unsigned t);

    // trigger a wave freeze event
    void triggerWaveFreezeEvent(unsigned t);

    // trigger alarm oxyCRG event
    void triggerAlarmOxyCRGEvent(const AlarmInfoSegment &almInfo, OxyCRGEventType type, unsigned t);

    // trigger oxyCRG event
    void triggerOxyCRGEvent(OxyCRGEventType type, unsigned t);

    // call periodically to clear completed event
    void checkCompletedEvent(void);

    void run();

    /**
     * @brief getEventTriggerFlag 获取事件触发标识
     * @return
     */
    bool getEventTriggerFlag(void);

    /**
     * @brief clearEventTriggerFlag 清空事件触发标识
     */
    void clearEventTriggerFlag(void);

    /**
     * @brief getPhyAlarmMessage get the alarm message
     * @param paramId param id
     * @param alarmType alarm type
     * @param isOneShot is oneshot alarm or not
     * @return the alarm message
     */
    QString getPhyAlarmMessage(ParamID paramId, int alarmType, bool isOneShot);

protected:
    /*reimplement*/
    void newPatientHandle();

private:
    Q_DECLARE_PRIVATE(EventStorageManager)
    EventStorageManager();
};

#define eventStorageManager (EventStorageManager::getInstance())


