/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/8
 **/

#pragma once

#include "ConfigManagerInterface.h"
#include "gmock/gmock.h"

class MockConfigManager : public ConfigManagerInterface
{
public:
    MOCK_METHOD1(getConfig, Config *(PatientType));
    MOCK_METHOD0(getCurConfig, Config&(void));
    MOCK_METHOD1(loadConfig, void(PatientType));
};
