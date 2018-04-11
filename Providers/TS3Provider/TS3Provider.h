#pragma once
#include "BLMProvider.h"
#include "Provider.h"
#include "SPO2ProviderIFace.h"

// 收到的数据帧类型。
enum SPO2PacketType
{
    TS3_RESPONSE_NACK                            = 0x00,       //NACK应答
    TS3_RESPONSE_ACK                             = 0x01,       //ACK应答

    TS3_CMD_GET_VERSION                          = 0x10,       // 获取版本信息。
    TS3_RSP_GET_VERSION                          = 0x11,       //

    TS3_CMD_SENSITIVITY                          = 0x12,       // 灵敏度设置
    TS3_RSP_SENSITIVITY                          = 0x13,       //
    TS3_CMD_STATUS                               = 0x14,       // 状态查询

    TS3_NOTIFY_START_UP                          = 0x40,       // 启动帧。
    TS3_NOTIFY_STATUS                            = 0x42,       // 状态帧。
    TS3_NOTIFY_ALIVE                             = 0x5B,       // 保活帧。
    TS3_NOTIFY_WAVE                              = 0x5C,       // 描记波帧。
    TS3_NOTIFY_DATA                              = 0x5D,       // 原始数据。
    TS3_NOTIFY_RESULT                            = 0x5F,       // 测量结果帧。
    TS3_DATA_ERROR                               = 0x76,       // 错误警告帧

    TS3_UPGRADE_ALIVE                            = 0xFE,       //升级保活帧
};

//算法状态
enum TS3LogicStatus
{
    TS3_LOGIC_INIT,                 // 初始化状态。
    TS3_LOGIC_SEARCHING,            // 脉搏波搜索状态。
    TS3_LOGIC_SEARCH_TOO_LONG,      // 脉搏波搜索时间过长。
    TS3_LOGIC_NORMAL,               // 算法正常计算状态
    TS3_LOGIC_NC,                   //
};

//调光调增益错误
enum TS3GainError
{
    TS3_GAIN_NORMAL,                 // 无错误。
    TS3_GAIN_SATURATION,             // 信号饱和。
    TS3_GAIN_WEAK,                   // 信号太弱。
    TS3_GAIN_NC,                     //
};

class TS3Provider: public BLMProvider, public SPO2ProviderIFace
{

public: // Provider的接口。
    virtual bool attachParam(Param &param);

public: // SPO2ProviderIFace 的接口。
    //获取版本号
    virtual void sendVersion(void);

    virtual int getSPO2WaveformSample(void) {return 250;}

    virtual int getSPO2BaseLine(void) {return 0x80;}

    virtual int getSPO2MaxValue(void) {return 256;}

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

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    TS3Provider();
    ~TS3Provider();

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    void _sendACK(unsigned char type);
    bool _isValuePR;

    bool _isCableOff;
    bool _isFingerOff;
    TS3GainError _gainError;
    bool _ledFault;
    TS3LogicStatus _logicStatus;

    static const int _packetLen = 9;      // 数据包长度。
    struct timeval _lastTime;
};
