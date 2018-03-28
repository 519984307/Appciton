#pragma once
#include <QString>
#include "IBPDefine.h"

// 将定义的枚举转换成符号。
class IBPSymbol
{
public:
    static const char *convert(IBPPressureName index)
    {
        static const char *symbol[IBP_PRESSURE_NR] =
        {
            "ART",  "PA", "CVP", "LAP", "RAP", "ICP", "AUXP1", "AUXP2"
        };

        return symbol[index];
    }

    static const char *convert(IBPSweepSpeed index)
    {
        static const char *symbol[IBP_SWEEP_SPEED_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
        };
        return symbol[index];
    }

    static const char *convert(IBPFilterMode index)
    {
        static const char *symbol[IBP_FILTER_MODE_NR] =
        {
            "12.5Hz", "40Hz"
        };
        return symbol[index];
    }

    static const char *convert(IBPRulerRange index)
    {
        static const char *symbol[IBP_RULER_RANGE_NR] =
        {
            "auto", "-10~10", "0-20", "0-30", "0-40", "0-60", "0-80",
            "60-140", "30-140", "0-140", "0-160", "0-200", "0-240", "0-300","m"
        };
        return symbol[index];
    }

    static const char *convert(IBPSensitivity index)
    {
        static const char *symbol[IBP_SENSITIVITY_NR] =
        {
            "High", "Medium", "Low"
        };
        return symbol[index];
    }

    static const unsigned char &convertNumber(IBPSensitivity index)
    {
        static const unsigned char symbol[IBP_SENSITIVITY_NR] =
        {
            1, 8, 12
        };
        return symbol[index];
    }

    static const char *convert(IBPLimitAlarmType index)
    {
        static const char *symbol[IBP_LIMIT_ALARM_NR] =
        {
            "IBP1SYSLow", "IBP1SYSHigh",
            "IBP1DIALow", "IBP1DIAHigh",
            "IBP1MEANLow", "IBP1MEANHigh",
            "IBP1PRLow", "IBP1PRHigh",

            "IBP2SYSLow", "IBP2SYSHigh",
            "IBP2DIALow", "IBP2DIAHigh",
            "IBP2MEANLow", "IBP2MEANHigh",
            "IBP2PRLow", "IBP2PRHigh",

        };
        return symbol[index];
    }

    static const char *convert(IBPOneShotType index)
    {
        static const char *symbol[IBP_ONESHOT_NR] =
        {
            "IBP1LeadOff", "IBP2LeadOff"
        };
        return symbol[index];
    }
};
