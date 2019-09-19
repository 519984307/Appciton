/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/28
 **/



#include "RESPDupParam.h"
#include "BaseDefine.h"
#include "ConfigManager.h"
#include "RESPTrendWidget.h"
#include "CO2TrendWidget.h"
#include "Debug.h"
#include "RESPParam.h"
#include "SystemManager.h"
#include "O2ParamInterface.h"
#include "RunningStatusBar.h"
#include "AlarmConfig.h"

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
        if (!alarmConfig.isLimitAlarmEnable(SUB_PARAM_RR_BR))
        {
            _trendWidget->isAlarm(false);
        }
        else
        {
            _trendWidget->isAlarm(_isAlarm);
        }
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

    handleBRRRValue();
    paramUpdateTimer->start(PARAM_UPDATE_TIMEOUT);
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

    handleBRRRValue();
    paramUpdateTimer->start(PARAM_UPDATE_TIMEOUT);
}

/**************************************************************************************************
 * 获取RESP的值。
 *************************************************************************************************/
short RESPDupParam::getRR(void)
{
    if (_isAutoBrSource)
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
    if (_manualBrSourceType == BR_SOURCE_ECG)
    {
        return _rrValue;
    }
    else if (_manualBrSourceType == BR_SOURCE_CO2)
    {
        return _brValue;
    }
    return InvData();
}

/***************************************************************************************************
 * get the br source type
 **************************************************************************************************/
RESPDupParam::BrSourceType RESPDupParam::getBrSource() const
{
    // 自动选择
    if (_isAutoBrSource)
    {
        BrSourceType sourceType;
        if (_brValue == InvData() && _rrValue != InvData())
        {
            sourceType = BR_SOURCE_ECG;
        }
        else
        {
            sourceType = BR_SOURCE_CO2;
        }
        return sourceType;
    }
    // 手动选择
    return _manualBrSourceType;
}

void RESPDupParam::setBrSource(RESPDupParam::BrSourceType type)
{
    _manualBrSourceType = type;
    emit brSourceStatusUpdate();
}

RESPDupParam::ParamSourceType RESPDupParam::getParamSourceType() const
{
    ParamSourceType type;
    if (_brValue != InvData())
    {
        type = BR;
    }
    else
    {
        type = RR;
    }
    return type;
}

void RESPDupParam::setAutoBrSourceStatue(bool isEnabled)
{
    _isAutoBrSource = isEnabled;
    emit brSourceStatusUpdate();
}

bool RESPDupParam::isAutoBrSourceEnabled() const
{
    return _isAutoBrSource;
}

void RESPDupParam::updateSubParamLimit(SubParamID id)
{
    if (id == SUB_PARAM_RR_BR)
    {
        _trendWidget->updateLimit();
    }
}

void RESPDupParam::setRRSource(BRRRSourceType source)
{
    switch (source)
    {
    case BR_RR_AUTO:
        setAutoBrSourceStatue(true);
        break;
    case BR_RR_SOURCE_CO2:
        setAutoBrSourceStatue(false);
        setBrSource(BR_SOURCE_CO2);
        break;
    case BR_RR_SOURCE_ECG:
        setAutoBrSourceStatue(false);
        setBrSource(BR_SOURCE_ECG);
        break;
    default:
        break;
    }
    int index = static_cast<int>(source);
    currentConfig.setNumValue("RESP|BrSource", index);
}

void RESPDupParam::updateRRSource()
{
    int index = 0;
    currentConfig.getNumValue("RESP|BrSource", index);
    BRRRSourceType rrSource = static_cast<BRRRSourceType>(index);
    switch (rrSource)
    {
    case BR_RR_AUTO:
        setAutoBrSourceStatue(true);
        break;
    case BR_RR_SOURCE_CO2:
        setAutoBrSourceStatue(false);
        setBrSource(BR_SOURCE_CO2);
        break;
    case BR_RR_SOURCE_ECG:
        setAutoBrSourceStatue(false);
        setBrSource(BR_SOURCE_ECG);
        break;
    default:
        break;
    }
}

void RESPDupParam::setRespApneaStimulation(bool sta)
{
    bool respApneaStimulation = false;
    currentConfig.getNumValue("ApneaStimulation|RESP", respApneaStimulation);
    if (respApneaStimulation)
    {
        O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
        if (o2Param)
        {
            o2Param->setVibrationReason(APNEASTIMULATION_REASON_RESP, sta);
        }
    }
}

void RESPDupParam::paramUpdateTimeout()
{
    _rrValue = InvData();
    handleBRRRValue();
}

void RESPDupParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_RESP || !systemManager.isSupport(CONFIG_RESP))
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    _trendWidget->updatePalette(pal);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPDupParam::RESPDupParam()
    : Param(PARAM_DUP_RESP),
      _trendWidget(NULL),
      _rrValue(InvData()),
      _brValue(InvData()),
      _isAlarm(false),
      _isAutoBrSource(true),
      _manualBrSourceType(BR_SOURCE_ECG)
{
    updateRRSource();
}

void RESPDupParam::handleBRRRValue()
{
    if (_trendWidget == NULL)
    {
        return;
    }

    if (_isAutoBrSource)
    {
        if (_brValue != InvData())  // set br value firstly when the br value is valid.
        {
            _trendWidget->setRRValue(_brValue, false, true);
        }
        else if (_rrValue != InvData())  // set rr value when the rr value is valid.
        {
            _trendWidget->setRRValue(_rrValue, true, true);
        }
        else  // set br value when the rr value is invalid.
        {
            _trendWidget->setRRValue(_rrValue, true, true);
        }
    }
    else if (_manualBrSourceType == BR_SOURCE_CO2)
    {
        _trendWidget->setRRValue(_brValue, false);
    }
    else if (_manualBrSourceType == BR_SOURCE_ECG)
    {
        _trendWidget->setRRValue(_rrValue, true);
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
    if (o2Param)
    {
        if ((_rrValue > 7 || _rrValue == InvData())
                && (_brValue > 7 || _brValue == InvData()))
        {
            o2Param->setVibrationReason(APNEASTIMULATION_REASON_RESP, false);
        }
    }
#endif
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPDupParam::~RESPDupParam()
{
}


