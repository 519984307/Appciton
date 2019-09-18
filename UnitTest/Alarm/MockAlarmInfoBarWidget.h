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
#include "AlarmInfoBarWidget.h"
#include "gmock/gmock.h"

class MockAlarmInfoBarWidget : public AlarmInfoBarWidget
{
public:
    MOCK_METHOD0(clear, void(void));
    MOCK_METHOD1(display, void(AlarmInfoNode &));
    MOCK_METHOD0(updateList, void(void));
};
