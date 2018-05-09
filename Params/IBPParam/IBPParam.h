#pragma once
#include <QPalette>
#include "Param.h"
#include "UnitManager.h"
#include "AlarmDefine.h"
#include "IBPSymbol.h"
#include "IBPDefine.h"
#include "IBPProviderIFace.h"
#include "PromptInfoBarWidget.h"

class IBPWaveWidget;
class IBPTrendWidget;
class IBPMenu;
class IBPParam: public Param
{
public:
    static IBPParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new IBPParam();
        }
        return *_selfObj;
    }
    ~IBPParam();

//    static IBPEntitleLimitInfo limit[IBP_PRESSURE_NR];
//    static IBPEntitleLimitInfo rulerLimit[IBP_RULER_RANGE_NR];
    QList<IBPScaleInfo> ibpScaleList;                      // all scale for ibp

public:
    // 处理demo数据
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // check whether the sub param is avaliabe
    virtual bool isSubParamAvaliable(SubParamID id);

    // 设置数据提供对象。
    void setProvider(IBPProviderIFace *provider);

    // 设置测量实时数据。
    void setRealTimeData(unsigned short sys, unsigned short dia, unsigned short map,
                         unsigned short pr, IBPSignalInput IBP);

    // 设置波形值。
    void addWaveformData(short wave, bool invalid, IBPSignalInput = IBP_INPUT_1);

    // 设置界面对象
    void setIBPTrendWidget(IBPTrendWidget *trendWidget, IBPSignalInput IBP = IBP_INPUT_1);
    void setWaveWidget(IBPWaveWidget *waveWidget, IBPSignalInput IBP = IBP_INPUT_1);
    void setInfobarWidget(PromptInfoBarWidget *infoBarWidget);

    /**
     * @brief getIBPScale get the IBP scale info
     * @param name
     * @return
     */
    IBPScaleInfo getIBPScale(IBPPressureName name);

public:
    // 校零。
    void zeroCalibration(IBPSignalInput IBP);

    // 校准
    void setCalibration(IBPSignalInput IBP, unsigned short value);

    // 校零校准信息
    void calibrationInfo(IBPCalibration calib, IBPSignalInput IBP, int calibinfo);
    
    // 导联状态
    void leadStatus(bool staIBP1, bool staIBP2);

    // 设置/获取波形速度。
    void setSweepSpeed(IBPSweepSpeed speed);

    // 设置标名。
    void setEntitle(IBPPressureName entitle, IBPSignalInput IBP);

    // get pressure name
    IBPPressureName getEntitle(IBPSignalInput signal) const;

    // 设置滤波
    void setFilter(IBPFilterMode filter);

    // 设置灵敏度
    void setSensitivity(IBPSensitivity sensitivity);

    // 设置获取IBP计算结果值
    IBPParamInfo getParamData(IBPSignalInput IBP);
    void setParamData(IBPSignalInput IBP, unsigned short sys, unsigned short dia,
                      unsigned short mean, unsigned short pr);

    // 获取ibp两通道标名对应的参数ID
    void getSubParamID(SubParamID &ibp1, SubParamID &ibp2);

public:


private:
    IBPParam();
    static IBPParam *_selfObj;
    void _setWaveformSpeed(IBPSweepSpeed speed);

    QMap<IBPPressureName, SubParamID> _ibpSubParamMap;      // 找到压力标名对应的字参数ID

    IBPProviderIFace *_provider;

    IBPWaveWidget *_waveWidgetIBP1;
    IBPWaveWidget *_waveWidgetIBP2;

    IBPTrendWidget *_trendWidgetIBP1;
    IBPTrendWidget *_trendWidgetIBP2;

    PromptInfoBarWidget *_infoBarWidget;

    IBPParamInfo _ibp1;
    IBPParamInfo _ibp2;

    bool _staIBP1;                        // 导联状态
    bool _staIBP2;
};
#define ibpParam (IBPParam::construction())
