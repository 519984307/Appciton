/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "COParam.h"
#include "COTrendWidget.h"
#include "ConfigManager.h"


class COParamPrivate
{
public:
    COParamPrivate()
        : provider(NULL),
          trendWidget(NULL),
          coVal(NULL),
          ciVal(NULL),
          tbVal(NULL),
          tiVal(NULL),
          connectedProvider(false)
    {}

    COProviderIFace *provider;

    COTrendWidget *trendWidget;

    short coVal;
    short ciVal;
    short tbVal;
    short tiVal;

    bool connectedProvider;
};

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
COParam &COParam::getInstance()
{
    static COParam instance;
    return instance;
}

COParam::~COParam()
{ }

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void COParam::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void COParam::handDemoTrendData()
{
    pimpl->coVal = 30;
    pimpl->ciVal = 15;
    pimpl->tbVal = 370;

    if (pimpl->trendWidget)
    {
        pimpl->trendWidget->setMeasureResult(pimpl->coVal, pimpl->ciVal);
        pimpl->trendWidget->setTBData(pimpl->tbVal);
    }
}

void COParam::exitDemo()
{
    pimpl->coVal = InvData();
    pimpl->ciVal = InvData();
    pimpl->tbVal = InvData();

    if (pimpl->trendWidget)
    {
        pimpl->trendWidget->setMeasureResult(InvData(), InvData());
        pimpl->trendWidget->setTBData(InvData());
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType COParam::getCurrentUnit(SubParamID id)
{
    if (id == SUB_PARAM_CO_CO)
    {
        return UNIT_LPM;
    }
    else if (id == SUB_PARAM_CO_CI)
    {
        return UNIT_LPMPSQM;
    }
    else if (id == SUB_PARAM_CO_TB)
    {
        return UNIT_TDC;
    }
    else
    {
        return UNIT_NONE;
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short COParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_CO_CO:
        return getCOData();
    case SUB_PARAM_CO_CI:
        return getCIData();
    case SUB_PARAM_CO_TB:
        return getTBData();
    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void COParam::setProvider(COProviderIFace *provider)
{
    pimpl->provider = provider;
}

void COParam::setConnected(bool isConnected)
{
    if (pimpl->connectedProvider == isConnected)
    {
        return;
    }
    pimpl->connectedProvider = isConnected;
}

bool COParam::isConnected()
{
    return pimpl->connectedProvider;
}

/**************************************************************************************************
 * 设置趋势对象。
 *************************************************************************************************/
void COParam::setCOTrendWidget(COTrendWidget *trendWidget)
{
    pimpl->trendWidget = trendWidget;
}

/**************************************************************************************************
 * C.O.系数。
 *************************************************************************************************/
void COParam::setCORatio(unsigned short coRatio)
{
    currentConfig.setNumValue("CO|Ratio", (unsigned)coRatio);
    if (pimpl->provider)
    {
        pimpl->provider->setDuctRatio(coRatio);
    }
}

unsigned short COParam::getCORatio()
{
    unsigned ratio = 0;
    currentConfig.getNumValue("CO|Ratio", ratio);
    return ratio;
}

/**************************************************************************************************
 * set source of injection temp.
 *************************************************************************************************/
void COParam::setTempSource(COTiSource source, unsigned short temp)
{
    currentConfig.setNumValue("CO|InjectionTempSource", static_cast<int>(source));
    currentConfig.setNumValue("CO|InjectionTemp", static_cast<unsigned>(temp));
    if (pimpl->provider)
    {
        pimpl->provider->setTiSource(source, temp);
    }
}

COTiSource COParam::getTempSource()
{
    int source = CO_TI_SOURCE_AUTO;
    currentConfig.getNumValue("CO|InjectionTempSource", source);
    return static_cast<COTiSource>(source);
}

unsigned short COParam::getInjectionTemp()
{
    int temp = 0;
    currentConfig.getNumValue("CO|InjectionTemp", temp);
    return temp;
}

/**************************************************************************************************
 * set injection volumn.
 *************************************************************************************************/
void COParam::setInjectionVolumn(unsigned char volumn)
{
    currentConfig.setNumValue("CO|InjectionVolumn", (unsigned)volumn);
    if (pimpl->provider)
    {
        pimpl->provider->setInjectionVolume(volumn);
    }
}

unsigned char COParam::getInjectionVolumn()
{
    int volumn = 0;
    currentConfig.getNumValue("CO|InjectionVolumn", volumn);
    return volumn;
}

/**************************************************************************************************
 * set injection volumn.
 *************************************************************************************************/
void COParam::measureCtrl(COMeasureCtrl ctrl)
{
    if (pimpl->provider)
    {
        pimpl->provider->measureCtrl(ctrl);
    }
}

COMeasureCtrl COParam::getMeasureCtrl()
{
    int ctrl = CO_MEASURE_STOP;
    currentConfig.getNumValue("CO|MeasureMode", ctrl);
    return (COMeasureCtrl)ctrl;
}

/**************************************************************************************************
 * C.O. and C.I. data content.
 *************************************************************************************************/
void COParam::measureResultCO(short coData, short ciData)
{
    pimpl->coVal = coData;
    pimpl->ciVal = ciData;
    if (NULL != pimpl->trendWidget)
    {
        pimpl->trendWidget->setMeasureResult(coData, ciData);
    }

    return;
}

/**************************************************************************************************
 * temp blood data content.
 *************************************************************************************************/
void COParam::realTimeTBData(short tbData)
{
    pimpl->tbVal = tbData;
    if (NULL != pimpl->trendWidget)
    {
        pimpl->trendWidget->setTBData(tbData);
    }

    return;
}

/**************************************************************************************************
 * get C.O. data.
 *************************************************************************************************/
short COParam::getCOData()
{
    return pimpl->coVal;
}

/**************************************************************************************************
 * get C.I. data.
 *************************************************************************************************/
short COParam::getCIData()
{
    return pimpl->ciVal;
}

/**************************************************************************************************
 * get TB data.
 *************************************************************************************************/
short COParam::getTBData()
{
    return pimpl->tbVal;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
COParam::COParam() : Param(PARAM_CO), pimpl(new COParamPrivate())
{
}
