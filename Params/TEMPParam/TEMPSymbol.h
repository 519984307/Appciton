#pragma once
#include "TEMPDefine.h"

// 将定义的枚举转换成符号。
class TEMPSymbol
{
public:
    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(TEMPLimitAlarmType index)
    {
        static const char *symbol[TEMP_LIMIT_ALARM_NR] =
        {
            "TEMP1Low", "TEMP1High","TEMP2Low", "TEMP2High","TEMPTDLow","TEMPTDHigh"
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
            "TEMPModuleDisable"
        };
        return symbol[index];
    }
};
