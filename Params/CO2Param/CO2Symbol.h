#pragma once
#include "CO2Define.h"

// 将定义的枚举转换成符号。
class CO2Symbol
{
public:
    static const char *convert(CO2ApneaTime index)
    {
        static const char *symbol[CO2_APNEA_TIME_NR] =
        {
            "Off", "_20sec", "_25sec", "_30sec",
            "_35sec", "_40sec", "_45sec",
            "_50sec", "_55sec", "_60sec"
        };
        return symbol[index];
    }

    static const char *convert(CO2SweepSpeed index)
    {
        static const char *symbol[CO2_SWEEP_SPEED_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s"
        };
        return symbol[index];
    }

    static const char *convert(CO2SweepMode index)
    {
        static const char *symbol[CO2_SWEEP_MODE_NR] =
        {
            "CO2Curve", "CO2Filled"
        };
        return symbol[index];
    }

    static const char *convert(CO2Compensation index)
    {
        static const char *symbol[CO2_COMPEN_NR] =
        {
            "CO2CompenO2", "CO2CompenN2O"
        };
        return symbol[index];
    }

    static const char *convert(CO2FICO2Display index)
    {
        static const char *symbol[CO2_FICO2_DISPLAY_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(CO2LimitAlarmType index)
    {
        static const char *symbol[CO2_LIMIT_ALARM_NR] =
        {
            "CO2EtCO2Low", "CO2EtCO2High",
            "CO2FiCO2Low", "CO2FiCO2High",
//            "CO2BRLow",    "CO2BRHigh"
        };
        return symbol[index];
    }

    static const char *convert(CO2OneShotType index)
    {
        static const char *symbol[CO2_ONESHOT_NR] =
        {
            "Apnea",
            "CO2SoftwareError",
            "CO2HardwareError",
            "CO2MotorError",
            "CO2MissFactoryCali",
            "CO2O2SensorError",
            "CO2ReplaceAdapter",
            "CO2SamplingLineClogged",
            "CO2NoAdapter",
            "CO2NoSamplingLine",
            "CO2O2PortFailure",
            "CO2OutRange",
            "CO2N2OOutRange",
            "CO2AXOutRange",
            "CO2O2OutRange",

            "CO2TempOutRange",
            "CO2BaroPressOutRange",

            "CO2ZeroRequired",
            "CO2AgentUnreliable",
            "CO2ZeroDisable",
            "CO2ZeroAndSpanDisable",
            "CO2ZeroInprogress",

            "CO2SpanCalibFailed",
            "CO2SpanCalibInprogress",
            "CO2IrO2Delay",

            "CO2ZeroingCompleted",
        };
        return symbol[index];
    }
};
