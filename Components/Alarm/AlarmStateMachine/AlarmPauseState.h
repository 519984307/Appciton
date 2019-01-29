/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/21
 **/

#pragma once
#include "AlarmState.h"

class AlarmPauseStatePrivate;
class AlarmPauseState : public AlarmState
{
    Q_OBJECT
public:
    AlarmPauseState();
    ~AlarmPauseState();

    void enter();
    void exit();

    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);

protected:
    void timerEvent(QTimerEvent *e);

private:
    AlarmPauseStatePrivate * const d_ptr;
};
