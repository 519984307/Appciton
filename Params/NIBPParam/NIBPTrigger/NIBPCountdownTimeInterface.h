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

class NIBPCountdownTimeInterface
{
public:
    virtual ~NIBPCountdownTimeInterface(){}

    static NIBPCountdownTimeInterface* registerNIBPCountTime(NIBPCountdownTimeInterface*);
    static NIBPCountdownTimeInterface* getNIBPCountdownTime();

    virtual void STATMeasureStart(void) = 0;
    virtual void STATMeasureStop(void) = 0;
    virtual void setSTATMeasureTimeout(bool flag) = 0;
};

