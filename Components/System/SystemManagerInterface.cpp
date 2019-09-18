/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#include "SystemManagerInterface.h"
#include <stddef.h>

static SystemManagerInterface *systemManager = NULL;

SystemManagerInterface *SystemManagerInterface::registerSystemManager(SystemManagerInterface *instance)
{
    SystemManagerInterface *old = systemManager;
    systemManager = instance;
    return old;
}

SystemManagerInterface *SystemManagerInterface::getSystemManager()
{
    return systemManager;
}
