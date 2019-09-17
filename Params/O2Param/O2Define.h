/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#pragma once

enum O2LimitAlarmType
{
    O2_LIMIT_ALARM_O2_LOW,
    O2_LIMIT_ALARM_O2_HIGH,
    O2_LIMIT_ALARM_NR
};

enum O2OneShotAlarmType
{
    O2_ONESHOT_ALARM_CALIBRATE_RESET,               // 校准数据复位
    O2_ONESHOT_ALARM_NOT_CALIBRATE,                 // 传感器未校准
    O2_ONESHOT_ALARM_MOTOR_NOT_IN_POSITION,         // 马达不在位
    O2_ONESHOT_ALARM_SENSOR_OFF,                    // 探头脱落
    O2_ONESHOT_ALARM_SEND_COMMUNICATION_STOP,
    O2_ONESHOT_ALARM_NR
};
