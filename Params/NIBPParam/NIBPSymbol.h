#pragma once
#include "NIBPDefine.h"

// 将定义的枚举转换成符号。
class NIBPSymbol
{
public:
    static const char *convert(NIBPMode index)
    {
        static const char *symbol[NIBP_MODE_NR] =
        {
            "NIBPManual", "NIBPAUTO", "NIBPCONT"
        };
        return symbol[index];
    }

    static const char *convert(NIBPIntelligentInflate index)
    {
        static const char *symbol[NIBP_INTELLIGENT_INFLATE_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    static const char *convert(NIBPIntervalTime index)
    {
        static const char *symbol[NIBP_INT_TIM_NR] =
        {
            "manual","auto", "1min","2min","2.5min","3min", "5min", "10min",
            "15min","20min"
        };
        return symbol[index];
    }

    static const char *convert(NIBPPRDisplay index)
    {
        static const char *symbol[NIBP_PR_DISPLAY_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(NIBPLimitAlarmType index)
    {
        static const char *symbol[NIBP_LIMIT_ALARM_NR] =
        {
            "NIBPSYSLow", "NIBPSYSHigh",
            "NIBPDIALow", "NIBPDIAHigh",
            "NIBPMAPLow", "NIBPMAPHigh",
//            "NIBPPRLow",  "NIBPPRHigh"
        };
        return symbol[index];
    }

    static const char *convert(NIBPOneShotType index)
    {
        static const char *symbol[NIBP_ONESHOT_NR] =
        {
            "NIBPNoneError",
            "NIBPMeasurementAbort",
            "NIBPCuffTypeError",
            "NIBPCuffError",
            "NIBPCuffAirLeakage",
            "NIBPPneumaticBlockage",
            "NIBPCuffOverPressure",
            "NIBPPressureError",
            "NIBPSignalWeak",
            "NIBPSignalSaturation",
            "NIBPMeasureOverRange",
            "NIBPMeasureTimeout",
            "NIBPExcessiveMoving",
            "NIBPHardwareError",
            "NIBPTransducerOverRange",
            "NIBPEEPROMFailure",
            "NIBPCommunicationStop",
            "NIBPCommunicationTimeout",
            "NIBPModuleDisable"
        };
        return symbol[index];
    }

    static const char *convert(NIBPAdultInitialCuff index)
    {
        static const char *symbol[NIBP_ADULT_INITIAL_CUFF_NR] =
        {
            "120",
            "140",
            "160",
            "180",
            "200",
            "220",
            "240",
            "260",
        };
        return symbol[index];
    }

    static const char *convert(NIBPPrediatrictInitialCuff index)
    {
        static const char *symbol[NIBP_PREDIATRIC_INITIAL_CUFF_NR] =
        {
            "80",
            "90",
            "100",
            "110",
            "120",
            "130",
            "140",
            "150",
        };
        return symbol[index];
    }

    static const char *convert(NIBPNeonatalInitialCuff index)
    {
        static const char *symbol[NIBP_NEONATAL_INITIAL_CUFF_NR] =
        {
            "60",
            "70",
            "80",
            "90",
            "100",
            "110",
            "120",
            "130",
        };
        return symbol[index];
    }

    static const char *convert(NIBPCalibratePressure index)
    {
        static const char *symbol[NIBP_Calibrate_Pressure_NR] =
        {
            "0",
            "20",
            "40",
            "60",
            "80",
            "100",
            "120",
            "140",
            "160",
            "180",
            "200",
            "220",
            "240",
            "250",
        };
        return symbol[index];
    }
};
