/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/26
 **/

#pragma once
#include "Provider.h"
#include "IBPProviderIFace.h"
#include "COProviderIFace.h"
#include <QFile>
#include <QTextStream>

#define DATA_TXT ("/usr/local/nPM/data/")

// 数据帧类型。
enum IBPPacketId
{
    CO_CMD_MODULE_INFO          =   0x01,       // 模块信息查询
    CO_CMD_SELF_CHECK           =   0x02,       // 模块自检结果查询
    IBP_CMD_SET_AVERG_TIME      =   0x04,       // IBP平均时间设置
    IBP_CMD_SET_ZERO            =   0x06,       // IBP校准、校零设置
    IBP_CMD_SET_FILTER          =   0x07,       // IBP滤波设置
    IBP_CMD_SET_INDICATE        =   0x09,       // IBP表明设置
    CO_CMD_INST_CTL             =   0x0D,       // CO 测量控制
    CO_CMD_SET_INTERVAL         =   0x0E,       // CO 测量时间间隔设置
    CO_CMD_SET_INPUT_MODULE     =   0x11,       // Ti 输入模式设置
    CO_CMD_SET_VOLUME           =   0x12,       // 注射液体积设定
    CO_CMD_SET_DUCT_RATIO       =   0x14,       // 漂浮导管系数设定
    CO_CMD_SET_HEMODYMIC        =   0x16,       // 血液动力计算参数设置
    IBP_CMD_SET_TIME_ZERO       =   0x18,       // IBP 校零/校准时间设定
    CO_CMD_HEMODYMIC_CAL        =   0x19,       // 血液动力学计算

    IBP_RSP_INFO                =   0x60,       // 应答信息
    IBP_RSP_MODULE_INFO         =   0x61,       // 模块信息
    IBP_RSP_SELF_CHECK          =   0x62,       // 模块自检结果
    IBP_RSP_ZERO_INFO           =   0x63,       // IBP 校零、校准信息
    IBP_RSP_WAVE_DATA           =   0x64,       // IBP 实时波形数据包
    IBP_RSP_RESULT_DATA         =   0x65,       // IBP 计算结果数据包
    CO_RSP_TBTI_DATA            =   0x66,       // 实时 TB、TI 数据包
    CO_RSP_MEASURE_RESULT       =   0x67,       // CO 测量结果
    CO_RSP_HEMODYMIC_RESULT     =   0x68,       // 血液动力学参数计算结果
    IBP_RSP_CYCLE_REPORT        =   0x6A,       // 周期报告信息
    IBP_RSP_ZERO_RESULT         =   0x6B,       // IBP 校零、校准结果
    CO_RSP_MEASURE_INFO         =   0x6C,       // CO 测量信息
};

enum IBPPacketType
{
    IBP_DATA_DC     =   0x01,       // 控制命令包
    IBP_DATA_DR     =   0x02,       // 请求命令包
    IBP_DATA_DA     =   0x03,       // 命令应答包
    IBP_DATA_DD     =   0x04        // 通用数据包
};

enum ParamType
{
    PARAM_TYPE_IBP      =   0x07,
    PARAM_TYPE_CO       =   0x08
};

struct CalibrationDate
{
    CalibrationDate()
    {
        timeSecond = 0;
        timeMinute = 0;
        timeHour = 0;
        timeDay = 0;
        timeMouth = 0;
        timeYear = 0;
    }
    unsigned char timeSecond;
    unsigned char timeMinute;
    unsigned char timeHour;
    unsigned char timeDay;
    unsigned char timeMouth;
    unsigned char timeYear;
};

struct WitleafProviderStatus
{
    // version information
    int softwaveVersion;
    int arithmeticVersion;
    int protocolVersion;

    // self check result.
    bool wdStatus;
    bool adStatus;
    bool cpuStatus;
    bool ramStatus;
    bool romStatus;

    // calibration information
    CalibrationDate ibp1ZeroCalibration;
    CalibrationDate ibp1Calibration;
    CalibrationDate ibp2ZeroCalibration;
    CalibrationDate ibp2Calibration;

    // circle report information.
    bool ibp1LeadStatus;
    bool ibp2LeadStatus;
    bool tbLeadStatus;
    bool tiLeadStatus;
    bool modulePowerStatus;

    // C.O. measure information.
    bool coMeasureStatus;
    bool coMeasureParam;
    bool coHemodynamicParam;
    bool coMeasureInterval;
    bool coTBLeadoff;
    bool coTILeadoff;
};

class WitleafProvider: public Provider, public IBPProviderIFace, public COProviderIFace
{
public:     // Provider的接口。
    virtual bool attachParam(Param *param);
    virtual void dataArrived(void);

    // 协议命令解析
    void handlePacket(unsigned char *data, int len);

