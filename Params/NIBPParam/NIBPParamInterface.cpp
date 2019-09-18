/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/6
 **/

#include "NIBPParamInterface.h"
#include <stddef.h>

static NIBPParamInterface *nibpParam = NULL;

NIBPParamInterface *NIBPParamInterface::registerNIBPParam(NIBPParamInterface *instance)
{
    NIBPParamInterface *old = nibpParam;
    nibpParam = instance;
    return old;
}

NIBPParamInterface *NIBPParamInterface::getNIBPParam()
{
    return nibpParam;
}
