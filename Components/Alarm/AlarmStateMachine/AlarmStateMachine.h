/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/3
 **/


#pragma once
#include <QMap>
#include <QObject>
#include "AlarmStateMachineInterface.h"

// 报警状态状态机
class QTimer;
class AlarmState;
class AlarmStateMachine : public QObject , public AlarmStateMachineInterface
{
    Q_OBJECT

public:
    static AlarmStateMachine &getInstance();

    ~AlarmStateMachine();

    // 状态机开始
    void start();

    // 事件处理
    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);

    // 状态切换
    void switchState(ALarmStateType type);

    // 获取报警使能状态
    inline bool isEnableAlarmAudioOff() const
    {
        return _isEnableAlarmAudioOff;
    }
    inline bool isEnableAlarmOff() const
    {
        return _isEnableAlarmOff;
    }

private slots:
    void _timeOut();

private:
    AlarmStateMachine();

private:
    bool _isEnableAlarmAudioOff;
    bool _isEnableAlarmOff;
    unsigned _pressTime;//按键时间

    AlarmState *_currentState;   //当前活跃的状态

    typedef QMap<ALarmStateType, AlarmState *> AlarmStateMap;
    AlarmStateMap _alarmStateMap;

    QTimer *_timer;
};
#define alarmStateMachine (AlarmStateMachine::getInstance())
