/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/10/21
 **/

#include "SPO2ParamInterface.h"
#include <stddef.h>

static SPO2ParamInterface *spo2Param = NULL;

SPO2ParamInterface *SPO2ParamInterface::registerSPO2Param(SPO2ParamInterface *handle)
{
    SPO2ParamInterface *old = spo2Param;
    spo2Param = handle;
    return old;
}

SPO2ParamInterface *SPO2ParamInterface::getSPO2Param()
{
    return spo2Param;
}
