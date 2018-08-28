/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "CO2Param.h"
#include "CO2Alarm.h"
#include "PatientManager.h"
#include "CO2ProviderIFace.h"
#include "CO2TrendWidget.h"
#include "CO2WaveWidget.h"
#include "IConfig.h"
#include "WaveformCache.h"
#include "RESPParam.h"
#include "ParamManager.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include "RESPDupParam.h"
#include "SoftKeyManager.h"
#include "QApplication"
#include "ComboListPopup.h"
#include "WaveWidgetSelectMenu.h"

CO2Param *CO2Param::_selfObj = NULL;

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void CO2Param::_setWaveformSpeed(CO2SweepSpeed speed)
{
    if (_waveWidget == NULL)
    {
        return;
    }
    switch (speed)
    {
    case CO2_SWEEP_SPEED_62_5:
        _waveWidget->setWaveSpeed(6.25);
        break;

    case CO2_SWEEP_SPEED_125:
        _waveWidget->setWaveSpeed(12.5);
        break;

    case CO2_SWEEP_SPEED_250:
        _waveWidget->setWaveSpeed(25);
        break;

    default:
        break;
    }

    QStringList currentWaveforms;
    windowManager.getCurrentWaveforms(currentWaveforms);
    int i = 0;
    int size = currentWaveforms.size();
    for (; i < size; i++)
    {
        if (currentWaveforms[i] == "CO2WaveWidget")
        {
            windowManager.resetWave();
            break;
        }
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void CO2Param::_setWaveformZoom(CO2DisplayZoom zoom)
{
    if (_provider == NULL)
    {
        return;
    }
    if (_waveWidget == NULL)
    {
        return;
    }
    switch (zoom)
    {
    case CO2_DISPLAY_ZOOM_4:
        _waveWidget->setValueRange(0, (_provider->getCO2MaxWaveform() * 4 + 19) / 20);
        _waveWidget->setRuler(zoom);
        break;

    case CO2_DISPLAY_ZOOM_8: // 0～8/15范围，加14为最大余数，保证完全显示波形。
        _waveWidget->setValueRange(0, (_provider->getCO2MaxWaveform() * 8 + 19) / 20);
        _waveWidget->setRuler(zoom);
        break;

    case CO2_DISPLAY_ZOOM_12:
        _waveWidget->setValueRange(0, (_provider->getCO2MaxWaveform() * 12 + 19) / 20);
        _waveWidget->setRuler(zoom);
        break;

    case CO2_DISPLAY_ZOOM_20:
        _waveWidget->setValueRange(0, _provider->getCO2MaxWaveform());
        _waveWidget->setRuler(zoom);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 获取CO2和RESP的窗体名称。
 *************************************************************************************************/
void CO2Param::_getCO2RESPWins(QString &co2Trend, QString &co2Wave,
                               QString &respTrend, QString &respWave)
{
    // 获取它们的窗体信息。
    getTrendWindow(co2Trend);
    getWaveWindow(co2Wave);
    respParam.getTrendWindow(respTrend);
    respParam.getWaveWindow(respWave);
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void CO2Param::initParam(void)
{
    int onoff = 1;
    currentConfig.getNumValue("CO2|CO2ModeDefault", onoff);
    _co2Switch = (1 == onoff) ? true : false;
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void CO2Param::handDemoWaveform(WaveformID id, short data)
{
    if (id != WAVE_CO2)
    {
        return;
    }
    if (NULL != _waveWidget)
    {
        _waveWidget->addData(data);
    }
    if (NULL != _oxyCRGCo2Widget)
    {
        _oxyCRGCo2Widget->addDataBuf(data, 0);
        _oxyCRGCo2Widget->addData(data, 0, false);
    }
    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void CO2Param::handDemoTrendData(void)
{
//    _etco2Value = qrand() % 80 + 1;
//    _fico2Value = qrand() % 80;
    _etco2Value = 60;
    _fico2Value = 20;
    _brVaule = qrand() % 15 + 15;

    if (NULL != _trendWidget)
    {
        _trendWidget->setEtCO2Value(_etco2Value);
        _trendWidget->setFiCO2Value(_fico2Value);
    }
    setBR(_brVaule);
}

void CO2Param::exitDemo()
{
    _etco2Value = InvData();
    _fico2Value = InvData();
    _brVaule = InvData();

    if (NULL != _trendWidget)
    {
        _trendWidget->setEtCO2Value(InvData());
        _trendWidget->setFiCO2Value(InvData());
    }
    setBR(InvData());
}

/**************************************************************************************************
 * 获取可得的波形控件集。
 *************************************************************************************************/
void CO2Param::getAvailableWaveforms(QStringList &waveforms, QStringList &waveformShowName, int)
{
    waveforms.clear();
    waveformShowName.clear();

    if (NULL != _waveWidget)
    {
        waveforms.append(_waveWidget->name());
        waveformShowName.append(_waveWidget->getTitle());
    }
}

/**************************************************************************************************
 * 获取可得的趋势窗体名。
 *************************************************************************************************/
void CO2Param::getTrendWindow(QString &trendWin)
{
    if (NULL != _trendWidget)
    {
        trendWin = _trendWidget->name();
    }
}

/**************************************************************************************************
 * 获取可得的波形窗体名。
 *************************************************************************************************/
void CO2Param::getWaveWindow(QString &waveWin)
{
    if (NULL != _waveWidget)
    {
        waveWin = _waveWidget->name();
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short CO2Param::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ETCO2:
        return getEtCO2();

    case SUB_PARAM_FICO2:
        return getFiCO2();

    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void CO2Param::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType CO2Param::getCurrentUnit(SubParamID /*id*/)
{
    return getUnit();
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void CO2Param::setProvider(CO2ProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    if (_waveWidget == NULL)
    {
        return;
    }

    _provider = provider;

    QString tile = _waveWidget->getTitle();

    // 请求波形缓冲区。
    waveformCache.registerSource(WAVE_CO2, _provider->getCO2WaveformSample(),
                                 0, _provider->getCO2MaxWaveform(), tile, _provider->getCO2BaseLine());

    // 界面显示部分。
    _waveWidget->setDataRate(_provider->getCO2WaveformSample());
    _setWaveformZoom(getDisplayZoom());
    _oxyCRGCo2Widget->setDataRate(_provider->getCO2WaveformSample());
    // 窒息时间。
    _provider->setApneaTimeout(getApneaTime());

    // 设置N2O补偿。
    _provider->setN2OCompensation(getCompensation(CO2_COMPEN_N2O));

    // 设置O2补偿。
    _provider->setO2Compensation(getCompensation(CO2_COMPEN_O2));
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void CO2Param::reset()
{
    if (NULL == _provider)
    {
        return;
    }

    // 窒息时间。
    _provider->setApneaTimeout(getApneaTime());

    // 设置N2O补偿。
    _provider->setN2OCompensation(getCompensation(CO2_COMPEN_N2O));

    // 设置O2补偿。
    _provider->setO2Compensation(getCompensation(CO2_COMPEN_O2));
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void CO2Param::setTrendWidget(CO2TrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _trendWidget = trendWidget;
    _trendWidget->setFiCO2Display(getFICO2Display());
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void CO2Param::setWaveWidget(CO2WaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }

    _waveWidget = waveWidget;
    _setWaveformSpeed(getSweepSpeed());
    _waveWidget->setWaveformMode(getSweepMode());
}

/**************************************************************************************************
 * 获取EtCO2。
 *************************************************************************************************/
short CO2Param::getEtCO2(void)
{
    return _etco2Value;
}

/**************************************************************************************************
 * 设置EtCO2。
 *************************************************************************************************/
void CO2Param::setEtCO2(short etco2)
{
    if (!(isEnabled() && _co2Switch))
    {
        etco2 = InvData();
    }

    _etco2Value = etco2;
    if (NULL != _trendWidget)
    {
        _trendWidget->setEtCO2Value(etco2);
    }
}

/**************************************************************************************************
 * 获取FiCO2。
 *************************************************************************************************/
short CO2Param::getFiCO2(void)
{
    return _fico2Value;
}

/**************************************************************************************************
 * 设置FiCO2。
 *************************************************************************************************/
void CO2Param::setFiCO2(short fico2)
{
    if (!(isEnabled() && _co2Switch))
    {
        fico2 = InvData();
    }

    _fico2Value = fico2;
    if (NULL != _trendWidget)
    {
        _trendWidget->setFiCO2Value(fico2);
    }
}

/**************************************************************************************************
 * 设置BR。
 *************************************************************************************************/
void CO2Param::setBR(short br)
{
    if (!(isEnabled() && _co2Switch))
    {
        br = InvData();
    }

    respDupParam.updateBR(br);
}

/**************************************************************************************************
 * 获取Baro。
 *************************************************************************************************/
short CO2Param::getBaro(void)
{
    return _baro;
}

/**************************************************************************************************
 * 设置Baro。
 *************************************************************************************************/
void CO2Param::setBaro(short baro)
{
    _baro = baro;
}

/**************************************************************************************************
 * 处理模块和主机窒息时间不一致的问题。
 *************************************************************************************************/
void CO2Param::verifyApneanTime(ApneaAlarmTime time)
{
    if (getApneaTime() == APNEA_ALARM_TIME_OFF)
    {
        return;
    }

    if (getApneaTime() != time)
    {
        if (_provider != NULL)
        {
            _provider->setApneaTimeout(getApneaTime());
        }
    }
}

/**************************************************************************************************
 * 处理模块和主机工作状态不一致的问题。
 *************************************************************************************************/
void CO2Param::verifyWorkMode(CO2WorkMode mode)
{
    if (mode == CO2_WORK_SELFTEST)
    {
        return;
    }

    if (NULL == _provider)
    {
        return;
    }

    // 主机为打开状态，当模块为sleep。
    if (getCO2Switch() && (mode == C02_WORK_SLEEP))
    {
        _provider->setWorkMode(CO2_WORK_MEASUREMENT);
    }

    // 主机为关闭状态，当模块为测量。
    if (!getCO2Switch() && (mode == CO2_WORK_MEASUREMENT))
    {
        _provider->setWorkMode(C02_WORK_SLEEP);
    }
}

/**************************************************************************************************
 * 设置连接，供给对象调用。
 *************************************************************************************************/
void CO2Param::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }

    _connectedProvider = isConnected;
    QStringList curWaveList;
    windowManager.getDisplayedWaveform(curWaveList);

    QString co2Trend, co2Wave, respTrend, respWave;
    _getCO2RESPWins(co2Trend, co2Wave, respTrend, respWave);
    int waveCount = curWaveList.count();

    // 关闭下拉弹出框
    bool hasZoomDialog = false;
    if (ComboListPopup::current())
    {
        hasZoomDialog = true;
    }


    if (isConnected)
    {
        if (_co2Switch)
        {
            this->enable();
            respParam.disable();
            if (-1 != curWaveList.indexOf(respWave))
            {
                windowManager.replaceWaveform(respWave, co2Wave, false, false);
            }
            else if (waveCount < 4)
            {
                windowManager.insertWaveform(curWaveList.at(waveCount - 1), co2Wave, false);
            }

            windowManager.replaceTrendWindow(respTrend, co2Trend);
        }
    }
    else
    {
        disable();
        _baro = 750;
        setEtCO2(InvData());
        setFiCO2(InvData());
        setBR(InvData());
        if (respParam.getRespMonitoring())
        {
            respParam.enableRespCalc(true);
            respParam.enable();
            if (-1 != curWaveList.indexOf(co2Wave))
            {
                windowManager.replaceWaveform(co2Wave, respWave, false, false);
            }
            else if (waveCount < 4)
            {
                windowManager.insertWaveform(curWaveList.at(waveCount - 1), respWave, false);
            }
            windowManager.replaceTrendWindow(co2Trend, respTrend);
        }
        else
        {
            respParam.disable();
            if (-1 != curWaveList.indexOf(co2Wave))
            {
                windowManager.removeWaveform(co2Wave, hasZoomDialog);
            }
            windowManager.removeTrendWindow(co2Trend);
        }

        co2OneShotAlarm.clear();
    }

    curWaveList.clear();

    softkeyManager.refresh();

    // 关闭波形菜单
    waveWidgetSelectMenu.close();
}

/**************************************************************************************************
 * 是否连接了真实的Provider，是指Provider有数据产生。
 *************************************************************************************************/
bool CO2Param::isConnected(void)
{
    return _connectedProvider;
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void CO2Param::addWaveformData(short wave, bool invalid)
{
    int flag = 0;
    if (invalid || !_co2Switch)
    {
        flag = 0x4000;
        wave = 0;
    }

    if (_waveWidget != NULL)
    {
        _oxyCRGCo2Widget->addDataBuf(wave, flag);
        _waveWidget->addData(wave, flag);
    }
    waveformCache.addData(WAVE_CO2, (flag << 16) | (wave & 0xFFFF));
}

/**************************************************************************************************
 * 设置OneShot报警。
 *************************************************************************************************/
void CO2Param::setOneShotAlarm(CO2OneShotType t, bool status)
{
    // 只有当CO2开关为ON状态时才报警
    if (_co2Switch)
    {
        co2OneShotAlarm.setOneShotAlarm(t, status);
    }
}

/**************************************************************************************************
 * 设置呼吸氧和中的CO2窗口波形。
 *************************************************************************************************/
void CO2Param::setOxyCRGCO2Widget(OxyCRGCO2Widget *p)
{
    if (p)
    {
        _oxyCRGCo2Widget = p;
    }
}


/**************************************************************************************************
 * 超限报警通知。
 *************************************************************************************************/
void CO2Param::noticeLimitAlarm(int id, bool flag)
{
    if (NULL != _trendWidget)
    {
        _trendWidget->isAlarm(id, flag);
    }
}

/**************************************************************************************************
 * 校零。
 *************************************************************************************************/
void CO2Param::zeroCalibration(void)
{
    if (_provider != NULL)
    {
        _provider->zeroCalibration();
    }
}

/**************************************************************************************************
 * 设置窒息时间。
 *************************************************************************************************/
void CO2Param::setApneaTime(ApneaAlarmTime t)
{
    if (_provider != NULL)
    {
        _provider->setApneaTimeout(t);
    }
}

/**************************************************************************************************
 * 获取窒息时间。
 *************************************************************************************************/
ApneaAlarmTime CO2Param::getApneaTime(void)
{
    int t = APNEA_ALARM_TIME_20_SEC;
    systemConfig.getNumValue("PrimaryCfg|Alarm|ApneaTime", t);
    return (ApneaAlarmTime)t;
}

/**************************************************************************************************
 * 获取LTA开关状态。
 *************************************************************************************************/
bool CO2Param::getAwRRSwitch(void)
{
    int enable = 0;
    QString path = "AlarmSource|" + patientManager.getTypeStr() + "|";
    path += paramInfo.getSubParamName(SUB_PARAM_RR_BR);
    currentConfig.getNumAttr(path, "Enable", enable);

    return enable;
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void CO2Param::setSweepSpeed(CO2SweepSpeed speed)
{
    currentConfig.setNumValue("CO2|SweepSpeed", static_cast<int>(speed));
    _setWaveformSpeed(speed);
}

/**************************************************************************************************
 * 获取波形速度。
 *************************************************************************************************/
CO2SweepSpeed CO2Param::getSweepSpeed(void)
{
    int speed = CO2_SWEEP_SPEED_62_5;
    currentConfig.getNumValue("CO2|SweepSpeed", speed);
    return (CO2SweepSpeed)speed;
}

/**************************************************************************************************
 * 获取波形模式。
 *************************************************************************************************/
CO2SweepMode CO2Param::getSweepMode(void)
{
    int mode = CO2_SWEEP_MODE_CURVE;
    currentConfig.getNumValue("CO2|CO2SweepMode", mode);
    return (CO2SweepMode)mode;
}

/**************************************************************************************************
 * 获取气体补偿。
 *************************************************************************************************/
void CO2Param::setCompensation(CO2Compensation which, int v)
{
    if (NULL == _provider)
    {
        return;
    }

    QString path("PrimaryCfg|CO2|");
    if (which == CO2_COMPEN_O2)
    {
        path += "O2Compensation";
        _provider->setO2Compensation(v);
    }
    else
    {
        path += "N2OCompensation";
        _provider->setN2OCompensation(v);
    }

    systemConfig.setNumValue(path, v);
}

/**************************************************************************************************
 * 获取气体补偿。
 *************************************************************************************************/
int CO2Param::getCompensation(CO2Compensation which)
{
    QString path("PrimaryCfg|CO2|");
    if (which == CO2_COMPEN_O2)
    {
        path += "O2Compensation";
    }
    else
    {
        path += "N2OCompensation";
    }

    int compensation = 0;
    systemConfig.getNumValue(path, compensation);
    return compensation;
}

/**************************************************************************************************
 * 设置波形放大标尺。
 *************************************************************************************************/
void CO2Param::setDisplayZoom(CO2DisplayZoom zoom)
{
    systemConfig.setNumValue("PrimaryCfg|CO2|DisplayZoom", static_cast<int>(zoom));
    _setWaveformZoom(zoom);
}

/**************************************************************************************************
 * 获取波形放大标尺。
 *************************************************************************************************/
CO2DisplayZoom CO2Param::getDisplayZoom(void)
{
    int zoom = CO2_DISPLAY_ZOOM_4;
    systemConfig.getNumValue("PrimaryCfg|CO2|DisplayZoom", zoom);
    return (CO2DisplayZoom)zoom;
}

/**************************************************************************************************
 * 获取FiCO2显示标志。
 *************************************************************************************************/
void CO2Param::setFiCO2Display(CO2FICO2Display disp)
{
    systemConfig.setNumValue("PrimaryCfg|CO2|FiCO2Display", static_cast<int>(disp));
    if (NULL != _trendWidget)
    {
        _trendWidget->setFiCO2Display(disp);
    }
}

/**************************************************************************************************
 * 获取FiCO2显示标志。
 *************************************************************************************************/
CO2FICO2Display CO2Param::getFICO2Display(void)
{
    int onoff = CO2_FICO2_DISPLAY_OFF;
    systemConfig.getNumValue("PrimaryCfg|CO2|FiCO2Display", onoff);
    return (CO2FICO2Display)onoff;
}

/**************************************************************************************************
 * 获取单位。
 *************************************************************************************************/
UnitType CO2Param::getUnit(void)
{
    return _curUnit;
}

/**************************************************************************************************
 * 设置CO2开关。
 *************************************************************************************************/
void CO2Param::setCO2Switch(int onoff)
{
    if (!onoff)
    {
        setEtCO2(InvData());
        setFiCO2(InvData());
        setBR(InvData());
        setBaro(InvData());
        co2OneShotAlarm.clear();
    }

    currentConfig.setNumValue("CO2|CO2ModeDefault", onoff);
    QStringList curWaveList;
    windowManager.getDisplayedWaveform(curWaveList);
    QString co2Trend, co2Wave, respTrend, respWave;
    _getCO2RESPWins(co2Trend, co2Wave, respTrend, respWave);
    int waveCount = curWaveList.count();

    bool hasZoomDialog = false;
    if (ComboListPopup::current())
    {
        hasZoomDialog = true;
    }

    if (isConnected())
    {
        if (0 == onoff)
        {
            disable();
            if (!respParam.getRespMonitoring())
            {
                respParam.disable();
                if (-1 != curWaveList.indexOf(co2Wave))
                {
                    windowManager.removeWaveform(co2Wave, hasZoomDialog);
                }
                windowManager.removeTrendWindow(co2Trend);
            }
            else
            {
                respParam.enableRespCalc(true);
                respParam.enable();
                if (-1 != curWaveList.indexOf(co2Wave))
                {
                    windowManager.replaceWaveform(co2Wave, respWave, false, false);
                }
                windowManager.replaceTrendWindow(co2Trend, respTrend);
            }
        }
        else
        {
            this->enable();
            respParam.disable();
            if (-1 != curWaveList.indexOf(respWave))
            {
                windowManager.replaceWaveform(respWave, co2Wave, false, false);
            }
            else if (waveCount < 4)
            {
                windowManager.insertWaveform(curWaveList.at(waveCount - 1), co2Wave, false);
            }
            windowManager.replaceTrendWindow(respTrend, co2Trend);
        }
    }

    if (1 == onoff)
    {
        _co2Switch = true;
        if (NULL != _provider)
        {
            _provider->setWorkMode(CO2_WORK_MEASUREMENT);
        }
    }
    else
    {
        _co2Switch = false;
        if (NULL != _provider)
        {
            _provider->setWorkMode(C02_WORK_SLEEP);
        }
    }

    // 关闭下拉弹出框
    if (ComboListPopup::current())
    {
        ComboListPopup::current()->close();
    }

    // 关闭波形菜单
    waveWidgetSelectMenu.close();

    curWaveList.clear();
}

/**************************************************************************************************
 * 获取CO2开关。
 *************************************************************************************************/
bool CO2Param::getCO2Switch()
{
    return _co2Switch;
}

/**************************************************************************************************
 * 获取CO2 Max value。
 *************************************************************************************************/
short CO2Param::getEtCO2MaxValue()
{
    return _etco2MaxVal;
}

/**************************************************************************************************
 * 获取CO2 Min value。
 *************************************************************************************************/
short CO2Param::getEtCO2MinValue()
{
    return _etco2MinVal;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2Param::CO2Param() : Param(PARAM_CO2)
{
//    disable();
    _provider = NULL;
    _trendWidget = NULL;
    _waveWidget = NULL;

    _etco2Value = InvData();
    _fico2Value = InvData();
    _brVaule = InvData();
    _baro = 750;
    _connectedProvider = false;
    _co2Switch = false;

    int t = UNIT_PERCENT;
    currentConfig.getNumValue("Local|CO2Unit", t);
    _curUnit = (UnitType)t;

    QString path = "AlarmSource|" + patientManager.getTypeStr() + "|";
    path += paramInfo.getSubParamName(SUB_PARAM_ETCO2);
    path += "|";
    if (UNIT_PERCENT == t)
    {
        path += Unit::getSymbol(UNIT_PERCENT);
    }
    else if (UNIT_MMHG == t)
    {
        path += Unit::getSymbol(UNIT_MMHG);
    }
    else if (UNIT_KPA)
    {

        path += Unit::getSymbol(UNIT_KPA);
    }
    path += "|";
    QString lowPath = path + "Low";
    QString highPath = path + "High";
    currentConfig.getNumAttr(lowPath, "Min", _etco2MinVal);
    currentConfig.getNumAttr(highPath, "Max", _etco2MaxVal);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2Param::~CO2Param()
{
}
