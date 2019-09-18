/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/18
 **/

#pragma once
#include "AlarmState.h"
#include "gmock/gmock.h"
#include "gmock/gmock-more-actions.h"
#include "AlarmNormalState.h"

using ::testing::_;
using ::testing::Invoke;

class MockAlarmState : public AlarmNormalState
{
public:
    MockAlarmState()
    {
//        ON_CALL(*this, enter()).WillByDefault(Invoke(&fakeState, &AlarmState::enter));
        ON_CALL(*this, handAlarmEvent(_, _, _)).WillByDefault(
                    Invoke(&fakeState, &AlarmState::handAlarmEvent)
                    );
    }
    MOCK_METHOD0(enter, void(void));
    MOCK_METHOD3(handAlarmEvent, void(AlarmStateEvent, unsigned char *, unsigned));

private:
    AlarmState fakeState; // Keeps an instance of the fake in the mock.
};
