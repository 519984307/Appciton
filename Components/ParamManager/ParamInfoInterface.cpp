/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#include "ParamInfoInterface.h"
#include <stddef.h>

static ParamInfoInterface *paramInfo = NULL;

ParamInfoInterface *ParamInfoInterface::registerParamInfo(ParamInfoInterface *instance)
{
    ParamInfoInterface *old = paramInfo;
    paramInfo = instance;
    return old;
}

ParamInfoInterface *ParamInfoInterface::getParamInfo()
{
    return paramInfo;
}
