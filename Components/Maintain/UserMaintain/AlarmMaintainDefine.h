#pragma once


/**************************************************************************************************
 * 报警暂停时间。
 *************************************************************************************************/
enum MaintainAlarmPauseTime
{
    ALARM_PAUSE_1_MIN,
    ALARM_PAUSE_2_MIN,
    ALARM_PAUSE_3_MIN,
    ALARM_PAUSE_5_MIN,
    ALARM_PAUSE_10_MIN,
    ALARM_PAUSE_15_MIN,
    ALARM_PAUSE_FOREVER,
    ALARM_PAUSE_NR
};

/**************************************************************************************************
 * ST报警延迟。
 *************************************************************************************************/
enum AlarmSTDelay
{
    ALARM_ST_DELAY_30S,
    ALARM_ST_DELAY_45S,
    ALARM_ST_DELAY_1MIN,
    ALARM_ST_DELAY_1D5MIN,
    ALARM_ST_DELAY_2MIN,
    ALARM_ST_DELAY_3MIN,
    ALARM_ST_DELAY_NR
};
