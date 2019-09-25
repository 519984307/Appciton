/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/24
 **/

#pragma once

/**
 * @brief The TEMPModuleType enum  体温模块类型枚举
 */
enum TEMPModuleType
{
    MODULE_BLM_T5,
    MODULE_TEMP_NR,
};

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
    TEMP_ONESHOT_ALARM_NOT_CALIBRATION_1,
    TEMP_ONESHOT_ALARM_NOT_CALIBRATION_2,
    TEMP_ONESHOT_ALARM_SEND_COMMUNICATION_STOP,
    TEMP_ONESHOT_ALARM_MODULE_ABNORMAL,
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

/**
 * @brief The TEMPChannelIndex enum
 */
enum TEMPChannelIndex
{
    TEMP_CHANNEL_ONE,
    TEMP_CHANNEL_TWO,
};

/**
 * @brief The TEMPChannelType enum
 */
enum TEMPChannelType
{
    TEMP_CHANNEL_T_ONE = 0,
    TEMP_CHANNEL_T_TWO = 0,
    TEMP_CHANNEL_T_SKIN,
    TEMP_CHANNEL_T_CORE,
    TEMP_CHANNEL_T_AXIL,
    TEMP_CHANNEL_T_NASO,
    TEMP_CHANNEL_T_ESO,
    TEMP_CHANNEL_T_RECT,
    TEMP_CHANNEL_T_AMB,
    TEMP_CHANNEL_T_AIRW,
    TEMP_CHANNEL_T_VESIC,
    TEMP_CHANNEL_T_BLOOD,
    TEMP_CHANNEL_T_MYO,
    TEMP_CHANNEL_T_TYMP,
    TEMP_CHANNEL_T_BRAIN,
    TEMP_CHANNEL_NR,
};
