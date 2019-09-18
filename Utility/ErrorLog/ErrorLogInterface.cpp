/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/2/28
 **/


#include "ErrorLogInterface.h"

static ErrorLogInterface *currentIface = NULL;

ErrorLogInterface *ErrorLogInterface::registerErrorLog(ErrorLogInterface *interface)
{
    ErrorLogInterface *old = currentIface;
    currentIface = interface;
    return old;
}

ErrorLogInterface *ErrorLogInterface::getErrorLog()
{
    return currentIface;
}
