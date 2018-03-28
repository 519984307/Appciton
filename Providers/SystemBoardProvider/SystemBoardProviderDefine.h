#pragma once

enum SystemBoardMessageType
{
    // 下行数据
    MSG_NASK                       = 0x00,   // nask
    MSG_ASK                        = 0x01,   // 应答

    MSG_CMD_QUERY_VERSION          = 0x10,   // 获取版本
    MSG_CMD_QUERY_MODE_AND_STATUS  = 0x12,   // 获取模式与状态
    MSG_CMD_SET_ALARM_LED          = 0x14,   // 报警灯控制
    MSG_CMD_SET_INDICATOR_LED      = 0x16,   // 指示灯控制
    MSG_CMD_QUERY_DEFIB_KEY_STATUS = 0x18,   // 获取除颤按键状态
    MSG_CMD_TRIGGER_BUZZER         = 0x1A,   // 蜂鸣控制器
    MSG_CMD_CFG_BAT_ARGS           = 0x1C,   // 配置电池参数
    MSG_CMD_FIXED_BAT_INFO         = 0x1E,   // 获取电池的固定信息
    MSG_CMD_SET_SHOCK_LED          = 0x20,   // 设置SHOCK背景灯
    MSG_CMD_LED_SELFTEST           = 0x22,   // 设置LED灯自检

    // 上行数据
    MSG_RSP_VERSION                = 0x11,    // 版本应答
    MSG_RSP_MODE_AND_STATUS        = 0x13,    // 模式与状态应答
    MSG_RSP_ALARM_LED_RESPONSE     = 0x15,    // 报警灯控制应答
    MSG_RSP_DEFIB_KEY_STATUS       = 0x19,    // 除颤按键状态应答
    MSG_RSP_TRIGGER_BUZZER         = 0x1B,    // 蜂鸣控制应答
    MSG_RSP_CFG_BAT_ARGS           = 0x1D,    // 配置电池参数应答
    MSG_RSP_FIXED_BAT_INFO         = 0x1F,    // 获取电池的固定信息应答
    MSG_RSP_SET_SHOCK_LED          = 0x21,    // 设置SHOCK背景灯应答
    MSG_RSP_LED_SELFTEST           = 0x22,    // 设置LED灯自检应答
    MSG_NOTIFY_START               = 0x40,    // 启动
    MSG_NOTIFY_KEY                 = 0x41,    // 按键
    MSG_NOTIFY_DEFIB_KEY           = 0x42,    // 除颤按键信息
    MSG_NOTIFY_MODE_CHANGE         = 0X43,    // 模式更改
    MSG_NOTIFY_AC_BAT_CHANGE       = 0x44,    // 电池状态更改
    MSG_PERIODIC_KEEP_ALIVE        = 0x5B,    // 保活
    MSG_PERIODIC_BAT_INFO          = 0x5C,    // 电池信息
    MSG_ERROR_INFO                 = 0x76,    // 错误帧
};
