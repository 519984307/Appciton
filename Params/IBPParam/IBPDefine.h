/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/27
 **/

#pragma once
#include "BaseDefine.h"
#define  IBP_AUTO_SCALE_INDEX               0
#define  IBP_MANUAL_SCALE_INDEX             14

/**************************************************************************************************
 * IBP压力标名
 *************************************************************************************************/
enum IBPPressureName
{
    IBP_PRESSURE_ART    =   0x00,               // 动脉压
    IBP_PRESSURE_PA ,                           // 肺动脉压
    IBP_PRESSURE_CVP,                           // 中心静脉压
    IBP_PRESSURE_LAP,                           // 左心房压
    IBP_PRESSURE_RAP,                           // 右心房压
    IBP_PRESSURE_ICP,                           // 颅内压
    IBP_PRESSURE_AUXP1,                         // 扩充压力P1
    IBP_PRESSURE_AUXP2,                         // 扩充压力P2
    IBP_PRESSURE_NR
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

/**************************************************************************************************
 * IBP信号输入接口：
 *      IBP1(0x00)
 *      IBP2(0x01)
 *************************************************************************************************/
enum IBPSignalInput
{
    IBP_INPUT_1         =   0x00,
    IBP_INPUT_2         =   0x01,
    IBP_INPUT_NR
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
enum IBPAuxiliarySet
{
    IBP_MEASURE_CALC_3          =   0x00,
    IBP_MEASURE_CALC_1          =   0x01,
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
    IBPParamInfo(){
        pressureName = IBP_PRESSURE_ART;
        sys = InvData();
        dia = InvData();
        mean = InvData();
        pr = InvData();
    }
    IBPPressureName pressureName;
    unsigned short sys;
    unsigned short dia;
    unsigned short mean;
    unsigned short pr;
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
    IBP_ONESHOT_NR,
};
