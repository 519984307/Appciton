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

typedef struct CCHDData
{
    CCHDData()
        : handValue(InvData()),
          footValue(InvData()),
          result(CCHD_NR),
          time(0)
    {}
    short handValue;
    short footValue;
    CCHDResult result;
    unsigned time;
}cchdData;

class SPO2TrendWidget;
class SPO2WaveWidget;
class OxyCRGSPO2TrendWidget;
class SPO2ProviderIFace;
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

    // 取值范围
    int getSPO2MaxValue(void);

    // 设置服务模式升级数据提供对象。
    void setServiceProvider(SPO2ProviderIFace *provider);

    // 设置界面对象。
    void setTrendWidget(SPO2TrendWidget *trendWidget);
    void setWaveWidget(SPO2WaveWidget *waveWidget);
    void setOxyCRGSPO2Trend(OxyCRGSPO2TrendWidget *trendWidget);

    // PR音量
    SoundManager::VolumeLevel getPluseToneVolume(void);

    // 设置/获取SPO2的值。
    void setSPO2(short spo2Value);
    short getSPO2(void);

    // 设置/获取PR的值。
    void setPR(short prValue);

    // 设置PI值
    void updatePIValue(short piValue);

    // 设置波形值。
    void addWaveformData(short wave);

    // 设置棒图值。
    void addBarData(short data);

    // 设置脉搏音标志。
    void setPulseAudio(bool pulse);

    /**
     * @brief setBeatVol
     * @param val
     */
    void setBeatVol(SoundManager::VolumeLevel vol);

    /**
     * @brief getBeatVol
     * @return
     */
    SoundManager::VolumeLevel getBeatVol(void) const;

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

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // set Sensor off
    int setSensorOff(bool flag);

    // received package
    void receivePackage();

public:
    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 设置/获取平均时间
    void setAverageTime(AverageTime index);
    AverageTime getAverageTime(void);

    /**
     * @brief setRawDataSend 设置原始数据发送开关
     * @param onOff
     */
    void enableRawDataSend(bool onOff);

    /**
     * @brief setSensitivity  设置灵敏度
     * @param sens  灵敏度值
     */
    void setSensitivity(int sens);

    /**
     * @brief getSensitivity  获取灵敏度
     * @return sens
     */
    int getSensitivity();

    // 设置/获取快速血氧
    void setFastSat(bool isFast);
    bool getFastSat(void);

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

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);

    /**
     * @brief setModuleType  设置模块类型
     * @param type  模块类型
     */
    void setModuleType(SPO2ModuleType type);

    /**
     * @brief getModuleType  获取模块类型
     * @return  模块类型
     */
    SPO2ModuleType getModuleType() const;

    /**
     * @brief setNibpSameSide 设置NIBP同侧功能
     * @param flag
     */
    void setNibpSameSide(bool flag);
    bool isNibpSameSide(void);

    void setCCHDData(short value, bool isHand);

    /**
     * @brief getCCHDResult 获取cchd的筛查结果
     * @param handValue 手部测量数据
     * @param footValue 足部测量数据
     * @return
     */
    CCHDResult updateCCHDResult();

    /**
     * @brief getCCHDDataList 获取cchd数据
     * @return
     */
    QList<cchdData> getCCHDDataList();

    /**
     * @brief clearCCHDData 清楚数据
     * @param isCleanup　是否清楚全部数据
     */
    void clearCCHDData(bool isCleanup = false);

    /**
     * @brief setPerfusionStatus  and the function of setting the SPO2 perfusion status
     * @param isLow  and true if low perfusion
     */
    void setPerfusionStatus(bool isLow);

    /**
     * @brief getPerfusionStatus  and the function of getting the SPO2 perfusion status
     * @return  and return true if low perfusion
     */
    bool getPerfusionStatus() const;

    /**
     * @brief initModule  and the function of initting the module
     */
    void initModule();

protected slots:
    virtual void paramUpdateTimeout();

private slots:
    void checkSelftest();
    void onPaletteChanged(ParamID id);

    /**
     * @brief onUpgradeT5ModuleCompleted  and the function of upgrading the T5 module when completed
     */
    void onUpgradeT5ModuleCompleted();

    /**
     * @brief onTempReset  and the function of temp reset
     */
    void onTempReset();

private:
    SPO2Param();
    void _setWaveformSpeed(SPO2WaveVelocity speed);

    SPO2ProviderIFace *_provider;
    SPO2TrendWidget *_trendWidget;
    SPO2WaveWidget *_waveWidget;

    bool _isEverCheckFinger;  // use to decide prompt sensor off
    bool _isEverSensorOn;   // use to decide display waveform at power on

    short _spo2Value;
    short _prValue;
    short _barValue;
    short _piValue;

    bool _isValid;
    bool _sensorOff;

    int _recPackageInPowerOn2sec;  // if receve 5 packages, selftest success, or selftest failed

    OxyCRGSPO2TrendWidget *_oxyCRGSPO2Trend;
    bool _connectedProvider;
    SPO2ModuleType _moduleType;

    QList<cchdData> _cchdDataList;
    int _repeatTimes;

    bool _isLowPerfusion;
    bool _isForceUpdatingPR;  // 当spo2的弱灌注状态发生变化时，该PR状态位为true
    bool _isForceUpdatingSPO2; // 当spo2的弱灌注状态发生变化时, 该SPO2状态位为true

    bool _isT5ModuleUpgradeCompleted;
};
#define spo2Param (SPO2Param::construction())
