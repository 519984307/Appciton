/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#pragma once
#include "Param.h"
#include "O2ParamInterface.h"

class O2ProviderIFace;
class O2TrendWidget;
class O2ParamPrivate;
class O2Param : public Param, public O2ParamInterface
{
    Q_OBJECT
public:
    static O2Param &getInstance();

public:
    // 初始化参数
    virtual void initParam(void);

    //  处理DEMO数据
    virtual void handDemoTrendData(void);
    virtual void exitDemo();

    // 获取子参数值
    virtual int16_t getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象
    void setProvider(O2ProviderIFace *provider);

    /**
     * @brief isServiceProviderOk  服务模式升级数据提供的对象是否存在
     * @return 存在返回true
     */
    bool isServiceProviderOk();

    // 设置界面对象
    void setTrendWidget(O2TrendWidget *trendWidget);

    // 设置/获取O2浓度
    void setO2Concentration(int16_t o2);
    int16_t getO2Concentration(void);

    // 超限报警通知
    void noticeLimitAlarm(bool flag);

    // 模块复位
    void reset();

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);

    /**
     * @brief setApneaAwakeStatus set apnea awake status
     * @param flag
     */
    void setApneaAwakeStatus(bool sta);

    /**
     * @brief getApneaAwakeStatus get apnea awake status
     * @return
     */
    bool getApneaAwakeStatus();

    /**
     * @brief newCreatPatient
     */
    void updateApneaStimulationStatus();
public:
    // 马达控制
    void setVibration(int vibrate);

    // 震动强度设置
    void vibrationIntensityControl(int intensity);

    // 浓度校准
    void sendCalibration(int concentration);

    // 浓度校准结果
    void calibrationResult(unsigned char *packet);

    // 获取校准是否回复
    bool getCalibrationReply();

    // 获取校准结果
    bool getCalibrationResult();

    // 设置唤醒器关联参数状态
    void setVibrationReason(ApneaStimulationReason reason, bool sta);

protected slots:
    virtual void paramUpdateTimeout();

    void updateVibrationTimeout();

    /**
     * @brief onPaletteChanged update the palette
     * @param id param id
     */
    void onPaletteChanged(ParamID id);

private:
    O2Param();
    ~O2Param();

    O2ParamPrivate *d_ptr;
};
#define o2Param O2Param::getInstance()
