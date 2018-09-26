/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/


#pragma once
#include <QPalette>
#include "Param.h"
#include "SPO2Symbol.h"
#include "SoundManager.h"

class SPO2TrendWidget;
class SPO2WaveWidget;
class OxyCRGSPO2Widget;
class OxyCRGSPO2TrendWidget;
class SPO2ProviderIFace;
class SPO2ParamPrivate;
class SPO2Param: public Param
{
    Q_OBJECT

public:
    static SPO2Param &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SPO2Param();
        }
        return *_selfObj;
    }
    static SPO2Param *_selfObj;
    ~SPO2Param();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);
    /* reimplement */
    virtual void exitDemo();

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList &waveforms,
                                       QStringList &waveformShowName, int flag);

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(SPO2ProviderIFace *provider);

    // 模块复位
    void reset();

    // 设置/获取增益。
    void setGain(SPO2Gain gain);
    SPO2Gain getGain(void);

    // 取值范围
    int getSPO2MaxValue(void);

    // 设置服务模式升级数据提供对象。
    void setServiceProvider(SPO2ProviderIFace *provider);

    // 设置界面对象。
    void setTrendWidget(SPO2TrendWidget *trendWidget);
    void setWaveWidget(SPO2WaveWidget *waveWidget);
    void setOxyCRGWaveWidget(OxyCRGSPO2Widget *waveWidget);
    void setOxyCRGTrendWidget(OxyCRGSPO2TrendWidget *trendWidget);

    // PR音量
    SoundManager::VolumeLevel getPluseToneVolume(void);

    // 设置/获取SPO2的值。
    void setSPO2(short spo2Value);
    short getSPO2(void);

    // 设置/获取PR的值。
    void setPR(short prValue);

    // 设置波形值。
    void addWaveformData(short wave);

    // 设置棒图值。
    void addBarData(short data);

    // 设置脉搏音标志。
    void setPulseAudio(bool pulse);

    // 设置波形上的提示信息。
    void setNotify(bool enable, QString str = " ");

    // 设置搜索脉搏标志。
    void setSearchForPulse(bool pulse);

    // 设置OneShot报警。
    void setOneShotAlarm(SPO2OneShotType t, bool f);

    // 是否超限报警
    void noticeLimitAlarm(bool isAlarm);

    // 有效状态
    void setValidStatus(bool isValid);
    bool isValid();
    bool isConnected();

    // set Sensor off
    int setSensorOff(bool flag);

    // received package
    void receivePackage();

public:
    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 设置/获取灵敏度。
    void setSensitivity(SPO2Sensitive sens);
    SPO2Sensitive getSensitivity(void);

    // 设置/获取智能音。
    void setSmartPulseTone(SPO2SMARTPLUSETONE sens);
    SPO2SMARTPLUSETONE getSmartPulseTone(void);

    // 设置/获取波形速度。
    void setSweepSpeed(int speed);
    int getSweepSpeed(void);

    // get is ever check finger
    bool getEverCheckFinger()
    {
        return _isEverCheckFinger;
    }

    // get is ever sensor on
    bool getEverSensorOn()
    {
        return _isEverSensorOn;
    }

private slots:
    void checkSelftest();

private:
    SPO2Param();
    void _setWaveformSpeed(SPO2WaveVelocity speed);

    SPO2ProviderIFace *_provider;
    SPO2TrendWidget *_trendWidget;
    SPO2WaveWidget *_waveWidget;
    OxyCRGSPO2Widget *_waveOxyCRGWidget;

    bool _isEverCheckFinger;  // use to decide prompt sensor off
    bool _isEverSensorOn;   // use to decide display waveform at power on

    short _spo2Value;
    short _prValue;
    short _barValue;

    bool _isValid;
    bool _sensorOff;

    int _recPackageInPowerOn2sec;  // if receve 5 packages, selftest success, or selftest failed

    SPO2ParamPrivate *const d_ptr;
};
#define spo2Param (SPO2Param::construction())
