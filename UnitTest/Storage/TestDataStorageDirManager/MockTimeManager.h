/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/9
 **/

#pragma once
#include "TimeManagerInterface.h"
#include "gmock/gmock.h"

class MockTimeManager : public TimeManagerInterface
{
public:
    MOCK_CONST_METHOD0(getCurTime, unsigned());
    MOCK_METHOD0(setElapsedTime, void());
    MOCK_CONST_METHOD0(getPowerOnSession, PowerOnSession());
    MOCK_CONST_METHOD0(getStartTime, unsigned());
};
