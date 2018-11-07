/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#include "AlarmPauseState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "LightManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmPauseState::AlarmPauseState() : AlarmState(ALARM_PAUSE_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmPauseState::~AlarmPauseState()
{
}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmPauseState::enter()
{
    alarmIndicator.setAudioStatus(ALARM_AUDIO_SUSPEND);
    lightManager.enableAlarmAudioMute(true);
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmPauseState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
#if 1
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        // 有栓锁的报警和新的技术报警
        alarmIndicator.techAlarmPauseStatusHandle();
        if (alarmIndicator.hasLatchPhyAlarm())
        {
            alarmIndicator.delLatchPhyAlarm();
        }

        // 有处于未暂停的报警
        if (alarmIndicator.hasNonPausePhyAlarm())
        {
            alarmIndicator.phyAlarmPauseStatusHandle();
        }

        break;
    }

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        alarmIndicator.phyAlarmPauseStatusHandle();
        alarmStateMachine.switchState(ALARM_NORMAL_STATE);
        break;
    }
#else
    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        // 有栓锁的报警和新的技术报警
        bool ret = alarmIndicator.techAlarmPauseStatusHandle();
        if (alarmIndicator.hasLatchPhyAlarm())
        {
            alarmIndicator.delLatchPhyAlarm();
            ret |= true;
        }

        // 有处于未暂停的报警
        if (alarmIndicator.hasNonPausePhyAlarm())
        {
            alarmIndicator.phyAlarmPauseStatusHandle();
            ret |= true;
        }

        if (ret)
        {
            return;
        }

        alarmIndicator.phyAlarmPauseStatusHandle();
        alarmStateMachine.switchState(ALARM_NORMAL_STATE);
        break;
    }
#endif

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME:
        if (alarmStateMachine.isEnableAlarmAudioOff())
        {
            alarmStateMachine.switchState(ALARM_AUDIO_OFF_STATE);
        }
        break;

    case ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED:
    case ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM:
        alarmStateMachine.switchState(ALARM_NORMAL_STATE);
        break;

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME:
        if (alarmStateMachine.isEnableAlarmOff())
        {
            alarmStateMachine.switchState(ALARM_OFF_STATE);
        }
        break;

    default:
        break;
    }
}




