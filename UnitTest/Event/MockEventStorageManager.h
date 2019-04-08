/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/4/1
 **/

#pragma once
#include "EventStorageManagerInterface.h"
#include "gmock/gmock.h"

class MockEventStorageManager : public EventStorageManagerInterface
{
public:
    MOCK_METHOD3(triggerAlarmEvent, void(const AlarmInfoSegment &almInfo, WaveformID paramWave, unsigned t));
};
