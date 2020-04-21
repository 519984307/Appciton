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
#include "SPO2Define.h"
#include "SPO2ParamInterface.h"

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
class PVITrendWidget;
class PITrendWidget;
class SPHBTrendWidget;
class SPOCTrendWidget;
class SPMETTrendWidget;
class SPCOTrendWidget;
class OxyCRGSPO2TrendWidget;
class SPO2ProviderIFace;
class SPO2ParamPrivate;
class SPO2Param: public Param, public SPO2ParamInterface
{
    Q_OBJECT

public:
    static SPO2Param &getInstance(void);
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
    virtual void getAvailableWaveforms(QStringList *waveforms,
                                       QStringList *waveformShowName, int flag);
    virtual QString getWaveWindow(bool isPlugIn = false);

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(SPO2ProviderIFace *provider, bool isPlugIn = false);

    // 模块复位
    void reset();

    // 设置界面对象。
    void setTrendWidget(SPO2TrendWidget *trendWidget);
    void setTrendWidget(SPHBTrendWidget *trendWidget);
    void setTrendWidget(SPOCTrendWidget *trendWidget);
    void setTrendWidget(SPMETTrendWidget *trendWidget);
    void setTrendWidget(PVITrendWidget *trendWidget);
    void setTrendWidget(PITrendWidget *trendWidget);
    void setTrendWidget(SPCOTrendWidget *trendWidget);
    void setWaveWidget(SPO2WaveWidget *waveWidget, bool isPlugIn = false);
    void setOxyCRGSPO2Trend(OxyCRGSPO2TrendWidget *trendWidget);

    // PR音量
    SoundManager::VolumeLevel getPluseToneVolume(void);

    // 设置/获取SPO2的值。
    void setSPO2(short spo2Value);
    void setPlugInSPO2(short spo2Value);
    short getSPO2(bool isPlugIn = false);

    // 获取血氧差值
    short getSPO2D();

    // 设置/获取SpHb的值。
    void setSpHb(short value);
    short getSpHb();

    // 设置/获取SpHb的值。
    void setSpOC(short value);
    short getSpOC();

    // 设置/获取SpHb的值。
    void setPVI(short value);
    short getPVI();

    // 设置/获取SpHb的值。
    void setSpMet(short value);
    short getSpMet();

    // 设置/获取PR的值。
    void setPR(short prValue);

    // 设置PI值
    void setPI(short piValue);
    short getPI();

    // 设置SpCO值
    void setSpCO(short spcoValue);
    short getSpCO();

    // 设置波形值。
    void addWaveformData(short wave, unsigned char waveFlag = 0, bool isPlugIn = false);

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
    void setNotify(bool enable, QString str = " ", bool isPlugIn = false);

    // 设置搜索脉搏标志。
    void setSearchForPulse(bool pulse, bool isPlugIn = false);

    // 设置OneShot报警。
    void setOneShotAlarm(SPO2OneShotType t, bool f, bool isPlugin = false);

    // 是否超限报警
    void noticeLimitAlarm(SubParamID id, bool isAlarm);

    /**
     * @brief setValidStatus 设置波形有效状态
     * @param isValid   是否有效，true：波形将画直线 false：波形将画虚线
     * @param isPlugIn  是否是插件模块
     */
    void setValidStatus(bool isValid, bool isPlugIn = false);
    bool isValid(bool isPlugIn = false);
    bool isConnected(bool isPlugin);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected, bool isPlugIn = false);

    // received package
    void receivePackage();

public:
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

    /**
     * @brief getSpHbAveragingMode 设置/获取SpHb平均模式
     * @return
     */
    void setSpHbAveragingMode(SpHbAveragingMode mode);
    SpHbAveragingMode getSpHbAveragingMode();

    /**
     * @brief getSpHbBloodVessel 设置/获取SpHb测量血管
     * @return
     */
    void setSpHbBloodVessel(SpHbBloodVesselMode mode);
    SpHbBloodVesselMode getSpHbBloodVessel();

    /**
     * @brief getSpHbPrecision 设置/获取SpHb精度模式
     * @return
     */
    void setSpHbPrecision(SpHbPrecisionMode mode);
    SpHbPrecisionMode getSpHbPrecision();

    /**
     * @brief getPviAveragingMode 设置/获取PVI平均模式
     * @return
     */
    void setPviAveragingMode(AveragingMode mode);
    AveragingMode getPviAveragingMode();

    // 设置/获取智能音。
    void setSmartPulseTone(SPO2SMARTPLUSETONE sens);
    SPO2SMARTPLUSETONE getSmartPulseTone(void);

    // 设置/获取波形速度。
    void setSweepSpeed(int speed);
    int getSweepSpeed(void);

    // get is ever check finger
    bool getEverCheckFinger(bool isPlugin = false);

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
    void setPerfusionStatus(bool isLow, bool isPlugIn = false);

    /**
     * @brief getPerfusionStatus  and the function of getting the SPO2 perfusion status
     * @return  and return true if low perfusion
     */
    bool getPerfusionStatus(bool isPlugIn = false) const;

    /**
     * @brief initModule  the function of initting the module
     * @param plugin true to initializing the plugin module, otherwise, initialize the internal module
     */
    void initModule(bool plugin);

    /**
     * @brief clearTrendWaveData 清除趋势波形的数据
     */
    void clearTrendWaveData();

    /**
     * @brief showSignalIQ 是否显示Signal IQ值
     * @param show
     */
    void showSignalIQ(bool show);

    /**
     * @brief isShowSignalIQ
     * @return
     */
    bool isShowSignalIQ();

    /**
     * @brief setProviderInfo
     * @param isPlugIn
     * @param type
     */
    void setProviderInfo(bool isPlugIn, SPO2RainbowType type);
    SPO2RainbowType getProviderInfo(bool isPlugIn);

    /**
     * @brief setSensor 设置当前使用的探头
     * @param sensor
     */
    void setSensor(SPO2RainbowSensor sensor);

signals:
    void clearTrendData();

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
    SPO2ParamPrivate * const d_ptr;
};
#define spo2Param (SPO2Param::getInstance())
