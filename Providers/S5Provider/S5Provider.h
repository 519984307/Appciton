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
#include "SPO2ProviderIFace.h"
#include <QTime>

// 收到的数据帧类型。
enum S5PacketType
{
    S5_RESPONSE_NACK                            = 0x00,       // NACK应答
    S5_RESPONSE_ACK                             = 0x01,       // ACK应答

    S5_CMD_GET_VERSION                          = 0x10,       // 获取版本信息。
    S5_RSP_GET_VERSION                          = 0x11,       //

    S5_CMD_SENSITIVITY                          = 0x12,       // 灵敏度设置
    S5_RSP_SENSITIVITY                          = 0x13,       //
    S5_CMD_STATUS                               = 0x14,       // 状态查询
    S5_CMD_RAW_DATA                             = 0x16,       // 原始数据发送
    S5_RSP_RAW_DATA                             = 0x17,

    S5_NOTIFY_START_UP                          = 0x40,       // 启动帧。
    S5_NOTIFY_STATUS                            = 0x42,       // 状态帧。
    S5_NOTIFY_ALIVE                             = 0x5B,       // 保活帧。
    S5_NOTIFY_WAVE                              = 0x5C,       // 描记波帧。
    S5_NOTIFY_DATA                              = 0x5D,       // 原始数据。
    S5_NOTIFY_RESULT                            = 0x5F,       // 测量结果帧。
    S5_DATA_ERROR                               = 0x76,       // 错误警告帧

    S5_UPGRADE_ALIVE                            = 0xFE,       //升级保活帧
};

//算法状态
enum S5LogicStatus
{
    S5_LOGIC_INIT,                 // 初始化状态。
    S5_LOGIC_SEARCHING,            // 脉搏波搜索状态。
    S5_LOGIC_SEARCH_TOO_LONG,      // 脉搏波搜索时间过长。
    S5_LOGIC_NORMAL,               // 算法正常计算状态
    S5_LOGIC_NC,                   //
};

//调光调增益错误
enum S5GainError
{
    S5_GAIN_NORMAL,                 // 无错误。
    S5_GAIN_SATURATION,             // 信号饱和。
    S5_GAIN_WEAK,                   // 信号太弱。
    S5_GAIN_NC,                     //
};

class S5Provider: public BLMProvider, public SPO2ProviderIFace
{

public: // Provider的接口。
    virtual bool attachParam(Param &param);

public: // SPO2ProviderIFace 的接口。
    //获取版本号
    virtual void sendVersion(void);

    virtual int getSPO2WaveformSample(void)
    {
        return 250;
    }

    virtual int getSPO2BaseLine(void)
    {
        return 0x80;
    }

    virtual int getSPO2MaxValue(void)
    {
        return 255;
    }

    // SPO2值与PR值。
    virtual bool isResultSPO2PR(unsigned char *packet);

    //描记波/棒图
    virtual bool isResult_BAR(unsigned char *packet);

    //状态
    virtual bool isStatus(unsigned char *packet);

    // 设置灵敏度。
    virtual void setSensitive(SPO2Sensitive sens);

    // 查询状态
    virtual void sendStatus(void);

    /**
     * @brief setRawDataSend 设置原始数据发送开关
     * @param onOff
     */
    virtual void enableRawDataSend(bool onOff);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    S5Provider();
    ~S5Provider();

    /* reimplment */
    void initModule();

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

private:
    void _sendACK(unsigned char type);
    bool _isValuePR;

    bool _isFirstConnectCable;
    bool _isCableOff;
    bool _isFingerOff;
    bool _isLedError;
    bool _isSeaching;
    S5GainError _gainError;
    bool _ledFault;
    S5LogicStatus _logicStatus;

    static const int _packetLen = 9;      // 数据包长度。
    struct timeval _lastTime;

    bool _isInvalidWaveData;

    QTime startUpTime;
    bool _firstStartUp;
    int _startUpError;
    bool _errorStatus;
};
