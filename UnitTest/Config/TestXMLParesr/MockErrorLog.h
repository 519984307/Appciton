/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/6
 **/

#pragma once
#include "Framework/ErrorLog/ErrorLogInterface.h"
#include "gmock/gmock.h"

class MockErrorLog : public ErrorLogInterface
{
public:
    MOCK_METHOD0(count, int());
    MOCK_METHOD1(getLog, ErrorLogItemBase*(int index));
    MOCK_METHOD1(append, void(ErrorLogItemBase *item));
    MOCK_METHOD0(clear, void());
    MOCK_METHOD1(getTypeCount, int(unsigned int type));
    MOCK_METHOD1(getLatestLog, ErrorLogItemBase*(unsigned int type));
    MOCK_METHOD0(getSummary, Summary());
};
