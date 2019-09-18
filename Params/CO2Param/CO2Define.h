/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/29
 **/

#pragma once

enum CO2ModuleType
{
    MODULE_BLM_CO2,
    MODULE_MASIMO_CO2,
    MODULE_CO2_NR
};

/**************************************************************************************************
 * CO2工作模式。
 *************************************************************************************************/
enum CO2WorkMode
{
    CO2_WORK_SELFTEST = 0x00,       //自检
    C02_WORK_SLEEP = 0x01,          //睡眠
    CO2_WORK_MEASUREMENT = 0x02     //测量
};

/**************************************************************************************************
 * CO2窒息时间。
 *************************************************************************************************/
enum CO2ApneaTime
{
    CO2_APNEA_TIME_20_SEC,
    CO2_APNEA_TIME_25_SEC,
    CO2_APNEA_TIME_30_SEC,
    CO2_APNEA_TIME_35_SEC,
    CO2_APNEA_TIME_40_SEC,
    CO2_APNEA_TIME_45_SEC,
    CO2_APNEA_TIME_50_SEC,
    CO2_APNEA_TIME_55_SEC,
    CO2_APNEA_TIME_60_SEC,
    CO2_APNEA_TIME_NR
};

/**************************************************************************************************
 * CO2波形速度。
 *************************************************************************************************/
enum CO2SweepSpeed
{
    CO2_SWEEP_SPEED_62_5,
    CO2_SWEEP_SPEED_125,
    CO2_SWEEP_SPEED_250,
    CO2_SWEEP_SPEED_500,
    CO2_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * CO2平衡气体。
 *************************************************************************************************/
enum BalanceGas
{
    CO2_BALANCE_GAS_ATMOSPHERE,
    CO2_BALANCE_GAS_Nitrous_oxide,
    CO2_BALANCE_GAS_HELIUM,
    CO2_BALANCE_GAS_NR
};

/**************************************************************************************************
 * O2浓度。
 *************************************************************************************************/
enum Oxygendensity
{
    OXYGEN_DENSITY_0,
    OXYGEN_DENSITY_30,
    OXYGEN_DENSITY_40,
    OXYGEN_DENSITY_50,
    OXYGEN_DENSITY_60,
    OXYGEN_DENSITY_70,
    OXYGEN_DENSITY_80,
    OXYGEN_DENSITY_90,
    OXYGEN_DENSITY_100,
    OXYGEN_DENSITY_NR
};


/**************************************************************************************************
 * 麻醉气体浓度。
 *************************************************************************************************/
enum GasStrength
{
    GAS_STRENGTH_0,
    GAS_STRENGTH_2,
    GAS_STRENGTH_4,
    GAS_STRENGTH_6,
    GAS_STRENGTH_8,
    GAS_STRENGTH_10,
    GAS_STRENGTH_12,
    GAS_STRENGTH_14,
    GAS_STRENGTH_16,
    GAS_STRENGTH_18,
    GAS_STRENGTH_20,
    GAS_STRENGTH_22,
    GAS_STRENGTH_24,
    GAS_STRENGTH_26,
    GAS_STRENGTH_28,
    GAS_STRENGTH_30,
    GAS_STRENGTH_NR
};

/**************************************************************************************************
 * 最大保持。
 *************************************************************************************************/
enum MaxHold
{
    MAX_HOLD_EVERY_BREATHE,
    MAX_HOLD_10S,
    MAX_HOLD_20S,
    MAX_HOLD_NR
};

/**************************************************************************************************
 * 操作模式
 *************************************************************************************************/
enum CO2OperationMode
{
    OPERATION_MODE_MEASURE,
    OPERATION_MODE_NR
};

/**************************************************************************************************
 * CO2波形模式。
 *************************************************************************************************/
enum CO2SweepMode
{
    CO2_SWEEP_MODE_CURVE,
    CO2_SWEEP_MODE_FILLED,
    CO2_SWEEP_MODE_NR
};

/**************************************************************************************************
 * CO2补偿。
 *************************************************************************************************/
enum CO2Compensation
{
    CO2_COMPEN_O2,
    CO2_COMPEN_N2O,
    CO2_COMPEN_NR
};

enum CO2DisableZeroReason
{
    CO2_ZERO_REASON_DISABLED,           // 校零失能
    CO2_ZERO_REASON_IN_PROGRESS,        // 正在校零
    CO2_ZERO_REASON_NO_ADAPTER          // 主流:没有适配器
};

/**************************************************************************************************
 * CO2显示放大系数。
 *************************************************************************************************/
enum CO2DisplayZoom
{
    CO2_DISPLAY_ZOOM_4,
    CO2_DISPLAY_ZOOM_8,
    CO2_DISPLAY_ZOOM_12,
    CO2_DISPLAY_ZOOM_20,
    CO2_DISPLAY_ZOOM_NR
};

/**************************************************************************************************
 * FiCO2显示控制。
 *************************************************************************************************/
enum CO2FICO2Display
{
    CO2_FICO2_DISPLAY_OFF,
    CO2_FICO2_DISPLAY_ON,
    CO2_FICO2_DISPLAY_NR
};

/**************************************************************************************************
 * OneShot事件定义，包括心率失常类型
 *************************************************************************************************/
enum CO2OneShotType
{
    CO2_ONESHOT_ALARM_APNEA,

