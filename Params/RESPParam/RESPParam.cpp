/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "RESPParam.h"
#include "RESPAlarm.h"
#include "RESPWaveWidget.h"
#include "RESPProviderIFace.h"
#include "IConfig.h"
#include "WaveformCache.h"
#include "RESPDupParam.h"
#include "SystemManager.h"
#include "LayoutManager.h"
#include "WindowManager.h"
#include "OxyCRGRRHRWaveWidget.h"
#include "LayoutManager.h"
#include "OxyCRGRESPWaveWidget.h"

RESPParam *RESPParam::_selfObj = NULL;

class RESPParamPrivate
{
public:
    RESPParamPrivate()
        : respWave(NULL)
    {
    }
    OxyCRGRESPWaveWidget *respWave;
};
/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void RESPParam::_setWaveformSpeed(RESPSweepSpeed speed)
{
    if (_waveWidget == NULL)
    {
        return;
    }
    switch (speed)
    {
    case RESP_SWEEP_SPEED_6_25:
        _waveWidget->setWaveSpeed(6.25);
        break;

    case RESP_SWEEP_SPEED_12_5:
        _waveWidget->setWaveSpeed(12.5);
        break;

    case RESP_SWEEP_SPEED_25_0:
        _waveWidget->setWaveSpeed(25.0);
        break;

    default:
        break;
    }

    QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();
    if (currentWaveforms.contains(_waveWidget->name()))
    {
        layoutManager.resetWave();
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void RESPParam::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void RESPParam::handDemoWaveform(WaveformID id, short data)
{
    if (id != WAVE_RESP)
    {
        return;
    }
    if (NULL != _waveWidget)
    {
        _waveWidget->addData(data);
    }

    if (d_ptr->respWave)
    {
        d_ptr->respWave->addDataBuf(data, 0);
    }
    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void RESPParam::handDemoTrendData(void)
{
    int rrValue = qrand() % 10 + 20;
    respDupParam.updateRR(rrValue);

    waveRrHrWidget->addRrDataBuf(rrValue, 0);
}

void RESPParam::exitDemo()
{
    respDupParam.updateRR(InvData());
}

/**************************************************************************************************
 * 获取可得的波形控件集。
 *************************************************************************************************/
void RESPParam::getAvailableWaveforms(QStringList &waveforms,
                                      QStringList &waveformShowName, int /*flag*/)
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
void RESPParam::getTrendWindow(QString &trendWin)
{
    respDupParam.getTrendWindow(trendWin);
}

/**************************************************************************************************
 * 获取可得的波形窗体名。
 *************************************************************************************************/
void RESPParam::getWaveWindow(QString &waveWin)
{
    if (NULL != _waveWidget)
    {
        waveWin = _waveWidget->name();
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short RESPParam::getSubParamValue(SubParamID /*id*/)
{
    return InvData();
}

/**************************************************************************************************
 * 功能： 获取子参数当前单位。
 *************************************************************************************************/
UnitType RESPParam::getCurrentUnit(SubParamID /*id*/)
{
    return UNIT_NONE;
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void RESPParam::setProvider(RESPProviderIFace *provider)
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
    _waveWidget->setDataRate(_provider->getRESPWaveformSample());
    d_ptr->respWave->setDataRate(_provider->getRESPWaveformSample());

    // 设置窒息时间
    _provider->setApneaTime(getApneaTime());

    // 设置呼吸导联
//    _provider->setRESPCalcLead(getCalcLead());

    // 是否开启RESP功能
    _provider->enableRESPCalc(getRespMonitoring());

    QString tile = _waveWidget->getTitle();
    // 请求波形缓冲区。
    waveformCache.registerSource(WAVE_RESP, _provider->getRESPWaveformSample(),
                                 _provider->minRESPWaveValue(), _provider->maxRESPWaveValue(), tile,
                                 _provider->getRESPBaseLine());
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void RESPParam::setWaveWidget(RESPWaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    _waveWidget = waveWidget;
    _setWaveformSpeed(getSweepSpeed());
    setZoom(getZoom());
}

void RESPParam::setOxyWaveWidget(OxyCRGRESPWaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    d_ptr->respWave = waveWidget;
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void RESPParam::addWaveformData(int wave, int flag)
{
    if (NULL != _waveWidget)
    {
        _waveWidget->addData(wave, flag);
    }
    if (d_ptr->respWave)
    {
        d_ptr->respWave->addDataBuf(wave, flag);
    }
    waveformCache.addData(WAVE_RESP, (flag << 16) | (wave & 0xFFFF));
}

/**************************************************************************************************
 * 设置RESP的值。
 *************************************************************************************************/
void RESPParam::setRR(short rrValue)
{
    respDupParam.updateRR(rrValue);
     waveRrHrWidget->addRrDataBuf(rrValue, 0);
}

/**************************************************************************************************
 * 电极脱落。
 *************************************************************************************************/
void RESPParam::setLeadoff(bool flag)
{
    if (NULL != _waveWidget)
    {
        _waveWidget->leadoff(flag);
    }
}

/**************************************************************************************************
 * 设置报警。
 *************************************************************************************************/
void RESPParam::setOneShotAlarm(RESPOneShotType t, bool f)
{
    respOneShotAlarm.setOneShotAlarm(t, f);
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void RESPParam::reset()
{
    if (NULL == _provider)
    {
        return;
    }

    // 设置窒息时间
    _provider->setApneaTime(getApneaTime());

    // 设置呼吸导联
//    _provider->setRESPCalcLead(getCalcLead());

    // 是否开启RESP功能
    _provider->enableRESPCalc(getRespMonitoring());
}

int RESPParam::getWaveDataRate() const
{
    if (!_provider)
    {
        return 0;
    }

    return _provider->getRESPWaveformSample();
}

void RESPParam::setOxyWaveRrHrWidget(OxyCRGRRHRWaveWidget *w)
{
    if (!w)
    {
        return;
    }
    waveRrHrWidget = w;
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void RESPParam::setSweepSpeed(RESPSweepSpeed speed)
{
    currentConfig.setNumValue("RESP|SweepSpeed", static_cast<int>(speed));
    _setWaveformSpeed(speed);
}

/**************************************************************************************************
 * 获取波形速度。
 *************************************************************************************************/
RESPSweepSpeed RESPParam::getSweepSpeed(void)
{
    int speed = RESP_SWEEP_SPEED_6_25;
    currentConfig.getNumValue("RESP|SweepSpeed", speed);
    return (RESPSweepSpeed)speed;
}

/**************************************************************************************************
 * 设置窒息选项。
 *************************************************************************************************/
void RESPParam::setApneaTime(ApneaAlarmTime t)
{
    if (NULL == _provider)
    {
        return;
    }

    _provider->setApneaTime(t);
}

/**************************************************************************************************
 * 获取窒息选项。
 *************************************************************************************************/
ApneaAlarmTime RESPParam::getApneaTime(void)
{
    int t = APNEA_ALARM_TIME_20_SEC;
    systemConfig.getNumValue("PrimaryCfg|Alarm|ApneaTime", t);

    return (ApneaAlarmTime)t;
}

/**************************************************************************************************
 * 设置波形的放大倍数。
 *************************************************************************************************/
void RESPParam::setZoom(RESPZoom zoom)
{
    systemConfig.setNumValue("PrimaryCfg|RESP|Zoom", static_cast<int>(zoom));
    if (NULL != _provider)
    {
        _provider->setWaveformZoom(zoom);
    }

    if (NULL != _waveWidget)
    {
        _waveWidget->setZoom(static_cast<int>(zoom));
    }
}

/**************************************************************************************************
 * 获取波形的放大倍数。
 *************************************************************************************************/
RESPZoom RESPParam::getZoom(void)
{
    int zoom = RESP_ZOOM_X100;
    systemConfig.getNumValue("PrimaryCfg|RESP|Zoom", zoom);

    return (RESPZoom)zoom;
}

/**************************************************************************************************
 * 设置呼吸监护功能。
 *************************************************************************************************/
void RESPParam::setRespMonitoring(int enable)
{
    if (enable > 1)
    {
        return;
    }

    if (enable == getRespMonitoring())
    {
        return;
    }

    QString name, trendName;
    getWaveWindow(name);
    getTrendWindow(trendName);


    int needUpdate = 0;
    currentConfig.setNumValue("RESP|AutoActivation", enable);
    _respMonitoring = enable;
    if (0 == enable)
    {
        this->disable();
        needUpdate |= layoutManager.setWidgetLayoutable(name, false);
        needUpdate |= layoutManager.setWidgetLayoutable(trendName, false);
        if (needUpdate)
        {
            layoutManager.updateLayout();
        }
    }
    else
    {
        this->enable();
        needUpdate |= layoutManager.setWidgetLayoutable(name, false);
        needUpdate |= layoutManager.setWidgetLayoutable(trendName, false);
        if (needUpdate)
        {
            layoutManager.updateLayout();
        }
    }

    enableRespCalc(enable);
}

/**************************************************************************************************
 * 设置呼吸导联。
 *************************************************************************************************/
void RESPParam::setCalcLead(RESPLead lead)
{
    systemConfig.setNumValue("PrimaryCfg|RESP|RespLead", static_cast<int>(lead));
    if (NULL != _provider)
    {
        _provider->setRESPCalcLead(lead);
    }
}

/**************************************************************************************************
 * 获取呼吸导联。
 *************************************************************************************************/
RESPLead RESPParam::getCalcLead(void)
{
    int lead = RESP_LEAD_I;
    systemConfig.getNumValue("PrimaryCfg|RESP|RespLead", lead);

    return (RESPLead)lead;
}

/**************************************************************************************************
 * 使能呼吸计算。
 *************************************************************************************************/
void RESPParam::enableRespCalc(bool enable)
{
    if (!enable)
    {
        setRR(InvData());
    }

    if (NULL != _provider)
    {
        _provider->enableRESPCalc(enable);
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPParam::RESPParam() : Param(PARAM_RESP),
                         d_ptr(new RESPParamPrivate)
{
//    disable();
    _provider = NULL;
    _waveWidget = NULL;
    int enable = 1;
    currentConfig.getNumValue("RESP|AutoActivation", enable);
    _respMonitoring = enable;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPParam::~RESPParam()
{
}
