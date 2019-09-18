/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/27
 **/

#include "AlarmIndicatorInterface.h"
#include "stddef.h"

static AlarmIndicatorInterface *curAlarmIndicatorHandle = NULL;

AlarmIndicatorInterface *AlarmIndicatorInterface::registerAlarmIndicator(AlarmIndicatorInterface *interface)
{
    AlarmIndicatorInterface *oldHandle = curAlarmIndicatorHandle;
    curAlarmIndicatorHandle = interface;
    return oldHandle;
}

AlarmIndicatorInterface *AlarmIndicatorInterface::getAlarmIndicator()
{
    return curAlarmIndicatorHandle;
}

