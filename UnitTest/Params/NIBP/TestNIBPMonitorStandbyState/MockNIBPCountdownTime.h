/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang <fangtongzhou@blmed.cn>, 2019/3/25
 **/
#pragma once

#include "NIBPCountdownTimeInterface.h"
#include "gmock/gmock.h"

class  MockNIBPCountdownTime : public NIBPCountdownTimeInterface
{
public:
    MOCK_METHOD0(STATMeasureStart, void(void));
    MOCK_METHOD0(STATMeasureStop, void(void));
    MOCK_METHOD1(setSTATMeasureTimeout, void(bool));
};