    CO2_ONESHOT_ALARM_SOFTWARE_ERROR,
    CO2_ONESHOT_ALARM_HARDWARE_ERROR,
    CO2_ONESHOT_ALARM_MOTOR_ERROR,
    CO2_ONESHOT_ALARM_MISS_FACTORY_CALI,
    CO2_ONESHOT_ALARM_O2_SENSOR_ERROR,

    CO2_ONESHOT_ALARM_REPLACE_ADAPTER,
    CO2_ONESHOT_ALARM_SAMPLING_LINE_CLOGGED,
    CO2_ONESHOT_ALARM_NO_ADAPTER,
    CO2_ONESHOT_ALARM_NO_SAMPLING_LINE,
    CO2_ONESHOT_ALARM_O2_PORT_FAILURE,

    CO2_ONESHOT_ALARM_CO2_OUT_RANGE,
    CO2_ONESHOT_ALARM_N2O_OUT_RANGE,
    CO2_ONESHOT_ALARM_AX_OUT_RANGE,
    CO2_ONESHOT_ALARM_O2_OUT_RANGE,
    CO2_ONESHOT_ALARM_TEMP_OUT_RANGE,
    CO2_ONESHOT_ALARM_PRESS_OUT_RANGE,
    CO2_ONESHOT_ALARM_ZERO_REQUIRED,
    CO2_ONESHOT_ALARM_AGENT_UNRELIABLE,

    CO2_ONESHOT_ALARM_ZERO_DISABLE,
    CO2_ONESHOT_ALARM_ZERO_AND_SPAN_DISABLE,
    CO2_ONESHOT_ALARM_ZERO_IN_PROGRESS,

    CO2_ONESHOT_ALARM_SPAN_CALIB_FAILED,
    CO2_ONESHOT_ALARM_SPAN_CALIB_IN_PROGRESS,
    CO2_ONESHOT_ALARM_IR_O2_DELAY,

    CO2_ONESHOT_ALARM_ZEROING_COMPLETED,
    CO2_ONESHOT_ALARM_STANDBY,

    CO2_ONESHOT_NR,
};

/**************************************************************************************************
 * 超限报警定义
 *************************************************************************************************/
enum CO2LimitAlarmType
{
    CO2_LIMIT_ALARM_ETCO2_LOW,
    CO2_LIMIT_ALARM_ETCO2_HIGH,
    CO2_LIMIT_ALARM_FICO2_LOW,
    CO2_LIMIT_ALARM_FICO2_HIGH,
    CO2_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * O2补偿。
 *************************************************************************************************/
enum O2Compensation
{
    O2_COMPEN_LOW,
    O2_COMPEN_MED,
    O2_COMPEN_HIGH,
    O2_COMPEN_NR
};

/**************************************************************************************************
 * N2O补偿。
 *************************************************************************************************/
enum N2OCompensation
{
    N2O_COMPEN_LOW,
    N2O_COMPEN_HIGH,
    N2O_COMPEN_NR
};

