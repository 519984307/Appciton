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
    AlarmNormalStatePrivate()
        : alarmIndicator(AlarmIndicatorInterface::getAlarmIndicator()),
          alarmStateMachine(AlarmStateMachineInterface::getAlarmStateMachine()),
          lightManager(LightManagerInterface::getLightManager())
    {}
    ~AlarmNormalStatePrivate(){}

    AlarmIndicatorInterface *alarmIndicator;
    AlarmStateMachineInterface *alarmStateMachine;
    LightManagerInterface *lightManager;
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
    d_ptr->alarmIndicator->setAlarmStatus(ALARM_STATUS_NORMAL);
    d_ptr->alarmIndicator->updateAlarmStateWidget();
    d_ptr->lightManager->enableAlarmAudioMute(false);
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
        d_ptr->alarmIndicator->delLatchPhyAlarm();
        d_ptr->alarmIndicator->phyAlarmResetStatusHandle();
        d_ptr->alarmIndicator->techAlarmResetStatusHandle();
        if (d_ptr->alarmIndicator->getAlarmCount())
        {
            // must has med priority alarm or high priority alarm before enter the reset state
            d_ptr->alarmStateMachine->switchState(ALARM_RESET_STATE);
        }
        break;
    }

    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        d_ptr->alarmIndicator->phyAlarmPauseStatusHandle();
        d_ptr->alarmStateMachine->switchState(ALARM_PAUSE_STATE);
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
        if (d_ptr->alarmStateMachine->isEnableAlarmAudioOff())
        {
            d_ptr->alarmStateMachine->switchState(ALARM_AUDIO_OFF_STATE);
        }
        break;
    }
    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME:
    {
        if (d_ptr->alarmStateMachine->isEnableAlarmOff())
        {
            d_ptr->alarmStateMachine->switchState(ALARM_OFF_STATE);
        }
        break;
    }
    default:
        break;
    }
}
