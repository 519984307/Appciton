/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/
#pragma once
#include "TEMPDefine.h"

// 将定义的枚举转换成符号。
class TEMPSymbol
{
public:
    static const char *convert(TEMPModuleType index)
    {
        static const char *symbol[MODULE_TEMP_NR] =
        {
            "BLM_T5"
        };
        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(TEMPLimitAlarmType index)
    {
        static const char *symbol[TEMP_LIMIT_ALARM_NR] =
        {
            "TEMP1Low", "TEMP1High", "TEMP2Low", "TEMP2High", "TEMPTDLow", "TEMPTDHigh"
        };
        return symbol[index];
    }

    static const char *convert(TEMPOneShotType index)
    {
        static const char *symbol[TEMP_ONESHOT_NR] =
        {
            "TEMP1SensorOff",
            "TEMP2SensorOff",
            "TEMPSensorOffAll",
            "TEMP1OverRange",
            "TEMP2OverRange",
            "TEMPOverRangeAll",
            "TEMPCommunicationStop",
            "TEMPModuleDisable",
            "TEMPModuleNotCalibration1",
            "TEMPModuleNotCalibration2",
            "TEMPSendCommunicationStop",
            "TEMPModuleAbnormal"
        };
        return symbol[index];
    }

    static const char *convert(TEMPChannelType index)
    {
        static const char *symbol[TEMP_CHANNEL_NR] =
        {
            "T1", "Tskin", "Tcore", "Taxil", "Tnaso", "Teso",
            "Trect", "Tamb", "Tairw", "Tvesic", "Tblood",
            "Tmyo", "Ttymp", "Tbrain"
        };
        return symbol[index];
    }
};
