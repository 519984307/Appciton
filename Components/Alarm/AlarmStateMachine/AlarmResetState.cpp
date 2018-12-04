/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/7
 **/

#include "AlarmResetState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "LightManager.h"

AlarmResetState::AlarmResetState() : AlarmState(ALARM_RESET_STATE)
{
}

AlarmResetState::~AlarmResetState()
{
}

void AlarmResetState::enter()
{
    alarmIndicator.setAlarmStatus(ALARM_STATUS_RESET);
    lightManager.enableAlarmAudioMute(false);
}

void AlarmResetState::handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len)
{
    Q_UNUSED(data);
    Q_UNUSED(len);

    switch (event)
    {
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        // already in reset state, do nothing
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

    case ALARM_STATE_EVENT_NEW_PHY_ALARM:
    case ALARM_STATE_EVENT_NEW_TECH_ALARM:
        // new alarm arrived, switch to normal state
        alarmStateMachine.switchState(ALARM_NORMAL_STATE);
        break;

    default:
        break;
    }
}
