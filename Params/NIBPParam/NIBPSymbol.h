/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/10/16
 **/



#pragma once
#include "NIBPDefine.h"

// 将定义的枚举转换成符号。
class NIBPSymbol
{
public:
    static const char *convert(NIBPModuleType index)
    {
        static const char *symbol[MODULE_NIBP_NR] =
        {
            "BLM_N5", "SUNTECH_NIBP"
        };
        return symbol[index];
    }

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

    static const char *convert(NIBPAutoInterval index)
    {
        static const char *symbol[NIBP_AUTO_INTERVAL_NR] =
        {
            "_2.5min", "_5min", "_10min", "_15min", "_20min",
            "_30min", "_45min", "_60min", "_90min", "_120min"
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
            "NIBPModuleDisable",
            "NIBPDataTransmissionStop",
            "NIBPSelftestFailed",
            "NIBPModuleAbnormal",
            "NIBPModuleNotCalibrate",
            "NIBPModuleError",
            "NIBPModuleOverPressure"
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
