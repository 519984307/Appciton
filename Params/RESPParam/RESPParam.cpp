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
#include "OxyCRGRESPWaveWidget.h"
#include "ConfigManager.h"
#include "WaveformCache.h"
#include "RESPDupParam.h"
#include "SystemManager.h"
#include "LayoutManager.h"
#include "OxyCRGRRHRWaveWidget.h"
#include "LayoutManager.h"
#include "OxyCRGRESPWaveWidget.h"
#include "RESPWaveWidget.h"
#include "RESPProviderIFace.h"
#include "ColorManager.h"
#include "AlarmSourceManager.h"
#include "PatientManager.h"

RESPParam *RESPParam::_selfObj = NULL;

class RESPParamPrivate
{
public:
    RESPParamPrivate()
        : oxyCRGRESPWave(NULL),
          provider(NULL),
          waveWidget(NULL),
          oxyCRGRrHrTrend(NULL),
          respMonitoring(false),
          connectedProvider(false),
          leadOff(false),
          isEverLeadOn(false),
          isRRInaccurate(false)
    {
    }
    /**
     * @brief setWaveformSpeed
     * @param speed
     */
    void setWaveformSpeed(RESPSweepSpeed speed);

    OxyCRGRESPWaveWidget *oxyCRGRESPWave;
    RESPProviderIFace *provider;
    RESPWaveWidget *waveWidget;
    OxyCRGRRHRWaveWidget *oxyCRGRrHrTrend;
    bool respMonitoring;
    bool connectedProvider;
    bool leadOff;
    bool isEverLeadOn;
    bool isRRInaccurate;
};
/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void RESPParamPrivate::setWaveformSpeed(RESPSweepSpeed speed)
{
    if (waveWidget == NULL)
    {
        return;
    }
    switch (speed)
    {
    case RESP_SWEEP_SPEED_6_25:
        waveWidget->setWaveSpeed(6.25);
        break;

    case RESP_SWEEP_SPEED_12_5:
        waveWidget->setWaveSpeed(12.5);
        break;

    case RESP_SWEEP_SPEED_25_0:
        waveWidget->setWaveSpeed(25.0);
        break;

    case RESP_SWEEP_SPEED_50_0:
        waveWidget->setWaveSpeed(50.0);
        break;

    default:
        break;
    }

    QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();
    if (currentWaveforms.contains(waveWidget->name()))
    {
        layoutManager.resetWave();
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void RESPParam::initParam(void)
{
    setZoom(getZoom());
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        setLeadoff(false);
    }
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

    if (preProcessor)
    {
        int flag = 0;
        int dataInt = data;
        preProcessor->preProcessWave(WAVE_RESP, &dataInt, &flag);
        data = dataInt;
    }

    if (NULL != d_ptr->waveWidget)
    {
        d_ptr->waveWidget->addData(data);
    }

    if (d_ptr->oxyCRGRESPWave)
    {
        d_ptr->oxyCRGRESPWave->addWaveData(data);
    }

    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void RESPParam::handDemoTrendData(void)
{
    int rrValue = 20;
    respDupParam.updateRR(rrValue);

    d_ptr->oxyCRGRrHrTrend->addRrTrendData(rrValue);
}

void RESPParam::exitDemo()
{
    respDupParam.updateRR(InvData());
}

/**************************************************************************************************
 * 获取可得的波形控件集。
 *************************************************************************************************/
void RESPParam::getAvailableWaveforms(QStringList *waveforms,
                                      QStringList *waveformShowName, int /*flag*/)
{
    waveforms->clear();
    waveformShowName->clear();

    if (NULL != d_ptr->waveWidget)
    {
        waveforms->append(d_ptr->waveWidget->name());
        waveformShowName->append(d_ptr->waveWidget->getTitle());
    }
}

/**************************************************************************************************
 * 获取可得的趋势窗体名。
 *************************************************************************************************/
QString RESPParam::getTrendWindowName()
{
    return respDupParam.getTrendWindowName();
}

/**************************************************************************************************
 * 获取可得的波形窗体名。
 *************************************************************************************************/
QString RESPParam::getWaveWindowName()
{
    if (NULL != d_ptr->waveWidget)
    {
        return d_ptr->waveWidget->name();
    }
    return QString();
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
    if (d_ptr->waveWidget == NULL)
    {
        return;
    }

    d_ptr->provider = provider;

    // 是否开启RESP功能
    d_ptr->provider->enableRESPCalc(true);

    d_ptr->waveWidget->setDataRate(d_ptr->provider->getRESPWaveformSample());
    d_ptr->oxyCRGRESPWave->setDataRate(d_ptr->provider->getRESPWaveformSample());

    QString tile = d_ptr->waveWidget->getTitle();
    // 请求波形缓冲区。
    waveformCache.registerSource(WAVE_RESP, d_ptr->provider->getRESPWaveformSample(),
                                 d_ptr->provider->minRESPWaveValue(), d_ptr->provider->maxRESPWaveValue(), tile,
                                 d_ptr->provider->getRESPBaseLine());
}

void RESPParam::setConnected(bool isConnected)
{
    if (d_ptr->connectedProvider == isConnected)
    {
        return;
    }
    d_ptr->connectedProvider = isConnected;
}

bool RESPParam::isConnected()
{
    return d_ptr->connectedProvider;
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
    d_ptr->waveWidget = waveWidget;
    d_ptr->setWaveformSpeed(getSweepSpeed());
    setZoom(getZoom());
    d_ptr->waveWidget->setWaveformMode(getSweepMode());
}

void RESPParam::setOxyCRGWaveRESPWidget(OxyCRGRESPWaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    d_ptr->oxyCRGRESPWave = waveWidget;
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void RESPParam::addWaveformData(int wave, int flag)
{
    if (preProcessor)
    {
        preProcessor->preProcessWave(WAVE_RESP, &wave, &flag);
    }

    if (NULL != d_ptr->waveWidget)
    {
        d_ptr->waveWidget->addData(wave, flag);
    }

    if (d_ptr->oxyCRGRESPWave)
    {
        d_ptr->oxyCRGRESPWave->addWaveData(wave);
    }

    waveformCache.addData(WAVE_RESP, (flag << 16) | (wave & 0xFFFF));
}

/**************************************************************************************************
 * 设置RESP的值。
 *************************************************************************************************/
void RESPParam::setRR(short rrValue)
{
    respDupParam.updateRR(rrValue);
    if (d_ptr->oxyCRGRrHrTrend)
    {
        d_ptr->oxyCRGRrHrTrend->addRrTrendData(rrValue);
    }
}

/**************************************************************************************************
 * 电极脱落。
 *************************************************************************************************/
void RESPParam::setLeadoff(bool flag, bool isFirstConnect)
{
    if (!isFirstConnect)
    {
        d_ptr->leadOff = flag;
        return;
    }
    if (flag != d_ptr->leadOff)
    {
        d_ptr->leadOff = flag;
        if (NULL != d_ptr->waveWidget)
        {
            d_ptr->waveWidget->leadoff(flag);
            setOneShotAlarm(RESP_ONESHOT_ALARM_CHECK_ELECTRODES, flag);
        }
    }
}


/**************************************************************************************************
 * 设置报警。
 *************************************************************************************************/
void RESPParam::setOneShotAlarm(RESPOneShotType t, bool f)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_RESP);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(t, f);
    }
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void RESPParam::reset()
{
    if (NULL == d_ptr->provider)
    {
        return;
    }

    // 是否开启RESP功能
    d_ptr->provider->enableRESPCalc(true);

    // 设置呼吸导联
    d_ptr->provider->setRESPCalcLead(getCalcLead());

    // 设置窒息时间
    d_ptr->provider->setApneaTime(getApneaTime());
}

int RESPParam::getWaveDataRate() const
{
    if (!d_ptr->provider)
    {
        return 0;
    }

    return d_ptr->provider->getRESPWaveformSample();
}

void RESPParam::setOxyCRGRrHrTrend(OxyCRGRRHRWaveWidget *w)
{
    if (!w)
    {
        return;
    }
    d_ptr->oxyCRGRrHrTrend = w;
}

void RESPParam::setSweepMode(RESPSweepMode mode)
{
    currentConfig.setNumValue("RESP|RESPSweepMode", static_cast<int>(mode));
    if (d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setWaveformMode(mode);
    }
}

RESPSweepMode RESPParam::getSweepMode(void)
{
    int mode = RESP_SWEEP_MODE_CURVE;
    currentConfig.getNumValue("RESP|RESPSweepMode", mode);
    return static_cast<RESPSweepMode>(mode);
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void RESPParam::setSweepSpeed(RESPSweepSpeed speed)
{
    currentConfig.setNumValue("RESP|SweepSpeed", static_cast<int>(speed));
    d_ptr->setWaveformSpeed(speed);
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
    if (NULL == d_ptr->provider)
    {
        return;
    }

    d_ptr->provider->setApneaTime(t);
}

/**************************************************************************************************
 * 获取窒息选项。
 *************************************************************************************************/
ApneaAlarmTime RESPParam::getApneaTime(void)
{
    int t = APNEA_ALARM_TIME_20_SEC;
    currentConfig.getNumValue("RESP|ApneaDelay", t);

    return (ApneaAlarmTime)t;
}

/**************************************************************************************************
 * 设置波形的放大倍数。
 *************************************************************************************************/
void RESPParam::setZoom(RESPZoom zoom)
{
    currentConfig.setNumValue("RESP|Gain", static_cast<int>(zoom));
    if (NULL != d_ptr->provider)
    {
        d_ptr->provider->setWaveformZoom(zoom);
    }

    // demo模式下，resp波形增益为定植
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO
        &&  NULL != d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setValueRange(0, 250);
        return;
    }

    if (NULL != d_ptr->waveWidget)
    {
        d_ptr->waveWidget->setZoom(static_cast<int>(zoom));
    }
}

/**************************************************************************************************
 * 获取波形的放大倍数。
 *************************************************************************************************/
RESPZoom RESPParam::getZoom(void)
{
    int zoom = RESP_ZOOM_X100;
    currentConfig.getNumValue("RESP|Gain", zoom);

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

    QString name = getWaveWindowName();
    QString trendName = getTrendWindowName();

    int needUpdate = 0;
    currentConfig.setNumValue("RESP|AutoActivation", enable);
    d_ptr->respMonitoring = enable;
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

int RESPParam::getRespMonitoring()
{
    return d_ptr->respMonitoring;
}

/**************************************************************************************************
 * 设置呼吸导联。
 *************************************************************************************************/
void RESPParam::setCalcLead(RESPLead lead)
{
    RESPLead respLead = lead;
    currentConfig.setNumValue("RESP|BreathLead", static_cast<int>(lead));
    if (NULL != d_ptr->provider)
    {
        if (lead == RESP_LEAD_AUTO)
        {
            lead = RESP_LEAD_II;  // 当为自动导联时，默认选择II导联
        }
        d_ptr->provider->setRESPCalcLead(lead);
    }
    emit calcLeadChanged(respLead);
}

/**************************************************************************************************
 * 获取呼吸导联。
 *************************************************************************************************/
RESPLead RESPParam::getCalcLead(void)
{
    int lead = RESP_LEAD_I;
    currentConfig.getNumValue("RESP|BreathLead", lead);

    return static_cast<RESPLead>(lead);
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

    if (NULL != d_ptr->provider)
    {
        d_ptr->provider->enableRESPCalc(enable);
    }
}

void RESPParam::rrInaccurate(bool isInaccurate)
{
    if (isInaccurate == d_ptr->isRRInaccurate)
    {
        return;
    }
    d_ptr->isRRInaccurate = isInaccurate;
}

bool RESPParam::isRRInaccurate()
{
    return d_ptr->isRRInaccurate;
}

int RESPParam::getRRMeasureMaxRange()
{
    switch (patientManager.getType())
    {
    case PATIENT_TYPE_ADULT:
        return 120;
    case PATIENT_TYPE_NEO:
    case PATIENT_TYPE_PED:
        return 150;
    default:
        break;
    }
    return -1;
}

int RESPParam::getRRMeasureMinRange()
{
    return 0;
}

RESPModuleType RESPParam::getModuleType() const
{
    if (d_ptr->provider)
    {
        return d_ptr->provider->getRespModuleType();
    }
    return MODULE_BLM_E5;
}

void RESPParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_RESP || !systemManager.isSupport(CONFIG_RESP))
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_RESP));
    d_ptr->waveWidget->setPalette(pal);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPParam::RESPParam() : Param(PARAM_RESP),
                         d_ptr(new RESPParamPrivate)
{
    int enable = 1;
    currentConfig.getNumValue("RESP|AutoActivation", enable);
    d_ptr->respMonitoring = enable;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPParam::~RESPParam()
{
    delete d_ptr;
}
