/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/28
 **/



#pragma once
#include "Param.h"
#include "RESPSymbol.h"

class RESPTrendWidget;
class CO2TrendWidget;
class RESPProviderIFace;
class RESPDupParam: public Param
{
    Q_OBJECT
public:
    enum BrSourceType {
        BR_SOURCE_CO2,
        BR_SOURCE_ECG,
    };

    enum ParamSourceType {
        BR,
        RR,
    };

    static RESPDupParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RESPDupParam();
        }
        return *_selfObj;
    }
    static RESPDupParam *_selfObj;

    // 析构。
    virtual ~RESPDupParam();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取趋势窗体
    virtual QString getTrendWindowName();

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 使能
    virtual bool isEnabled();

    virtual QVariantMap getTrendVariant(int id);

    // 设置界面对象。
    void setTrendWidget(RESPTrendWidget *trendWidget);

    // 是否发生报警
    void isAlarm(bool isAlarm, bool isLimit);

    // 设置/获取RR的值。
    void updateRR(short rr);
    void updateBR(short br);
    short getRR(void);

    // get br source type
    BrSourceType getBrSource() const;

    /**
     * @brief setBrSource  set br source
     * @param type
     */
    void setBrSource(BrSourceType type);

    /**
     * @brief getParamSourceType 获取参数来源类型 br或者rr
     * @return
     */
    ParamSourceType getParamSourceType() const;

    /**
     * @brief setAutoBrSourceStatue
     * @param isEnabled
     */
    void setAutoBrSourceStatue(bool isEnabled);

    /**
     * @brief isAutoBrSourceEnabled
     * @return
     */
    bool isAutoBrSourceEnabled() const;

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);

    void setRRSource(BRRRSourceType source);
    void updateRRSource();

    // resp窒息报警状态
    void setRespApneaStimulation(bool sta);
signals:
    /**
     * @brief brSourceStatusUpdate  br来源状态更新
     */
    void brSourceStatusUpdate();

protected slots:
    virtual void paramUpdateTimeout();

private slots:
    void onPaletteChanged(ParamID id);

private:
    // 构造。
    RESPDupParam();

    /**
     * @brief handleBRRRValue  handle br/rr value when update br/rr value
     */
    void handleBRRRValue();

    RESPTrendWidget *_trendWidget;

    short _rrValue;
    short _brValue;
    bool _isAlarm;

    bool _isAutoBrSource;
    BrSourceType _manualBrSourceType;
};
#define respDupParam (RESPDupParam::construction())

