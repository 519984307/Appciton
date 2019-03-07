/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/5
 **/

#include "LightManagerInterface.h"

static LightManagerInterface* lightManager = NULL;
LightManagerInterface *LightManagerInterface::registerLightManager(LightManagerInterface *handle)
{
    LightManagerInterface *old = lightManager;
    lightManager = handle;
    return old;
}

LightManagerInterface *LightManagerInterface::getLightManager()
{
    return lightManager;
}
