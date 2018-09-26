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
#include "OxyCRGRESPWidget.h"

class RESPWaveWidget;
class RESPProviderIFace;
class RESPFullDisclosure;
class RESPParam: public Param
{
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
    virtual void getAvailableWaveforms(QStringList &waveforms,
            QStringList &waveformShowName, int flag);
    virtual void getTrendWindow(QString &trendWin);
    virtual void getWaveWindow(QString &waveWin);

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

    // 设置血氧界面对象。
    void setOxyWaveWidget(OxyCRGRESPWidget *waveWidget);

    // 设置波形值。
    void addWaveformData(int wave, int flag);

    // 设置RR
    void setRR(short rrValue);

    // 电极脱落
    void setLeadoff(bool flag);

    // 设置OneShot报警。
    void setOneShotAlarm(RESPOneShotType t, bool f);

    // 模块复位
    void reset();

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
    int getRespMonitoring() { return _respMonitoring; }

    // 设置/获取呼吸导联。
    void setCalcLead(RESPLead lead);
    RESPLead getCalcLead(void);

    // 使能呼吸计算
    void enableRespCalc(bool enable);

private:
    RESPParam();
    void _setWaveformSpeed(RESPSweepSpeed speed);

    RESPProviderIFace *_provider;
    RESPWaveWidget *_waveWidget;
    OxyCRGRESPWidget *_waveOxyCRGWidget;
    bool _respMonitoring;
    bool _connectedProvider;
};
#define respParam (RESPParam::construction())
