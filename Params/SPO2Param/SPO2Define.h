/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/15
 **/

#pragma once

/**
 * @brief The SPO2ModuleType enum  SPO2模块类型枚举
 */
enum SPO2ModuleType
{
    MODULE_BLM_S5,
    MODULE_MASIMO_SPO2,
    MODULE_RAINBOW_SPO2,
    MODULE_SPO2_NR,
};
/**************************************************************************************************
 * 血氧灵敏度。
 *************************************************************************************************/
enum SPO2Sensitive
{
    SPO2_SENS_LOW,
    SPO2_SENS_MED,
    SPO2_SENS_HIGH,
    SPO2_SENS_NR
};

/**************************************************************************************************
 * 血氧波形速度。
 *************************************************************************************************/
enum SPO2WaveVelocity
{
    SPO2_WAVE_VELOCITY_62D5,
    SPO2_WAVE_VELOCITY_125,
    SPO2_WAVE_VELOCITY_250,
    SPO2_WAVE_VELOCITY_500,
    SPO2_WAVE_VELOCITY_NR
};


/**************************************************************************************************
 * 血氧模块功能使能或关闭。
 *************************************************************************************************/
enum SPO2ModuleControl
{
    SPO2_MODULE_DISABLE,
    SPO2_MODULE_ENABLE,
    SPO2_MODULE_NR
};

/**************************************************************************************************
 * 智能脉搏音开关控制。
 *************************************************************************************************/
enum SPO2SMARTPLUSETONE
{
    SPO2_SMART_PLUSE_TONE_OFF,
    SPO2_SMART_PLUSE_TONE_ON,
    SPO2_SMART_PLUSE_TONE_NR
};

/**************************************************************************************************
 * SPO2OneShot事件定义，包括心率失常类型
 *************************************************************************************************/
enum SPO2OneShotType
{
    SPO2_ONESHOT_ALARM_CHECK_SENSOR,
    SPO2_ONESHOT_ALARM_CABLE_OFF,
    SPO2_ONESHOT_ALARM_FINGER_OFF,
    SPO2_ONESHOT_ALARM_LED_FAULT,
    SPO2_ONESHOT_ALARM_COMMUNICATION_STOP,
    SPO2_ONESHOT_ALARM_SIGNAL_WEAK,               // 信号太弱。
    SPO2_ONESHOT_ALARM_SIGNAL_SATURATION,         // 信号饱和。
    SPO2_ONESHOT_ALARM_LOW_PERFUSION,             // 低弱灌注。
    SPO2_ONESHOT_ALARM_DEFECTIVE_CABLE,                  // defective cable
    SPO2_ONESHOT_ALARM_CABLE_EXPIRED,                    // cable expired
    SPO2_ONESHOT_ALARM_INCOMPATIBLE_CABLE,               // incompatible cable
    SPO2_ONESHOT_ALARM_UNRECONGNIZED_CABLE,              // unrecongnized cable
    SPO2_ONESHOT_ALARM_CABLE_NEAR_EXPIRATION,            // cable near expiration
    SPO2_ONESHOT_ALARM_SENSOR_EXPIRED,                   // sensor expired
    SPO2_ONESHOT_ALARM_INCOMPATIBLE_SENSOR,              // incompatible sensor
    SPO2_ONESHOT_ALARM_UNRECONGNIZED_SENSOR,             // unrecongnized sensor
    SPO2_ONESHOT_ALARM_DEFECTIVE_SENSOR,                 // defective sensor
    SPO2_ONESHOT_ALARM_CHECK_CABLE_AND_SENSOR_FAULT,     // check cable and sensor fault
    SPO2_ONESHOT_ALARM_SENSOR_NEAR_EXPIRATION,           // sensor near expiration
    SPO2_ONESHOT_ALARM_NO_ADHESIVE_SENSOR,               // no adhesive sensor
    SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_EXPIRATION,       // adhesive sensor expiraton
    SPO2_ONESHOT_ALARM_INCOMPATIBLE_ADHESIVE_SENSOR,     // incompatible adhesive sensor
    SPO2_ONESHOT_ALARM_UNRECONGNIZED_ADHESIVE_SENSOR,    // unrecongnized adhesive sensor
    SPO2_ONESHOT_ALARM_DEFECTIVE_ADHESIVE_SENSOR,        // defective adhesive sensor
    SPO2_ONESHOT_ALARM_SENSOR_INITING,                   // sensor initing
    SPO2_ONESHOT_ALARM_INTERFERENCE_DETECTED,            // interference detected
    SPO2_ONESHOT_ALARM_DEMO_MODE,                        // demo mode
    SPO2_ONESHOT_ALARM_ADHESIVE_SENSOR_NEAR_EXPIRATION,  // adhesive sensor near expiration
    SPO2_ONESHOT_ALARM_CHECK_SENSOR_CONNECTION,          // check sensor connection
    SPO2_ONESHOT_ALARM_SPO2_ONLY_MODE,                   // spo2 only mode
    SPO2_ONESHOT_ALARM_BOARD_FAILURE,                    // spo2 board failure
    SPO2_ONESHOT_ALARM_SEND_COMMUNICATION_STOP,         // 发送通信中断
    SPO2_ONESHOT_NR
};

