/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/1/7
 **/

#pragma once
#include "BLMProvider.h"
#include "ECGProviderIFace.h"
#include "RESPProviderIFace.h"

#include <QFile>
#include <QTextStream>

// 收到的数据帧类型。
enum RevPacketType
{
    TE3_RSP_VERSION                        = 0x11,       // 版本。
    TE3_RSP_SYSTEM_TEST                    = 0x13,       // 系统测试结果。
    TE3_RSP_PACE_SYNC_STATUS               = 0x15,       // PACE_SYNC信号使能控制。
    TE3_RSP_ECG_LEAD_MODE                  = 0x17,       // ECG导联模式。
    TE3_RSP_FILTER_PARAMETER               = 0x19,       // 滤波参数。
    TE3_RSP_NOTCH_FILTER                   = 0x1B,       // 工频滤波参数。
    TE3_RSP_ECG_SAMPLE_CONFIG              = 0x1D,       // 心电的采集配置。
    TE3_RSP_ECG_CALC_LEAD                  = 0x1F,       // 心电计算导联。
    TE3_RSP_SET_PACEMARK_ONOFF             = 0x21,       // pace检测开关状态。
    TE3_RSP_ECG_PATIENT_TYPE               = 0x23,       // 设置病人类型的回复
    TE3_RSP_ECG_DATA_SYNC                  = 0x25,       // data sync response
    TE3_RSP_SET_WORK_MODE_IN_MONITOR       = 0x27,       // 设置监护下工作模式回复
    TE3_RSP_SET_WORK_MODE                  = 0x29,       // 设置工作模式回复
    TE3_RSP_ENABLE_PD_BLANK_SIGANL         = 0x31,       // 使能pd blank回复
    TE3_RSP_ENABLE_VF_CALC                 = 0x33,       // enable vf calc response
    TE3_RSP_RESP_APNEA_INTERVAL            = 0x4F,       // 呼吸的窒息结果。
    TE3_RSP_ECG_ENABLE_RAW_DATA            = 0x63,       // 原始数据开关
    TE3_RSP_ECG_LEAD_CABLE_TYPE            = 0x7D,       // 心电导联类型。
    TE3_RSP_SELFTEST_RESULT                = 0x7F,       // 发送自测结果。

    TE3_NOTIFY_SYSTEM_STARTED              = 0x80,       // TE3启动了。
    TE3_NOTIFY_SYSTEM_TEST_RESULT          = 0x81,       // 发送自测结果。
    TE3_NOTIFY_ECG_LEAD_CABEL_TYPE         = 0x90,       // 心电导联线类型。
    TE3_NOTIFY_RESP_ALARM                  = 0xD2,       // RESP报警
    TE3_NOTIFY_VF_ALARM                    = 0xA0,       // VF报警
    TE3_NOTIFY_ASYS_ALARM                  = 0xA1,       // 停搏报警
    TE3_NOTIFY_RESP_INACCURATE             = 0XD3,       // 呼吸率低

    TE3_CYCLE_ACTIVE                       = 0xB0,       // 保活。
    TE3_CYCLE_STORE                        = 0xB9,       // 需要将数据存储至TXT
    TE3_CYCLE_ECG                          = 0xBA,       // ECG数据。
    TE3_CYCLE_RESP                         = 0xD0,       // 呼吸数据。
    TE3_CYCLE_RR                           = 0xD1,       // 呼吸率数据。
    TE3_CYCLE_HR                           = 0xBB,       // HR value
    TE3_CYCLE_VFVT                         = 0xBC,       // vfvt value

    TE3_WARNING_ERROR_CODE                 = 0xF0,       // 错误报告。
};

// 发送的数据帧类型。
enum SendPacketType
{
    TE3_CMD_NASK                           = 0x00,   // nask
    TE3_CMD_ASK                            = 0x01,   // 应答

