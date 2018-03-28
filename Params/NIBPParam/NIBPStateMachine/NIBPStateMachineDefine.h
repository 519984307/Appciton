#pragma once
#include <QMap>

enum NIBPStateMachineType
{
    NIBP_STATE_MACHINE_NONE,
    NIBP_STATE_MACHINE_MONITOR,           // 正常模式。
    NIBP_STATE_MACHINE_SERVICE,           // 维护模式
    NIBP_STATE_MACHINE_NR
};
