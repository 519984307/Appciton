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

COParam *COParam::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
COParam::~COParam()
{
}

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
    _coData = 400;
    _ciData = 30;
    _tbData = 3800;

    if (NULL != _trendWidget)
    {
        _trendWidget->setMeasureResult(_coData, _ciData);
        _trendWidget->setTBData(_tbData);
    }
}

void COParam::exitDemo()
{
    _coData = InvData();
    _ciData = InvData();
    _tbData = InvData();
    if (NULL != _trendWidget)
    {
        _trendWidget->setMeasureResult(InvData(), InvData());
        _trendWidget->setTBData(InvData());
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
    if (provider == NULL)
    {
        return;
    }
    if (_trendWidget == NULL)
    {
        return;
    }

    _provider = provider;
}

void COParam::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }
    _connectedProvider = isConnected;
}

bool COParam::isConnected()
{
    return _connectedProvider;
}

/**************************************************************************************************
 * 设置趋势对象。
 *************************************************************************************************/
void COParam::setCOTrendWidget(COTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    else
    {
        _trendWidget = trendWidget;
    }
}

/**************************************************************************************************
 * C.O.系数。
 *************************************************************************************************/
void COParam::setCORatio(unsigned short coRatio)
{
    currentConfig.setNumValue("CO|Ratio", (unsigned)coRatio);
    _provider->setDuctRatio(coRatio);
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
void COParam::setTempSource(COTiMode source, unsigned short temp)
{
    currentConfig.setNumValue("CO|InjectionTempSource", static_cast<int>(source));
    currentConfig.setNumValue("CO|InjectionTemp", static_cast<unsigned>(temp));
    _provider->setInputMode(source, temp);
}

COTiMode COParam::getTempSource()
{
    int source = CO_TI_MODE_AUTO;
    currentConfig.getNumValue("CO|InjectionTempSource", source);
    return (COTiMode)source;
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
    _provider->setVolume(volumn);
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
void COParam::measureCtrl(COInstCtl sta)
{
    currentConfig.setNumValue("CO|MeasureMode", static_cast<int>(sta));
    _provider->measureCtrl(sta);
}

COInstCtl COParam::getMeasureCtrl()
{
    int ctrl = CO_INST_START;
    currentConfig.getNumValue("CO|MeasureMode", ctrl);
    return (COInstCtl)ctrl;
}

/**************************************************************************************************
 * C.O. and C.I. data content.
 *************************************************************************************************/
void COParam::measureResultCO(unsigned short coData, unsigned short ciData)
{
    if (NULL != _trendWidget)
    {
        _coData = coData;
        _ciData = ciData;
        _trendWidget->setMeasureResult(coData, ciData);
    }

    return;
}

/**************************************************************************************************
 * temp blood data content.
 *************************************************************************************************/
void COParam::realTimeTBData(unsigned short tbData)
{
    if (NULL != _trendWidget)
    {
        _tbData = tbData;
        _trendWidget->setTBData(tbData);
    }

    return;
}

/**************************************************************************************************
 * get C.O. data.
 *************************************************************************************************/
unsigned short COParam::getCOData()
{
    return _coData;
}

/**************************************************************************************************
 * get C.I. data.
 *************************************************************************************************/
unsigned short COParam::getCIData()
{
    return _ciData;
}

/**************************************************************************************************
 * get TB data.
 *************************************************************************************************/
unsigned short COParam::getTBData()
{
    return _tbData;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
COParam::COParam() : Param(PARAM_CO), _connectedProvider(false)
{
    _provider = NULL;
    _trendWidget = NULL;
    _coData = InvData();
    _ciData = InvData();
    _tbData = InvData();
}