    TE3_CMD_GET_VERSION                    = 0x10,       // 获取版本。
    TE3_CMD_SYSTEM_TEST                    = 0x12,       // 系统测试。
    TE3_CMD_ENABLE_PACE_SYNC               = 0x14,       // PACE_SYNC信号使能控制。
    TE3_CMD_SET_ECG_LEAD_MODE              = 0x16,       // ECG导联模式。
    TE3_CMD_SET_FILTER_PARAMETER           = 0x18,       // 滤波参数。
    TE3_CMD_SET_NOTCH_FILTER               = 0x1A,       // 工频滤波参数。
    TE3_CMD_GET_ECG_SAMPLE_CONFIG          = 0x1C,       // 心电的采集配置。
    TE3_CMD_SET_ECG_CALC_LEAD              = 0x1E,       // 设置心电计算导联。
    TE3_CMD_SET_PACEMARK_ONOFF             = 0x20,       // 设置pace检测开关。
    TE3_CMD_SET_PATIENT_TYPE               = 0x22,       // 设置病人类型
    TE3_CMD_SET_DATA_SYNC                  = 0x24,       // data sync control
    TE3_CMD_SET_WORK_MODE_IN_MONITOR       = 0x26,       // 设置监护工作模式
    TE3_CMD_SET_WORK_MODE                  = 0x28,       // 设置工作模式
    TE3_CMD_ENABLE_PD_BLANK_SIGNAL         = 0x30,       // 使能PD Blank信号
    TE3_CMD_ENABLE_VF_CALC                 = 0x32,       // vt calc
    TE3_CMD_SET_RESP_APNEA_INTERVAL        = 0x4E,       // 设置呼吸的窒息时间。
    TE3_CMD_SET_RESP_CALC_LEAD             = 0x50,       // 设置呼吸计算导联
    TE3_CMD_SET_RESP_CALS_SWITCH           = 0x60,       // 设置呼吸计算开关
    TE3_CMD_ECG_ENABLE_RAW_DATA            = 0x62,       // 原始数据开关
    TE3_CMD_GET_LEAD_CABLE_TYPE            = 0x7C,       // 获取导联线类型
    TE3_CMD_GET_SELFTEST_RESULT            = 0x7E,       // 获取自测结果。
    TE3_UPGRADE_ALIVE                      = 0xFE,       // 升级保活帧
};

class E5Provider: public BLMProvider, public ECGProviderIFace,
        public RESPProviderIFace
{
#ifdef CONFIG_UNIT_TEST
friend class TestParamFunction;
#endif
public:     // Provider的接口。
    virtual bool attachParam(Param *param);

    // 协议命令解析
    virtual void handlePacket(unsigned char *data, int len);

    // 发送协议命令
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

public:     // ECGProviderIFace 的接口。
    // 获取自检结果。
    virtual void getSelfTestStatus(void);

    // 获取波形的采样速度。
    virtual int getWaveformSample(void);

    // 设置波形的采样速度。
    virtual void setWaveformSample(int rate);

    // 获取波形基线
    virtual int getBaseLine(void) {return 0;}

    // 获取+/-0.5mV对应的数值。
    virtual int getP05mV();
    virtual int getN05mV();

    // 获取导联线类型
    virtual void getLeadCabelType();

    // 设置导联系统。
    virtual void setLeadSystem(ECGLeadMode leadSystem);

    // 设置计算导联。
    virtual void setCalcLead(ECGLead lead);

    // 设置病人类型。0表示成人，1表示小孩，2表示新生儿，3表示空
    virtual void setPatientType(unsigned char type);

    // 设置工作模式
    virtual void setWorkMode(ECGWorkMode workMode);

    // set filter mode
    virtual void setFilterMode(ECGFilterMode mode);

    // 设置带宽。
    virtual void setBandwidth(ECGBandwidth bandwidth);

    // set self learn
    virtual void setSelfLearn(bool onOff);

    // set threshold
    virtual void setARRThreshold(ECGAlg::ARRPara parameter, short value);

    // 起搏器设置。
    virtual void enablePacermaker(ECGPaceMode onoff);

    // 设置工频滤波。
    virtual void setNotchFilter(ECGNotchFilter notch);

    // ST开关。
    virtual void enableSTAnalysis(bool onoff);

    // enable vf calc control
    virtual void enableVFCalcCtrl(bool enable);

    // ST点设置。
    virtual void setSTPoints(int iso, int st);

    // raw data
    virtual void enableRawData(bool onoff);

    //获取版本号
    virtual void sendVersion(void);

public:     // RESPProviderIFace的接口。
    // 获取最大值
    virtual int maxRESPWaveValue() {return 0x3FFF;}

    // 获取最小值
    virtual int minRESPWaveValue() {return -0x4000;}

    // 获取波形的采样速度。
    virtual int getRESPWaveformSample(void);

    // 获取波形基线
    virtual int getRESPBaseLine(void) {return 0;}

    // 关闭窒息处理。
    virtual void disableApnea(void);

    // 设置窒息时间。
    virtual void setApneaTime(ApneaAlarmTime t);

    // 获取波形的采样速度。
    virtual void setWaveformZoom(RESPZoom zoom);

    // 设置呼吸导联
    virtual void setRESPCalcLead(RESPLead lead);

    // 启用RESP
    virtual void enableRESPCalc(bool enable);

    /* reimplement */
    RESPModuleType getRespModuleType() const;

public:
    E5Provider();
    ~E5Provider();

protected:
    virtual void disconnected(void);
    virtual void reconnected(void);
    virtual void sendDisconnected();

private:
    int _waveSampleRate;
    int _isFristConnect;
    bool _isRespFirstConnect;
    bool _isSupportRESP;

private:
    void _handleECGRawData(const unsigned char *data, unsigned len);
    void _handleRESPRawData(const unsigned char *data, unsigned len);
};
