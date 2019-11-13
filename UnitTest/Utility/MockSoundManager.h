/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/

#pragma once
#include "Framework/Sound/SoundManagerInterface.h"
#include "gmock/gmock.h"

class MockSoundManager : public SoundManagerInterface
{
public:
    MOCK_METHOD2(setVolume, void(SoundType, VolumeLevel));
    MOCK_METHOD1(getVolume, VolumeLevel(SoundType));
    MOCK_METHOD2(updateAlarm, void(bool, AlarmPriority));
    MOCK_METHOD0(keyPressTone, void());
};
