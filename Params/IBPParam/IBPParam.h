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
#include "Framework/Utility/Unit.h"
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
    Q_OBJECT
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

    QList<IBPScaleInfo> ibpScaleList;                      // all scale for ibp

    struct ChannelData {
        IBPWaveWidget *waveWidget;      /* channel wave */
        IBPTrendWidget *trendWidget;    /* channel trend */

        IBPParamInfo paramData;         /* channel parameter data */
        IBPScaleInfo scale;             /* channel scale */

        bool leadOff;                   // 导联状态
        bool needZero;                  // chnnel need zero flag
        bool lastZeroResult;            // last zero result, true for success
        bool zeroReply;                 // 校零回复

        bool calibReply;                /* got calibrate reply */
        bool lastCalibResult;           /* last calibrate result */
    };

public:
    // 处理demo数据
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);
    /* reimplement */
    virtual void exitDemo();

    // 显示
    virtual void showSubParamValue();

    // 是否超限报警
    void noticeLimitAlarm(int id, bool isAlarm, IBPChannel chn);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList *waveforms,
                                       QStringList *waveformShowName, int);

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // check whether the sub param is avaliabe
    virtual bool isSubParamAvaliable(SubParamID id);

    // 设置数据提供对象。
    void setProvider(IBPProviderIFace *provider);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置测量实时数据。
    void setRealTimeData(short sys, short dia, short map, short pr, IBPChannel chn);

    // 设置波形值。
    void addWaveformData(short wave, bool invalid, IBPChannel chn);

    // 设置界面对象
    void setIBPTrendWidget(IBPTrendWidget *trendWidget, IBPChannel chn);
    void setWaveWidget(IBPWaveWidget *waveWidget, IBPChannel chn);

    /*reimplement*/
    QList<SubParamID> getShortTrendList(IBPChannel chn);

    /**
     * @brief getIBPScale get the IBP scale info
     * @param name
     * @return
     */
    IBPScaleInfo getIBPScale(IBPLabel name);

    // 设置/获取波形标尺限。
    void setRulerLimit(IBPRulerLimit ruler, IBPChannel chn);
    void setRulerLimit(int low, int high, IBPChannel chn);
    IBPRulerLimit getRulerLimit(IBPChannel chn);
    IBPRulerLimit getRulerLimit(IBPLabel name);

    // 设置/获取标尺信息
    void setScaleInfo(const IBPScaleInfo &info, const IBPLabel &name);
    IBPScaleInfo getScaleInfo(IBPChannel chn);

    // 清空校准相关报警
    void clearCalibAlarm(void);

    /**
     * @brief hasIBPZeroReply check whether received ibp zero reply
     * @param chn the channel
     * @return true when got reply
     */
    bool hasIBPZeroReply(IBPChannel chn);

    /**
     * @brief getLastZeroResult get last zero result
     * @param chn the IBP channel
     * @return the result
     */
    bool getLastZeroResult(IBPChannel chn);

    /**
     * @brief hasIBPCalibReply check whether received ibp calibrate reply
     * @param chn the ibp channel
     * @return  true when got reply
     */
    bool hasIBPCalibReply(IBPChannel chn);

    /**
     * @brief getLaseCalibResult get last calibrate result
     * @param chn the IBP channel
     * @return the result
     */
    bool getLaseCalibResult(IBPChannel chn);

    /**
     * @brief channelNeedZero check whether the channel need zero
     * @param chn the channel id
     * @return true when need zero
     */
    bool channelNeedZero(IBPChannel chn) const;

    /**
     * @brief getMoudleType get the connect module type
     * @return the module type
     */
    IBPModuleType getMoudleType() const;

public:
    /**
     * @brief zeroChannel zero channel
     * @param chn the channel
     */
    void zeroChannel(IBPChannel chn);

    // 校准
    void setCalibration(IBPChannel chn, unsigned short value);

    // 校零校准信息
    void setCalibrationInfo(IBPCalibration calib, IBPChannel chn, int calibinfo);

    /**
     * @brief setLeadStatus set the channel lead status
     * @param chn the channel id
     * @param leadOff leadof status
     */
    void setLeadStatus(IBPChannel chn, bool leadOff);

    /**
     * @brief isIBPLeadOff  获取ibp导联脱落状态
     * @param chn  ibp通道
     * @return  导联脱落状态
     */
    bool isIBPLeadOff(IBPChannel chn);

    // 设置/获取波形速度。
    void setSweepSpeed(IBPSweepSpeed speed);
    IBPSweepSpeed getSweepSpeed(void);

    // 设置标名。
    void setEntitle(IBPLabel entitle, IBPChannel chn);

    /**
     * @brief getUnit
     * @return
     */
    UnitType getUnit(void);

    /**
     * @brief setUnit  set unit
     * @param type unit type
     */
    void setUnit(UnitType type);

    // get pressure name
    IBPLabel getEntitle(IBPChannel chn) const;
    IBPLabel getEntitle(IBPLimitAlarmType alarmType) const;

    // 设置滤波
    void setFilter(IBPFilterMode filter);
    IBPFilterMode getFilter(void);

    // 设置灵敏度
    void setSensitivity(IBPSensitivity sensitivity);
    IBPSensitivity getSensitivity(void);

    // 设置获取IBP计算结果值
    IBPParamInfo getParamData(IBPChannel chn);
    void setParamData(IBPChannel chn, unsigned short sys, unsigned short dia,
                      unsigned short mean, unsigned short pr);

    /**
     * @brief getSubParamID 获取ibp两通道标名对应的参数ID。
     * @param inputID
     * @return
     */
    SubParamID getSubParamID(IBPChannel chn);

    // 参数名获取标名
    IBPLabel getPressureName(SubParamID id);
    IBPLabel getPressureName(WaveformID id);
    SubParamID getSubParamID(IBPLabel name);
    WaveformID getWaveformID(IBPLabel name);

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);
private slots:
    void onPaletteChanged(ParamID id);

private:
    IBPParam();
    static IBPParam *_selfObj;
    void _setWaveformSpeed(IBPSweepSpeed speed);

    IBPProviderIFace *_provider;
    bool _connectedProvider;

    ChannelData _chnData[IBP_CHN_NR];
    unsigned _lastPrUpdateTime;
};
#define ibpParam (IBPParam::construction())
