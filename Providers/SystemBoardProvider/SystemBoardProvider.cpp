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
#include "crc8.h"
#include "Debug.h"
#include "SystemManager.h"
#include "SystemBoardProvider.h"
#include "WindowManager.h"
#include "PowerManager.h"
#include "BLMEDUpgradeParam.h"
#include "ServiceVersion.h"
#include "SystemAlarm.h"
#include "KeyActionManager.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "IConfig.h"
#include "RawDataCollectionTxt.h"

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
    MSG_CMD_CFG_BAT_ARGS           = 0x1C,   // 配置电池参数
    MSG_CMD_FIXED_BAT_INFO         = 0x1E,   // 获取电池的固定信息
    MSG_CMD_SET_ALARM_MUTE_LED     = 0x20,   // set alarm mute LED
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
    MSG_RSP_FIXED_BAT_INFO         = 0x1F,    // 获取电池的固定信息应答
    MSG_RSP_SET_ALARM_MUTE_LED     = 0x21,    // set alarm mute led response
    MSG_RSP_LED_SELFTEST           = 0x23,    // 设置LED灯自检应答
    MSG_NOTIFY_START               = 0x40,    // 启动
    MSG_NOTIFY_KEY                 = 0x41,    // 按键
    MSG_NOTIFY_MODE_CHANGE         = 0X43,    // 模式更改
    MSG_NOTIFY_AC_BAT_CHANGE       = 0x44,    // 电池状态更改
    MSG_NOTIFY_MODE_FAST_CHANGE    = 0x45,    // 模式快速改变
    MSG_NOTIFY_XHOST_INT_FINISH    = 0x46,    // XHOST_INT 100ms
    MSG_PERIODIC_KEEP_ALIVE        = 0x5B,    // 保活
    MSG_PERIODIC_BAT_INFO          = 0x5C,    // 电池信息
    MSG_PERIODIC_BAT_VALUE         = 0x5D,    // 电池ADC电压
    MSG_ERROR_INFO                 = 0x76,    // 错误帧

    MSG_UPGRADE_ALIVE              = 0xFE,    //升级保活帧
};

SystemBoardProvider *SystemBoardProvider::_selfObj = NULL;

/***************************************************************************************************
 * 版本信息解析
 **************************************************************************************************/
void SystemBoardProvider::_parseVersionInfo(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 10)
    {
        debug("Invalid version info packet!");
        return;
    }

    modeStatus.hwVersion = QString("%1.%2.%3")
                           .arg(QString::number(data[1]))
                           .arg(QString::number(data[2]))
                           .arg(QString::number(data[3]));
    modeStatus.swVersion = QString("%1.%2.%3")
                           .arg(QString::number(data[4]))
                           .arg(QString::number(data[5]))
                           .arg(QString::number(data[6]));
    modeStatus.protoVersion = QString("%1.%2.%3")
                              .arg(QString::number(data[7]))
                              .arg(QString::number(data[8]))
                              .arg(QString::number(data[9]));
}

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

    modeStatus.powerSuply = (PowerSuplyType)suplyType;
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

    _notifyAck(&data[0], len);
}

/***************************************************************************************************
 * 模式改变解析
 **************************************************************************************************/
void SystemBoardProvider::_parseFastOperateMode(unsigned char *data, int len)
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

    _gotInitSwitchKeyStatus = true;
}

/***************************************************************************************************
 * 函数说明：
 *      解析运行时不变的电池信息。
 **************************************************************************************************/
void SystemBoardProvider::_parseFixedBatteryInfo(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 151)
    {
        debug("Invalid power on status packet!");
        return;
    }

    powerManager.batteryMessage.updateFixedBatMessage(&data[1]);
    return;
}

/***************************************************************************************************
 * 函数说明:
 *      解析周期性电池信息
 **************************************************************************************************/
