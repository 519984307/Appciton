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
            "SPO2Low", "SPO2High"
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

    static const char *convert(SPO2OneShotType index)
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
            "SPO2UnrecongnizedCable",
            "SPO2CableNearExpiration",
            "SPO2SensorExpired",
            "SPO2IncompatibleSensor",
            "SPO2UnrecongnizedSensor",
            "SPO2DefectiveSensor",
            "SPO2CheckCableAndSensorFault",
            "SPO2SensorNearExpiration",
            "SPO2NoAdhesiveSensor",
            "SPO2AdhesiveSensorExpiraton",
            "SPO2IncompatibleAdhesiveSensor",
            "SPO2UnrecongnizedAdhesiveSensor",
            "SPO2DefectiveAdhesiveSensor",
            "SPO2SensorIniting",
            "SPO2InterferenceDetected",
            "SPO2DemoMode",
            "SPO2AdhesiveSensorNearExpiration",
            "SPO2CheckSensorConnection",
            "SPO2OnlyMode",
            "SPO2BoardFailure"
            "SPO2SendCommunicationStop"
        };
        return symbol[index];
    }

    static const char *convert(CCHDResult index)
    {
        static const char *symbol[CCHD_NR] =
        {
            "Negative", "Positive", "RepeatCheck"
        };
        return symbol[index];
    }
};
