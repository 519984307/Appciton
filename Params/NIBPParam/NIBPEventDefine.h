/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/20
 **/

#pragma once

enum NIBPEvent
{
    // NIBP模块自检事件
    NIBP_EVENT_MODULE_RESET,                             // NIBP模块重启
    NIBP_EVENT_MODULE_ERROR,                             // NIBP模块错误
    NIBP_EVENT_CONNECTION_ERROR,                         // NIBP模块通信错误
    NIBP_EVENT_CONNECTION_NORMAL,                        // NIBP模块通信正常
    NIBP_EVENT_TIMEOUT,                                  // NIBP事件超时
    NIBP_EVENT_CURRENT_PRESSURE,                         // NIBP当前压力
    NIBP_EVENT_TRIGGER_BUTTON,                           // NIBP按钮触发
    NIBP_EVENT_TRIGGER_PATIENT_TYPE,                     // NIBP切换病人
    NIBP_EVENT_TRIGGER_MODEL,                            // NIBP模式切换
    NIBP_EVENT_ZERO_SELFTEST,                            // NIBP较零自测

    // monitor组事件。
    NIBP_EVENT_MONITOR_STOP,                             // NIBP停止
    NIBP_EVENT_MONITOR_START_MEASURE,                    // NIBP启动测量
    NIBP_EVENT_MONITOR_MEASURE_DONE,                     // NIBP测量完成
    NIBP_EVENT_MONITOR_GET_RESULT,                       // NIBP获取测量结果



    // service组事件。
    NIBP_EVENT_SERVICE_REPAIR_ENTER_SUCCESS,             // 进入NIBP维护模式成功
    NIBP_EVENT_SERVICE_REPAIR_ENTER_FAIL,                // 进入NIBP维护模式失败
    NIBP_EVENT_SERVICE_REPAIR_RETURN,                    // 退出NIBP维护模式

    NIBP_EVENT_SERVICE_CALIBRATE_ENTER,                  // 进入NIBP校准
    NIBP_EVENT_SERVICE_CALIBRATE_RETURN,                 // 退出NIBP校准

    NIBP_EVENT_SERVICE_CALIBRATE_CMD_PRESSURE_POINT,     // NIBP校准点压力值输入
    NIBP_EVENT_SERVICE_CALIBRATE_RSP_PRESSURE_POINT,

    NIBP_EVENT_SERVICE_MANOMETER_ENTER,                  // 进入NIBP压力计
    NIBP_EVENT_SERVICE_MANOMETER_RETURN,                 // 退出NIBP压力计

    NIBP_EVENT_SERVICE_PRESSURECONTROL_ENTER,            // 进入NIBP压力控制
    NIBP_EVENT_SERVICE_PRESSURECONTROL_RETURN,           // 退出NIBP压力控制
    NIBP_EVENT_SERVICE_PRESSURECONTROL_INFLATE,          // 充气控制
    NIBP_EVENT_SERVICE_PRESSURECONTROL_DEFLATE,          // 放气控制
    NIBP_EVENT_SERVICE_PRESSURECONTROL_PUMP,             // 气阀控制
    NIBP_EVENT_SERVICE_PRESSURECONTROL_VALVE,            // 气阀控制

    NIBP_EVENT_SERVICE_CALIBRATE_ZERO_ENTER,             // 进入NIBP校零
    NIBP_EVENT_SERVICE_CALIBRATE_ZERO_RETURN,            // 退出NIBP校零

    NIBP_EVENT_SERVICE_CALIBRATE_ZERO,                   // 校准零点值

    NIBP_EVENT_SERVICE_Pressurepoint,                    // 校准点压力值输入应答
    NIBP_EVENT_SERVICE_STATE_CHANGE,                     // NIBP状态改变

    NIBP_EVENT_NR
};
