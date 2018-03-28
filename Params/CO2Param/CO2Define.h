#pragma once

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
    CO2_APNEA_TIME_OFF,
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
    CO2_SWEEP_SPEED_NR
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
