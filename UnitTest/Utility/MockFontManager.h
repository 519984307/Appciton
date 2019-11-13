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
#include "FontManagerInterface.h"
#include "gmock/gmock.h"

class MockFontManager : public FontManagerInterface
{
public:
    MOCK_METHOD2(textFont, QFont(int, bool));
    MOCK_METHOD1(getFontSize, int(int));
};
