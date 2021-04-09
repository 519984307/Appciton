/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#include <QTimer>
#include "Debug.h"
#include "SystemManager.h"
#include "SystemBoardProvider.h"
#include "SystemAlarm.h"
#include "KeyActionManager.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "IConfig.h"
#include "PowerOffWindow.h"
#include "PowerManager.h"
#include "AlarmSourceManager.h"

enum SystemBoardMessageType
{
    // 下行数据
    MSG_NASK                       = 0x00,   // nask
    MSG_ASK                        = 0x01,   // 应答

    MSG_CMD_QUERY_VERSION          = 0x10,   // 获取版本
    MSG_CMD_QUERY_SWITCH_KEY       = 0x12,   // 获取旋钮开关的位置和快捷键的状态
    MSG_CMD_SET_ALARM_LED          = 0x14,   // 报警灯控制
    MSG_CMD_SET_INDICATOR_LED      = 0x16,   // 指示灯控制
    MSG_CMD_QUERY_KEY_STATUS       = 0x18,   // 获取除颤按键状态
    MSG_CMD_TRIGGER_BUZZER         = 0x1A,   // 蜂鸣控制器
    MSG_CMD_BATTERY_INFO           = 0x1E,   // 获取电池信息
    MSG_CMD_SET_KEYBOARD_BACKLIGHT = 0x20,   // 设置按键板背光
    MSG_CMD_LED_SELFTEST           = 0x22,   // 设置LED灯自检
    MSG_CMD_REQUEST_SHUTDOWM       = 0x24,   // 请求关机
    MSG_CMD_QEQUEST_RESET          = 0x26,   // 请求复位

    // 上行数据
    MSG_RSP_VERSION                = 0x11,    // 版本应答
    MSG_RSP_SWITCH_KEY             = 0x13,    // 模式与状态应答
    MSG_RSP_ALARM_LED_RESPONSE     = 0x15,    // 报警灯控制应答
    MSG_RSP_KEY_STATUS             = 0x19,    // 按键状态应答
    MSG_RSP_TRIGGER_BUZZER         = 0x1B,    // 蜂鸣控制应答
    MSG_RSP_CFG_BAT_ARGS           = 0x1D,    // 配置电池参数应答
    MSG_RSP_BATTERY_INFO           = 0x1F,    // 获取电池信息应答
    MSG_RSP_SET_ALARM_MUTE_LED     = 0x21,    // set alarm mute led response
    MSG_RSP_LED_SELFTEST           = 0x23,    // 设置LED灯自检应答
    MSG_NOTIFY_START               = 0x40,    // 启动
    MSG_NOTIFY_KEY                 = 0x41,    // 按键
    MSG_NOTIFY_MODE_CHANGE         = 0X43,    // 模式更改
    MSG_NOTIFY_AC_BAT_CHANGE       = 0x44,    // 电池状态更改
    MSG_PERIODIC_KEEP_ALIVE        = 0x5B,    // 保活
    MSG_PERIODIC_BAT_INFO          = 0x5C,    // 电池信息
    MSG_BACKLIGHT_BRIGHTNESS       = 0x5D,    // 背光亮度
    MSG_ERROR_INFO                 = 0x76,    // 错误帧

    MSG_UPGRADE_ALIVE              = 0xFE,    //升级保活帧
};

SystemBoardProvider *SystemBoardProvider::_selfObj = NULL;

/***************************************************************************************************
 * 按键信息解析
 **************************************************************************************************/
void SystemBoardProvider::_parseKeyEvent(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 3)
    {
        debug("Invalid key event packet!");
        return;
    }

    unsigned char keyData[2];
    keyData[0] = data[1];   // 按键字符
    keyData[1] = data[2];   // 按下弹起状态

    systemManager.parseKeyValue(keyData, sizeof(keyData));
}

/***************************************************************************************************
 * 交直流状态信息解析
 **************************************************************************************************/
void SystemBoardProvider::_parsePowerStat(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        debug("Invalid power status packet!");
        return;
    }

    unsigned char suplyType = data[1];
    if (suplyType >= POWER_SUPLY_NR)
    {
        suplyType = POWER_SUPLY_UNKOWN;
    }

    modeStatus.powerSuply = static_cast<PowerSuplyType>(suplyType);
}

/***************************************************************************************************
 * 模式改变解析
 **************************************************************************************************/
void SystemBoardProvider::_parseOperateMode(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        debug("Invalid operate mode packet!");
        return;
    }

    if (!powerOffWindow.isActiveWindow())
    {
        powerOffWindow.exec();
    }
}

/***************************************************************************************************
 * 开机状态信息解析
 **************************************************************************************************/
