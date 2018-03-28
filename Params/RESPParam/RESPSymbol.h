#pragma once
#include "RESPDefine.h"

// 将定义的枚举转换成符号。
class RESPSymbol
{
public:
    static const char *convert(RESPSweepSpeed index)
    {
        static const char *symbol[RESP_SWEEP_SPEED_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s"
        };
        return symbol[index];
    }

    static const char *convert(RESPZoom index)
    {
        static const char *symbol[RESP_ZOOM_NR] =
        {
            "x0.25", "x0.5", "x1.0", "x2.0", "x3.0", "x4.0", "x5.0"
        };
        return symbol[index];
    }

    static const char *convert(RESPLead index)
    {
        static const char *symbol[RESP_LEAD_NR] =
        {
            "I", "II"
        };
        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(RESPDupLimitAlarmType index)
    {
        static const char *symbol[RESP_DUP_LIMIT_ALARM_NR] =
        {
            "RRLow", "RRHigh", "CO2BRLow", "CO2BRHigh"
        };
        return symbol[index];
    }

    static const char *convert(RESPOneShotType index)
    {
        static const char *symbol[RESP_ONESHOT_NR] =
        {
            "Apnea",
            "RESPCommunicationStop"
        };
        return symbol[index];
    }
};
