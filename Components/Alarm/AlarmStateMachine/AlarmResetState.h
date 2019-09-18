/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/11/7
 **/

#pragma once

#include "AlarmState.h"

class AlarmResetStatePrivate;
class AlarmResetState : public AlarmState
{
    Q_OBJECT
public:
    AlarmResetState();
    ~AlarmResetState();

    /* reimplement */
    void enter();

    /* reimplement */
    void handAlarmEvent(AlarmStateEvent event, unsigned char *data, unsigned len);

private:
    AlarmResetStatePrivate * const d_ptr;
};
