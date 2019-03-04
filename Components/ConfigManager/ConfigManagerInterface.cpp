/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/1
 **/

#include "ConfigManagerInterface.h"
#include <stddef.h>

static ConfigManagerInterface *curConfigManager = NULL;

ConfigManagerInterface *ConfigManagerInterface::registerConfigManager(ConfigManagerInterface *handle)
{
    ConfigManagerInterface *old = curConfigManager;
    if (curConfigManager == NULL)
    {
        curConfigManager = handle;
    }
    return old;
}

ConfigManagerInterface *ConfigManagerInterface::getConfigManager()
{
    return curConfigManager;
}
