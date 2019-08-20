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
#include "AlarmIndicatorInterface.h"
#include "AlarmStateMachineInterface.h"
#include "LightManagerInterface.h"

class AlarmNormalStatePrivate
{
public:
    AlarmNormalStatePrivate(){}
    ~AlarmNormalStatePrivate(){}
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmNormalState::AlarmNormalState()
    : AlarmState(ALARM_NORMAL_STATE)
    , d_ptr(new AlarmNormalStatePrivate())
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmNormalState::~AlarmNormalState()
{
    delete d_ptr;
}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmNormalState::enter()
{
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    alarmIndicator->setAlarmStatus(ALARM_STATUS_NORMAL);
    alarmIndicator->updateAlarmAudioState();
    LightManagerInterface *lightManager = LightManagerInterface::getLightManager();
    lightManager->enableAlarmAudioMute(false);
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmNormalState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    AlarmIndicatorInterface *alarmIndicator = AlarmIndicatorInterface::getAlarmIndicator();
    AlarmStateMachineInterface *alarmStateMachine = AlarmStateMachineInterface::getAlarmStateMachine();
    switch (event)
    {
#if 1
    case ALARM_STATE_EVENT_RESET_BTN_PRESSED:
    {
        //删除栓锁报警
        alarmIndicator->delLatchPhyAlarm();
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
        alarmStateMachine->switchState(ALARM_PAUSE_STATE);
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
    {
        if (!alarmIndicator->isAlarmAudioState())
        {
            alarmIndicator->setAlarmAudioState(true);
        }
        break;
    }
    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME:
    {
        if (alarmStateMachine->isEnableAlarmOff())
        {
            alarmStateMachine->switchState(ALARM_OFF_STATE);
        }
        break;
    }
    default:
        break;
    }
}