void SystemBoardProvider::_parsePowerOnStat(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 9)
    {
        debug("Invalid power on status packet!");
        return;
    }

    // 电源类型。
    unsigned char pwrSuply = data[3];
    if (pwrSuply >= POWER_SUPLY_NR)
    {
        pwrSuply = POWER_SUPLY_UNKOWN;
    }
    modeStatus.powerSuply = (PowerSuplyType)pwrSuply;

    // 电池状态信息。
    switch (data[4])
    {
    case 0:
        modeStatus.errorCode = ERR_CODE_NONE;
        break;

    case 1:
        modeStatus.errorCode = ERR_CODE_BAT_COMM_FAULT;
        break;

    case 2:
        modeStatus.errorCode = ERR_CODE_BAT_ERROR;
        break;

    default:
        break;
    }
}

/***************************************************************************************************
 * 函数说明：
 *      解析运行时不变的电池信息。
 **************************************************************************************************/
void SystemBoardProvider::_parseBatteryInfo(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 4)
    {
        qdebug("Invalid battery info packet!");
        return;
    }

    modeStatus.isCharging = data[1];        // 是否正在充电
    modeStatus.adcValue = (data[3] << 8) | data[2];     // ADC值
    if (modeStatus.adcValue < 500)
    {
        // 不合理的ADC值，过滤
        return;
    }

    if (modeStatus.powerSuply == POWER_SUPLY_AC_BAT)
    {
        powerManger.setBatteryCapacity(modeStatus.adcValue);
    }
    else if (modeStatus.powerSuply == POWER_SUPLY_BAT)
    {
        powerManger.setBatteryCapacity(modeStatus.adcValue);
    }
}

/***************************************************************************************************
 * 函数说明:
 *      解析错误警告码
 **************************************************************************************************/
void SystemBoardProvider::_parseErrorWaringCode(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 2)
    {
        debug("Invalid power on status packet!");
        return;
    }

    switch (data[1])
    {
    case 0:
        modeStatus.errorCode = ERR_CODE_NONE;
        break;

    case 1:
        modeStatus.errorCode = ERR_CODE_BAT_COMM_FAULT;
        break;

    case 2:
        modeStatus.errorCode = ERR_CODE_BAT_ERROR;
        break;

    default:
        break;
    }

    return;
}

/***************************************************************************************************
 * 数据包处理
 **************************************************************************************************/
void SystemBoardProvider::handlePacket(unsigned char *data, int len)
{
    if ((NULL == data) || (len < 1))
    {
        return;
    }

    BLMProvider::handlePacket(data, len);

    unsigned char type = data[0];

    switch (type)
    {
    case MSG_RSP_VERSION:
        break;

    case MSG_RSP_SWITCH_KEY:
        _parsePowerOnStat(data, len);
        break;

    case MSG_RSP_ALARM_LED_RESPONSE:
        break;

    case MSG_RSP_KEY_STATUS:
        break;

    case MSG_RSP_TRIGGER_BUZZER:
    case MSG_RSP_CFG_BAT_ARGS:
        break;

    case MSG_RSP_BATTERY_INFO:
        _parsePowerStat(data, len);
        break;

    case MSG_RSP_SET_ALARM_MUTE_LED:
        break;

    case MSG_RSP_LED_SELFTEST:
        break;

    case MSG_NOTIFY_START:
    {
        _notifyAck(&data[0], len);

        ErrorLogItem *item = new ErrorLogItem();
        item->setName("System Board Start");
        errorLog.append(item);
        break;
    }

    case MSG_NOTIFY_KEY:
        _notifyAck(&data[0], len);
        _parseKeyEvent(data, len);
        break;

    case MSG_NOTIFY_MODE_CHANGE:
        _parseOperateMode(data, len);
        break;

    case MSG_NOTIFY_AC_BAT_CHANGE:
        _notifyAck(&data[0], len);
        _parsePowerStat(data, len);
        break;

    case MSG_PERIODIC_BAT_INFO:
        _parseBatteryInfo(data, len);
        break;

    case MSG_PERIODIC_KEEP_ALIVE:
        feed();
        break;

    case MSG_ERROR_INFO:
        _notifyAck(&data[0], len);
        _parseErrorWaringCode(data, len);
        break;
    case MSG_BACKLIGHT_BRIGHTNESS:
        systemManager.setAutoBrightness(static_cast<BrightnessLevel>(data[1]));
        break;
    default:
        debug("Unknown message type = 0x%x!", type);
        break;
    }
}

/**************************************************************************************************
 * 通信中断。
 *************************************************************************************************/
void SystemBoardProvider::disconnected(void)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SYSTEM);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP, true);
    }
}

/**************************************************************************************************
 * 通信恢复。
 *************************************************************************************************/
void SystemBoardProvider::reconnected(void)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SYSTEM);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP, false);
    }
}

void SystemBoardProvider::sendDisconnected()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_SYSTEM);
    if (alarmSource && !alarmSource->isAlarmed(SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP))
    {
        alarmSource->setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_SEND_COMMUNICATION_STOP, true);
    }
}


/***************************************************************************************************
 * 获取电源类型。
 **************************************************************************************************/
