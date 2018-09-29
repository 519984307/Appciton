/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/29
 **/



#include "ECGDupParam.h"
#include "BaseDefine.h"
#include "IConfig.h"
#include "ECGTrendWidget.h"
#include "Debug.h"
#include "ECGParam.h"
#include "SoundManager.h"

ECGDupParam *ECGDupParam::_selfObj = NULL;

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void ECGDupParam::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void ECGDupParam::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
}

/**************************************************************************************************
 * 产生DEMO数据。
 *************************************************************************************************/
void ECGDupParam::handDemoTrendData(void)
{
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
void ECGDupParam::getAvailableWaveforms(QStringList &/*waveforms*/,
                                        QStringList &/*waveformShowName*/, int /*flag*/)
{
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short ECGDupParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_HR_PR:
        return getHR();

    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void ECGDupParam::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void ECGDupParam::setProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
}


/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType ECGDupParam::getCurrentUnit(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
        return UNIT_MV;
    default:
        break;
    }

    return UNIT_BPM;
}

/**************************************************************************************************
 * 设置趋势窗体。
 *************************************************************************************************/
void ECGDupParam::setTrendWidget(ECGTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _trendWidget = trendWidget;
}

/**************************************************************************************************
 * 更新PR的值。
 *************************************************************************************************/
void ECGDupParam::updatePR(short pr)
{
    _prValue = pr;

    if (_trendWidget == NULL)
    {
        return;
    }

    // HR is valid。
    if (_hrValue != InvData())
    {
        return;
    }

    // 当HR为无效值时才使用PR。
    if (_prValue != InvData())
    {
        _hrBeatFlag = false;
        _trendWidget->setHRValue(_prValue, false);
    }
    else // HR & PR all invalid
    {
        _hrBeatFlag = false;
        _trendWidget->setHRValue(_prValue, true);
    }
}

/**************************************************************************************************
 * 更新VFVT数值。
 *************************************************************************************************/
void ECGDupParam::updateVFVT(bool onoff)
{
    Q_UNUSED(onoff)
    return;
}

/**************************************************************************************************
 * 更新HR心跳图标。
 *************************************************************************************************/
void ECGDupParam::updateHRBeatIcon()
{
    if (_hrBeatFlag)
    {
        if (NULL != _trendWidget && _hrValue != InvData())
        {
            _trendWidget->blinkBeatPixmap();
        }
    }
}

/**************************************************************************************************
 * 更新PR心跳图标。
 *************************************************************************************************/
void ECGDupParam::updatePRBeatIcon()
{
    if (!_hrBeatFlag)
    {
        if (NULL != _trendWidget && _prValue != InvData())
        {
            _trendWidget->blinkBeatPixmap();
        }
    }
}

/**************************************************************************************************
 * 更新HR的值。
 *************************************************************************************************/
void ECGDupParam::updateHR(short hr)
{
    _hrValue = hr;

    if (_trendWidget == NULL)
    {
        return;
    }

    // HR不为无效时即显示。
    if (_hrValue != InvData())
    {
        _hrBeatFlag = true;
        _trendWidget->setHRValue(_hrValue, true);
    }
    else if (_prValue != InvData())
    {
        _hrBeatFlag = false;
        _trendWidget->setHRValue(_prValue, false);
    }
    else // HR和PR都为无效时。
    {
        _hrBeatFlag = true;
        _trendWidget->setHRValue(_hrValue, true);
    }
}

/**************************************************************************************************
 * 获取HR的值。
 *************************************************************************************************/
short ECGDupParam::getHR(void) const
{
    if (InvData() != _hrValue)
    {
        return _hrValue;
    }

    if (InvData() != _prValue)
    {
        return _prValue;
    }

    return InvData();
}

/**************************************************************************************************
 * 是否为HR有效。
 *************************************************************************************************/
bool ECGDupParam::isHRValid(void)
{
    if (_hrValue != InvData())
    {
        return true;
    }

    if (_prValue == InvData())
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGDupParam::isAlarm(bool isAlarm, bool isLimit)
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

/***************************************************************************************************
 * get the hr source
 **************************************************************************************************/
ECGDupParam::HrSourceType ECGDupParam::getHrSource() const
{
    if (_isAutoHrSource)
    {
        HrSourceType type;
        if (_hrValue == InvData() && _prValue != InvData())
        {
            type = HR_SOURCE_SPO2;
        }
        else
        {
            type = HR_SOURCE_ECG;
        }
        return type;
    }

    return _manualHrSource;
}


/**************************************************************************************************
 * 设置计算导联字串。
 *************************************************************************************************/
void ECGDupParam::setECGTrendWidgetCalcName(ECGLead calLead)
{
    _trendWidget->setTrendWidgetCalcName(calLead);
}

void ECGDupParam::setAutoHrSourceStatus(bool isEnabled)
{
    _isAutoHrSource = isEnabled;
}

void ECGDupParam::manualSetHrSource(ECGDupParam::HrSourceType type)
{
    if (_isAutoHrSource)
    {
        return;
    }
    _manualHrSource = type;
}

bool ECGDupParam::isAutoTypeHrSouce() const
{
    return _isAutoHrSource;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGDupParam::ECGDupParam()
    : Param(PARAM_DUP_ECG),
      _provider(NULL),
      _trendWidget(NULL),
      _hrValue(InvData()),
      _prValue(InvData()),
      _hrBeatFlag(true),
      _isAlarm(false),
      _isAutoHrSource(true),
      _manualHrSource(HR_SOURCE_ECG)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGDupParam::~ECGDupParam()
{
}