    // 发送协议命令
    void sendCmd(unsigned char len, unsigned char paramType,
                 unsigned char dataType, unsigned char dataId,
                 const unsigned char *data, const unsigned char *dataSN);

    //
    unsigned char calcCheckSum(const unsigned char *data, unsigned int len);

    virtual void sendVersion(void) {}
    virtual void disconnected(void);          // 模块连接断开时回调，之类实现。
    virtual void reconnected(void);           // 模块连接恢复时回调，之类实现。

public:     // IBPProviderIFace的接口
    // 模块信息查询
    virtual void moduleInfo(void);
    virtual bool isModuleInfo(unsigned char */*packet*/)
    {
        return false;
    }

    // 模块自检结果查询
    virtual void moduleSelfCheck(void);
    virtual bool isModuleSelfCheck(unsigned char */*packet*/)
    {
        return false;
    }

    // IBP 平均时间设置
    virtual void setAvergTime(IBPSignalInput /*IBP1/2*/, unsigned char /*time*/);
    virtual bool isSetAvergTime(unsigned char */*packet*/)
    {
        return false;
    }

    // IBP校准、校零设置
    virtual void setZero(IBPSignalInput /*IBP1/2*/, IBPCalibration /*calibration*/, unsigned short /*pressure*/);
    virtual bool isSetZero(unsigned char */*packet*/)
    {
        return false;
    }

    // IBP滤波设置
    virtual void setFilter(IBPSignalInput /*IBP1/2*/, IBPFilterMode /*filter*/);
    virtual bool isSetFilter(unsigned char */*packet*/)
    {
        return false;
    }

    // IBP表明设置
    virtual void setIndicate(IBPPressureName /*pressurenameIBP1*/, IBPPressureName /*pressurenameIBP2*/,
                             IBPAuxiliarySet /*auxiliarysetIBP1*/, IBPAuxiliarySet /*auxiliarysetIBP2*/);
    virtual bool isSetIndicate(unsigned char */*packet*/)
    {
        return false;
    }

    // CO 测量控制
    virtual void measureCtrl(COInstCtl /*instctl*/);
    virtual bool isMeasureCtrl(unsigned char */*packet*/)
    {
        return false;
    }

    // CO 测量时间间隔设置
    virtual void setInterval(COMeasureInterval /*interval*/);
    virtual bool isSetInterval(unsigned char */*packet*/)
    {
        return false;
    }

    // Ti 输入模式设置
    virtual void setInputMode(COTiMode /*inputmode*/, unsigned short /*watertemp*/);
    virtual bool isSetInputMode(unsigned char */*packet*/)
    {
        return false;
    }

    // 注射液体积设定
    virtual void setVolume(unsigned char /*volume*/);
    virtual bool isSetVolume(unsigned char */*packet*/)
    {
        return false;
    }

    // 漂浮导管系数设定
    virtual void setDuctRatio(unsigned short /*ratio*/);
    virtual bool isSetDuctRatio(unsigned char */*packet*/)
    {
        return false;
    }

    // 血液动力计算参数设置
    virtual void setHemodymicParam(void);
    virtual bool isSetHemodymicParam(unsigned char */*packet*/)
    {
        return false;
    }

    // IBP 校零/校准时间设定
    virtual void setTimeZero(IBPSignalInput /*IBP1/2*/, IBPCalibration /*calibration*/,
                             unsigned char /*second*/, unsigned char /*minute*/,
                             unsigned char /*hour*/, unsigned char /*day*/,
                             unsigned char /*month*/, unsigned char /*year*/);
    virtual bool isSetTimeZero(unsigned char */*packet*/)
    {
        return false;
    }

    // 血液动力学计算
    virtual void hemodymicCalc(void);
    virtual bool isHemodymicCalc(unsigned char */*packet*/)
    {
        return false;
    }

    // 获取波形采样率
    virtual int getIBPWaveformSample(void)
    {
        return 128;
    }

    // 获取波形基线
    virtual int getIBPBaseLine(void)
    {
        return 0;
    }

    // 获取最大的波形值
    virtual int getIBPMaxWaveform(void)
    {
        return 300;
    }

public:
    WitleafProvider();
    ~WitleafProvider();

private:
    // 协议数据校验
    bool _checkPacketValid(const unsigned char *data, unsigned int len);

private:
    WitleafProvider(const WitleafProvider &other);  /* no implement, just stop cppcheck complain */
    static const int _minPacketLen = 6;          // 最小数据包长度: Start,Length,ParamType,DataType,DataID,CheckSum
    static const int _maxPacketLen = (1 << 9);   // 最大数据包长度: 1字节最大表示范围。

    static const int _STX = 0xFF;               // 数据包头。

    WitleafProviderStatus _status;
};
