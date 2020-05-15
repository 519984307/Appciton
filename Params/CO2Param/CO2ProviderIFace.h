/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/7/16
 **/

#pragma once
#include "CO2Define.h"
#include "AlarmDefine.h"

// 定义ECG数据提供对象需要实现的接口方法。
class CO2ProviderIFace
{
public:
    // 校零。
    virtual void zeroCalibration(void) = 0;

    // 设置窒息超时时间。
    virtual void setApneaTimeout(ApneaAlarmTime t) = 0;

    // 设置N2O补偿。
    virtual void setN2OCompensation(int comp) = 0;

    // 设置O2补偿。
    virtual void setO2Compensation(int comp) = 0;

    // 获取波形的采样速度。
    virtual int getCO2WaveformSample(void) = 0;

    // 获取波形基线
    virtual int getCO2BaseLine(void) = 0;

    // 获取最大的波形值。
    virtual int getCO2MaxWaveform(void) = 0;

	// 设置工作模式
    virtual void setWorkMode(CO2WorkMode mode) = 0;

    // 进入升级模式
    virtual void enterUpgradeMode() = 0;

    // 设置大气压值。
//    virtual bool setBaroPress(int press) = 0;

    virtual void sendCalibrateData(int value) = 0;

    /**
     * @brief getCO2ModuleType get the module type
     * @return the module type
     */
    virtual CO2ModuleType getCo2ModuleType() const = 0;

    virtual ~CO2ProviderIFace() { }
};
