#pragma once
#include <QString>
#include <QTime>
#include "NIBPProviderIFace.h"
#include "Debug.h"

enum NIBPMonitorStateID
{
    NIBP_MONITOR_STANDBY_STATE,                    // 待机状态。
    NIBP_MONITOR_STARTING_STATE,                   // 启动测量状态。
    NIBP_MONITOR_MEASURE_STATE,                    // 正在测量状态。
    NIBP_MONITOR_GET_RESULT_STATE,                 // 获取结果状态。
    NIBP_MONITOR_SAFEWAITTIME_STATE,               // 安全等待时间。
    NIBP_MONITOR_STOPE_STATE,                      // 停止状态。
    NIBP_MONITOR_ERROR_STATE,                      // 错误状态。
    NIBP_MONITOR_NR
};
