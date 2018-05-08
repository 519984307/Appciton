#pragma once
#include <QString>
#include <QTime>
#include "NIBPProviderIFace.h"
#include "Debug.h"

enum NIBPServiceStateID
{
    NIBP_SERVICE_STANDBY_STATE,                          // 准备模式。
    NIBP_SERVICE_CALIBRATE_STATE,                        // 校准模式。
    NIBP_SERVICE_MANOMETER_STATE,                        // 压力计模式。
    NIBP_SERVICE_PRESSURECONTROL_STATE,                  // 压力操控模式。
    NIBP_SERVICE_CALIBRATE_ZERO_STATE,                   // 校零模式。
    NIBP_SERVICE_UPGRADE_STATE,                          // 升级模式
    NIBP_SERVICE_ERROR_STATE,                            // 错误模式。
    NIBP_SERVICE_NULL,                                   // 空
};
