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
    MODULE_NELLCOR_SPO2,
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
    SPO2_SMART_PLUSE_TONE_ON,
    SPO2_SMART_PLUSE_TONE_OFF,
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
    SPO2_ONESHOT_NR
};

/**************************************************************************************************
 * SPO2超限报警定义
 *************************************************************************************************/
enum SPO2LimitAlarmType
{
    SPO2_LIMIT_ALARM_SPO2_LOW,
    SPO2_LIMIT_ALARM_SPO2_HIGH,
    SPO2_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * ECG增益
 *************************************************************************************************/
enum SPO2Gain
{
    SPO2_GAIN_X10,      // x1
    SPO2_GAIN_X20,      // x2
    SPO2_GAIN_X40,      // x4
    SPO2_GAIN_X80,      // x8
    SPO2_GAIN_NR
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

enum RainbowLineFrequency
{
    RB_LINE_FREQ_50HZ,
    RB_LINE_FREQ_60HZ,
};
