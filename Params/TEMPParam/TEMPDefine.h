#pragma once

/**************************************************************************************************
 * 体温OneShot事件定义，包括心率失常类型
 *************************************************************************************************/
enum TEMPOneShotType
{
    TEMP_SENSOR_OFF_1,
    TEMP_SENSOR_OFF_2,
    TEMP_SENSOR_OFF_ALL,
    TEMP_OVER_RANGR_1,
    TEMP_OVER_RANGR_2,
    TEMP_OVER_RANGR_ALL,
    TEMP_ONESHOT_ALARM_COMMUNICATION_STOP,
    TEMP_ONESHOT_ALARM_MODULE_DISABLE,
    TEMP_ONESHOT_NR,
};

/**************************************************************************************************
 * 体温超限报警定义
 *************************************************************************************************/
enum TEMPLimitAlarmType
{
    TEMP_LIMIT_ALARM_T1_LOW,
    TEMP_LIMIT_ALARM_T1_HIGH,
    TEMP_LIMIT_ALARM_T2_LOW,
    TEMP_LIMIT_ALARM_T2_HIGH,
    TEMP_LIMIT_ALARM_TD_LOW,
    TEMP_LIMIT_ALARM_TD_HIGH,
    TEMP_LIMIT_ALARM_NR
};
