/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/19
 **/

#include "AlarmInterface.h"
#include <stddef.h>

static AlarmInterface *alarm = NULL;

AlarmInterface *AlarmInterface::registerAlarm(AlarmInterface *instance)
{
    AlarmInterface *old = alarm;
    alarm = instance;
    return old;
}

AlarmInterface *AlarmInterface::getAlarm()
{
    return alarm;
}
