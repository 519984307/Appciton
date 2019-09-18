/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/15
 **/

#pragma once
#include "WindowManagerInterface.h"
#include "gmock/gmock.h"

class MockWindowManager : public WindowManagerInterface
{
public:
    MOCK_METHOD2(showWindow, void(Dialog *, ShowBehavior));
    MOCK_METHOD0(topWindow, Dialog *(void));
};
