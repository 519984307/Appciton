/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include <QString>
#include "IBPDefine.h"

// 将定义的枚举转换成符号。
class IBPSymbol
{
public:
    static const char *convert(IBPLabel index)
    {
        static const char *symbol[IBP_LABEL_NR] =
        {
            "ART",  "PA", "CVP", "LAP", "RAP", "ICP", "AUXP1", "AUXP2"
        };

        return symbol[index];
    }

    static const char *convert(IBPModuleType type)
    {
        static const char *symbol[IBP_MODULE_NR] = {
            "SMART_IBP", "WITLEAF_IBP"
        };

        return symbol[type];
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
            "ARTSYSLow", "ARTSYSHigh",
            "ARTDIALow", "ARTDIAHigh",
            "ARTMAPLow", "ARTMAPHigh",
            "ARTPRLow", "ARTPRHigh",

            "PASYSLow", "PASYSHigh",
            "PADIALow", "PADIAHigh",
            "PAMAPLow", "PAMAPHigh",
            "PAPRLow", "PAPRHigh",

            "CVPMAPLow", "CVPMAPHigh",
            "CVPPRLow", "CVPPRHigh",

            "LAPMAPLow", "LAPMAPHigh",
            "LAPPRLow", "LAPPRHigh",

            "RAPMAPLow", "RAPMAPHigh",
            "RAPPRLow", "RAPPRHigh",

            "ICPMAPLow", "ICPMAPHigh",
            "ICPPRLow", "ICPPRHigh",

            "AUXP1SYSLow", "AUXP1SYSHigh",
            "AUXP1DIALow", "AUXP1DIAHigh",
            "AUXP1MAPLow", "AUXP1MAPHigh",
            "AUXP1PRLow", "AUXP1PRHigh",

            "AUXP2SYSLow", "AUXP2SYSHigh",
            "AUXP2DIALow", "AUXP2DIAHigh",
            "AUXP2MAPLow", "AUXP2MAPHigh",
            "AUXP2PRLow", "AUXP2PRHigh",
        };
        return symbol[index];
    }

    static const char *convert(IBPOneShotType index)
    {
        static const char *symbol[IBP_ONESHOT_NR] =
        {
            "IBP1LeadOff", "IBP2LeadOff",
            "IBP1ZeroCalibSuccess",
            "IBP1MeasurePressureIsPulse",
            "IBP1ZeroCalibPressureBeyondMeasureRange",
            "IBP1LeadOffZeroCalibFail",
            "IBP1NoSetZeroCalibTime",
            "IBP2ZeroCalibSuccess",
            "IBP2MeasurePressureIsPulse",
            "IBP2ZeroCalibPressureBeyondMeasureRange",
            "IBP2LeadOffZeroCalibFail",
            "IBP2NoSetZeroCalibTime",

            "IBP1CalibrationSuccess",
            "IBP1MeasurePressureIsPulse",
            "IBP1CalibrationPressureBeyondMeasureRange",
            "IBP1NoZeroCalib",
            "IBP1LeadOffCalibrationFail",
            "IBP1NoSetCalibrationTime",
            "IBP2CalibrationSuccess",
            "IBP2MeasurePressureIsPulse",
            "IBP2CalibrationPressureBeyondMeasureRange",
            "IBP2NoZeroCalib",
            "IBP2LeadOffCalibrationFail",
            "IBP2NoSetCalibrationTime",
        };
        return symbol[index];
    }
};
