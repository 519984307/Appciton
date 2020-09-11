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
#include "COMeasureWindow.h"
#include "TrendDataStorageManager.h"
#include "Framework/TimeDate/TimeDate.h"
#include "TrendCache.h"
#include <QVector>
#include "IConfig.h"

/* store 6 measure result at most */
#define MAX_MEASURE_CACHE   6

class COParamPrivate
{
public:
    COParamPrivate()
        : provider(NULL),
          trendWidget(NULL),
          measureWin(NULL),
          tiSrc(CO_TI_SOURCE_MANUAL),
          coAvgVal(InvData()),
          ciAvgVal(InvData()),
          tbVal(InvData()),
          tiVal(InvData()),
          connectedProvider(false),
          isMeasuring(false),
          sensorOff(true)
    {}

    COProviderIFace *provider;

    COTrendWidget *trendWidget;
    COMeasureWindow *measureWin;

    COTiSource tiSrc;

    short coAvgVal;
    short ciAvgVal;
    short tbVal;
    short tiVal;

    bool connectedProvider;
    bool isMeasuring;
    bool sensorOff;
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
{
    if (pimpl->measureWin)
    {
        pimpl->measureWin->deleteLater();
    }
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
    pimpl->sensorOff = false;
    // demo value: tb, ti
    pimpl->tbVal = 370;
    // Update the ti value when the ti source is automatic.
    if (pimpl->tiSrc == CO_TI_SOURCE_AUTO)
    {
        pimpl->tiVal = 20;
    }

    if (pimpl->trendWidget)
    {
        pimpl->trendWidget->setTb(pimpl->tbVal);
    }
}

void COParam::exitDemo()
{
    pimpl->sensorOff = true;
    pimpl->coAvgVal = InvData();
    pimpl->ciAvgVal = InvData();
    pimpl->tbVal = InvData();
    // Reset ti value
    if (pimpl->tiSrc == CO_TI_SOURCE_MANUAL)
    {
        pimpl->tiVal = getManualTi();
    }
    else
    {
        pimpl->tiVal = InvData();
    }

    if (pimpl->trendWidget)
    {
        pimpl->trendWidget->setMeasureResult(InvData(), InvData(), 0);
        pimpl->trendWidget->setTb(InvData());
    }

    if (pimpl->measureWin)
    {
        pimpl->measureWin->exitDemo();
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
        return getUnit();
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
        return pimpl->coAvgVal;
    case SUB_PARAM_CO_CI:
        return pimpl->ciAvgVal;
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
    // CO param is disconnected, we set value to invalid.
    if (isConnected)
    {
        pimpl->coAvgVal = InvData();
        pimpl->ciAvgVal = InvData();
        pimpl->tbVal = InvData();
        // Reset ti value
        if (pimpl->tiSrc == CO_TI_SOURCE_MANUAL)
        {
            pimpl->tiVal = getManualTi();
        }
        else
        {
            pimpl->tiVal = InvData();
        }

        if (pimpl->trendWidget)
        {
            pimpl->trendWidget->setMeasureResult(InvData(), InvData(), 0);
            pimpl->trendWidget->setTb(InvData());
        }
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

void COParam::setMeasureWindow(COMeasureWindow *w)
{
    /* delete the old window */
    if (pimpl->measureWin)
    {
        pimpl->measureWin->deleteLater();
    }
    pimpl->measureWin = w;
}

COMeasureWindow *COParam::getMeasureWindow() const
{
    return pimpl->measureWin;
}

void COParam::showSubParamValue()
{
    if (NULL != pimpl->trendWidget)
    {
        pimpl->trendWidget->showValue();
    }
}

void COParam::notifyLimitAlarm(SubParamID id, bool alarm)
{
    if (pimpl->trendWidget == NULL)
    {
        return;
    }

    if (id == SUB_PARAM_CO_TB)
    {
        pimpl->trendWidget->setTbAlarm(alarm);
    }
}

UnitType COParam::getUnit()
{
    int type = UNIT_TC;
    systemConfig.getNumValue("Unit|TemperatureUnit", type);
    return static_cast<UnitType> (type);
}

void COParam::setUnit(UnitType unit)
{
    systemConfig.setNumValue("Unit|TemperatureUnit", static_cast<int>(unit));

    if (pimpl->measureWin)
    {
        pimpl->measureWin->setUnit(unit);
    }
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
        int fahrenheitTi = 0;   // fahrenheit unit ti value
        int celsiusTi = 0;      // celsius unit ti value
        UnitType curUnit = coParam.getUnit();
        UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_CO_TB);
        if (curUnit != defUnit)
        {
            // cur unit is fahrenheit
            fahrenheitTi = static_cast<int>(temp);
            celsiusTi = Unit::convert(defUnit, curUnit, temp * 1.0 / 10).toDouble() * 10;
        }
        else
        {
            // cur unit is celsius
            fahrenheitTi = Unit::convert(UNIT_TF, defUnit, temp * 1.0 / 10).toDouble() * 10;
            celsiusTi = static_cast<int>(temp);
        }
        currentConfig.setNumValue("CO|InjectateTemp|fahrenheit", fahrenheitTi);
        currentConfig.setNumValue("CO|InjectateTemp|celsius", celsiusTi);
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
    QString tiPrefix = QString("CO|InjectateTemp") + Unit::getSymbol(getUnit());
    currentConfig.getNumValue(tiPrefix, temp);
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
    /*  Once we got the result, current measurement should be already stopped */
    pimpl->isMeasuring = false;

    if (pimpl->measureWin)
    {
        pimpl->measureWin->setMeasureResult(co, ci);
    }
}

void COParam::setAverageResult(short co, short ci)
{
    unsigned t = timeDate->time();
    pimpl->coAvgVal = co;
    pimpl->ciAvgVal = ci;
    if (pimpl->trendWidget)
    {
        pimpl->trendWidget->setMeasureResult(co, ci, t);
    }
    /* collect trend data immediately */
    trendCache.collectTrendData(t, true);
    trendDataStorageManager.storeData(t, TrendDataStorageManager::CollectStatusCOResult);

    /* clear the average value after it is collected */
    pimpl->coAvgVal = InvData();
    pimpl->ciAvgVal = InvData();
}

/**************************************************************************************************
 * temp blood data content.
 *************************************************************************************************/
void COParam::setTb(short tb)
{
    pimpl->tbVal = tb;
    if (NULL != pimpl->trendWidget)
    {
        pimpl->trendWidget->setTb(tb);
    }

    if (NULL != pimpl->measureWin)
    {
        pimpl->measureWin->setTb(tb);
    }
}

void COParam::setTi(short ti)
{
    if (pimpl->tiSrc == CO_TI_SOURCE_AUTO)
    {
        UnitType curUnit = getUnit();
        UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_CO_TB);
        if (curUnit != defUnit)
        {
            // cur unit is fahrenheit
            pimpl->tiVal = Unit::convert(curUnit, defUnit, ti * 1.0 / 10).toDouble() * 10;
        }
        else
        {
            // cur unit is celsius
            pimpl->tiVal = ti;
        }
    }
}

void COParam::setOneshotAlarm(COOneShotType t, bool f)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_CO);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(t, f);
    }

    if (pimpl->isMeasuring && pimpl->measureWin && f)
    {
        if (t == CO_ONESHOT_ALARM_MEASURE_TIMEOUT)
        {
            pimpl->measureWin->timeout();
        }
        else if (t == CO_ONESHOT_ALARM_MEASURE_FAIL)
        {
            pimpl->measureWin->fail();
        }
        else if (t == CO_ONESHOT_ALARM_COMMUNICATION_STOP)
        {
            pimpl->measureWin->fail();
        }
    }
}

short COParam::getMeasureWaveRate() const
{
    if (pimpl->provider)
    {
        return pimpl->provider->getMeasureWaveRate();
    }
    return 25;
}

bool COParam::isSensorOff() const
{
    return pimpl->sensorOff;
}

void COParam::setSensorOff(bool off)
{
    pimpl->sensorOff = off;
    if (pimpl->measureWin)
    {
        pimpl->measureWin->setSensorOff(off);
    }
}

void COParam::addMeasureWaveData(short data)
{
    if (pimpl->measureWin)
    {
        pimpl->measureWin->addMeasureWaveData(data);
    }
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
