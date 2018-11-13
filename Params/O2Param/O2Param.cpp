/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#include "O2Param.h"
#include "O2ProviderIFace.h"
#include "O2TrendWidget.h"

class O2ParamPrivate
{
public:
    O2ParamPrivate() : provider(NULL), trendWidget(NULL),
        o2Value(InvData())
    {}

    O2ProviderIFace *provider;
    O2TrendWidget *trendWidget;
    int16_t o2Value;
};

O2Param &O2Param::getInstance()
{
    static O2Param *instance = NULL;
    if (instance == NULL)
    {
        instance = new O2Param();
    }
    return *instance;
}

void O2Param::initParam()
{
}

void O2Param::handDemoTrendData()
{
    d_ptr->o2Value = 25;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

void O2Param::exitDemo()
{
    d_ptr->o2Value = InvData();
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

int16_t O2Param::getSubParamValue(SubParamID id)
{
}

void O2Param::showSubParamValue()
{
}

UnitType O2Param::getCurrentUnit(SubParamID id)
{
    Q_UNUSED(id)
    return UNIT_PERCENT;
}

void O2Param::setProvider(O2ProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }

    d_ptr->provider = provider;
    d_ptr->provider->sendO2SelfTest();
    d_ptr->provider->sendProbeState();
}

void O2Param::setTrendWidget(O2TrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    d_ptr->trendWidget = trendWidget;
}

void O2Param::setO2Concentration(int16_t o2)
{
    if (d_ptr->o2Value == o2)
    {
        return;
    }
    d_ptr->o2Value = o2;
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->setO2Value(d_ptr->o2Value);
    }
}

int16_t O2Param::getO2Concentration()
{
    return d_ptr->o2Value;
}

void O2Param::noticeLimitAlarm(bool flag)
{
    if (NULL != d_ptr->trendWidget)
    {
        d_ptr->trendWidget->isAlarm(flag);
    }
}

O2Param::O2Param() : Param(PARAM_O2), d_ptr(new O2ParamPrivate())
{
}

O2Param::~O2Param()
{
    delete d_ptr;
}
