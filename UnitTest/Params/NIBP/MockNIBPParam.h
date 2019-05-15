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
#include "NIBPParamInterface.h"
#include "gmock/gmock.h"
#include "NIBPDefine.h"
class MockNIBPParam : public NIBPParamInterface
{
public:
    MOCK_METHOD0(stopMeasure, void(void));
    MOCK_METHOD1(setText, void(QString text));
    MOCK_METHOD0(clearResult, void(void));
    MOCK_METHOD0(getMeasurMode, NIBPMode(void));
    MOCK_METHOD1(setAutoMeasure, void(bool));
    MOCK_METHOD1(setSTATMeasure, void(bool));
    MOCK_METHOD0(switchToAuto, void(void));
    MOCK_METHOD0(switchToManual, void(void));
    MOCK_METHOD1(setModelText, void(QString));
    MOCK_METHOD0(getSuperMeasurMode, NIBPMode(void));
    MOCK_METHOD0(getAutoInterval, NIBPAutoInterval(void));
    MOCK_METHOD1(createSnapshot, void(NIBPOneShotType));
    MOCK_METHOD0(provider, NIBPProviderIFace&(void));
    MOCK_METHOD1(setAdditionalMeasure, void(bool));
    MOCK_METHOD1(setSTATClose, void(bool));
    MOCK_METHOD0(isAutoMeasure, bool(void));
    MOCK_METHOD0(isSTATMeasure, bool(void));
    MOCK_METHOD1(setFirstAuto, void(bool));
    MOCK_METHOD0(isFirstAuto, bool(void));
};
