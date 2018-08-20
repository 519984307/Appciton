/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#include "AlarmNormalState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "LightManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmNormalState::AlarmNormalState() : AlarmState(ALARM_NORMAL_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmNormalState::~AlarmNormalState()
{
}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmNormalState::enter()
{
    alarmIndicator.setAudioStatus(ALARM_AUDIO_NORMAL);
    lightManager.enableAlarmAudioMute(false);
}

void AlarmNormalState::exit()
{
    lightManager.enableAlarmAudioMute(true);
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmNormalState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
#if 1
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        //删除栓锁报警
        alarmIndicator.delLatchPhyAlarm();
        alarmIndicator.techAlarmPauseStatusHandle();
        break;
    }

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        if (alarmIndicator.phyAlarmPauseStatusHandle())
        {
            alarmStateMachine.switchState(ALARM_PAUSE_STATE);
        }
        break;
    }
#else
    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        //删除栓锁报警
        alarmIndicator.delLatchPhyAlarm();
        alarmIndicator.techAlarmPauseStatusHandle();

        if (alarmIndicator.phyAlarmPauseStatusHandle())
        {
            alarmStateMachine.switchState(ALARM_PAUSE_STATE);
        }
        break;
    }
#endif

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME:
        if (alarmStateMachine.isEnableAlarmAudioOff())
        {
            alarmStateMachine.switchState(ALARM_AUDIO_OFF_STATE);
        }
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
