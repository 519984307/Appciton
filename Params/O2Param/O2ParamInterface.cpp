/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/27
 **/

#include "O2ParamInterface.h"
#include "stddef.h"

static O2ParamInterface *curO2ParamInterface = NULL;

O2ParamInterface *O2ParamInterface::registerO2ParamInterface(O2ParamInterface *interface)
{
    O2ParamInterface *oldHandle = curO2ParamInterface;
    curO2ParamInterface = interface;
    return oldHandle;
}

O2ParamInterface *O2ParamInterface::getO2ParamInterface()
{
    return curO2ParamInterface;
}
