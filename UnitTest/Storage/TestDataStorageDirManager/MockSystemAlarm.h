/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/4/11
 **/

#pragma once
#include "gmock/gmock.h"
#include "AlarmParamIFace.h"

class MockSystemAlarm : public AlarmOneShotIFace
{
public:
    MOCK_METHOD1(toString, const char *(int));
    MOCK_METHOD0(getAlarmSourceName, QString(void));
    MOCK_METHOD0(getParamID, ParamID(void));
    MOCK_METHOD1(getWaveformID, WaveformID(int));
    MOCK_METHOD1(getAlarmPriority, AlarmPriority(int));
    MOCK_METHOD1(isAlarmEnable, bool(int));
    MOCK_METHOD1(getSubParamID, SubParamID(int));
    MOCK_METHOD0(getAlarmSourceNR, int(void));
};
