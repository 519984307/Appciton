#include "COParam.h"
#include "COTrendWidget.h"

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
    _coData = qrand() % 200 + 600;
    _ciData = qrand() % 10 + 30;
    _tbData = qrand() % 100 + 3800;

    if (NULL != _trendWidget)
    {
        _trendWidget->setMeasureResult(_coData, _ciData);
        _trendWidget->setTBData(_tbData);
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
    _provider->setDuctRatio(coRatio);
}

/**************************************************************************************************
 * set source of injection temp.
 *************************************************************************************************/
void COParam::setTempSource(COTiMode source, unsigned short temp)
{
    _provider->setInputMode(source, temp);
}

/**************************************************************************************************
 * set injection volumn.
 *************************************************************************************************/
void COParam::setInjectionVolumn(unsigned char volumn)
{
    _provider->setVolume(volumn);
}

/**************************************************************************************************
 * set injection volumn.
 *************************************************************************************************/
void COParam::measureCtrl(COInstCtl sta)
{
    _provider->measureCtrl(sta);
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
COParam::COParam() : Param(PARAM_CO)
{
    _provider = NULL;
    _trendWidget = NULL;
}
