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
#include "AlarmAudioOffState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "IConfig.h"
#include "AlarmDefine.h"
#include "SoundManager.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmAudioOffState::AlarmAudioOffState() : AlarmState(ALARM_AUDIO_OFF_STATE)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmAudioOffState::~AlarmAudioOffState()
{
}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmAudioOffState::enter()
{
    // 定时发出报警声音关闭提示音
    int time = ALARM_CLOSE_PROMPT_OFF;
    systemConfig.getNumValue("Alarms|AlarmOffPrompting", time);
    if (time != ALARM_CLOSE_PROMPT_OFF && time < ALARM_CLOSE_PROMPT_NR)
    {
        beginTimer(time * 5 * 60 * 1000);
    }

    alarmIndicator.setAlarmStatus(ALARM_STATUS_AUDIO_OFF);
    alarmIndicator.clearAlarmPause();
}


/**************************************************************************************************
 * 状态退出。
 *************************************************************************************************/
void AlarmAudioOffState::exit()
{
    endTimer();
}

/**************************************************************************************************
 * 定时器事件处理。
 *************************************************************************************************/
void AlarmAudioOffState::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == getTimerID())
    {
        // soundManager.playSound(SOUND_TYPE_TECH_ALARM, ALARM_PRIO_LOW);
    }
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmAudioOffState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
#if 1
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        alarmIndicator.techAlarmResetStatusHandle();
        if (alarmIndicator.hasLatchPhyAlarm())
        {
            alarmIndicator.delLatchPhyAlarm();
        }
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
        bool ret = alarmIndicator.techAlarmPauseStatusHandle();
        if (alarmIndicator.hasLatchPhyAlarm())
        {
            alarmIndicator.delLatchPhyAlarm();
            ret |= true;
        }
        break;
    }
#endif

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


