/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/28
 **/

#include "SystemAlarm.h"
#include "SystemManager.h"
#include "KeyActionManager.h"
#include "AlarmConfig.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemAlarm::SystemAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SystemAlarm::~SystemAlarm()
{
}

/**************************************************************************************************
 * 获取报警类型。
 *************************************************************************************************/
QString SystemAlarm::getAlarmSourceName(void)
{
    return "SystemOneAlarm";
}

/**************************************************************************************************
 * 获取报警类型。
 *************************************************************************************************/
AlarmPriority SystemAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
    case SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP:
    case SYSTEM_ONE_SHOT_ALARM_SEND_COMMUNICATION_STOP:
        return ALARM_PRIO_MED;

    case POWERUP_PANEL_RECORD_PRESSED:
        return ALARM_PRIO_LOW;

    case SOME_LIMIT_ALARM_DISABLED:
        return ALARM_PRIO_PROMPT;

    case STORAGE_SPACE_FULL:
    case SYSTEM_ONE_SHOT_ALARM_AUDIO_OFF:
    case SYSTEM_ONE_SHOT_ALARM_OFF:
        return ALARM_PRIO_PROMPT;

    default:
        return ALARM_PRIO_LOW;
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
bool SystemAlarm::isAlarmed(int id)
{
    bool keyPressed = false;
//    static bool keyStunk[POWERUP_PANEL_ENERGY_DN_PRESSED - POWERUP_PANEL_SYNC_PRESSED + 1];

    switch (id)
    {
    case POWERUP_PANEL_RECORD_PRESSED:
        keyPressed = keyActionManager.checkKeyPressed(KEY_F9_PRESSED);
        break;
    case SOME_LIMIT_ALARM_DISABLED:
        return alarmConfig.hasLimitAlarmDisable();
    default:
        return AlarmOneShotIFace::isAlarmed(id);
    }

    bool bret = keyPressed;
    return bret;
}

/**************************************************************************************************
 * 获取报警类型。
 *************************************************************************************************/
AlarmType SystemAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

static const char *alarmLimitOneshotStr[] =
{
    "SomeLimitAlarmDisabled",
    "PowerupRecorderPressed",
    "PowerupCommunicationStop",
    "CurrentPatientStorageSpaceFull",
    "AlarmAudioOff",
    "AlarmOff"
};

/**************************************************************************************************
 * 获取报警字串。
 *************************************************************************************************/
const char *SystemAlarm::toString(int id)
{
    if (id >= SYSTEM_ONE_SHOT_ALARM_NR)
    {
        return NULL;
    }

    return alarmLimitOneshotStr[id];
}
