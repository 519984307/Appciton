/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/8
 **/


#pragma once

enum COModuleType
{
    CO_MODULE_SMART_CO,
    CO_MODULE_NR
};

/**************************************************************************************************
 * CO测量控制：
 *      终止(0x00)
 *      启动(0x01)
 *      中断(0x02)
 *************************************************************************************************/
enum COMeasureCtrl
{
    CO_MEASURE_START  = 0x00,
    CO_MEASURE_STOP   = 0x01,
    CO_MEASURE_NR
};

/**************************************************************************************************
 * CO测量间隔：
 *      30(0x00)
 *      40(0x01)
 *      60(0x02)
 *      90(0x03)
 *************************************************************************************************/
enum COMeasureInterval
{
    CO_INTERVAL_30              =   0x00,
    CO_INTERVAL_40              =   0x01,
    CO_INTERVAL_60              =   0x02,
    CO_INTERVAL_90              =   0x03,
    CO_INTERVAL_NR
};

/**************************************************************************************************
 * Ti输入模式设置：
 *      自动TI输入(0x00)
 *      手动设置水温(0x01)
 *************************************************************************************************/
enum COTiSource
{
    CO_TI_SOURCE_AUTO             =   0x00,
    CO_TI_SOURCE_MANUAL           =   0x01,
    CO_TI_SOURCE_NR
};

/**************************************************************************************************
 * 超限报警定义
 *************************************************************************************************/
enum COLimitAlarmType
{
    CO_LIMIT_ALARM_CO_LOW,
    CO_LIMIT_ALARM_CO_HIGH,

    CO_LIMIT_ALARM_CI_LOW,
    CO_LIMIT_ALARM_CI_HIGH,

    CO_LIMIT_ALARM_TB_LOW,
    CO_LIMIT_ALARM_TB_HIGH,

    CO_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * one shot alarm define.
 *************************************************************************************************/
enum COOneShotType
{
    CO_ONESHOT_ALARM_TB_SENSOR_OFF,
    CO_ONESHOT_ALARM_MEASURE_TIMEOUT,
    CO_ONESHOT_ALARM_MEASURE_FAIL,
    CO_ONESHOT_ALARM_COMMUNICATION_STOP,
    CO_ONESHOT_NR,
};
