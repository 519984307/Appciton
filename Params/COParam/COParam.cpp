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
#include "COAlarm.h"
#include "AlarmSourceManager.h"
#include <QVector>

/* store 6 measure result at most */
#define MAX_MEASURE_CACHE   6

class COParamPrivate
{
public:
    COParamPrivate()
        : provider(NULL),
          trendWidget(NULL),
          tiSrc(CO_TI_SOURCE_MANUAL),
          coAvgVal(InvData()),
          ciAvgVal(InvData()),
          tbVal(InvData()),
          tiVal(InvData()),
          connectedProvider(false),
          isMeasuring(false)
    {}

    COProviderIFace *provider;

    COTrendWidget *trendWidget;

    COTiSource tiSrc;

    short coAvgVal;
    short ciAvgVal;
    short tbVal;
    short tiVal;

    bool connectedProvider;
    bool isMeasuring;

    COMeasureData curMeasureData;
    QVector<COMeasureData> cacheMeasureData;
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
    pimpl->coAvgVal = 50;
    pimpl->ciAvgVal = 18;
    pimpl->tbVal = 370;

    if (pimpl->trendWidget)
    {
        pimpl->trendWidget->setMeasureResult(pimpl->coAvgVal, pimpl->ciAvgVal);
        pimpl->trendWidget->setTBData(pimpl->tbVal);
    }
}

void COParam::exitDemo()
{
    pimpl->coAvgVal = InvData();
    pimpl->ciAvgVal = InvData();
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
        return getAvgCo();
    case SUB_PARAM_CO_CI:
        return getAvgCi();
    case SUB_PARAM_CO_TB:
        return getTb();
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
    if (provider)
    {
        provider->setCatheterCoeff(getCatheterCoeff());
        provider->setInjectionVolume(getInjectionVolume());
        provider->setTiSource(getTiSource(), getTi());
    }
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

void COParam::setCatheterCoeff(unsigned short coef)
{
    currentConfig.setNumValue("CO|ComputationConst", (unsigned)coef);
    if (pimpl->provider)
    {
        pimpl->provider->setCatheterCoeff(coef);
    }
}

unsigned short COParam::getCatheterCoeff()
{
    unsigned ratio = 0;
    currentConfig.getNumValue("CO|ComputationConst", ratio);
    return ratio;
}

/**************************************************************************************************
 * set source of injection temp.
 *************************************************************************************************/
void COParam::setTiSource(COTiSource source, unsigned short temp)
{
    pimpl->tiSrc = source;

    currentConfig.setNumValue("CO|TISource", static_cast<int>(source));
    if (source == CO_TI_SOURCE_MANUAL)
    {
        currentConfig.setNumValue("CO|InjectateTemp", static_cast<int>(temp));
        pimpl->tiVal = temp;
    }
    else
    {
        pimpl->tiVal = InvData();
    }

    if (pimpl->provider)
    {
        pimpl->provider->setTiSource(source, temp);
    }
}

COTiSource COParam::getTiSource() const
{
    return pimpl->tiSrc;
}

unsigned short COParam::getTi() const
{
    return pimpl->tiVal;
}

unsigned short COParam::getManualTi()
{
    int temp = 20;
    currentConfig.getNumValue("CO|InjectateTemp", temp);
    return temp;
}

/**************************************************************************************************
 * set injection volumn.
 *************************************************************************************************/
void COParam::setInjectionVolume(unsigned char volume)
{
    currentConfig.setNumValue("CO|InjectateVolume", (unsigned)volume);
    if (pimpl->provider)
    {
        pimpl->provider->setInjectionVolume(volume);
    }
}

unsigned char COParam::getInjectionVolume() const
{
    int volumn = 0;
    currentConfig.getNumValue("CO|InjectateVolume", volumn);
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

void COParam::startMeasure()
{
    if (pimpl->isMeasuring)
    {
        /* already start */
        return;
    }
    pimpl->isMeasuring = true;

    if (pimpl->provider)
    {
        pimpl->provider->measureCtrl(CO_MEASURE_START);
    }
}

bool COParam::isMeasuring() const
{
    return pimpl->isMeasuring;
}

void COParam::stopMeasure()
{
    if (!pimpl->isMeasuring)
    {
        /* haven't start yet */
        return;
    }
    pimpl->isMeasuring = false;

    if (pimpl->provider)
    {
        pimpl->provider->measureCtrl(CO_MEASURE_STOP);
    }
}

/**************************************************************************************************
 * C.O. and C.I. data content.
 *************************************************************************************************/
void COParam::setMeasureResult(short co, short ci)
{
    pimpl->curMeasureData.co = co;
    pimpl->curMeasureData.ci = ci;

    /*  Once we got the result, current measurement should be already stopped */
    pimpl->isMeasuring = false;
}

/**************************************************************************************************
 * temp blood data content.
 *************************************************************************************************/
void COParam::setTb(short tb)
{
    pimpl->tbVal = tb;
    if (NULL != pimpl->trendWidget)
    {
        pimpl->trendWidget->setTBData(tb);
    }

    return;
}

void COParam::setTi(short ti)
{
    if (pimpl->tiSrc == CO_TI_SOURCE_AUTO)
    {
        pimpl->tiVal = ti;
    }
}

void COParam::addMeasureWaveData(short data)
{
    pimpl->curMeasureData.measureWave.append(data);
}

void COParam::setOneshotAlarm(COOneShotType t, bool f)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(t, f);
    }
}

/**************************************************************************************************
 * get C.O. data.
 *************************************************************************************************/
short COParam::getAvgCo() const
{
    return pimpl->coAvgVal;
}

/**************************************************************************************************
 * get C.I. data.
 *************************************************************************************************/
short COParam::getAvgCi() const
{
    return pimpl->ciAvgVal;
}

/**************************************************************************************************
 * get TB data.
 *************************************************************************************************/
short COParam::getTb() const
{
    return pimpl->tbVal;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
COParam::COParam() : Param(PARAM_CO), pimpl(new COParamPrivate())
{
    /* update the TI source */
    int source = CO_TI_SOURCE_AUTO;
    currentConfig.getNumValue("CO|TISource", source);
    pimpl->tiSrc = static_cast<COTiSource>(source);

    if (pimpl->tiSrc == CO_TI_SOURCE_MANUAL)
    {
        pimpl->tiVal = getManualTi();
    }
}
