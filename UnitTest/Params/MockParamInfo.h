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

#include "gmock/gmock.h"
#include "ParamInfoInterface.h"

class MockParamInfo : public ParamInfoInterface
{
public:
    MOCK_METHOD1(getParamID, ParamID(SubParamID));
};
