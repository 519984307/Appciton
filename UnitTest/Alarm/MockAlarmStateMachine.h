/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/1
 **/



#pragma once

#include "AlarmStateMachineInterface.h"
#include "gmock/gmock.h"

class MockAlarmStateMachine : public AlarmStateMachineInterface
{
public:
    MOCK_METHOD1(switchState, void(ALarmStateType type));
    MOCK_CONST_METHOD0(isEnableAlarmAudioOff, bool());
    MOCK_CONST_METHOD0(isEnableAlarmOff, bool());
    MOCK_METHOD3(handAlarmEvent, void(AlarmStateEvent, unsigned char *, unsigned));
};
