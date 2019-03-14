/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/12
 **/

#pragma once
#include "LightManagerInterface.h"
#include "gmock/gmock.h"

class MockLightManager : public LightManagerInterface
{
public:
    MOCK_METHOD1(setProvider, void(LightProviderIFace *));
    MOCK_METHOD2(updateAlarm, void(bool, AlarmPriority));
    MOCK_METHOD1(enableAlarmAudioMute, void(bool));
    MOCK_METHOD3(sendCmdData, void(unsigned char, const unsigned char* , unsigned int));
    MOCK_METHOD1(stopHandlingLight, void(bool));
};
