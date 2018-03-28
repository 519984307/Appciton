#pragma once
#include "BLMProvider.h"
#include "Provider.h"
#include "NIBPProviderIFace.h"
#include <QFile>
#include <QTextStream>

#define DATA_TXT ("/usr/local/nPM/data/")

// 收到的数据帧类型。
enum NIBPPacketType
{
    TN3_RESPONSE_NACK                            = 0x00,       //NACK应答
    TN3_RESPONSE_ACK                             = 0x01,       //ACK应答

    TN3_CMD_GET_VERSION                          = 0x10,       // 获取版本信息。
    TN3_RSP_GET_VERSION                          = 0x11,       //

    TN3_CMD_PATIENT_TYPE                         = 0x12,       // 病人类型。
    TN3_RSP_PATIENT_TYPE                         = 0x13,       //

    TN3_CMD_START_MEASURE                        = 0x14,       // 启动测量。
    TN3_RSP_START_MEASURE                        = 0x15,       //

    TN3_CMD_STOP_MEASURE                         = 0x16,       // 停止测量。
    TN3_RSP_STOP_MEASURE                         = 0x17,       //

    TN3_CMD_GET_MEASUREMENT                      = 0x18,       // 获取测量结果。
    TN3_RSP_GET_MEASUREMENT                      = 0x19,       //

    TN3_CMD_SMART_PRESSURE                       = 0x1A,       // 设置智能充气开关。
    TN3_RSP_SMART_PRESSURE                       = 0x1B,       //

    TN3_CMD_INIT_PRESSURE                        = 0x1C,       // 设置预充气值。
    TN3_RSP_INIT_PRESSURE                        = 0x1D,       //

    TN3_CMD_SELFTEST                             = 0x1E,       // 开机自检。
    TN3_RSP_SELFTEST                             = 0x1F,       //

    TN3_CMD_LOW_PRESSURE                         = 0x20,       // 获取当前压力值。
    TN3_RSP_LOW_PRESSURE                         = 0x21,       //

    TN3_NOTIFY_LOW_PRESSURE                      = 0x22,       // <15mmHg压力值周期性数据帧。

    TN3_NOTIFY_PASSTHROUGH_MODE                  = 0x25,       // 透传模式。

    TN3_NOTIFY_START_UP                          = 0x40,       // 启动帧。
    TN3_NOTIFY_END                               = 0x41,       // 测量结束帧。
    TN3_NOTIFY_ALIVE                             = 0x5B,       // 保活帧。
    TN3_NOTIFY_PRESSURE                          = 0x5C,       // 压力帧。
    TN3_NOTIFY_DATA                              = 0x5D,       // 原始数据。

    TN3_DATA_ERROR                               = 0x76,       // 错误警告帧

    TN3_CMD_ENTER_SERVICE                        = 0x80,       //进入维护模式
    TN3_RSP_ENTER_SERVICE                        = 0x81,       //
    TN3_CMD_CALIBRATE                            = 0x82,       //校准模式控制
    TN3_RSP_CALIBRATE                            = 0x83,       //
    TN3_CMD_PRESSURE_POINT                       = 0x84,       //校准点压力值输入
    TN3_RSP_PRESSURE_POINT                       = 0x85,       //
    TN3_CMD_MANOMETER                            = 0x86,       //压力计模式控制
    TN3_RSP_MANOMETER                            = 0x87,       //
    TN3_CMD_PRESSURE_CONTROL                     = 0x88,       //压力操控模式控制
    TN3_RSP_PRESSURE_CONTROL                     = 0x89,       //
    TN3_CMD_PRESSURE_INFLATE                     = 0x8A,       //压力控制（充气）
    TN3_RSP_PRESSURE_INFLATE                     = 0x8B,       //
    TN3_CMD_PRESSURE_DEFLATE                     = 0x8C,       //放气控制
    TN3_RSP_PRESSURE_DEFLATE                     = 0x8D,       //
    TN3_CMD_Valve                                = 0x90,       //气阀控制
    TN3_RSP_Valve                                = 0x91,       //
    TN3_CMD_CALIBRATE_Zero                       = 0x92,       //进入校零模式
    TN3_RSP_CALIBRATE_Zero                       = 0x93,       //
    TN3_CMD_PRESSURE_Zero                        = 0x94,       //校准零点值
    TN3_RSP_PRESSURE_Zero                        = 0x95,       //
    TN3_CMD_PRESSURE_Protect                     = 0x96,       //过压保护
    TN3_RSP_PRESSURE_Protect                     = 0x97,       //

