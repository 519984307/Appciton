/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#include "EventStorageManagerInterface.h"
#include <stddef.h>

static EventStorageManagerInterface *eventStorageManager = NULL;

EventStorageManagerInterface *EventStorageManagerInterface::registerEventStorageManager(EventStorageManagerInterface *instance)
{
    EventStorageManagerInterface *old = eventStorageManager;
    eventStorageManager = instance;
    return old;
}

EventStorageManagerInterface *EventStorageManagerInterface::getEventStorageManager()
{
    return eventStorageManager;
}
