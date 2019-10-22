/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#pragma once
#include "BLMProvider.h"
#include "Provider.h"
#include "TEMPProviderIFace.h"

// 收到的数据帧类型。
enum T5PacketType
{
    T5_RESPONSE_NACK                            = 0x00,       // NACK应答
    T5_RESPONSE_ACK                             = 0x01,       // ACK应答

    T5_CMD_GET_VERSION                          = 0x10,       // 获取版本信息。
    T5_RSP_GET_VERSION                          = 0x11,       //

    T5_CMD_SET_PROBE                            = 0x12,       // 设置探头类型。
    T5_RSP_SET_PROBE                            = 0x13,       //
    T5_CMD_SELF_STATE                           = 0x14,       // 获取自检状态。
    T5_RSP_SELF_STATE                           = 0x15,       //
    T5_CMD_PROBE_STATE                          = 0x16,       // 获取测量状态。
    T5_RSP_PROBE_STATE                          = 0x17,       //
    T5_CMD_CHANNEL                              = 0x18,       // 体温定标。
    T5_RSP_CHANNEL                              = 0x19,       //

    T5_CMD_CALIBRATE_STATE                      = 0x1A,       // 校准模式
    T5_RSP_CALIBRATE_STATE                      = 0x1B,

    T5_NOTIFY_START_UP                          = 0x40,       // 启动帧。
    T5_NOTIFY_DATA                              = 0x41,       // 测量数据
    T5_NOTIFY_OVERRANGE                         = 0x42,       // 测量超界帧
    T5_NOTIFY_PROBE_OFF                         = 0x43,       // 探头脱落帧

    T5_CYCLE_ALIVE                              = 0x5B,       // 保活帧
    T5_CYCLE_DATA                               = 0x5C,       // 原始数据
    T5_OHM_DATA                                 = 0x5D,       // 体温电阻值

    T5_DATA_ERROR                               = 0x76,       // 错误警告帧

    T5_UPGRADE_ALIVE                            = 0xFE,       //升级保活帧
};

class T5Provider: public BLMProvider, public TEMPProviderIFace
{
public: // Provider的接口。
    virtual bool attachParam(Param &param);

public: // TEMPProviderIFace 的接口。
    T5Provider();
    ~T5Provider();

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    //获取版本号
    virtual void sendVersion(void);

    //发送自检指令
    virtual void sendTEMPSelfTest(void);

    //获取测量状态
    virtual void sendProbeState(void);

    // 定标数据。
    virtual void sendCalibrateData(int channel, int value);

    virtual void enterCalibrateState(void);
    virtual void exitCalibrateState(void);

    //通信中断
    virtual bool isDisconnected(void)
    {
        return _disconnected;
    }

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

private:
    //应答
    void _sendACK(unsigned char type);

    //接收自检状态
    void _selfTest(unsigned char *packet, int len);

    // 错误代码
    void _errorWarm(unsigned char *packet, int len);

    //接收测量状态
    void _probeState(unsigned char *packet);

    //体温值
    void _result(unsigned char *packet);

    //电阻值
    void ohmResult(unsigned char *packet);

    //探头脱离
    void _sensorOff(unsigned char *packet);

    //报警显示
    void _shotAlarm();

    void _limitHandle(unsigned char *packer);

    //温度边界值改变
    int borderValueChange(int lowBorderValue, int high, int temp);

    bool _disconnected;
    bool _overRang1;
    bool _overRang2;
    bool _sensorOff1;
    bool _sensorOff2;
    bool _hasBeenConnected1;
    bool _hasBeenConnected2;

    short _temp1;
    short _temp2;
    short _tempd;

    int ohm1;
    int ohm2;
};
