/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/13
 **/

#pragma once

enum AlarmType
{
    ALARM_TYPE_PHY,
    ALARM_TYPE_LIFE,
    ALARM_TYPE_TECH
};

enum LimitAlarmType
{
    NotLimitAlarm,
    LowLimitAlarm,
    HighLimitAlarm
};

enum AlarmPriority
{
    ALARM_PRIO_PROMPT = -1,     // 指示提示信息。
    ALARM_PRIO_LOW,             // 低级。
    ALARM_PRIO_MED,             // 中级。
    ALARM_PRIO_HIGH,            // 高级。
};
