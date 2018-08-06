/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "Param.h"
#include "COProviderIFace.h"

class COTrendWidget;
class COMenu;
class COParam : public Param
{
public:
    static COParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new COParam();
        }
        return *_selfObj;
    }
    ~COParam();

public:
    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, int16_t data);
    virtual void handDemoTrendData(void);

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual int16_t getSubParamValue(SubParamID id);

    // 设置数据提供对象。
    void setProvider(COProviderIFace *provider);

    // 设置界面对象
    void setCOTrendWidget(COTrendWidget *trendWidget);

public:// Data sent by host
    // C.O.系数
    void setCORatio(u_int16_t coRatio);
    u_int16_t getCORatio(void);

    // set source of injection temp.
    void setTempSource(COTiMode source, u_int16_t temp = 0);
    COTiMode getTempSource(void);
    u_int16_t getInjectionTemp(void);

    // set injection volumn.
    void setInjectionVolumn(unsigned char volumn);
    unsigned char getInjectionVolumn(void);

    // measure control start stop  and interrupt.
    void measureCtrl(COInstCtl sta);
    COInstCtl getMeasureCtrl(void);

public:// Data sent by module
    // C.O. and C.I. data content.
    void measureResultCO(u_int16_t coData, u_int16_t ciData);

    // temp blood data content.
    void realTimeTBData(u_int16_t tbData);

public:
    // get C.O. data.
    u_int16_t getCOData(void);

    // get C.I. data.
    u_int16_t getCIData(void);

    // get TB data.
    u_int16_t getTBData(void);


private:
    COParam();
    static COParam *_selfObj;

    COProviderIFace *_provider;

    COTrendWidget *_trendWidget;

    u_int16_t _coData;
    u_int16_t _ciData;
    u_int16_t _tbData;
};
#define coParam (COParam::construction())
