/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#include "ECGParamInterface.h"
#include <stddef.h>

static ECGParamInterface *ecgParam = NULL;

ECGParamInterface *ECGParamInterface::registerECGParam(ECGParamInterface *instance)
{
    ECGParamInterface *old = ecgParam;
    ecgParam = instance;
    return old;
}

ECGParamInterface *ECGParamInterface::getECGParam()
{
    return ecgParam;
}
