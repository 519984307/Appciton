/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/29
 **/



#pragma once
#include "Param.h"
#include "ECGSymbol.h"
#include "ECGDefine.h"
#include "SoundManager.h"

#include "ECGParam.h"

class ECGTrendWidget;
class ECGWaveWidget;
class ECGProviderIFace;
class ECGDupParam: public Param
{
    Q_OBJECT
public:
    enum HrSourceType{
        HR_SOURCE_AUTO,
        HR_SOURCE_ECG,
        HR_SOURCE_SPO2,
    };

    static ECGDupParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ECGDupParam();
        }
        return *_selfObj;
    }
    static ECGDupParam *_selfObj;

    // 析构。
    virtual ~ECGDupParam();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList &waveforms,
                                       QStringList &waveformShowName, int flag = 0);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(ECGProviderIFace *provider);

    // 设置界面对象。
    void setTrendWidget(ECGTrendWidget *trendWidget);
    void setWaveWidget(ECGWaveWidget *waveWidget, ECGLead whichLead);

    // 更新/读取HR的值。
    void updatePR(short pr);
    void updateHR(short hr);
    // could return pr if hr is unvalid
    short getHR(void) const;
    // 是否为HR有效。
    bool isHRValid(void);

    // 更新VFVT的值。
    void updateVFVT(bool onoff);

    // 更新心跳图标
    void updateHRBeatIcon();
    void updatePRBeatIcon();

    // 是否发生报警
    void isAlarm(bool isAlarm, bool isLimit);

    // get HR souce
    HrSourceType getHrSource() const;

    // 设置计算导联字串。
    void setECGTrendWidgetCalcName(ECGLead calLead);

    /**
     * @brief setHrSource  设置hr来源
     * @param type
     */
    void setHrSource(HrSourceType type);

    /**
     * @brief isAutoTypeHrSouce
     * @return
     */
    bool isAutoTypeHrSouce(void) const;

private slots:
    void onPaletteChanged(ParamID id);

private:
    // 构造。
    ECGDupParam();

    ECGProviderIFace *_provider;
    ECGTrendWidget *_trendWidget;

    short _hrValue;
    short _prValue;
    bool _hrBeatFlag;
    bool _isAlarm;
    bool _isAutoHrSource;
    HrSourceType _hrSource;
};
#define ecgDupParam (ECGDupParam::construction())
