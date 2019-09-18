/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/28
 **/

#pragma once

#include "AlarmStateDefine.h"

class AlarmStateMachineInterface
{
public:
    AlarmStateMachineInterface(){}
    virtual ~AlarmStateMachineInterface(){}

    /**
     * @brief registerAlarmStateMachine register a alarm state machine
     * @return
     */
    static AlarmStateMachineInterface *registerAlarmStateMachine(AlarmStateMachineInterface *);

    /**
     * @brief getAlarmStateMachine get alarm state machine handle
     * @return
     */
    static AlarmStateMachineInterface *getAlarmStateMachine();

    /**
     * @brief switchState 状态切换
     * @param type
     */
    virtual void switchState(ALarmStateType type) = 0;

    /**
     * @brief isEnableAlarmAudioOff 获取报警音使能状态
     * @return
     */
    virtual bool isEnableAlarmAudioOff() const = 0;

    /**
     * @brief isEnableAlarmOff 获取报警使能状态
     * @return
     */
    virtual bool isEnableAlarmOff() const = 0;

    /**
     * @brief handAlarmEvent 处理报警事件
     * @param event
     * @param data
     * @param len
     */
    virtual void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len) = 0;
};
