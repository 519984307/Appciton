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
#include "RESPSymbol.h"
#include "AlarmDefine.h"
#include "RESPParamInterface.h"

class OxyCRGRESPWaveWidget;
class RESPParamPrivate;
class RESPWaveWidget;
class RESPProviderIFace;
class RESPFullDisclosure;
class OxyCRGRRHRWaveWidget;
class RESPParam: public Param, public RESPParamInterface
{
    Q_OBJECT
public:
    static RESPParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RESPParam();
        }
        return *_selfObj;
    }
    static RESPParam *_selfObj;
    ~RESPParam();

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

    // 获取子参数当前单位
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(RESPProviderIFace *provider);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置界面对象。
    void setWaveWidget(RESPWaveWidget *waveWidget);

    /**
     * @brief setOxyCRGWaveRESPWidget
     * @param waveWidget
     */
    void setOxyCRGWaveRESPWidget(OxyCRGRESPWaveWidget *waveWidget);

    // 设置波形值。
    void addWaveformData(int wave, int flag);

    // 设置RR
    void setRR(short rrValue);

    // 电极脱落
    void setLeadoff(bool flag, bool isFirstConnect = true);

    // 设置OneShot报警。
    void setOneShotAlarm(RESPOneShotType t, bool f);

    // 模块复位
    void reset();

    /**
     * @brief getWaveDataRate  获取波形速率
     * @return
     */
    int getWaveDataRate(void) const;

    /**
     * @brief setOxyCRGRrHrTrend
     * @param w
     */
    void setOxyCRGRrHrTrend(OxyCRGRRHRWaveWidget *w);

    /**
     * @brief setSweepMode  设置波形模式
     * @param mode 波形模式
     */
    void setSweepMode(RESPSweepMode mode);

    /**
     * @brief getSweepMode  获取波形模式
     * @return  返回波形模式
     */
    RESPSweepMode getSweepMode(void);

public:
    // 设置/获取波形速度。
    void setSweepSpeed(RESPSweepSpeed speed);
    RESPSweepSpeed getSweepSpeed(void);

    // 设置/获取窒息选项。
    void setApneaTime(ApneaAlarmTime t);
    ApneaAlarmTime getApneaTime(void);

    // 设置/波形的放大倍数。
    void setZoom(RESPZoom zoom);
    RESPZoom getZoom(void);

    // 设置/获取RESP监护功能
    void setRespMonitoring(int enable);

    /**
     * @brief getRespMonitoring
     * @return
     */
    int getRespMonitoring();

    // 设置/获取呼吸导联。
    void setCalcLead(RESPLead lead);
    RESPLead getCalcLead(void);

    // 使能呼吸计算
    void enableRespCalc(bool enable);

    /**
     * @brief rrInaccurate 设置Resp值低状态
     * @param isInaccurate
     */
    void rrInaccurate(bool isInaccurate);
    bool isRRInaccurate();

    /**
     * @brief getRRMeasureMaxRange 获取RR的测量范围
     * @return
     */
    int getRRMeasureMaxRange();
    int getRRMeasureMinRange();

    /**
     * @brief getModuleType get the module type
     * @return
     */
    RESPModuleType getModuleType() const;

signals:
    void calcLeadChanged(RESPLead lead);

private slots:
    void onPaletteChanged(ParamID id);

private:
    RESPParam();

    RESPParamPrivate *const d_ptr;
};
#define respParam (RESPParam::construction())