void SystemBoardProvider::_parsePeriodicBatteryInfo(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 51)
    {
        debug("Invalid power on status packet!");
        return;
    }

    if (_recordBatInfo)
    {
        unsigned char buf[53];
        ::memcpy(buf, data, 51);
        buf[51] = _adcValue[0];
        buf[52] = _adcValue[1];
        rawDataCollectionTxt.PushData("Battery", buf, sizeof(buf));
    }

    powerManager.batteryMessage.updatePeriodicBatMessage(&data[1]);

    return;
}

void SystemBoardProvider::_parsePeriodicBatteryValue(unsigned char *data, int len)
{
    if (NULL == data)
    {
        return;
    }

    if (len != 3)
    {
        debug("Invalid power on status packet!");
        return;
    }

    _adcValue[0] = data[1];
    _adcValue[1] = data[2];
    powerManager.batteryMessage.updatePeriodicBatValue(&data[1]);

    return;
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
        _parseVersionInfo(data, len);
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

    case MSG_RSP_FIXED_BAT_INFO:
        _parseFixedBatteryInfo(data, len);
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

    case MSG_NOTIFY_MODE_FAST_CHANGE:
        _notifyAck(&data[0], len);
        _parseFastOperateMode(data, len);
        break;

    case MSG_NOTIFY_XHOST_INT_FINISH:
        _notifyAck(&data[0], len);
        break;

    case MSG_PERIODIC_BAT_INFO:
        _parsePeriodicBatteryInfo(data, len);
        break;

    case MSG_PERIODIC_BAT_VALUE:
        _parsePeriodicBatteryValue(data, len);
        break;

    case MSG_PERIODIC_KEEP_ALIVE:
        feed();
        break;

    case MSG_ERROR_INFO:
        _notifyAck(&data[0], len);
        _parseErrorWaringCode(data, len);
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
    systemAlarm.setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP, true);
}

/**************************************************************************************************
 * 通信恢复。
 *************************************************************************************************/
void SystemBoardProvider::reconnected(void)
{
    systemAlarm.setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP, false);
}


/***************************************************************************************************
 * 获取电源类型。
 **************************************************************************************************/
PowerSuplyType SystemBoardProvider::getPowerSuplyType(void)
{
    return modeStatus.powerSuply;
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
    unsigned char alarmLevel = (hasAlarmed) ? (priority + 1) : 0;
    sendCmd(MSG_CMD_SET_ALARM_LED, &alarmLevel, 1);
}

/***************************************************************************************************
 * 查询初始模式与状态。
 **************************************************************************************************/
void SystemBoardProvider::querySwitchKeyStatus(void)
{
    sendCmd(MSG_CMD_QUERY_SWITCH_KEY, NULL, 0);
    _gotInitSwitchKeyStatus = false;
}

/***************************************************************************************************
 * 查询固定不变的电池信息
 **************************************************************************************************/
void SystemBoardProvider::queryFixedBatMessage(void)
{
    sendCmd(MSG_CMD_FIXED_BAT_INFO, NULL, 0);
}

/***************************************************************************************************
 * 触发蜂鸣器
 **************************************************************************************************/
void SystemBoardProvider::triggerBuzzer()
{
    sendCmd(MSG_CMD_TRIGGER_BUZZER, NULL, 0);
}

/***************************************************************************************************
 * 版本信息
 **************************************************************************************************/
void SystemBoardProvider::getVersionInfo(QString &hwVersion,
        QString &swVersion, QString &protoVersion)
{
    hwVersion = modeStatus.hwVersion;
    swVersion = modeStatus.swVersion;
    protoVersion = modeStatus.protoVersion;
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
 * 更新静音报警灯
 * 参数:
 *    enable:当前是否为静音状态
 **************************************************************************************************/
void SystemBoardProvider::enableAlarmAudioMute(bool enable)
{
    unsigned char stat = enable ? 1 : 0;
    sendCmd(MSG_CMD_SET_ALARM_MUTE_LED, &stat, 1);
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
    _gotInitSwitchKeyStatus = false;

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
    // 清0 _adcValue
    ::memset(_adcValue, 0, sizeof(_adcValue));
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
SystemBoardProvider::~SystemBoardProvider(void)
{
}
