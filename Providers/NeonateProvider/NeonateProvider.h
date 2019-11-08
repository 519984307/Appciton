/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/12
 **/

#pragma once
#include "O2ProviderIFace.h"
#include "BLMProvider.h"

enum NeonatePacketType
{
    NEONATE_RESPONSE_NACK           = 0x00,       // NACK应答
    NEONATE_RESPONSE_ACK            = 0x01,       // ACK应答

    NEONATE_CMD_VERSION             = 0x10,     // 获取版本信息
    NEONATE_RSP_VERSION             = 0x11,
    NEONATE_CMD_SELF_STATUS         = 0x12,     // 获取自检状态
    NEONATE_RSP_SELF_STATUS         = 0x13,
    NEONATE_CMD_PROBE_MOTOR         = 0x14,     // 获取探头,马达状态
    NEONATE_RSP_PROBE_MOTOR         = 0x15,
    NEONATE_CMD_CALIBRATION         = 0x16,     // 传感器校准
    NEONATE_RSP_CALIBRATION         = 0x17,
    NEONATE_CMD_MOTOR_CONTROL       = 0x18,     // 马达控制
    NEONATE_RSP_MOTOR_CONTROL       = 0x19,
    NEONATE_CMD_VIBRATION_INTENSITY = 0x1A,     // 震动强度
    NEONATE_RSP_VIBRATION_INTENSITY = 0x1B,

    NEONATE_NOTIFY_START_UP         = 0x40,     // 启动帧
    NEONATE_NOTIFY_PROBE_MOTOR      = 0x41,     // 马达与探头状态帧

    NEONATE_CYCLE_ALIVE             = 0x5B,     // 保活帧
    NEONATE_CYCLE_O2_DATA           = 0x5C,     // 氧气浓度数据帧
    NEONATE_CYCLE_AD_DATA           = 0x5D,     // 原始数据帧(AD值)

    NEONATE_DATA_ERROR              = 0x76,     // 错误警告帧

    NEONATE_UPGRADE                 = 0xF6,     // 进入升级模式
};

enum O2Concentration
{
    O2_CONCENTRATION_21_PERCENT     = 0x01,
    O2_CONCENTRATION_100_PERCENT    = 0x02
};

class NeonateProvider : public BLMProvider, public O2ProviderIFace
{
public:     // Provider的接口
    virtual bool attachParam(Param *param);

public:     // O2ProviderIFace的接口
    NeonateProvider();
    ~NeonateProvider();

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 获取版本号
    virtual void sendVersion(void);

    // 发送自检指令
    virtual void sendO2SelfTest(void);

    // 获取探头状态
    virtual void sendProbeState(void);

    // 传感器校准
    virtual void sendCalibration(int concentration);

    // 马达控制
    virtual void sendMotorControl(bool status);

    // 震动强度控制
    virtual void sendVibrationIntensity(int intensity);

    // 应答
    void sendACK(unsigned char type);

protected:
    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

private:
    void _selfTest(unsigned char *packet, int len);
};
