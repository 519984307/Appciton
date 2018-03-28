#include "RESPDupParam.h"
#include "BaseDefine.h"
#include "IConfig.h"
#include "RESPTrendWidget.h"
#include "CO2TrendWidget.h"
#include "Debug.h"
#include "RESPParam.h"
#include "SystemManager.h"

RESPDupParam *RESPDupParam::_selfObj = NULL;

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void RESPDupParam::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void RESPDupParam::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
}

/**************************************************************************************************
 * 产生DEMO数据。
 *************************************************************************************************/
void RESPDupParam::handDemoTrendData(void)
{

}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
void RESPDupParam::getAvailableWaveforms(QStringList &/*waveforms*/,
                                         QStringList &/*waveformShowName*/, int /*flag*/)
{

}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short RESPDupParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
        case SUB_PARAM_RR_BR:
            return getRR();

        default:
            return InvData();
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
void RESPDupParam::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 功能： 获取子参数当前单位。
 *************************************************************************************************/
UnitType RESPDupParam::getCurrentUnit(SubParamID /*id*/)
{
    return UNIT_RPM;
}

/**************************************************************************************************
 * 功能： 使能。
 *************************************************************************************************/
bool RESPDupParam::isEnabled()
{
    return respParam.isEnabled();
}

/**************************************************************************************************
 * 获取可得的趋势窗体名。
 *************************************************************************************************/
void RESPDupParam::getTrendWindow(QString &trendWin)
{
    if (NULL != _trendWidget)
    {
        trendWin = _trendWidget->name();
    }
}

/**************************************************************************************************
 * 设置趋势窗体。
 *************************************************************************************************/
void RESPDupParam::setTrendWidget(RESPTrendWidget *trendWidget)
{
    if (NULL != trendWidget)
    {
        _trendWidget = trendWidget;
    }
}

void RESPDupParam::isAlarm(bool isAlarm, bool isLimit)
{
    _isAlarm |= isAlarm;
    if (isLimit)
    {
        return;
    }

    if (NULL != _trendWidget)
    {
        _trendWidget->isAlarm(_isAlarm);
        _isAlarm = false;
    }
}


/**************************************************************************************************
 * 设置RESP的值。
 *************************************************************************************************/
void RESPDupParam::updateRR(short rr)
{
    _rrValue = rr;

    if (_trendWidget == NULL)
    {
        return;
    }

    // BR为有效时即显示。
    if (_brValue != InvData())
    {
        return;
    }

    // 当BR为无效值时才使用RR。
    if (_rrValue != InvData())
    {
        _trendWidget->setRRValue(_rrValue, true);
    }
    else
    {
        _trendWidget->setRRValue(_brValue, false);
    }
}

/**************************************************************************************************
 * 设置RESP的值。
 *************************************************************************************************/
void RESPDupParam::updateBR(short br)
{
    _brValue = br;

    if (_trendWidget == NULL)
    {
        return;
    }

    // BR不为无效时即显示。
    if (_brValue != InvData())
    {
        _trendWidget->setRRValue(_brValue, false);
    }
    else if (_rrValue != InvData())
    {
        _trendWidget->setRRValue(_rrValue, true);
    }
    else // HR和PR都为无效时。
    {
        _trendWidget->setRRValue(_brValue, false);
    }
}

/**************************************************************************************************
 * 获取RESP的值。
 *************************************************************************************************/
short RESPDupParam::getRR(void)
{
    if (InvData() != _brValue)
    {
        return _brValue;
    }

    if (InvData() != _rrValue)
    {
        return _rrValue;
    }

    return InvData();
}

/***************************************************************************************************
 * get the br source type
 **************************************************************************************************/
RESPDupParam::BrSourceType RESPDupParam::getBrSource() const
{
    if(_brValue == InvData() && _rrValue != InvData())
    {
        return BR_SOURCE_RESP;
    }
    else
    {
        return BR_SOURCE_CO2;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPDupParam::RESPDupParam() : Param(PARAM_DUP_RESP)
{
    // 初始化成员。
    _trendWidget = NULL;
    _rrValue = InvData();
    _brValue = InvData();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPDupParam::~RESPDupParam()
{

}


