/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/3
 **/


#include <QTimer>
#include "AlarmDefine.h"
#include "AlarmStateMachine.h"
#include "AlarmNormalState.h"
#include "AlarmPauseState.h"
#include "AlarmAudioOffState.h"
#include "AlarmOffState.h"
#include "AlarmResetState.h"
#include "IConfig.h"
#include "SystemManager.h"
#include "Debug.h"

AlarmStateMachine *AlarmStateMachine::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmStateMachine::AlarmStateMachine()
{
    _alarmStateMap.clear();

    AlarmNormalState *normalState = new AlarmNormalState();
    AlarmPauseState *pauseState = new AlarmPauseState();
    AlarmAudioOffState *audioOffState = new AlarmAudioOffState();
    AlarmOffState *offState = new AlarmOffState();
    AlarmResetState *resetState = new AlarmResetState();

    _alarmStateMap.insert(normalState->type(), normalState);
    _alarmStateMap.insert(pauseState->type(), pauseState);
    _alarmStateMap.insert(audioOffState->type(), audioOffState);
    _alarmStateMap.insert(offState->type(), offState);
    _alarmStateMap.insert(resetState->type(), resetState);
    _currentState = normalState;

    _timer = new QTimer();
    _timer->setInterval(100);
    connect(_timer, SIGNAL(timeout()), this, SLOT(_timeOut()));

    _pressTime = 0;

    systemConfig.getNumValue("Alarms|EnableAlarmAudioOff", _isEnableAlarmAudioOff);
    systemConfig.getNumValue("Alarms|EnableAlarmOff", _isEnableAlarmOff);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmStateMachine::~AlarmStateMachine()
{
    AlarmStateMap::iterator it = _alarmStateMap.begin();
    for (; it != _alarmStateMap.end(); ++it)
    {
        delete it.value();
    }
    _alarmStateMap.clear();

    if (NULL != _timer)
    {
        delete _timer;
        _timer = NULL;
    }
}

/**************************************************************************************************
 * 状态开始。
 *************************************************************************************************/
void AlarmStateMachine::start()
{
    int value = 0;
    systemConfig.getNumValue("PrimaryCfg|Alarm|AlarmStatus", value);

    int alarmOffAtPowerOn = 0;
    systemConfig.getNumValue("Alarms|AlarmOffAtPowerOn", alarmOffAtPowerOn);

    int enableAlarmOff = 0;
    systemConfig.getNumValue("Alarms|EnableAlarmOff", enableAlarmOff);
    if (alarmOffAtPowerOn && enableAlarmOff)
    {
        value = ALARM_OFF_STATE;
        systemConfig.setNumValue("Alarms|AlarmOffAtPowerOn", 0);
    }

    AlarmStateMap::iterator it = _alarmStateMap.find((ALarmStateType)value);
    if (it != _alarmStateMap.end() && NULL != it.value())
    {
        _currentState = it.value();
    }

    _currentState->enter();
}

/**************************************************************************************************
 * 定时器。
 *************************************************************************************************/
void AlarmStateMachine::_timeOut()
{
    ++_pressTime;
    if (_pressTime > 10 && _pressTime <= 11)  // 1-3s
    {
        if (NULL != _currentState)
        {
            _currentState->handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME, 0, 0);
        }
    }
    else if (_pressTime > 28)  // >3s,系统有延迟这里少等待200ms
    {
        if (NULL != _currentState)
        {
            _currentState->handAlarmEvent(ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME, 0, 0);
        }

        _timer->stop();
        _pressTime = 0;
    }
}

/**************************************************************************************************
 * 状态切换。
 *************************************************************************************************/
void AlarmStateMachine::switchState(ALarmStateType type)
{
    AlarmStateMap::iterator it = _alarmStateMap.find(type);
    if (it == _alarmStateMap.end())
    {
        return;
    }

    if (type != it.value()->type() || NULL == it.value())
    {
        return;
    }

    if (it.value() == _currentState)
    {
        return;
    }

    if (NULL != _currentState)
    {
        _currentState->exit();
    }

    _currentState = it.value();
    _currentState->enter();
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmStateMachine::handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len)
{
    switch (event)
    {
    case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
    {
        _pressTime = 0;
        _timer->start();
        break;
    }
    case ALARM_STATE_EVENT_MUTE_BTN_RELEASED:
    {
        _timer->stop();
        break;
    }
    default:
        break;
    }

    if (NULL != _currentState)
    {
        _currentState->handAlarmEvent(event, data, len);
    }
}

