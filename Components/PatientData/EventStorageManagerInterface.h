/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#pragma once
#include "EventDataDefine.h"

class EventStorageManagerInterface
{
public:
    virtual ~EventStorageManagerInterface(){}

    static EventStorageManagerInterface *registerEventStorageManager(EventStorageManagerInterface *instance);

    static EventStorageManagerInterface *getEventStorageManager(void);

    /**
     * @brief triggerAlarmEvent
     * @param almInfo alarm info
     * @param paramWave param relate wave
     */
    virtual void triggerAlarmEvent(const AlarmInfoSegment &almInfo, WaveformID paramWave, unsigned t) = 0;

    /**
     * @brief clearEventItemList 清空事件列表
     */
    virtual void clearEventItemList() {}
};
