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

/**
 * @brief The NIBPModuleType enum  nibp模块类型枚举
 */
enum NIBPModuleType
{
    MODULE_N5,
    MODULE_SUNTECH_NIBP,
    MODULE_NIBP_NR,
};

/**************************************************************************************************
 * 血压工作模式。
 *************************************************************************************************/
enum NIBPMode
{
    NIBP_MODE_MANUAL,               // 手动。
    NIBP_MODE_AUTO,                 // 自动。
    NIBP_MODE_STAT,                 // 连续。
    NIBP_MODE_NR,
};

/**************************************************************************************************
 * 血压智能充气开关控制。
 *************************************************************************************************/
enum NIBPIntelligentInflate
{
    NIBP_INTELLIGENT_INFLATE_OFF,
    NIBP_INTELLIGENT_INFLATE_ON,
    NIBP_INTELLIGENT_INFLATE_NR
};

/**************************************************************************************************
 * 血压自动测量时间间隔。
 *************************************************************************************************/
enum NIBPAutoInterval
{
    NIBP_AUTO_INTERVAL_2_5,
    NIBP_AUTO_INTERVAL_5,
    NIBP_AUTO_INTERVAL_10,
    NIBP_AUTO_INTERVAL_15,
    NIBP_AUTO_INTERVAL_20,
    NIBP_AUTO_INTERVAL_30,
    NIBP_AUTO_INTERVAL_45,
    NIBP_AUTO_INTERVAL_60,
    NIBP_AUTO_INTERVAL_90,
    NIBP_AUTO_INTERVAL_120,
    NIBP_AUTO_INTERVAL_NR
};

/**************************************************************************************************
 * 初始充气电压
 *************************************************************************************************/
enum NIBPInitialInflatingPressure
{
    NIBP_INIT_PRE_80,
    NIBP_INIT_PRE_90,
    NIBP_INIT_PRE_100,
    NIBP_INIT_PRE_110,
    NIBP_INIT_PRE_120,
    NIBP_INIT_PRE_130,
    NIBP_INIT_PRE_140,
    NIBP_INIT_PRE_150,
    NIBP_INIT_PRE_160,
    NIBP_INIT_PRE_170,
    NIBP_INIT_PRE_NR
};

/**************************************************************************************************
 * 血压PR显示控制。
 *************************************************************************************************/
enum NIBPPRDisplay
{
    NIBP_PR_DISPLAY_OFF,
    NIBP_PR_DISPLAY_ON,
    NIBP_PR_DISPLAY_NR
};

/**************************************************************************************************
 * OneShot事件定义，其实也是NIBP的错误码定义。
 *************************************************************************************************/
enum NIBPOneShotType
{
    NIBP_ONESHOT_NONE,
    NIBP_ONESHOT_ABORT,                           // measurement abort
    NIBP_ONESHOT_ALARM_CUFF_TYPE_ERROR,           // 袖带错。
    NIBP_ONESHOT_ALARM_CUFF_ERROR,                // 袖带松，未接或未扎。
    NIBP_ONESHOT_ALARM_CUFF_AIR_LEAKAGE,          // 漏气。
    NIBP_ONESHOT_ALARM_PNEUMATIC_BLOCKAGE,        // 气管堵塞。
    NIBP_ONESHOT_ALARM_CUFF_OVER_PRESSURE,        // 袖带压过压。
    NIBP_ONESHOT_ALARM_PESSURE_ERROR,             // 压力不稳，如软管扭结。
    NIBP_ONESHOT_ALARM_SIGNAL_WEAK,               // 脉搏信号弱。
    NIBP_ONESHOT_ALARM_SIGNAL_SATURATION,         // 信号饱和。
    NIBP_ONESHOT_ALARM_MEASURE_OVER_RANGE,        // 测量结果超出范围。
    NIBP_ONESHOT_ALARM_MEASURE_TIMEOUT,           // 测量超时。
    NIBP_ONESHOT_ALARM_EXCESSIVE_MOVING,          // 过度运动。
    NIBP_ONESHOT_ALARM_HARDWARE_ERROR,            // 硬件错误。
    NIBP_ONESHOT_ALARM_TRANSDUCER_OVER_RANGE,     // 传感器超界
    NIBP_ONESHOT_ALARM_EEPROM_FAILURE,            // 内存出错
    NIBP_ONESHOT_ALARM_COMMUNICATION_STOP,        // 模块通信连接错误。
    NIBP_ONESHOT_ALARM_COMMUNICATION_TIMEOUT,     // 模块通信超时。
    NIBP_ONESHOT_ALARM_MODULE_DISABLE,            // 模块禁用。
    NIBP_ONESHOT_ALARM_SEND_COMMUNICATION_STOP,             // 发送通信中断
    NIBP_ONESHOT_NR,
};

/**************************************************************************************************
 * 超限报警定义
 *************************************************************************************************/
enum NIBPLimitAlarmType
{
    NIBP_LIMIT_ALARM_SYS_LOW,
    NIBP_LIMIT_ALARM_SYS_HIGH,
    NIBP_LIMIT_ALARM_DIA_LOW,
    NIBP_LIMIT_ALARM_DIA_HIGH,
    NIBP_LIMIT_ALARM_MAP_LOW,
    NIBP_LIMIT_ALARM_MAP_HIGH,
    NIBP_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * 校准模式压力
 *************************************************************************************************/
enum NIBPCalibratePressure
{
    NIBP_Calibrate_Pressure_0,
    NIBP_Calibrate_Pressure_20,
    NIBP_Calibrate_Pressure_40,
    NIBP_Calibrate_Pressure_60,
    NIBP_Calibrate_Pressure_80,
    NIBP_Calibrate_Pressure_100,
    NIBP_Calibrate_Pressure_120,
    NIBP_Calibrate_Pressure_140,
    NIBP_Calibrate_Pressure_160,
    NIBP_Calibrate_Pressure_180,
    NIBP_Calibrate_Pressure_200,
    NIBP_Calibrate_Pressure_220,
    NIBP_Calibrate_Pressure_240,
    NIBP_Calibrate_Pressure_250,
    NIBP_Calibrate_Pressure_NR
};


/***************************************************************************************************
 * NIBP measurement status
 **************************************************************************************************/
enum NIBPMeasureResult
{
    NIBP_MEASURE_RESULT_NONE,
    NIBP_MEASURE_FAIL,
    NIBP_MEASURE_SUCCESS
};
