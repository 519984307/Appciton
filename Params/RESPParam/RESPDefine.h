/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/28
 **/



#pragma once

enum RESPSweepMode
{
    RESP_SWEEP_MODE_CURVE,
    RESP_SWEEP_MODE_FILLED,
    RESP_SWEEP_MODE_NR
};

/**
 * @brief The RESPModuleType enum  RESP模块类型枚举
 */
enum RESPModuleType
{
    MODULE_BLM_E5,
    MODULE_NR,
};

enum BRRRSourceType
{
    BR_RR_AUTO,
    BR_RR_SOURCE_ECG,
    BR_RR_SOURCE_CO2,
    BR_RR_NR,
};
/**************************************************************************************************
 * 血氧波形速度。
 *************************************************************************************************/
enum RESPSweepSpeed
{
    RESP_SWEEP_SPEED_6_25,
    RESP_SWEEP_SPEED_12_5,
    RESP_SWEEP_SPEED_25_0,
    RESP_SWEEP_SPEED_50_0,
    RESP_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * RESP窒息时间。
 *************************************************************************************************/
enum RESPApneaTime
{
    RESP_APNEA_TIME_0_SEC,
    RESP_APNEA_TIME_20_SEC,
    RESP_APNEA_TIME_25_SEC,
    RESP_APNEA_TIME_30_SEC,
    RESP_APNEA_TIME_35_SEC,
    RESP_APNEA_TIME_40_SEC,
    RESP_APNEA_TIME_45_SEC,
    RESP_APNEA_TIME_50_SEC,
    RESP_APNEA_TIME_55_SEC,
    RESP_APNEA_TIME_60_SEC,
    RESP_APNEA_TIME_NR
};

/**************************************************************************************************
 * RESP波形放大倍数
 *************************************************************************************************/
enum RESPZoom
{
    RESP_ZOOM_X025,      // x0.25
    RESP_ZOOM_X050,      // x0.5
    RESP_ZOOM_X100,      // x1
    RESP_ZOOM_X200,      // x2
    RESP_ZOOM_X300,      // x3
    RESP_ZOOM_X400,      // x4
    RESP_ZOOM_X500,      // x5
    RESP_ZOOM_NR
};

/**************************************************************************************************
 * RESP Lead
 *************************************************************************************************/
enum RESPLead
{
    RESP_LEAD_II,
    RESP_LEAD_I,
    RESP_LEAD_AUTO,
    RESP_LEAD_NR
};

/**************************************************************************************************
 * RESPOneShot事件定义。
 *************************************************************************************************/
enum RESPOneShotType
{
    RESP_ONESHOT_ALARM_APNEA,
    RESP_ONESHOT_ALARM_COMMUNICATION_STOP,
    RESP_ONESHOT_ALARM_CHECK_ELECTRODES,
    RESP_ONESHOT_NR,
};

/**************************************************************************************************
 * RESP超限报警定义
 *************************************************************************************************/
enum RESPLimitAlarmType
{
    RESP_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * RESP DUP超限报警定义
 *************************************************************************************************/
enum RESPDupLimitAlarmType
{
    RESP_DUP_LIMIT_ALARM_RR_LOW,
    RESP_DUP_LIMIT_ALARM_RR_HIGH,
    RESP_DUP_LIMIT_ALARM_BR_LOW,
    RESP_DUP_LIMIT_ALARM_BR_HIGH,
    RESP_DUP_LIMIT_ALARM_NR
};