PowerSuplyType SystemBoardProvider::getPowerSuplyType(void)
{
    return modeStatus.powerSuply;
}

short SystemBoardProvider::getPowerADC()
{
    return modeStatus.adcValue;
}

bool SystemBoardProvider::isPowerCharging()
{
    return modeStatus.isCharging;
}

/**************************************************************************************************
 * 获取版本号。
 *************************************************************************************************/
void SystemBoardProvider::sendVersion()
{
    sendCmd(MSG_CMD_QUERY_VERSION, NULL, 0);
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void SystemBoardProvider::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    sendCmd(cmdId, data, len);
}

/***************************************************************************************************
 * 灯光自检
 **************************************************************************************************/
void SystemBoardProvider::selfTest(void)
{
    // 灯光自检
    sendCmd(MSG_CMD_LED_SELFTEST, NULL, 0);

    // 查询所有按键状态
    sendCmd(MSG_CMD_QUERY_KEY_STATUS, NULL, 0);
}

/***************************************************************************************************
 * 更新报警灯
 * 参数:
 *    hasAlarmed:当前是否有报警
 *    priority:报警级别
 **************************************************************************************************/
void SystemBoardProvider::updateAlarm(bool hasAlarmed, AlarmPriority priority)
{
    unsigned char alarmLevel = 0;
    if (hasAlarmed)
    {
        switch (priority)
        {
        case ALARM_PRIO_PROMPT:
            alarmLevel = 4;
            break;
        case ALARM_PRIO_LOW:
            alarmLevel = 1;
            break;
        case ALARM_PRIO_MED:
            alarmLevel = 2;
            break;
        case ALARM_PRIO_HIGH:
            alarmLevel = 3;
            break;
        default:
            break;
        }
    }
    sendCmd(MSG_CMD_SET_ALARM_LED, &alarmLevel, 1);
}

/***************************************************************************************************
 * query the battery info
 **************************************************************************************************/
void SystemBoardProvider::queryBatteryInfo(void)
{
    sendCmd(MSG_CMD_BATTERY_INFO, NULL, 0);
}

/***************************************************************************************************
 * 触发蜂鸣器
 **************************************************************************************************/
void SystemBoardProvider::triggerBuzzer()
{
    sendCmd(MSG_CMD_TRIGGER_BUZZER, NULL, 0);
}

/***************************************************************************************************
 * 获取错误警告码
 **************************************************************************************************/
ErrorWaringCode SystemBoardProvider::getErrorWaringCode(void)
{
    return modeStatus.errorCode;
}

/***************************************************************************************************
 * 请求关机
 **************************************************************************************************/
void SystemBoardProvider::requestShutdown()
{
    sendCmd(MSG_CMD_REQUEST_SHUTDOWM, NULL, 0);
}

/***************************************************************************************************
 * 请求复位
 **************************************************************************************************/
void SystemBoardProvider::requestReset()
{
    sendCmd(MSG_CMD_QEQUEST_RESET, NULL, 0);
}

/***************************************************************************************************
 * 通知消息应答
 * 参数:
 *    type:应答类型
 **************************************************************************************************/
void SystemBoardProvider::_notifyAck(unsigned char *data, int len)
{
    sendCmd(MSG_ASK, data, len);
}

/***************************************************************************************************
 * ebable the backlight of the Keyboard
 * Param:
 *    enable: true: trun on the backlight of the keyboard; false: turn off.
 **************************************************************************************************/
void SystemBoardProvider::enableKeyboardBacklight(bool enable)
{
    unsigned char stat = enable ? 1 : 0;
    sendCmd(MSG_CMD_SET_KEYBOARD_BACKLIGHT, &stat, 1);
}

/***************************************************************************************************
 * 更新指示灯
 * 参数:
 *    enable: true打开，false关闭。
 **************************************************************************************************/
void SystemBoardProvider::enableIndicatorLight(bool enable)
{
    unsigned char stat = enable ? 1 : 0;
    sendCmd(MSG_CMD_SET_INDICATOR_LED, &stat, 1);
}

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
// 如果一个子类继承的父类中具有参数构造函数，那么子类在构造对象时有责任将相关参数传递到父类
SystemBoardProvider::SystemBoardProvider() : BLMProvider("SystemBoard"),
    PowerManagerProviderIFace(), LightProviderIFace()
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    // 初始化串口
    initPort(portAttr);
    setDisconnectThreshold(5);

    // 主动给系统板一个应答，让其尽快发送其他数据。
    // 相当于模拟底板上行一个启动帧到系统版
    unsigned char start = MSG_NOTIFY_START;
    // 系统版立即给底板一个回应帧
    _notifyAck(&start, 1);

    _recordBatInfo = false;
    // 从XML文件读取指定节点的配置值
    machineConfig.getNumValue("Record|Battery", _recordBatInfo);
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
SystemBoardProvider::~SystemBoardProvider(void)
{
}