    TN3_STATE_CHANGE                             = 0xA0,       //状态改变
    TN3_SERVICE_PRESSURE                         = 0xDB,       // 压力帧。

    TN3_UPGRADE_ALIVE                            = 0xFE,       //升级保活帧
};

class TN3Provider: public BLMProvider, public NIBPProviderIFace
{
public: // Provider的接口。
    virtual bool attachParam(Param &param);

#ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
#endif
public: // NIBPProviderIFace的接口。
    //获取版本号
    virtual void sendVersion(void);

    // 起停测量。
    virtual void startMeasure(PatientType type);
    virtual void stopMeasure(void);

    // <15mmHg压力值周期性数据帧
    virtual short lowPressure(unsigned char *packet);

    // 透传模式。
    virtual void setPassthroughMode(bool flag);

    // 获取自检状态
    virtual void sendSelfTest();

    // 设置预充气压力值。
    virtual void setInitPressure(short pressure);

    // 设置智能压力使能。
    virtual void enableSmartPressure(bool enable);

    // 设置病人类型。
    virtual void setPatientType(unsigned char type);

    // 发送启动指令是否有该指令的应答。
    virtual bool needStartACK(void);
    virtual bool isStartACK(unsigned char *packet);

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void);
    virtual bool isStopACK(unsigned char *packet);

    // 压力数据，不是压力数据返回-1。
    virtual short cuffPressure(unsigned char *packet);

    // 测量是否结束。
    virtual bool isMeasureDone(unsigned char *packet);

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char *packet);

    // 发送获取结果请求。
    virtual void getResult(void);

    // 是否为结果包。
    virtual bool isResult(unsigned char *packet,short &sys,
                          short &dia, short &map, short &pr, NIBPOneShotType &err);

    // 进入维护模式。
    virtual void service_Enter(bool enter);
    virtual bool isService_Enter(unsigned char *packet);

    // 进入校准模式。
    virtual void service_Calibrate(bool enter);
    virtual bool isService_Calibrate(unsigned char *packet);

    //校准点压力值输入
    virtual void service_Pressurepoint(int num, short pressure);
    virtual bool isService_Pressurepoint(unsigned char *packet);

    // 进入压力计模式控制。
    virtual void service_Manometer(bool enter);
    virtual bool isService_Manometer(unsigned char *packet);

    // 进入压力操控模式。
    virtual void service_Pressurecontrol(bool enter);
    virtual bool isService_Pressurecontrol(unsigned char *packet);

    //压力控制（充气）
    virtual void service_Pressureinflate(short pressure);
    virtual bool isService_Pressureinflate(unsigned char *packet);

    //放气控制
    virtual void service_Pressuredeflate(void);
    virtual bool isService_Pressuredeflate(unsigned char *packet);

    // 压力数据，不是压力数据返回-1。
    virtual int service_cuffPressure(unsigned char *packet);

    //进入校零模式
    virtual void service_CalibrateZero(bool enter);
    virtual bool isService_CalibrateZero(unsigned char *packet);

    //校零
    virtual void service_PressureZero(void);
    virtual bool isService_PressureZero(unsigned char *packet);

    //气阀控制
    virtual void service_Valve(bool enter);
    virtual bool isService_Valve(unsigned char *packet);

    //过压保护
    virtual void service_PressureProtect(bool enter);
    virtual bool isService_PressureProtect(unsigned char *packet);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);



    TN3Provider();
    ~TN3Provider();

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    void _sendACK(unsigned char type);

    // 接收自检状态
    void _selfTest(unsigned char *packet, int len);

    // 错误代码
    void _errorWarm(unsigned char *packet, int len);
};
