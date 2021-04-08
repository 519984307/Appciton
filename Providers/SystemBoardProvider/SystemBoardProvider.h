/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#pragma once
#include "BLMProvider.h"
#include "SystemDefine.h"
#include "PowerManagerProviderIFace.h"
#include "LightProviderIFace.h"

/***************************************************************************************************
 * 系统前面板数据提供者对象定义
 **************************************************************************************************/
class SystemBoardProvider : public BLMProvider,
    public PowerManagerProviderIFace, public LightProviderIFace
{
    Q_OBJECT

public:
    //构造本类对象函数
    static SystemBoardProvider &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemBoardProvider();
        }
        return *_selfObj;
    }
    static SystemBoardProvider *_selfObj;

    SystemBoardProvider();
    ~SystemBoardProvider();

public:
    // PowerParamProviderIFace.
    virtual PowerSuplyType getPowerSuplyType(void);

    virtual short getPowerADC(void);

    // 判断电池是否正常充电
    virtual bool isPowerCharging();

    //获取版本号
    virtual void sendVersion(void);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

public:
    // LightProviderIFace
    // 自检，发生在开机时刻，依次亮静音灯、低、中、高级别报警灯，最后熄灭。
    virtual void selfTest(void);

    // 设置当前的报警级别。
    // 当hasAlarmed为false时，表明当前无报警后面的priority不起作用。
    // 当hasAlarmed为true时，priority表示报警的级别。
    virtual void updateAlarm(bool hasAlarmed, AlarmPriority priority);

    // ebable the backlight of the Keyboard
    virtual void enableKeyboardBacklight(bool enable);

    // 指示灯控制。
    virtual void enableIndicatorLight(bool enable);

public:
    // query the battery info
    void queryBatteryInfo(void);

    // 触发蜂鸣器。
    void triggerBuzzer(void);

    // 获取错误警告码
    ErrorWaringCode getErrorWaringCode(void);

    // 请求关机
    void requestShutdown(void);

    // 请求复位
    void requestReset(void);

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

private:
    void _parseKeyEvent(unsigned char *data, int len);
    void _parsePowerStat(unsigned char *data, int len);
    void _parseOperateMode(unsigned char *data, int len);
    void _parsePowerOnStat(unsigned char *data, int len);
    void _parseBatteryInfo(unsigned char *data, int len);
    void _parseErrorWaringCode(unsigned char *data, int len);

    // 通知消息应答。
    void _notifyAck(unsigned char *data, int len);

private:
    bool _gotInitSwitchKeyStatus;

    struct ModeStatus
    {
        ModeStatus()
        {
            powerSuply = POWER_SUPLY_UNKOWN;
            errorCode = ERR_CODE_NONE;
            adcValue = 0;
            isCharging = false;
        }

        PowerSuplyType powerSuply;
        ErrorWaringCode errorCode;
        short adcValue;
        bool isCharging;
    } modeStatus;

    bool _recordBatInfo;
};
#define systemBoardProvider (SystemBoardProvider::construction())
#define deleteSystemBoardProvider() (delete SystemBoardProvider::_selfObj)
