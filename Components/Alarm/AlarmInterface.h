/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/19
 **/

#pragma once
#include "AlarmDefine.h"

class AlarmInterface
{
public:
    virtual ~AlarmInterface(){}

    static AlarmInterface *registerAlarm(AlarmInterface *instance);

    static AlarmInterface *getAlarm(void);

    /**
     * @brief addAlarmStatus 添加报警状态
     * @param status
     */
    virtual void addAlarmStatus(AlarmStatus status) = 0;

    /**
     * @brief removeAllLimitAlarm 移除生理报警的跟踪对象
     */
    virtual void removeAllLimitAlarm() = 0;
};
