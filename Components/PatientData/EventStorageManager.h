#pragma once
#include "StorageManager.h"
#include "EventDataDefine.h"

class EventStorageManagerPrivate;
class EventStorageManager : public StorageManager
{
public:
    static EventStorageManager &getInstance();
    ~EventStorageManager();

    void triggerEvent(EventType type);

    /**
     * @brief triggerAlarmEvent
     * @param almInfo alarm info
     * @param paramWave param relate wave
     */
    void triggerAlarmEvent(const AlarmInfoSegment &almInfo, WaveformID paramWave);

    //trigger an codemarker event
    void triggerCodeMarkerEvent(const char * codeName);

    //trigger and realtime print event
    void triggerRealtimePrintEvent();

    //trigger a nibp measurement event
    void triggerNIBPMeasurementEvent();

    //trigger a wave freeze event
    void triggerWaveFreezeEvent();

    //trigger alarm oxyCRG event
    void triggerAlarmOxyCRGEvent(const AlarmInfoSegment &almInfo, OxyCRGEventType type);

    //trigger oxyCRG event
    void triggerOxyCRGEvent(OxyCRGEventType type);

    //call periodically to clear completed event
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

private:
    Q_DECLARE_PRIVATE(EventStorageManager)
    EventStorageManager();
};

#define eventStorageManager (EventStorageManager::getInstance())


