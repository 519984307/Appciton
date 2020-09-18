/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/12
 **/

#pragma once
#include "SPO2Define.h"

class SPO2Symbol
{
public:
    static const char *convert(SPO2ModuleType index)
    {
        static const char *symbol[MODULE_SPO2_NR] =
        {
            "BLM_S5", "MASIMO_SPO2", "RAINBOW_SPO2"
        };
        return symbol[index];
    }

    static const char *convert(AverageTime index)
    {
        static const char *symbol[SPO2_AVER_TIME_NR] =
        {
            "2-4s", "4-6s", "8s", "10s", "12s", "14s", "16s"
        };
        return symbol[index];
    }

    static const char *convert(SPO2Sensitive index)
    {
        static const char *symbol[SPO2_SENS_NR] =
        {
            "SPO2SensLow", "SPO2SensMed", "SPO2SensHigh"
        };
        return symbol[index];
    }

    static const char *convert(SensitivityMode index)
    {
        static const char *symbol[SPO2_MASIMO_SENS_NR] =
        {
            "MaxSensitivity", "Normal", "APOD"
        };
        return symbol[index];
    }

    static const char *convert(SPO2WaveVelocity index)
    {
        static const char *symbol[SPO2_WAVE_VELOCITY_NR] =
        {
            "6.25 mm/s", "12.5 mm/s", "25.0 mm/s", "50.0 mm/s"
        };
        return symbol[index];
    }

    static const char *convert(SPO2ModuleControl index)
    {
        static const char *symbol[SPO2_MODULE_NR] =
        {
            "Disable", "Enable"
        };
        return symbol[index];
    }

    /**********************************************************************************************
     * 以下为报警字串转换。
     ********************************************************************************************/
    static const char *convert(SPO2LimitAlarmType index)
    {
        static const char *symbol[SPO2_LIMIT_ALARM_NR] =
        {
            "SPO2Low", "SPO2High",
            "SPO22Low", "SPO22High",
            "SPO2DLow", "SPO2DHigh",
            "PVILow", "PVIHigh",
            "SPHBLow", "SPHBHigh",
            "SPOCLow", "SPOCHigh",
            "SPMETLow", "SPMETHigh",
            "SPCOLow", "SPCOHigh"
        };
        return symbol[index];
    }

    static const char *convert(SPO2SMARTPLUSETONE index)
    {
        static const char *symbol[SPO2_SMART_PLUSE_TONE_NR] =
        {
            "Off",
            "On",
        };
        return symbol[index];
    }

    static const char *convert(SPO2OneShotType index, bool isPlugin)
    {
        if (isPlugin)
        {
            static const char *symbol[SPO2_ONESHOT_NR] =
            {
                // 插件血氧
                "SPO22CheckSensor",
                "SPO22CableOff",
                "SPO22FingerOff",
                "SPO22LEDFault",
                "SPO22CommunicationStop",
                "SPO22SignalWeak",
                "SPO22SignalSaturation",
                "SPO22LowPerfusion",
                "SPO22DefectiveCable",
                "SPO22CableExpired",
                "SPO22IncompatibleCable",
                "SPO22UnrecognizedCable",
                "SPO22CableNearExpiration",
                "SPO22SensorExpired",
                "SPO22IncompatibleSensor",
                "SPO22UnrecognizedSensor",
                "SPO22DefectiveSensor",
                "SPO22CheckCableAndSensorFault",
                "SPO22SensorNearExpiration",
                "SPO22NoAdhesiveSensor",
                "SPO22AdhesiveSensorExpiraton",
                "SPO22IncompatibleAdhesiveSensor",
                "SPO22UnrecognizedAdhesiveSensor",
                "SPO22DefectiveAdhesiveSensor",
                "SPO22SensorIniting",
                "SPO22InterferenceDetected",
                "SPO22DemoMode",
                "SPO22AdhesiveSensorNearExpiration",
                "SPO22CheckSensorConnection",
                "SPO22OnlyMode",
                "SPO22BoardFailure",
                "SPO22LowSignalIQ",
                "SPO22LowPRConfidence",
                "SPO22LowPIConfidence",
                "SPO22InvalidSmoothPI",
                "SPO22LowSpMetConfidence",
                "SPO22LowSpMetPerfusionIndex",
                "SPO22LowSpHbConfidence",
                "SPO22LowSpHbPerfusionIndex",
                "SPO22LowSpOCConfidence",
                "SPO22LowSpOCPerfusionIndex",
                "SPO22LowPVIConfidence",
            };
            return symbol[index];
        }
        else
        {
            static const char *symbol[SPO2_ONESHOT_NR] =
            {
                "SPO2CheckSensor",
                "SPO2CableOff",
                "SPO2FingerOff",
                "SPO2LEDFault",
                "SPO2CommunicationStop",
                "SPO2SignalWeak",
                "SPO2SignalSaturation",
                "SPO2LowPerfusion",
                "SPO2DefectiveCable",
                "SPO2CableExpired",
                "SPO2IncompatibleCable",
                "SPO2UnrecognizedCable",
                "SPO2CableNearExpiration",
                "SPO2SensorExpired",
                "SPO2IncompatibleSensor",
                "SPO2UnrecognizedSensor",
                "SPO2DefectiveSensor",
                "SPO2CheckCableAndSensorFault",
                "SPO2SensorNearExpiration",
                "SPO2NoAdhesiveSensor",
                "SPO2AdhesiveSensorExpiraton",
                "SPO2IncompatibleAdhesiveSensor",
                "SPO2UnrecognizedAdhesiveSensor",
                "SPO2DefectiveAdhesiveSensor",
                "SPO2SensorIniting",
                "SPO2InterferenceDetected",
                "SPO2DemoMode",
                "SPO2AdhesiveSensorNearExpiration",
                "SPO2CheckSensorConnection",
                "SPO2OnlyMode",
                "SPO2BoardFailure",

                "SPO2LowSignalIQ",
                "SPO2LowPRConfidence",
                "SPO2LowPIConfidence",
                "SPO2InvalidSmoothPI",
                "SPO2LowSpMetConfidence",
                "SPO2LowSpMetPerfusionIndex",
                "SPO2LowSpHbConfidence",
                "SPO2LowSpHbPerfusionIndex",
                "SPO2LowSpOCConfidence",
                "SPO2LowSpOCPerfusionIndex",
                "SPO2LowPVIConfidence",
            };
            return symbol[index];
        }
    }

    static const char *convert(CCHDResult index)
    {
        static const char *symbol[CCHD_NR] =
        {
            "Negative", "Positive", "RepeatCheck"
        };
        return symbol[index];
    }

    static const char *convert(AveragingMode index)
    {
        static const char *symbol[AVERAGING_MODE_NR] =
        {
            "AveragingNormal", "AveragingFast"
        };
        return symbol[index];
    }

    static const char *convert(SpHbPrecisionMode index)
    {
        static const char *symbol[PRECISION_NR] =
        {
            "Nearest1", "Nearest5", "WholeNumber"
        };
        return symbol[index];
    }

    static const char *convert(SpHbBloodVesselMode index)
    {
        static const char *symbol[BLOOD_VESSEL_NR] =
        {
            "Arterial", "Venous"
        };
        return symbol[index];
    }

    static const char *convert(SpHbAveragingMode index)
    {
        static const char *symbol[SPHB_AVERAGING_NR] =
        {
            "SpHbAveragingLong", "SpHbAveragingMedium", "SpHbAveragingShort"
        };
        return symbol[index];
    }
};
