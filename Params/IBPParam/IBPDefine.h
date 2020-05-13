/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include "BaseDefine.h"

enum IBPModuleType
{
    IBP_MODULE_SMART_IBP,
    IBP_MODULE_WITLEAF,
    IBP_MODULE_NR,
};

/**************************************************************************************************
 * IBP压力标名
 *************************************************************************************************/
enum IBPLabel
{
    IBP_LABEL_ART    =   0x00,               // 动脉压
    IBP_LABEL_PA ,                           // 肺动脉压
    IBP_LABEL_CVP,                           // 中心静脉压
    IBP_LABEL_LAP,                           // 左心房压
    IBP_LABEL_RAP,                           // 右心房压
    IBP_LABEL_ICP,                           // 颅内压
    IBP_LABEL_AUXP1,                         // 扩充压力P1
    IBP_LABEL_AUXP2,                         // 扩充压力P2
    IBP_LABEL_NR
};

/**
 * @brief The IBPScaleInfo struct
 *        use to store the IBP scale info
 */
struct IBPScaleInfo
{
    IBPScaleInfo(): low(0), high(300), isAuto(false){}
    int low;            // low value
    int high;           // high value
    bool isAuto;        // is automatically adjust or not
};

/* IBP channel define */
enum IBPChannel
{
    IBP_CHN_1         =   0x00,
    IBP_CHN_2         =   0x01,
    IBP_CHN_NR
};

/**************************************************************************************************
 * IBP校零，校准
 *      校零(0x00)
 *      校准(0x01)
 *************************************************************************************************/
enum IBPCalibration
{
    IBP_CALIBRATION_ZERO        =   0x00,
    IBP_CALIBRATION_SET         =   0x01,
    IBP_CALIBRATION_NR
};

/**************************************************************************************************
 * IBP滤波模式：
 *      Filter0(0x00): 12.5Hz 低通
 *      Filter1(0x01):   40Hz 低通
 *************************************************************************************************/
enum IBPFilterMode
{
    IBP_FILTER_MODE_0           =   0x00,
    IBP_FILTER_MODE_1           =   0x01,
    IBP_FILTER_MODE_NR
};

/**************************************************************************************************
 * IBP灵敏度：
 *      Sensitivity　High(0x00): 1s平均时间
 *      Sensitivity　Mid(0x01):  8s平均时间
 *      Sensitivity　Low(0x02):  12s平均时间
 *************************************************************************************************/
enum IBPSensitivity
{
    IBP_SENSITIVITY_HIGH           =   0x00,
    IBP_SENSITIVITY_MID            =   0x01,
    IBP_SENSITIVITY_LOW            =   0x02,
    IBP_SENSITIVITY_NR
};

/**************************************************************************************************
 * IBP辅助设置：
 *      测量计算3(0x00): sys,map,dia
 *      测量计算1(0x01): map
 *************************************************************************************************/
enum IBPMeasueType
{
    IBP_MEASURE_SYS_DIA_MAP     =   0x00,
    IBP_MEASURE_MAP             =   0x01,
    IBP_MEASURE_CALC_NR
};

/**************************************************************************************************
 * IBP波形速度。
 *************************************************************************************************/
enum IBPSweepSpeed
{
    IBP_SWEEP_SPEED_62_5,
    IBP_SWEEP_SPEED_125,
    IBP_SWEEP_SPEED_250,
    IBP_SWEEP_SPEED_500,
    IBP_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * IBP压力标名
 *************************************************************************************************/
struct IBPParamInfo
{
    IBPParamInfo()
        : pressureName(IBP_LABEL_ART),
        sys(InvData()),
        dia(InvData()),
        mean(InvData()),
        pr(InvData())
    {
    }
    IBPLabel pressureName;
    short sys;
    short dia;
    short mean;
    short pr;
};

enum IBPZeroResult
{
    IBP_ZERO_SUCCESS = 0,
    IBP_ZERO_IS_PULSE = 1,
    IBP_ZERO_BEYOND_RANGE = 2,
    IBP_ZERO_FAIL = 4,
    IBP_ZERO_NOT_SET_TIME = 5,
};

enum IBPCalibrationResult
{
    IBP_CALIBRATION_SUCCESS = 0,
    IBP_CALIBRATION_IS_PULSE = 1,
    IBP_CALIBRATION_BEYOND_RANGE = 2,
    IBP_CALIBRATION_NOT_ZERO = 3,
    IBP_CALIBRATION_FAIL = 4,
    IBP_CALIBRATION_NOT_SET_TIME = 6,
};

/**************************************************************************************************
 * 超限报警定义
 *************************************************************************************************/
enum IBPLimitAlarmType
{
    ART_LIMIT_ALARM_SYS_LOW,
    ART_LIMIT_ALARM_SYS_HIGH,
    ART_LIMIT_ALARM_DIA_LOW,
    ART_LIMIT_ALARM_DIA_HIGH,
    ART_LIMIT_ALARM_MEAN_LOW,
    ART_LIMIT_ALARM_MEAN_HIGH,
    ART_LIMIT_ALARM_PR_LOW,
    ART_LIMIT_ALARM_PR_HIGH,

