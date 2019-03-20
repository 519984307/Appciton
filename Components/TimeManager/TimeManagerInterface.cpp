/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/20
 **/

#include "TimeManagerInterface.h"
#include <stddef.h>

static TimeManagerInterface *timeManager = NULL;

TimeManagerInterface *TimeManagerInterface::registerTimeManager(TimeManagerInterface *instance)
{
    TimeManagerInterface *old = timeManager;
    timeManager = instance;
    return old;
}

TimeManagerInterface *TimeManagerInterface::getTimeManager()
{
    return timeManager;
}
