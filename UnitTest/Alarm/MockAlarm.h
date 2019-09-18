/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/22
 **/

#pragma once
#include "AlarmInterface.h"
#include "gmock/gmock.h"

class MockAlarm : public AlarmInterface
{
public:
    MOCK_METHOD1(setAlarmLightOnAlarmReset, void(bool));
    MOCK_METHOD0(getAlarmLightOnAlarmReset, bool(void));
    MOCK_METHOD1(addAlarmStatus, void(AlarmStatus));
    MOCK_METHOD0(removeAllLimitAlarm, void(void));
};