    PA_LIMIT_ALARM_SYS_LOW,
    PA_LIMIT_ALARM_SYS_HIGH,
    PA_LIMIT_ALARM_DIA_LOW,
    PA_LIMIT_ALARM_DIA_HIGH,
    PA_LIMIT_ALARM_MEAN_LOW,
    PA_LIMIT_ALARM_MEAN_HIGH,
    PA_LIMIT_ALARM_PR_LOW,
    PA_LIMIT_ALARM_PR_HIGH,

    CVP_LIMIT_ALARM_MEAN_LOW,
    CVP_LIMIT_ALARM_MEAN_HIGH,
    CVP_LIMIT_ALARM_PR_LOW,
    CVP_LIMIT_ALARM_PR_HIGH,

    LAP_LIMIT_ALARM_MEAN_LOW,
    LAP_LIMIT_ALARM_MEAN_HIGH,
    LAP_LIMIT_ALARM_PR_LOW,
    LAP_LIMIT_ALARM_PR_HIGH,

    RAP_LIMIT_ALARM_MEAN_LOW,
    RAP_LIMIT_ALARM_MEAN_HIGH,
    RAP_LIMIT_ALARM_PR_LOW,
    RAP_LIMIT_ALARM_PR_HIGH,

    ICP_LIMIT_ALARM_MEAN_LOW,
    ICP_LIMIT_ALARM_MEAN_HIGH,
    ICP_LIMIT_ALARM_PR_LOW,
    ICP_LIMIT_ALARM_PR_HIGH,

    AUXP1_LIMIT_ALARM_SYS_LOW,
    AUXP1_LIMIT_ALARM_SYS_HIGH,
    AUXP1_LIMIT_ALARM_DIA_LOW,
    AUXP1_LIMIT_ALARM_DIA_HIGH,
    AUXP1_LIMIT_ALARM_MEAN_LOW,
    AUXP1_LIMIT_ALARM_MEAN_HIGH,
    AUXP1_LIMIT_ALARM_PR_LOW,
    AUXP1_LIMIT_ALARM_PR_HIGH,

    AUXP2_LIMIT_ALARM_SYS_LOW,
    AUXP2_LIMIT_ALARM_SYS_HIGH,
    AUXP2_LIMIT_ALARM_DIA_LOW,
    AUXP2_LIMIT_ALARM_DIA_HIGH,
    AUXP2_LIMIT_ALARM_MEAN_LOW,
    AUXP2_LIMIT_ALARM_MEAN_HIGH,
    AUXP2_LIMIT_ALARM_PR_LOW,
    AUXP2_LIMIT_ALARM_PR_HIGH,

    IBP_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * one shot alarm define.
 *************************************************************************************************/
enum IBPOneShotType
{
    IBP1_LEAD_OFF,
    IBP2_LEAD_OFF,
    IBP1_ZERO_SUCCESS,
    IBP1_ZERO_IS_PULSE,
    IBP1_ZERO_BEYOND_RANGE,
    IBP1_ZERO_FAIL,
    IBP1_ZERO_NOT_SET_TIME,
    IBP2_ZERO_SUCCESS,
    IBP2_ZERO_IS_PULSE,
    IBP2_ZERO_BEYOND_RANGE,
    IBP2_ZERO_FAIL,
    IBP2_ZERO_NOT_SET_TIME,
    IBP1_CALIB_SUCCESS,
    IBP1_CALIB_IS_PULSE,
    IBP1_CALIB_BEYOND_RANGE,
    IBP1_CALIB_NOT_ZERO,
    IBP1_CALIB_FAIL,
    IBP1_CALIB_NOT_SET_TIME,
    IBP2_CALIB_SUCCESS,
    IBP2_CALIB_IS_PULSE,
    IBP2_CALIB_BEYOND_RANGE,
    IBP2_CALIB_NOT_ZERO,
    IBP2_CALIB_FAIL,
    IBP2_CALIB_NOT_SET_TIME,
    IBP_ONESHOT_NR,
};

enum IBPRulerLimit
{
    IBP_RULER_LIMIT_AOTU,
    IBP_RULER_LIMIT_10_10,
    IBP_RULER_LIMIT_0_20,
    IBP_RULER_LIMIT_0_30,
    IBP_RULER_LIMIT_0_40,
    IBP_RULER_LIMIT_0_60,
    IBP_RULER_LIMIT_0_80,
    IBP_RULER_LIMIT_60_140,
    IBP_RULER_LIMIT_30_140,
    IBP_RULER_LIMIT_0_140,
    IBP_RULER_LIMIT_0_160,
    IBP_RULER_LIMIT_0_200,
    IBP_RULER_LIMIT_0_240,
    IBP_RULER_LIMIT_0_300,
    IBP_RULER_LIMIT_MANUAL
};
