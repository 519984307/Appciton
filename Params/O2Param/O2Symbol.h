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
#include "O2Define.h"

class O2Symbol
{
public:
    /**
     * @brief convert 生理报警字符转换
     * @param index
     * @return
     */
    static const char *convert(O2LimitAlarmType index)
    {
        static const char *symbol[O2_LIMIT_ALARM_NR] =
        {
            "O2Low", "O2High"
        };
        return symbol[index];
    }

    /**
     * @brief convert 技术报警字符转换
     * @param index
     * @return
     */
    static const char *convert(O2OneShotAlarmType index)
    {
        static const char *symbol[O2_ONESHOT_ALARM_NR] =
        {
            "O2CalibrateReset",
            "O2NotCalibrate",
            "O2MotorNotInPositon",
            "O2SensorOff",
            "O2DataTransmissionStop"
        };
        return symbol[index];
    }
};