/**************************************************************************************************
 * SPO2超限报警定义
 *************************************************************************************************/
enum SPO2LimitAlarmType
{
    SPO2_LIMIT_ALARM_SPO2_LOW,
    SPO2_LIMIT_ALARM_SPO2_HIGH,
    SPO2_LIMIT_ALARM_SPO2_2_LOW,
    SPO2_LIMIT_ALARM_SPO2_2_HIGH,
    SPO2_LIMIT_ALARM_SPO2_D_LOW,
    SPO2_LIMIT_ALARM_SPO2_D_HIGH,
    SPO2_LIMIT_ALARM_PVI_LOW,
    SPO2_LIMIT_ALARM_PVI_HIGH,
    SPO2_LIMIT_ALARM_SPHB_LOW,
    SPO2_LIMIT_ALARM_SPHB_HIGH,
    SPO2_LIMIT_ALARM_SPOC_LOW,
    SPO2_LIMIT_ALARM_SPOC_HIGH,
    SPO2_LIMIT_ALARM_SPMET_LOW,
    SPO2_LIMIT_ALARM_SPMET_HIGH,
    SPO2_LIMIT_ALARM_SPCO_LOW,
    SPO2_LIMIT_ALARM_SPCO_HIGH,
    SPO2_LIMIT_ALARM_NR
};

enum SensitivityMode
{
    SPO2_MASIMO_SENS_MAX,
    SPO2_MASIMO_SENS_NORMAL,
    SPO2_MASIMO_SENS_APOD,
    SPO2_MASIMO_SENS_NR
};

enum AverageTime
{
    SPO2_AVER_TIME_2_4SEC,
    SPO2_AVER_TIME_4_6SEC,
    SPO2_AVER_TIME_8SEC,
    SPO2_AVER_TIME_10SEC,
    SPO2_AVER_TIME_12SEC,
    SPO2_AVER_TIME_14SEC,
    SPO2_AVER_TIME_16SEC,
    SPO2_AVER_TIME_NR,
};

enum Spo2SatSecondsType
{
    SPO2_SAT_SECOND_0 = 0,
    SPO2_SAT_SECOND_10 = 10,
    SPO2_SAT_SECOND_25 = 25,
    SPO2_SAT_SECOND_50 = 50,
    SPO2_SAT_SECOND_100 = 100
};

enum WaveUpdateType
{
    WAVE_UPDATE_0 = 0,  // 无效值
    WAVE_UPDATE_76,  // 76HZ
    WAVE_UPDATE_38,  // 38HZ
    WAVE_UPDATE_25,  // 25.3HZ
    WAVE_UPDATE_19,  // 19HZ
    WAVE_UPDATE_NR  // 无效值
};

enum SPO2LineFrequencyType
{
    SPO2_LINE_FREQ_50HZ,
    SPO2_LINE_FREQ_60HZ,
};

// CCHD筛查结果
enum CCHDResult
{
    Negative,     // 阴性
    Positive,     // 阳性
    RepeatCheck,  // 重复筛查
    CCHD_NR
};

enum SpHbPrecisionMode
{
    PRECISION_NEAREST_0_1,      // nearest 0.1
    PRECISION_NEAREST_0_5,      // nearest 0.5
    PRECISION_WHOLE_NUMBER,     // whole number
    PRECISION_NR
};

enum AveragingMode
{
    AVERAGING_MODE_NORMAL,
    AVERAGING_MODE_FAST,
    AVERAGING_MODE_NR
};

enum SpHbAveragingMode
{
    SPHB_AVERAGING_MODE_LONG,
    SPHB_AVERAGING_MODE_MED,
    SPHB_AVERAGING_MODE_SHORT,
    SPHB_AVERAGING_NR
};

enum SpHbUnitType
{
    SPHB_UNIT_G_DL,
    SPHB_UNIT_MMOL_L,
    SPHB_UNIT_NR,
};

enum SpHbBloodVesselMode
{
    BLOOD_VESSEL_ARTERIAL,
    BLOOD_VESSEL_VENOUS,
    BLOOD_VESSEL_NR
};


enum SPO2RainbowType
{
    SPO2_RAINBOW_TYPE_DAVID,
    SPO2_RAINBOW_TYPE_BLM,
    SPO2_RAINBOW_TYPE_NR
};

enum SPO2RainbowSensor
{
    SPO2_RAINBOW_SENSOR_M_LNCS,
    SPO2_RAINBOW_SENSOR_R25,
    SPO2_RAINBOW_SENSOR_R1
};

enum SPO2RainbowProgramMessageType
{
    SPO2_RAINBOW_PROG_MSG_REQUEST_ENTER_PROGRAM_MODE,
    SPO2_RAINBOW_PROG_MSG_PROGRAM_COMPLETE,
    SPO2_RAINBOW_PROG_MSG_COMMUNICATION_ERROR,
    SPO2_RAINBOW_PROG_MSG_FLASH_ERROR,
    SPO2_RAINBOW_PROG_MSG_INVALID_UPGRADE_APPLICATION,
    SPO2_RAINBOW_PROG_MSG_INVALID_TOOL_CONFIGURATION,
    SPO2_RAINBOW_PROG_MSG_NR
};
