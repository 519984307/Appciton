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
#include "AlarmIndicatorInterface.h"
#include <gmock/gmock.h>

class MockAlarmIndicator : public AlarmIndicatorInterface
{
public:
    MOCK_METHOD0(delLatchPhyAlarm, void(void));
    MOCK_METHOD1(setAlarmStatus, void(AlarmStatus status));
    MOCK_METHOD0(updateAlarmStateWidget, void(void));
    MOCK_METHOD0(phyAlarmResetStatusHandle, bool(void));
    MOCK_METHOD0(techAlarmResetStatusHandle, bool(void));
    MOCK_METHOD1(getAlarmCount, int(AlarmType type));
    MOCK_METHOD0(getAlarmCount, int(void));
    MOCK_METHOD1(getAlarmCount, int(AlarmPriority priority));
    MOCK_METHOD0(phyAlarmPauseStatusHandle, bool(void));
};
