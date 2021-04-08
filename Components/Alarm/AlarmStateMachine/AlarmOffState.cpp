/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#include <QTimerEvent>
#include "AlarmOffState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "IConfig.h"
#include "AlarmDefine.h"
#include "SoundManager.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmOffState::AlarmOffState() : AlarmState(ALARM_OFF_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmOffState::~AlarmOffState()
{
}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmOffState::enter()
{
    // 定时发出报警关闭提示音
    int time = ALARM_CLOSE_PROMPT_OFF;
    systemConfig.getNumValue("Alarms|AlarmOffPrompting", time);
    if (time > ALARM_CLOSE_PROMPT_OFF && time < ALARM_CLOSE_PROMPT_NR)
    {
        beginTimer(time * 5 * 60 * 1000);
    }

    alarmIndicator.setAlarmStatus(ALARM_STATUS_OFF);
    alarmIndicator.delAllPhyAlarm();
    alarmIndicator.updateAlarmPauseTime(INT_MAX);
}

/**************************************************************************************************
 * 状态退出。
 *************************************************************************************************/
void AlarmOffState::exit()
{
    endTimer();
    alarmIndicator.updateAlarmPauseTime(-1);
}

/**************************************************************************************************
 * 定时器事件处理。
 *************************************************************************************************/
void AlarmOffState::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == getTimerID())
    {
        //    soundManager.playSound(SOUND_TYPE_ALARM_OFF_PROMPT, ALARM_PRIO_PROMPT);
    }
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmOffState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
#if 1
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        alarmIndicator.delLatchPhyAlarm();
        alarmIndicator.techAlarmResetStatusHandle();
        break;
    }

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        alarmStateMachine.switchState(ALARM_NORMAL_STATE);
        break;
    }
#else
    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        if (alarmIndicator.techAlarmPauseStatusHandle())
        {
            return;
        }
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

    default:
        break;
    }
}






