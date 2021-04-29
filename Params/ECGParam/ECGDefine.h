/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/1
 **/


#pragma once

/**
 * @brief The PrSourceType enum  ECG 脉率来源
 */
enum PRSourceType
{
    PR_SOURCE_SPO2 = 0,
    PR_SOURCE_IBP,
    PR_SOURCE_AUTO,
};

/**
 * @brief The ECGHRPRSourceType enum  ECG 心率来源
 */
enum HRSourceType
{
    HR_SOURCE_ECG,
    HR_SOURCE_SPO2,
    HR_SOURCE_IBP,
    HR_SOURCE_AUTO,
    HR_SOURCE_NR
};

/**************************************************************************************************
 * ECG导联系统。
 *************************************************************************************************/
enum ECGLeadMode
{
    ECG_LEAD_MODE_3,       // 3导联
    ECG_LEAD_MODE_5,       // 5导联
    ECG_LEAD_MODE_12,      // 12导联
    ECG_LEAD_MODE_NR
};

/**************************************************************************************************
 * ECG显示模式。
 *************************************************************************************************/
enum ECGDisplayMode
{
    ECG_DISPLAY_NORMAL,
    ECG_DISPLAY_12_LEAD_FULL,
    ECG_DISPLAY_NR
};

/**************************************************************************************************
 * ECG alarm source
 *************************************************************************************************/
enum ECGAlarmSource
{
    ECG_ALARM_SRC_HR,
    ECG_ALARM_SRC_PR,
    ECG_ALARM_SRC_AUTO,
    ECG_ALARM_SRC_NR,
};

// 12导显示格式。
enum Display12LeadFormat
{
    DISPLAY_12LEAD_STAND,
    DISPLAY_12LEAD_CABRELA,
    DISPLAY_12LEAD_NR
};

/**************************************************************************************************
 * ECG导联
 *************************************************************************************************/
enum ECGLead
{
    ECG_LEAD_I,
    ECG_LEAD_II,
    ECG_LEAD_III,
    ECG_LEAD_AVR,
    ECG_LEAD_AVL,
    ECG_LEAD_AVF,
    ECG_LEAD_V1,
    ECG_LEAD_V2,
    ECG_LEAD_V3,
    ECG_LEAD_V4,
    ECG_LEAD_V5,
    ECG_LEAD_V6,
    ECG_LEAD_NR
};

/**************************************************************************************************
 * ECG_ST
 *************************************************************************************************/
enum ECGST
{
    ECG_ST_I,
    ECG_ST_II,
    ECG_ST_III,
    ECG_ST_aVR,
    ECG_ST_aVL,
    ECG_ST_aVF,
    ECG_ST_V1,
    ECG_ST_V2,
    ECG_ST_V3,
    ECG_ST_V4,
    ECG_ST_V5,
    ECG_ST_V6,
    ECG_ST_NR
};

/**************************************************************************************************
 * ECG增益
 *************************************************************************************************/
enum ECGGain
{
    ECG_GAIN_X0125,      // x0.125
    ECG_GAIN_X025,      // x0.25
    ECG_GAIN_X05,      // x0.5
    ECG_GAIN_X10,      // x1
    ECG_GAIN_X20,      // x2
    ECG_GAIN_X40,      // x4
    ECG_GAIN_AUTO,     // AUTO
    ECG_GAIN_NR
};

/**************************************************************************************************
 * ECG起搏分析开关
 *************************************************************************************************/
enum ECGPaceMode
{
    ECG_PACE_OFF,     // 关
    ECG_PACE_ON,      // 开
    ECG_PACE_NR
};

/**************************************************************************************************
 * ECG波形速度。
 *************************************************************************************************/
