#pragma once


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
enum SPO2SweepSpeed
{
    SPO2_SWEEP_SPEED_125,
    SPO2_SWEEP_SPEED_250,
    SPO2_SWEEP_SPEED_500,
    SPO2_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * 血氧PR显示控制。
 *************************************************************************************************/
enum SPO2PRDisplay
{
    SPO2_PR_DISPLAY_OFF,
    SPO2_PR_DISPLAY_ON,
    SPO2_PR_DISPLAY_NR
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
