/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/17
 **/

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
    N5_RESPONSE_NACK                            = 0x00,       // NACK应答
    N5_RESPONSE_ACK                             = 0x01,       // ACK应答

    N5_CMD_GET_VERSION                          = 0x10,       // 获取版本信息。
    N5_RSP_GET_VERSION                          = 0x11,       //

    N5_CMD_PATIENT_TYPE                         = 0x12,       // 病人类型。
    N5_RSP_PATIENT_TYPE                         = 0x13,       //

    N5_CMD_START_MEASURE                        = 0x14,       // 启动测量。
    N5_RSP_START_MEASURE                        = 0x15,       //

    N5_CMD_STOP_MEASURE                         = 0x16,       // 停止测量。
    N5_RSP_STOP_MEASURE                         = 0x17,       //

    N5_CMD_GET_MEASUREMENT                      = 0x18,       // 获取测量结果。
    N5_RSP_GET_MEASUREMENT                      = 0x19,       //

    N5_CMD_SMART_PRESSURE                       = 0x1A,       // 设置智能充气开关。
    N5_RSP_SMART_PRESSURE                       = 0x1B,       //

    N5_CMD_INIT_PRESSURE                        = 0x1C,       // 设置预充气值。
    N5_RSP_INIT_PRESSURE                        = 0x1D,       //

    N5_CMD_SELFTEST                             = 0x1E,       // 开机自检。
    N5_RSP_SELFTEST                             = 0x1F,       //

    N5_CMD_LOW_PRESSURE                         = 0x20,       // 获取当前压力值。
    N5_RSP_LOW_PRESSURE                         = 0x21,       //

    N5_NOTIFY_LOW_PRESSURE                      = 0x22,       // <15mmHg压力值周期性数据帧。

    N5_NOTIFY_PASSTHROUGH_MODE                  = 0x25,       // 透传模式。
    N5_RSP_PASSTHROUGH_MODE                     = 0x26,

    N5_NOTIFY_START_UP                          = 0x40,       // 启动帧。
    N5_NOTIFY_END                               = 0x41,       // 测量结束帧。
    N5_NOTIFY_ALIVE                             = 0x5B,       // 保活帧。
    N5_NOTIFY_PRESSURE                          = 0x5C,       // 压力帧。
    N5_NOTIFY_DATA                              = 0x5D,       // 原始数据。

    N5_DATA_ERROR                               = 0x76,       // 错误警告帧

    N5_CMD_ENTER_SERVICE                        = 0x80,       // 进入维护模式
    N5_RSP_ENTER_SERVICE                        = 0x81,       //
    N5_CMD_CALIBRATE                            = 0x82,       // 校准模式控制
    N5_RSP_CALIBRATE                            = 0x83,       //
    N5_CMD_PRESSURE_POINT                       = 0x84,       // 校准点压力值输入
    N5_RSP_PRESSURE_POINT                       = 0x85,       //
    N5_CMD_MANOMETER                            = 0x86,       // 压力计模式控制
    N5_RSP_MANOMETER                            = 0x87,       //
    N5_CMD_PRESSURE_CONTROL                     = 0x88,       // 压力操控模式控制
    N5_RSP_PRESSURE_CONTROL                     = 0x89,       //
    N5_CMD_PRESSURE_INFLATE                     = 0x8A,       // 压力控制（充气）
    N5_RSP_PRESSURE_INFLATE                     = 0x8B,       //
    N5_CMD_PRESSURE_DEFLATE                     = 0x8C,       // 放气控制
    N5_RSP_PRESSURE_DEFLATE                     = 0x8D,       //
    N5_CMD_PUMP                                 = 0x8E,       // 气泵控制
    N5_RSP_PUMP                                 = 0x8F,       //
    N5_CMD_VALVE                                = 0x90,       // 气阀控制
    N5_RSP_VALVE                                = 0x91,       //
    N5_CMD_CALIBRATE_ZERO                       = 0x92,       // 进入校零模式
    N5_RSP_CALIBRATE_ZERO                       = 0x93,       //
    N5_CMD_PRESSURE_ZERO                        = 0x94,       // 校准零点值
    N5_RSP_PRESSURE_ZERO                        = 0x95,       //
    N5_CMD_PRESSURE_PROTECT                     = 0x96,       // 过压保护
    N5_RSP_PRESSURE_PROTECT                     = 0x97,       //

    N5_STATE_CHANGE                             = 0xA0,       // 状态改变
    N5_STATE_ZERO_SELFTEST                      = 0xA1,       // 自校零状态
    N5_STATE_PRESSURE_PROTECT                   = 0xA2,       // 过压保护状态
    N5_SERVICE_PRESSURE                         = 0xDB,       // 压力帧。

    N5_UPGRADE_ALIVE                            = 0xFE,       //升级保活帧
};

class N5Provider: public BLMProvider, public NIBPProviderIFace
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

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void);

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char *packet);

    // 发送获取结果请求。
    virtual void getResult(void);

    // 进入维护模式。
    virtual void serviceEnter(bool enter);

    // 进入校准模式。
    virtual void serviceCalibrate(bool enter);

    //校准点压力值输入
    virtual void servicePressurepoint(const unsigned char *data, unsigned int len);

    // 进入压力计模式控制。
    virtual void serviceManometer(bool enter);

    // 进入压力操控模式。
    virtual void servicePressurecontrol(bool enter);

    //压力控制（充气）
    virtual void servicePressureinflate(short pressure);
    virtual bool isServicePressureinflate(unsigned char *packet);

    //放气控制
    virtual void servicePressuredeflate(void);

    // 压力数据，不是压力数据返回-1。
    virtual int serviceCuffPressure(unsigned char *packet);

    //进入校零模式
    virtual void serviceCalibrateZero(bool enter);
    virtual bool isServiceCalibrateZero(unsigned char *packet);

    //校零
    virtual void servicePressureZero(void);
    virtual bool isServicePressureZero(unsigned char *packet);

    // 气泵控制
    virtual void servicePump(bool enter, unsigned char pump);

    //气阀控制
    virtual void serviceValve(bool enter);
    virtual bool isServiceValve(unsigned char *packet);

    //过压保护
    virtual void servicePressureProtect(bool enter);
    virtual bool isServicePressureProtect(unsigned char *packet);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 转换错误码
    virtual unsigned char convertErrcode(unsigned char code);

    N5Provider();
    ~N5Provider();

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

private:
    void _sendACK(unsigned char type);

    // 接收自检状态
    void _selfTest(unsigned char *packet, int len);

    // 错误代码
    void _errorWarm(unsigned char *packet, int len);
};
