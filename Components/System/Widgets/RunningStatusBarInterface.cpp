/**
 ** This file is part of the unittest project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/7
 **/

#include "RunningStatusBarInterface.h"
#include <stddef.h>

static RunningStatusBarInterface *runningStatusBar = NULL;

RunningStatusBarInterface *RunningStatusBarInterface::registerRunningStatusBar(RunningStatusBarInterface *instance)
{
    RunningStatusBarInterface *old = runningStatusBar;
    runningStatusBar = instance;
    return old;
}

RunningStatusBarInterface *RunningStatusBarInterface::getRunningStatusBar()
{
    return runningStatusBar;
}
