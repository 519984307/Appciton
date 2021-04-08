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
#include "AlarmIndicatorInterface.h"
#include "AlarmStateMachineInterface.h"
#include <QTimerEvent>
#include "IConfig.h"

class AlarmPauseStatePrivate
{
public:
    AlarmPauseStatePrivate()
        : leftPauseTime(-1)
    {}

    /**
     * @brief getAlarmPausetime convert the Alarm Pause Time to seconds
     * @param time time
     * @return the corresponse seconds
     */
    int getAlarmPausetime(AlarmPauseTime time)
    {
        switch (time)
        {
        case ALARM_PAUSE_TIME_1MIN:
            return 60;
        case ALARM_PAUSE_TIME_2MIN:
            return 120;
        case ALARM_PAUSE_TIME_3MIN:
            return 180;
        case ALARM_PAUSE_TIME_5MIN:
            return 300;
        case ALARM_PAUSE_TIME_10MIN:
            return 600;
        case ALARM_PAUSE_TIME_15MIN:
            return 900;
        default:
            break;
        }
        return 120;
    }

    int leftPauseTime;
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmPauseState::AlarmPauseState()
    : AlarmState(ALARM_PAUSE_STATE),
      d_ptr(new AlarmPauseStatePrivate())
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmPauseState::~AlarmPauseState()
{
    delete d_ptr;
}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmPauseState::enter()
{
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    alarmIndicator->setAlarmStatus(ALARM_STATUS_PAUSE);
    beginTimer(1000);
    int index = ALARM_PAUSE_TIME_2MIN;
    systemConfig.getNumValue("Alarms|AlarmPauseTime", index);
    d_ptr->leftPauseTime = d_ptr->getAlarmPausetime(static_cast<AlarmPauseTime>(index));
    alarmIndicator->updateAlarmPauseTime(d_ptr->leftPauseTime);
    alarmIndicator->removeAllAlarmResetStatus();
}

void AlarmPauseState::exit()
{
    endTimer();
    d_ptr->leftPauseTime = -1;
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    alarmIndicator->updateAlarmPauseTime(d_ptr->leftPauseTime);
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmPauseState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
    switch (event)
    {
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        alarmIndicator->clearAlarmPause();
        alarmIndicator->phyAlarmResetStatusHandle();
        alarmIndicator->techAlarmResetStatusHandle();
        if (alarmIndicator->getAlarmCount())
        {
            // must has med priority alarm or high priority alarm before enter the reset state
            alarmStateMachine->switchState(ALARM_RESET_STATE);
        }
        break;
    }

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        alarmIndicator->phyAlarmPauseStatusHandle();
        alarmStateMachine->switchState(ALARM_NORMAL_STATE);
        break;
    }

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME:
        if (alarmStateMachine->isEnableAlarmAudioOff())
        {
            alarmStateMachine->switchState(ALARM_AUDIO_OFF_STATE);
        }
        break;

    case ALARM_STATE_EVENT_NEW_PHY_ALARM:
    case ALARM_STATE_EVENT_NEW_TECH_ALARM:
        alarmIndicator->clearAlarmPause();
        alarmStateMachine->switchState(ALARM_NORMAL_STATE);
        break;

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME:
        if (alarmStateMachine->isEnableAlarmOff())
        {
            alarmStateMachine->switchState(ALARM_OFF_STATE);
        }
        break;

    default:
        break;
    }
}

void AlarmPauseState::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == getTimerID())
    {
        AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
        d_ptr->leftPauseTime--;
        alarmIndicator->updateAlarmPauseTime(d_ptr->leftPauseTime);
        if (d_ptr->leftPauseTime <= 0)
        {
            AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
            alarmStateMachine->switchState(ALARM_NORMAL_STATE);
        }
    }
}
