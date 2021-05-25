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
#include "CO2Symbol.h"
#include "Framework/Utility/Unit.h"
#include "CO2ParamInterface.h"
#include "AlarmDefine.h"

class CO2TrendWidget;
class CO2WaveWidget;
class CO2ProviderIFace;
class CO2ParamPrivate;
class OxyCRGCO2WaveWidget;
class CO2Param: public Param, public Co2ParamInterface
{
    Q_OBJECT

public:
    static CO2Param &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new CO2Param();
        }
        return *_selfObj;
    }
    static CO2Param *_selfObj;
    ~CO2Param();

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
    virtual QString getTrendWindowName();
    virtual QString getWaveWindowName();

    // 获取子参数值
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    virtual QVariantMap getTrendVariant(int id);

    // 设置数据提供对象。
    void setProvider(CO2ProviderIFace *provider);

    // 模块复位
    void reset();

    // 设置界面对象。
    void setTrendWidget(CO2TrendWidget *trendWidget);
    void setWaveWidget(CO2WaveWidget *waveWidget);

    // 设置获取EtCO2。
    short getEtCO2(void);
    void setEtCO2(short);

    // 设置获取FiCO2。
    short getFiCO2(void);
    void setFiCO2(short);

    // 获取BR。
    void setBR(short);

    // 设置RR
    void setRR(short);
    short getAWRR();

    // 设置获取实时大气压值。
    short getBaro(void);
    void setBaro(short);

    // 处理模块的状态与主机状态同步。
    void verifyApneanTime(ApneaAlarmTime time);
    void verifyWorkMode(CO2WorkMode mode);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置波形值。
    void addWaveformData(short wave, bool invalid);

    // 设置OneShot报警。
    void setOneShotAlarm(CO2OneShotType t, bool status);

    // 设置呼吸氧和中的CO2窗口波形
    void setOxyCRGCO2Widget(OxyCRGCO2WaveWidget* p);

    // 超限报警通知
    void noticeLimitAlarm(int id, bool flag);

    /**
     * @brief updateUnit 单位变更后刷新对应的窗口控件
     */
    void updateUnit(void);

    /**
     * @brief setModuleWorkMode  设置模块的工作模式
     * @param mode  工作模式
     * @return   是否设置成功
     */
    bool setModuleWorkMode(CO2WorkMode mode);

    void sendCalibrateData(int value);
    void setCalibrateData(const unsigned char *packet);
    bool getCalibrateResult();
    bool getCalibrateReply();


    /**
     * @brief setZeroStatus 设置失能CO2校零功能状态
     * @param reason
     * @param status
     */
    void setZeroStatus(CO2DisableZeroReason reason, bool status);

    /**
     * @brief getDisableZeroStatus 获取是否失能CO2校零功能
     * @return
     */
    bool getDisableZeroStatus();

    /**
     * @brief enableO2Compensation 设置气体补偿使能
     * @param enable
     */
    void enableCompensation(CO2Compensation gas, bool enable);
    bool getCompensationEnabled(CO2Compensation gas);

    /* reimplement */
    CO2ModuleType getCo2ModuleType() const;

signals:
    void updateZeroSta(bool sta);

    /**
     * @brief updateO2Compensation 更新气体是否可补偿的状态
     * @param enable
     */
    void updateCompensation(CO2Compensation gas, bool enable);

public:
    // 校零。
    void zeroCalibration(void);

    // 设置/获取窒息时间。
    void setApneaTime(ApneaAlarmTime t);
    ApneaAlarmTime getApneaTime(void);

    // 获取参数开关开关状态
    bool getAwRRSwitch(void);

    // 设置/获取波形速度。
    void setSweepSpeed(CO2SweepSpeed speed);
    CO2SweepSpeed getSweepSpeed(void);

    // 获取波形模式。
    void setSweepMode(CO2SweepMode mode);
    CO2SweepMode getSweepMode(void);

    // 设置/获取气体补偿。
    void setCompensation(CO2Compensation which, int v);
    int getCompensation(CO2Compensation which);

    // 设置/获取波形放大标尺。
    void setDisplayZoom(CO2DisplayZoom zoom);
    void updateDisplayZoom();
    CO2DisplayZoom getDisplayZoom(void);

    // 设置/获取FiCO2显示。
    void setFiCO2Display(CO2FICO2Display disp);
    void updateFiCO2Display();
    CO2FICO2Display getFICO2Display(void);

    // 获取单位。
    UnitType getUnit(void);

    /**
     * @brief setCO2Switch 设置CO2开关
     * @param on  if is on
     */
    void setCO2Switch(bool on);
    /**
     * @brief getCO2Switch  获取CO2开关
     * @return  返回CO2开关状态
     */
    bool getCO2Switch();

    // 获取CO2
    short getEtCO2MaxValue();
    short getEtCO2MinValue();

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);

    // resp窒息报警状态
    void setRespApneaStimulation(bool sta);

protected slots:
    virtual void paramUpdateTimeout();

private slots:
    void onPaletteChanged(ParamID id);

signals:
    /**
     * @brief connectStatusUpdated  and the signal function of the provider`s status updated
     * @param isConnected  and if is connected
     */
    void connectStatusUpdated(bool isConnected);

private:
    CO2Param();

    CO2ParamPrivate *const d_ptr;
};
#define co2Param (CO2Param::construction())
