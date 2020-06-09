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
#include <QScopedPointer>

class COTrendWidget;
class COMenu;
class COParamPrivate;
class COParam : public Param
{
    Q_OBJECT
public:
    static COParam &getInstance();
    ~COParam();

public:
    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, int16_t data);
    virtual void handDemoTrendData(void);
    /* reimplement */
    virtual void exitDemo();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 获取子参数值。
    virtual int16_t getSubParamValue(SubParamID id);

    // 设置数据提供对象。
    void setProvider(COProviderIFace *provider);

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接了真实的Provider，是指Provider有数据产生。
    bool isConnected(void);

    // 设置界面对象
    void setCOTrendWidget(COTrendWidget *trendWidget);

public:
    // C.O.系数
    void setCORatio(u_int16_t coRatio);
    u_int16_t getCORatio(void);

    // set source of injection temp.
    void setTempSource(COTiSource source, u_int16_t temp = 0);
    COTiSource getTempSource(void);
    u_int16_t getInjectionTemp(void);

    // set injection volumn.
    void setInjectionVolumn(unsigned char volumn);
    unsigned char getInjectionVolumn(void);

    // measure control start stop  and interrupt.
    void measureCtrl(COMeasureCtrl ctrl);
    COMeasureCtrl getMeasureCtrl(void);

public:
    // C.O. and C.I. data content.
    void measureResultCO(short coData, short ciData);

    // temp blood data content.
    void realTimeTBData(short tbData);

public:
    // get C.O. data.
    short getCOData(void);

    // get C.I. data.
    short getCIData(void);

    // get TB data.
    short getTBData(void);

private:
    COParam();

    QScopedPointer<COParamPrivate> pimpl;
};

#define coParam (COParam::getInstance())
