/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/1
 **/

#pragma once
#include "Framework/Utility/Unit.h"
#include "BaseDefine.h"

/**************************************************************************************************
 * AG work mode.
 *************************************************************************************************/
enum AGWorkMode
{
    AG_WORK_SELFTEST = 0x00,       //自检
    AG_WORK_SLEEP = 0x01,          //睡眠
    AG_WORK_MEASUREMENT = 0x02     //测量
};

/**************************************************************************************************
 * AG parameter value and unit.
 *************************************************************************************************/
struct AGParamInfo
{
    AGParamInfo() : value(InvData()), unit(UNIT_PERCENT)
    {
    }
    short value;
    UnitType unit;
};

/**************************************************************************************************
 * AG waveform speed.
 *************************************************************************************************/
enum AGSweepSpeed
{
    AG_SWEEP_SPEED_62_5,
    AG_SWEEP_SPEED_125,
    AG_SWEEP_SPEED_250,
    AG_SWEEP_SPEED_500,
    AG_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * AG气体类型
 *************************************************************************************************/
enum AGTypeGas
{
    AG_TYPE_CO2     =       0x00,
    AG_TYPE_N2O,
    AG_TYPE_AA1,
    AG_TYPE_AA2,
    AG_TYPE_O2,
    AG_TYPE_NR
};

/**************************************************************************************************
 * AG显示放大系数
 *************************************************************************************************/
enum AGDisplayZoom
{
    AG_DISPLAY_ZOOM_4,
    AG_DISPLAY_ZOOM_8,
    AG_DISPLAY_ZOOM_15,
    AG_DISPLAY_ZOOM_NR
};

/**************************************************************************************************
 * AG麻醉类型
 *************************************************************************************************/
enum AGAnaestheticType
{
    AG_ANAESTHETIC_NO               =       0x00,
    AG_ANAESTHETIC_HALOTHANE,                           // 氟烷
    AG_ANAESTHETIC_ENFLURANE,                           // 安氟醚
    AG_ANAESTHETIC_ISOFLURANE,                          // 异氟醚
    AG_ANAESTHETIC_SEVOFLURANE,                         // 七氟醚
    AG_ANAESTHETIC_DESFLURANE,                          // 地氟醚
    AG_ANAESTHETIC_NR
};

/**************************************************************************************************
 * 超限报警定义
 *************************************************************************************************/
enum AGLimitAlarmType
{
    AG_LIMIT_ALARM_ETCO2_LOW,
    AG_LIMIT_ALARM_ETCO2_HIGH,
    AG_LIMIT_ALARM_FICO2_LOW,
    AG_LIMIT_ALARM_FICO2_HIGH,

    AG_LIMIT_ALARM_ETN2O_LOW,
    AG_LIMIT_ALARM_ETN2O_HIGH,
    AG_LIMIT_ALARM_FIN2O_LOW,
    AG_LIMIT_ALARM_FIN2O_HIGH,

    AG_LIMIT_ALARM_ETAA1_LOW,
    AG_LIMIT_ALARM_ETAA1_HIGH,
    AG_LIMIT_ALARM_FIAA1_LOW,
    AG_LIMIT_ALARM_FIAA1_HIGH,

    AG_LIMIT_ALARM_ETAA2_LOW,
    AG_LIMIT_ALARM_ETAA2_HIGH,
    AG_LIMIT_ALARM_FIAA2_LOW,
    AG_LIMIT_ALARM_FIAA2_HIGH,

    AG_LIMIT_ALARM_ETO2_LOW,
    AG_LIMIT_ALARM_ETO2_HIGH,
    AG_LIMIT_ALARM_FIO2_LOW,
    AG_LIMIT_ALARM_FIO2_HIGH,

    AG_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * one shot alarm define.
 *************************************************************************************************/
enum AGOneShotType
{
    AG_ONESHOT_ALARM_APNEA,

    AG_ONESHOT_ALARM_SOFTWARE_ERROR,
    AG_ONESHOT_ALARM_HARDWARE_ERROR,
    AG_ONESHOT_ALARM_MOTOR_ERROR,
    AG_ONESHOT_ALARM_MISS_FACTORY_CALI,
    AG_ONESHOT_ALARM_O2_SENSOR_ERROR,

    AG_ONESHOT_ALARM_REPLACE_ADAPTER,
    AG_ONESHOT_ALARM_SAMPLING_LINE_CLOGGED,
    AG_ONESHOT_ALARM_NO_ADAPTER,
    AG_ONESHOT_ALARM_NO_SAMPLING_LINE,
    AG_ONESHOT_ALARM_O2_PORT_FAILURE,

    AG_ONESHOT_ALARM_CO2_OUT_RANGE,
    AG_ONESHOT_ALARM_N2O_OUT_RANGE,
    AG_ONESHOT_ALARM_AX_OUT_RANGE,
    AG_ONESHOT_ALARM_O2_OUT_RANGE,
    AG_ONESHOT_ALARM_TEMP_OUT_RANGE,
    AG_ONESHOT_ALARM_PRESS_OUT_RANGE,
    AG_ONESHOT_ALARM_ZERO_REQUIRED,
    AG_ONESHOT_ALARM_AGENT_UNRELIABLE,

    AG_ONESHOT_ALARM_ZERO_DISABLE,
    AG_ONESHOT_ALARM_ZERO_AND_SPAN_DISABLE,
    AG_ONESHOT_ALARM_ZERO_IN_PROGRESS,

    AG_ONESHOT_ALARM_SPAN_CALIB_FAILED,
    AG_ONESHOT_ALARM_SPAN_CALIB_IN_PROGRESS,
    AG_ONESHOT_ALARM_IR_O2_DELAY,

    AG_ONESHOT_ALARM_ZEROING_COMPLETED,

    AG_ONESHOT_NR,
};

/**************************************************************************************************
 * AG 模块开关
 *************************************************************************************************/
enum AGModuleSwitch
{
    AG_MODULE_SWITCH_ENABLE,
    AG_MODULE_SWITCH_DISABLE,
    AG_MODULE_SWITCH_NR
};

/**************************************************************************************************
 * CO2 菜单项
 *************************************************************************************************/
enum AGModuleCO2Option
{
    AG_MODULE_CO2_OPTION_SET,
    AG_MODULE_CO2_OPTION_NR,
};

/**************************************************************************************************
 * N2O 菜单项
 *************************************************************************************************/
enum AGModuleN2OOption
{
    AG_MODULE_N2O_OPTION_SET,
    AG_MODULE_N2O_OPTION_NR,
};

/**************************************************************************************************
 * AA1 菜单项
 *************************************************************************************************/
enum AGModuleAA1Option
{
    AG_MODULE_AA1_OPTION_SET,
    AG_MODULE_AA1_OPTION_NR,
};

/**************************************************************************************************
 * AA2 菜单项
 *************************************************************************************************/
enum AGModuleAA2Option
{
    AG_MODULE_AA2_OPTION_SET,
    AG_MODULE_AA2_OPTION_NR,
};

/**************************************************************************************************
 * O2 菜单项
 *************************************************************************************************/
enum AGModuleO2Option
{
    AG_MODULE_O2_OPTION_SET,
    AG_MODULE_O2_OPTION_NR,
};
