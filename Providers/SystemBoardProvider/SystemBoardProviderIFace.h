#pragma once
#include <QString>
#include "SystemDefine.h"

/***************************************************************************************************
 * 定义系统前面板通讯对象需要实现的接口方法
 **************************************************************************************************/
class SystemBoardProviderIFace
{
public:
    // 版本信息。
    virtual void getVersionInfo(QString &hwVer, QString &swVer, QString &protoVer) = 0;

    // 交直流模式。
    virtual void getPowerStat(PowerStat &powerStat) = 0;

    // 操作模式。
    virtual void getOperateMode(OperateMode &operateMode) = 0;

    // 版本查询。
    virtual void queryVersion(void) = 0;

    // 模式状态查询。
    virtual void queryOperateMode(void) = 0;

    // 触发蜂鸣器。
    virtual void triggerBuzzer(void) = 0;

    // 发送保活(keep alive)信息。
    virtual void keepAlive(void) = 0;

    // 系统时钟设置。
    virtual void setSysTime(int time) = 0;

    // 报警灯控制。
    virtual void setAlarmLed(unsigned char alarmLevel) = 0;

    // 指示灯控制。
    virtual void setIndicatorLed(bool isOn) = 0;

    // 自检设置。
    virtual void setSelfTest(int clock, int interval) = 0;

    SystemBoardProviderIFace() {}
    virtual ~SystemBoardProviderIFace() {}
};
