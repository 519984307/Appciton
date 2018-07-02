#include "RESPParam.h"
#include "RESPAlarm.h"
#include "RESPWaveWidget.h"
#include "RESPProviderIFace.h"
#include "IConfig.h"
#include "WaveformCache.h"
#include "CO2Param.h"
#include "RESPDupParam.h"
#include "SystemManager.h"
#include "WindowManager.h"

RESPParam *RESPParam::_selfObj = NULL;

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

    QStringList currentWaveforms;
    windowManager.getCurrentWaveforms(currentWaveforms);
    int i = 0;
    int size = currentWaveforms.size();
    for (;i < size; i++)
    {
        if (currentWaveforms[i] == "RESPWaveWidget")
        {
            windowManager.resetWave();
            break;
        }
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
    if(NULL != _waveOxyCRGWidget)
    {
        _waveOxyCRGWidget->addDataBuf(data, 0);
        _waveOxyCRGWidget->addData(data,0,false);
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
    _waveOxyCRGWidget->setDataRate(_provider->getRESPWaveformSample());

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
}

void RESPParam::setOxyWaveWidget(OxyCRGRESPWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    _waveOxyCRGWidget = waveWidget;
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
    waveformCache.addData(WAVE_RESP, (flag << 16) | (wave & 0xFFFF));
}

/**************************************************************************************************
 * 设置RESP的值。
 *************************************************************************************************/
void RESPParam::setRR(short rrValue)
{
    respDupParam.updateRR(rrValue);
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

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void RESPParam::setSweepSpeed(RESPSweepSpeed speed)
{
    currentConfig.setNumValue("RESP|SweepSpeed", (int)speed);
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
    systemConfig.setNumValue("PrimaryCfg|RESP|Zoom", (int)zoom);
    if (NULL != _provider)
    {
        _provider->setWaveformZoom(zoom);
    }

    if (NULL != _waveWidget)
    {
        _waveWidget->setZoom((int)zoom);
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

    QStringList waveList;
    windowManager.getDisplayedWaveform(waveList);
    QString co2WaveName, co2TrendName;
    co2Param.getWaveWindow(co2WaveName);
    co2Param.getTrendWindow(co2TrendName);

    currentConfig.setNumValue("RESP|AutoActivation", enable);
    _respMonitoring = enable;
    if (co2Param.isConnected())
    {
        if (0 == co2Param.getCO2Switch())
        {
            if (0 == enable)
            {
                disable();
                if (-1 != waveList.indexOf(name))
                {
                    windowManager.removeWaveform(name, false);
                }
                windowManager.removeTrendWindow(trendName);
            }
            else
            {
                this->enable();
                if (waveList.count() < 4)
                {
                    windowManager.insertWaveform(waveList.at(waveList.count() - 1),
                            name, false);
                }
                windowManager.replaceTrendWindow(co2TrendName, trendName);
            }
        }
    }
    else
    {
        if (0 == enable)
        {
            this->disable();
            if (-1 != waveList.indexOf(name))
            {
                windowManager.removeWaveform(name, false);
            }
            windowManager.removeTrendWindow(trendName);
        }
        else
        {
            this->enable();
            if (waveList.count() < 4)
            {
                windowManager.insertWaveform(waveList.at(waveList.count() - 1),
                        name, false);
            }
            windowManager.insertTrendWindow(trendName);
        }
    }

    waveList.clear();

    enableRespCalc(enable);
}

/**************************************************************************************************
 * 设置呼吸导联。
 *************************************************************************************************/
void RESPParam::setCalcLead(RESPLead lead)
{
    systemConfig.setNumValue("PrimaryCfg|RESP|RespLead", (int)lead);
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
RESPParam::RESPParam() : Param(PARAM_RESP)
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
