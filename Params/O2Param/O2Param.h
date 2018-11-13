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

class O2ProviderIFace;
class O2TrendWidget;
class O2ParamPrivate;
class O2Param : public Param
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

    // 设置界面对象
    void setTrendWidget(O2TrendWidget *trendWidget);

    // 设置/获取O2浓度
    void setO2Concentration(int16_t o2);
    int16_t getO2Concentration(void);

    // 超限报警通知
    void noticeLimitAlarm(bool flag);


private:
    O2Param();
    ~O2Param();

    O2ParamPrivate *d_ptr;
};
#define o2Param O2Param::getInstance()
