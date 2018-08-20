/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#pragma once
#include "AlarmState.h"

class AlarmOffState : public AlarmState
{
public:
    AlarmOffState();
    ~AlarmOffState();

    void enter();
    void exit();

    void timerEvent(QTimerEvent *e);

    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);
};