enum ECGSweepSpeed
{
    ECG_SWEEP_SPEED_625,
    ECG_SWEEP_SPEED_125,
    ECG_SWEEP_SPEED_250,
    ECG_SWEEP_SPEED_500,
    ECG_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * ECG文件传输格式。
 *************************************************************************************************/
enum ECGTransferFileFormat
{
    ECG_TRANSFER_FILE_FORMAT_PDF = 0,
    ECG_TRANSFER_FILE_FORMAT_JSON,
    ECG_TRANSFER_FILE_FORMAT_NR
};

/**************************************************************************************************
 * ECG带宽
 *************************************************************************************************/
enum ECGBandwidth
{
    ECG_BANDWIDTH_067_20HZ = 0,
    ECG_BANDWIDTH_067_40HZ,
    ECG_BANDWIDTH_0525_40HZ,
    ECG_BANDWIDTH_0525_150HZ,
    ECG_BANDWIDTH_NR,
    ECG_BANDWIDTH_MULTIPLE = ECG_BANDWIDTH_NR,   // multiple bandwidth, equal to the bandwidth NR
};

/**************************************************************************************************
 * ECG work mode
 *************************************************************************************************/
enum ECGWorkMode
{
    ECG_WORKMODE_DIAGNOSTIC,
    ECG_WORKMODE_MONITOR,
    ECG_WORKMODE_NR,
};

/**************************************************************************************************
 * ECG filter mode
 *************************************************************************************************/
enum ECGFilterMode
{
    ECG_FILTERMODE_SURGERY,
    ECG_FILTERMODE_MONITOR,
    ECG_FILTERMODE_DIAGNOSTIC,
    ECG_FILTERMODE_ST,
    ECG_FILTERMODE_NR,
};

/**************************************************************************************************
 * ECG工频滤波频率
 *************************************************************************************************/
enum ECGNotchFilter
{
    ECG_NOTCH_OFF,             // 工频关
    ECG_NOTCH_50HZ,            // 50Hz滤波
    ECG_NOTCH_60HZ,            // 60Hz滤波
    ECG_NOTCH_50_AND_60HZ,     // 50&60Hz滤波
    ECG_NOTCH_NR
};

/**************************************************************************************************
 * ECGOneShot事件定义，包括心率失常类型
 *************************************************************************************************/
enum ECGOneShotType
{
    ECG_ONESHOT_ARR_ASYSTOLE,     // 心脏停搏
    ECG_ONESHOT_ARR_VFIBVTAC,     // 室颤/室速
    ECG_ONESHOT_CHECK_PATIENT_ALARM,       // Check patient alarm

    ECG_ONESHOT_ALARM_LEADOFF,
    ECG_ONESHOT_ALARM_V1_LEADOFF,
    ECG_ONESHOT_ALARM_V2_LEADOFF,
    ECG_ONESHOT_ALARM_V3_LEADOFF,
    ECG_ONESHOT_ALARM_V4_LEADOFF,
    ECG_ONESHOT_ALARM_V5_LEADOFF,
    ECG_ONESHOT_ALARM_V6_LEADOFF,
    ECG_ONESHOT_ALARM_OVERLOAD,
    ECG_ONESHOT_ALARM_COMMUNICATION_STOP,
    ECG_ONESHOT_ALARM_SEND_COMMUNICATION_STOP,
    ECG_ONESHOT_NONE,
    ECG_ONESHOT_NR = ECG_ONESHOT_NONE,
};

/**************************************************************************************************
 * ECG 超限报警定义
 *************************************************************************************************/
enum ECGLimitAlarmType
{
    ECG_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * ECG 复用超限报警定义
 *************************************************************************************************/
enum ECGDupLimitAlarmType
{
    ECG_DUP_LIMIT_ALARM_HR_LOW,
    ECG_DUP_LIMIT_ALARM_HR_HIGH,
    ECG_DUP_LIMIT_ALARM_PR_LOW,
    ECG_DUP_LIMIT_ALARM_PR_HIGH,
    ECG_DUP_LIMIT_ALARM_PLUGIN_PR_LOW,
    ECG_DUP_LIMIT_ALARM_PLUGIN_PR_HIGH,
    ECG_DUP_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * ECG 导联命名协议
 *************************************************************************************************/
enum ECGLeadNameConvention
{
    ECG_CONVENTION_AAMI,
    ECG_CONVENTION_IEC,
    ECG_CONVENTION_NR
};

/***************************************************************************************************
 * ECG WAVE NOTIFY MESSAGE
 **************************************************************************************************/
enum ECGWaveNotify
{
    ECG_WAVE_NOTIFY_NORMAL,
    ECG_WAVE_NOTIFY_LEAD_OFF,
    ECG_WAVE_NOTIFY_CHECK_PATIENT,
    ECG_WAVE_NOTIFY_NR
};
