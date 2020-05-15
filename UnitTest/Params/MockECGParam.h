/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/5/13
 **/

#pragma once
#include "ECGParamInterface.h"
#include "gmock/gmock.h"

class MockECGParam : public ECGParamInterface
{
public:
    MOCK_CONST_METHOD0(getLeadConvention, ECGLeadNameConvention(void));
    MOCK_METHOD1(setPatientType, void(unsigned char));
    MOCK_METHOD0(getPatientType, unsigned char(void));
    MOCK_METHOD1(setPacermaker, void(ECGPaceMode));
    MOCK_METHOD0(getPacermaker, ECGPaceMode(void));
    MOCK_METHOD0(updatePacermaker, void(void));
    MOCK_CONST_METHOD0(getLeadMode, ECGLeadMode(void));
    MOCK_METHOD0(reset, void(void));
};
