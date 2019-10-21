/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn,2018/7/25
 **/

#pragma once
#include "Param.h"
#include "TEMPSymbol.h"

class TEMPTrendWidget;
class TEMPWaveWidget;
class TEMPProviderIFace;
class TEMPParam: public Param
{
    Q_OBJECT
public:
    static TEMPParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TEMPParam();
        }
        return *_selfObj;
    }
    static TEMPParam *_selfObj;
    ~TEMPParam();

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, int16_t data);
    virtual void handDemoTrendData(void);
    /* reimplement */
    virtual void exitDemo();

    // 获取子参数值
    virtual int16_t getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(TEMPProviderIFace *provider);

    // 模块复位
    void reset();

    // 模块错误,模块禁用
    void setErrorDisable(void);
    void setModuleEnable(void);
    bool getErrorDisable(void) { return _isTEMPDisable;}

    // 设置窗口是否禁用显示
    void setWidgetErrorShow(bool error);

    // 通信中断
    bool getDisconnected(void);

    // 设置服务模式升级数据提供对象。
    void setServiceProvider(TEMPProviderIFace *provider);

    /**
     * @brief isServiceProviderOk  服务模式升级数据提供的对象是否存在
     * @return 存在返回true
     */
    bool isServiceProviderOk();

    // 设置界面对象。
    void setTrendWidget(TEMPTrendWidget *trendWidget);

    // 设置/获取TEMP的值。
    void setTEMP(int16_t t1, int16_t t2, int16_t td);
    void getTEMP(int16_t &t1, int16_t &t2, int16_t &td);

    // 设置OneShot报警。
    void setOneShotAlarm(TEMPOneShotType t, bool f);

    // 获取趋势界面。
    virtual void getTrendWindow(QString &trendWin);

    // 超限报警通知
    void noticeLimitAlarm(int id, bool flag);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    //接收定标数据
    void sendCalibrateData(int channel, int value);

    // 进入校准模式
    void enterCalibrateState(void);

    // 退出校准模式
    void exitCalibrateState(void);
    //接收定标数据
    void getCalibrateData(unsigned char *packet);

    // 刷新上下限
    virtual void updateSubParamLimit(SubParamID id);

    // 获取校准是否回复
    bool getCalibrationReply();

    // 获取校准结果
    bool getCalibrationResult();

    // 设置体温校准下的电阻值
    void setOhm(int ohm1, int ohm2);
    void getOhm(int &ohm1, int &ohm2);
public:
    // 设置/获取单位。
    void setUnit(UnitType u);
    UnitType getUnit(SubParamID id = SUB_PARAM_T1);

signals:
    /**
     * @brief updateTempName 更新温度通道名称信号
     * @param channel 通道号
     * @param TEMPChannelType 通道类型
     */
    void updateTempName(TEMPChannelIndex channel, TEMPChannelType type);

    /**
     * @brief tempReset  and the signal of temp reset
     */
    void tempReset();

protected slots:
    virtual void paramUpdateTimeout();

private slots:
    void onPaletteChanged(ParamID id);

private:
    TEMPParam();

    TEMPProviderIFace *_provider;
    TEMPTrendWidget *_trendWidget;

    int16_t _t1Value;
    int16_t _t2Value;
    int16_t _tdValue;

    int _ohm1;
    int _ohm2;

    int _calibrateChannel;
    int _calibrateValue;

    bool _isTEMPDisable;

    bool _calibrationReply;
    bool _calibrationResult;
};
#define tempParam (TEMPParam::construction())
