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
        o2Value(InvData()), calibConcentration(InvData()),
        calibResult(false), calibReply(false)
    {}
    O2ProviderIFace *provider;
    O2TrendWidget *trendWidget;
    int16_t o2Value;
    int calibConcentration;
    int calibResult;
    int calibReply;
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

bool O2Param::isServiceProviderOk()
{
    if (d_ptr->provider)
    {
        return true;
    }
    return false;
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

void O2Param::reset()
{
    if (NULL == d_ptr->provider)
    {
        return;
    }
    d_ptr->provider->sendO2SelfTest();
    d_ptr->provider->sendProbeState();
}

void O2Param::sendCalibration(int concentration)
{
    d_ptr->calibConcentration = concentration;
    d_ptr->provider->sendCalibration(concentration);
}

void O2Param::calibrationResult(unsigned char *packet)
{
    if (packet[1] == d_ptr->calibConcentration)
    {
        d_ptr->calibReply = true;
        d_ptr->calibResult = packet[2];
    }
}

bool O2Param::getCalibrationReply()
{
    bool reply = d_ptr->calibReply;
    if (reply)
    {
        d_ptr->calibReply = false;
    }
    return reply;
}

bool O2Param::getCalibrationResult()
{
    return d_ptr->calibResult;
}

O2Param::O2Param() : Param(PARAM_O2), d_ptr(new O2ParamPrivate())
{
}

O2Param::~O2Param()
{
    delete d_ptr;
}
