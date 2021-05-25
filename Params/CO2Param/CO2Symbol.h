/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/16
 **/


#pragma once
#include "CO2Define.h"

// 将定义的枚举转换成符号。
class CO2Symbol
{
public:
    static const char *convert(CO2ModuleType index)
    {
        static const char *symbol[MODULE_CO2_NR] =
        {
            "BLM_CO2",
            "MASIMO_CO2"
        };
        return symbol[index];
    }

    static const char *convert(CO2ApneaTime index)
    {
        static const char *symbol[CO2_APNEA_TIME_NR] =
        {
            "_20sec", "_25sec", "_30sec",
            "_35sec", "_40sec", "_45sec",
            "_50sec", "_55sec", "_60sec"
        };
        return symbol[index];
    }

    static const char *convert(BalanceGas index)
    {
        static const char *symbol[CO2_BALANCE_GAS_NR] =
        {
            "Atmosphere", "Nitrous Oxide", "Helium"
        };
        return symbol[index];
    }

    static const char *convert(Oxygendensity index)
    {
        static const char *symbol[OXYGEN_DENSITY_NR] =
        {
            "0", "30%", "40%", "50%", "60%", "70%", "80%", "90%", "100%",
        };
        return symbol[index];
    }

    static const char *convert(GasStrength index)
    {
        static const char *symbol[GAS_STRENGTH_NR] =
        {
            "0", "2%", "4%", "6%", "8%", "10%", "12%", "14%",
            "16%", "18%", "20%", "22%", "24%", "26%", "28%", "30%",
        };
        return symbol[index];
    }

    static const char *convert(MaxHold index)
    {
        static const char *symbol[MAX_HOLD_NR] =
        {
            "Every Breathe", "10s", "20s",
        };
        return symbol[index];
    }

    static const char *convert(CO2OperationMode index)
    {
        static const char *symbol[OPERATION_MODE_NR] =
        {
            "Measure Mode",
        };
        return symbol[index];
    }

    static const char *convert(CO2SweepSpeed index)
    {
        static const char *symbol[CO2_SWEEP_SPEED_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
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

    static const char *convert(O2Compensation index)
    {
        static const char *symbol[O2_COMPEN_NR] =
        {
            "Low", "Med", "High"
        };
        return symbol[index];
    }

    static const char *convert(N2OCompensation index)
    {
        static const char *symbol[N2O_COMPEN_NR] =
        {
            "Low", "High"
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
            "CO2awRRLow",  "CO2awRRHigh"
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
            "CO2Standby"
        };
        return symbol[index];
    }
};
