/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/24
 **/

#include "IBPParam.h"
#include "IBPWaveWidget.h"
#include "IBPTrendWidget.h"
#include "TrendTableWindow.h"
#include "WaveformCache.h"
#include "ConfigManager.h"
#include "IBPDefine.h"
#include "LayoutManager.h"
#include "ECGDupParam.h"
#include "IBPAlarm.h"
#include "AlarmSourceManager.h"
#include "IConfig.h"
#include "Framework/TimeDate/TimeDate.h"

IBPParam *IBPParam::_selfObj = NULL;

#define IBP_SCALE_NUM       15
// each pressure name ruler high limit
#define DEFAULT_ART_P1_P2_RULER_HIGH_LIMIT (150)
#define DEFAULT_PA_RULER_HIGH_LIMIT (100)
#define DEFAULT_CVP_RAP_LAP_ICP_RULER_HIGH_LIMIT (40)

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPParam::IBPParam() : Param(PARAM_IBP),  _provider(NULL),  _connectedProvider(false), _lastPrUpdateTime(0)
{
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        _chnData[i].waveWidget = NULL;
        _chnData[i].trendWidget = NULL;
        _chnData[i].leadOff = false;
        _chnData[i].needZero = true;
        _chnData[i].zeroReply = false;
        _chnData[i].lastZeroResult = false;
        _chnData[i].calibReply = false;
        _chnData[i].lastCalibResult = false;
    }

    _chnData[IBP_CHN_1].paramData.pressureName = IBP_LABEL_ART;
    _chnData[IBP_CHN_2].paramData.pressureName = IBP_LABEL_PA;

    ibpScaleList.clear();
    IBPScaleInfo manualScale;
    ibpScaleList.append(manualScale);
    int lowLimit[IBP_SCALE_NUM] = { -10, 0, 0, 0, 0, 0, 0, 60, 30, 0, 0, 0, 0, 0, 0};
    int highLimit[IBP_SCALE_NUM] = {10, 20, 30, 40, 60, 80, 100, 140, 140, 140, 150, 160, 200, 240, 300};

    for (int i = 0; i < IBP_SCALE_NUM; i ++)
    {
        IBPScaleInfo subScale;
        subScale.low = lowLimit[i];
        subScale.high = highLimit[i];
        subScale.isAuto = false;
        ibpScaleList.append(subScale);
    }
    IBPScaleInfo autoScale;
    autoScale.isAuto = true;
    ibpScaleList.append(autoScale);
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void IBPParam::_setWaveformSpeed(IBPSweepSpeed speed)
{
    bool resetWave = false;
    QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        IBPWaveWidget *waveWidget = _chnData[i].waveWidget;
        if (waveWidget == NULL)
        {
            continue;
        }
        switch (speed)
        {
        case IBP_SWEEP_SPEED_62_5:
            waveWidget->setWaveSpeed(6.25);
            break;

        case IBP_SWEEP_SPEED_125:
            waveWidget->setWaveSpeed(12.5);
            break;

        case IBP_SWEEP_SPEED_250:
            waveWidget->setWaveSpeed(25);
            break;

        case IBP_SWEEP_SPEED_500:
            waveWidget->setWaveSpeed(50);
            break;

        default:
            break;
        }

        if (currentWaveforms.contains(waveWidget->name()))
        {
            resetWave = true;
        }
    }

    if (resetWave)
    {
        layoutManager.resetWave();
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
IBPParam::~IBPParam()
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void IBPParam::handDemoWaveform(WaveformID id, short data)
{
    if (id == WAVE_ART)
    {
        /* Need to scale for ART, the demo data is stored in 1 byte, so it's not scaled yet. */
        data *= 10;
    }

    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        WaveformID waveID  = getWaveformID(getEntitle(static_cast<IBPChannel>(i)));
        if (waveID == id)
        {
            IBPWaveWidget *waveWidget = _chnData[i].waveWidget;
            if (waveWidget)
            {
                waveWidget->addData(data);
            }
        }
    }

    waveformCache.addData(id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void IBPParam::handDemoTrendData()
{
    IBPParamInfo &ch1Data = _chnData[IBP_CHN_1].paramData;
    ch1Data.sys = 120;
    ch1Data.dia = 75;
    ch1Data.mean = 90;
    ch1Data.pr = 60;
    IBPTrendWidget *ch1Trend = _chnData[IBP_CHN_1].trendWidget;

    if (ch1Trend)
    {
        ch1Trend->setZeroFlag(true);
        ch1Trend->setData(ch1Data.sys, ch1Data.dia, ch1Data.mean);
    }

    IBPParamInfo &ch2Data = _chnData[IBP_CHN_2].paramData;
    ch2Data.sys = 25;
    ch2Data.dia = 9;
    ch2Data.mean = 14;
    ch2Data.pr = 60;
    IBPTrendWidget *ch2Trend = _chnData[IBP_CHN_2].trendWidget;

    if (ch2Trend)
    {
        ch2Trend->setZeroFlag(true);
        ch2Trend->setData(ch2Data.sys, ch2Data.dia, ch2Data.mean);
    }
}

void IBPParam::exitDemo()
{
    IBPParamInfo &ch1Data = _chnData[IBP_CHN_1].paramData;
    ch1Data.sys = InvData();
    ch1Data.dia = InvData();
    ch1Data.mean = InvData();
    ch1Data.pr = InvData();
    IBPTrendWidget *ch1Trend = _chnData[IBP_CHN_1].trendWidget;
    if (ch1Trend)
    {
        ch1Trend->setData(InvData(), InvData(), InvData());
    }

    IBPParamInfo &ch2Data = _chnData[IBP_CHN_2].paramData;
    ch2Data.sys = InvData();
    ch2Data.dia = InvData();
    ch2Data.mean = InvData();
    ch2Data.pr = InvData();
    IBPTrendWidget *ch2Trend = _chnData[IBP_CHN_2].trendWidget;
    if (ch2Trend)
    {
        ch2Trend->setData(InvData(), InvData(), InvData());
    }
}

void IBPParam::showSubParamValue()
{
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].trendWidget)
        {
            _chnData[i].trendWidget->showValue();
        }
    }
}

void IBPParam::noticeLimitAlarm(int id, bool isAlarm, IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }

    if (_chnData[chn].trendWidget)
    {
        _chnData[chn].trendWidget->isAlarm(id, isAlarm);
    }
}

void IBPParam::getAvailableWaveforms(QStringList *waveforms, QStringList *waveformShowName, int)
{
    waveforms->clear();
    waveformShowName->clear();

    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        IBPWaveWidget *waveWidget = _chnData[i].waveWidget;
        if (waveWidget)
        {
            waveforms->append(waveWidget->name());
            waveformShowName->append(waveWidget->getTitle());
        }
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType IBPParam::getCurrentUnit(SubParamID id)
{
    if (id == SUB_PARAM_ART_PR || id == SUB_PARAM_PA_PR ||
            id == SUB_PARAM_CVP_PR || id == SUB_PARAM_LAP_PR ||
            id == SUB_PARAM_RAP_PR || id == SUB_PARAM_ICP_PR ||
            id == SUB_PARAM_AUXP1_PR || id == SUB_PARAM_AUXP2_PR)
    {
        return UNIT_BPM;
    }
    else
    {
        return getUnit();
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short IBPParam::getSubParamValue(SubParamID id)
{
    enum ValueType {
        SYS,
        DIA,
        MAP,
        PR
    } type;

    IBPLabel label = getPressureName(id);
    switch (id)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP2_SYS:
        type = SYS;
        break;
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP2_DIA:
        type = DIA;
        break;
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP2_MAP:
        type = MAP;
        break;
    case SUB_PARAM_ART_PR:
    case SUB_PARAM_PA_PR:
    case SUB_PARAM_CVP_PR:
    case SUB_PARAM_LAP_PR:
    case SUB_PARAM_RAP_PR:
    case SUB_PARAM_ICP_PR:
    case SUB_PARAM_AUXP1_PR:
    case SUB_PARAM_AUXP2_PR:
        type = PR;
    default:
        return InvData();
    }

    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        IBPParamInfo &data = _chnData[i].paramData;
        if (data.pressureName == label)
        {
            switch (type) {
            case SYS:
                return data.sys;
            case DIA:
                return data.dia;
            case MAP:
                return data.mean;
            case PR:
                return data.pr;
            default:
                break;
            }
        }
    }

    return InvData();
}

bool IBPParam::isSubParamAvaliable(SubParamID id)
{
    IBPLabel label = getPressureName(id);
    if (label >= IBP_LABEL_NR)
    {
        return false;
    }

    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].paramData.pressureName == label)
        {
            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void IBPParam::setProvider(IBPProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].waveWidget == NULL)
        {
            return;
        }
    }

    _provider = provider;

    // 注册波形缓存
    QString title = IBPSymbol::convert(IBP_LABEL_ART);
    waveformCache.registerSource(WAVE_ART, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_PA);
    waveformCache.registerSource(WAVE_PA, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_CVP);
    waveformCache.registerSource(WAVE_CVP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_RAP);
    waveformCache.registerSource(WAVE_RAP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_LAP);
    waveformCache.registerSource(WAVE_LAP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_ICP);
    waveformCache.registerSource(WAVE_ICP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_AUXP1);
    waveformCache.registerSource(WAVE_AUXP1, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_LABEL_AUXP2);
    waveformCache.registerSource(WAVE_AUXP2, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());

    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        _chnData[i].waveWidget->setDataRate(provider->getIBPWaveformSample());
    }

    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].trendWidget)
        {
            _chnData[i].trendWidget->setZeroFlag(!_chnData[i].needZero);
        }
    }
}

void IBPParam::setConnected(bool isConnected)
{
    _connectedProvider = isConnected;
    setLeadStatus(IBP_CHN_1, !isConnected);
    setLeadStatus(IBP_CHN_2, !isConnected);
}

bool IBPParam::isConnected()
{
    return _connectedProvider;
}

/**************************************************************************************************
 * 设置实时数据。
 *************************************************************************************************/
void IBPParam::setRealTimeData(unsigned short sys, unsigned short dia, unsigned short map,
                               unsigned short pr, IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }

    IBPParamInfo &data = _chnData[chn].paramData;
    if (data.sys == sys && data.dia == dia && data.mean == map && data.pr == pr)
    {
        /* nothing changed, do nothing */
        return;
    }

    setParamData(chn, sys, dia, map, pr);

    if (_chnData[chn].trendWidget)
    {
        _chnData[chn].trendWidget->setData(sys, dia, map);
    }
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void IBPParam::addWaveformData(short wave, bool invalid, IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }

    /* NOTE: The wave data's unit should be 0.1 mmhg */
    int flag = 0;

    if (invalid)
    {
        flag = 0x4000;
        wave = 0;
    }

    IBPWaveWidget *waveWidget = _chnData[chn].waveWidget;
    WaveformID waveID = getWaveformID(getEntitle(chn));
    if (waveWidget)
    {
        waveWidget->addWaveformData(wave, flag);
    }
    waveformCache.addData(waveID, (flag << 16) | wave);
}

/**************************************************************************************************
 * 设置趋势对象。
 *************************************************************************************************/
void IBPParam::setIBPTrendWidget(IBPTrendWidget *trendWidget, IBPChannel chn)
{
    if (trendWidget == NULL)
    {
        return;
    }
    if (chn >= IBP_CHN_NR)
    {
        return;
    }
    _chnData[chn].trendWidget = trendWidget;
    if (trendWidget)
    {
        trendWidget->setZeroFlag(!_chnData[chn].needZero);
    }
}

/**************************************************************************************************
 * 设置波形对象。
 *************************************************************************************************/
void IBPParam::setWaveWidget(IBPWaveWidget *waveWidget, IBPChannel chn)
{
    if (waveWidget == NULL || chn >= IBP_CHN_NR)
    {
        return;
    }

    _chnData[chn].waveWidget = waveWidget;

    waveWidget->setLimit(getIBPScale(waveWidget->getEntitle()).low, getIBPScale(waveWidget->getEntitle()).high);
    _setWaveformSpeed(getSweepSpeed());
}

QList<SubParamID> IBPParam::getShortTrendList(IBPChannel chn)
{
    QList<SubParamID> paraList;
    if (chn < IBP_CHN_NR)
    {
        if (_chnData[chn].trendWidget)
        {
            paraList = _chnData[chn].trendWidget->getShortTrendSubParams();
        }
    }
    return paraList;
}

IBPScaleInfo IBPParam::getIBPScale(IBPLabel name)
{
    IBPScaleInfo info;
    int highLimit = 0;
    switch (name)
    {
    case IBP_LABEL_ART:
    case IBP_LABEL_AUXP1:
    case IBP_LABEL_AUXP2:
        highLimit = DEFAULT_ART_P1_P2_RULER_HIGH_LIMIT;
        break;
    case IBP_LABEL_PA:
        highLimit = DEFAULT_PA_RULER_HIGH_LIMIT;
        break;
    case IBP_LABEL_CVP:
    case IBP_LABEL_LAP:
    case IBP_LABEL_RAP:
    case IBP_LABEL_ICP:
        highLimit = DEFAULT_CVP_RAP_LAP_ICP_RULER_HIGH_LIMIT;
        break;
    default:
        break;
    }

    for (QList<IBPScaleInfo>::iterator it = ibpScaleList.begin(); it != ibpScaleList.end(); ++it)
    {
        if ((*it).high == highLimit)
        {
            info = (*it);
            break;
        }
    }
    return info;
}

void IBPParam::setRulerLimit(IBPRulerLimit ruler, IBPChannel chn)
{
    if (chn == IBP_CHN_1)
    {
        currentConfig.setNumValue("IBP|RulerLimit1", static_cast<int>(ruler));
        if (_chnData[IBP_CHN_1].waveWidget != NULL)
        {
            _chnData[IBP_CHN_1].waveWidget->setRulerLimit(ruler);
        }
    }
    else if (chn == IBP_CHN_2)
    {
        currentConfig.setNumValue("IBP|RulerLimit2", static_cast<int>(ruler));
        if (_chnData[IBP_CHN_2].waveWidget != NULL)
        {
            _chnData[IBP_CHN_2].waveWidget->setRulerLimit(ruler);
        }
    }
}

void IBPParam::setRulerLimit(int low, int high, IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }

    IBPWaveWidget *waveWidget = _chnData[chn].waveWidget;
    if (waveWidget)
    {
        waveWidget->setLimit(low, high);
    }
}

IBPRulerLimit IBPParam::getRulerLimit(IBPChannel chn)
{
    int ruler = IBP_RULER_LIMIT_0_160;
    if (chn == IBP_CHN_1)
    {
        currentConfig.getNumValue("IBP|RulerLimit1", ruler);
    }
    else if (chn == IBP_CHN_2)
    {
        currentConfig.getNumValue("IBP|RulerLimit2", ruler);
    }
    return static_cast<IBPRulerLimit>(ruler);
}

IBPRulerLimit IBPParam::getRulerLimit(IBPLabel name)
{
    IBPRulerLimit ruler = IBP_RULER_LIMIT_0_160;
    switch (name)
    {
    case IBP_LABEL_ART:
    case IBP_LABEL_AUXP1:
    case IBP_LABEL_AUXP2:
        ruler = IBP_RULER_LIMIT_0_150;
        break;
    case IBP_LABEL_PA:
        ruler = IBP_RULER_LIMIT_0_100;
        break;
    case IBP_LABEL_CVP:
    case IBP_LABEL_LAP:
    case IBP_LABEL_RAP:
    case IBP_LABEL_ICP:
        ruler = IBP_RULER_LIMIT_0_40;
        break;
    default:
        break;
    }
    return ruler;
}

void IBPParam::setScaleInfo(const IBPScaleInfo &info, const IBPLabel &name)
{
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].paramData.pressureName == name)
        {
            _chnData[i].scale = info;
        }
    }
}

IBPScaleInfo IBPParam::getScaleInfo(IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return IBPScaleInfo();
    }

    return _chnData[chn].scale;
}

/**************************************************************************************************
 * 设置校零。
 *************************************************************************************************/
void IBPParam::zeroChannel(IBPChannel chn)
{
    clearCalibAlarm();
    unsigned zeroTime = timeDate->time();
    unsigned int year = timeDate->getDateYear(zeroTime) - 2000;
    unsigned int month = timeDate->getDateMonth(zeroTime);
    unsigned int day = timeDate->getDateDay(zeroTime);
    unsigned int hour = timeDate->getTimeHour(zeroTime);
    unsigned int min = timeDate->getTimeMinute(zeroTime);
    unsigned int second = timeDate->getTimeSeconds(zeroTime);
    _provider->setTimeZero(chn, IBP_CALIBRATION_ZERO,
                           (unsigned int)second, (unsigned int)min,
                           (unsigned int)hour, (unsigned int)day,
                           (unsigned int)month, (unsigned int)year);
    _provider->setZero(chn, IBP_CALIBRATION_ZERO, 0x00);
    _chnData[chn].lastZeroResult = false;
}

/**************************************************************************************************
 * 设置校准。
 *************************************************************************************************/
void IBPParam::setCalibration(IBPChannel chn, unsigned short value)
{
    clearCalibAlarm();
    _provider->setZero(chn, IBP_CALIBRATION_SET, value);
    _chnData[chn].lastCalibResult = false;
}

/**************************************************************************************************
 * 校零校准信息。
 *************************************************************************************************/
void IBPParam::setCalibrationInfo(IBPCalibration calib, IBPChannel chn, int calibinfo)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource == NULL)
    {
        return;
    }

    if (calib == IBP_CALIBRATION_ZERO)
    {
        if (chn == IBP_CHN_1)
        {
            _chnData[chn].zeroReply = true;
            _chnData[chn].needZero = false;
            switch (static_cast<IBPZeroResult>(calibinfo))
            {
            case IBP_ZERO_SUCCESS:
            {
                alarmSource->setOneShotAlarm(IBP1_ZERO_SUCCESS, true);
                _chnData[chn].lastZeroResult = true;
                if (_chnData[chn].trendWidget)
                {
                    _chnData[chn].trendWidget->setZeroFlag(true);
                }
                break;
            }
            case IBP_ZERO_IS_PULSE:
            {
                alarmSource->setOneShotAlarm(IBP1_ZERO_IS_PULSE, true);
                break;
            }
            case IBP_ZERO_BEYOND_RANGE:
            {
                alarmSource->setOneShotAlarm(IBP1_ZERO_BEYOND_RANGE, true);
                break;
            }
            case IBP_ZERO_FAIL:
            {
                alarmSource->setOneShotAlarm(IBP1_ZERO_FAIL, true);
                break;
            }
            case IBP_ZERO_NOT_SET_TIME:
            {
                alarmSource->setOneShotAlarm(IBP1_ZERO_NOT_SET_TIME, true);
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if (chn == IBP_CHN_2)
        {
            _chnData[chn].zeroReply = true;
            _chnData[chn].needZero = false;
            switch (static_cast<IBPZeroResult>(calibinfo))
            {
            case IBP_ZERO_SUCCESS:
            {
                alarmSource->setOneShotAlarm(IBP2_ZERO_SUCCESS, true);
                _chnData[chn].lastZeroResult = true;
                if (_chnData[chn].trendWidget)
                {
                    _chnData[chn].trendWidget->setZeroFlag(true);
                }
                break;
            }
            case IBP_ZERO_IS_PULSE:
            {
                alarmSource->setOneShotAlarm(IBP2_ZERO_IS_PULSE, true);
                break;
            }
            case IBP_ZERO_BEYOND_RANGE:
            {
                alarmSource->setOneShotAlarm(IBP2_ZERO_BEYOND_RANGE, true);
                break;
            }
            case IBP_ZERO_FAIL:
            {
                alarmSource->setOneShotAlarm(IBP2_ZERO_FAIL, true);
                break;
            }
            case IBP_ZERO_NOT_SET_TIME:
            {
                alarmSource->setOneShotAlarm(IBP2_ZERO_NOT_SET_TIME, true);
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else
        {
            return;
        }
    }
    else if (calib == IBP_CALIBRATION_SET)
    {
        if (chn == IBP_CHN_1)
        {
            _chnData[chn].calibReply = true;
            switch (static_cast<IBPCalibrationResult>(calibinfo))
            {
            case IBP_CALIBRATION_SUCCESS:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_SUCCESS, true);
                _chnData[chn].lastCalibResult = true;
                break;
            }
            case IBP_CALIBRATION_IS_PULSE:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_IS_PULSE, true);
                break;
            }
            case IBP_CALIBRATION_BEYOND_RANGE:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_BEYOND_RANGE, true);
                break;
            }
            case IBP_CALIBRATION_NOT_ZERO:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_NOT_ZERO, true);
                break;
            }
            case IBP_CALIBRATION_FAIL:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_FAIL, true);
                break;
            }
            case IBP_CALIBRATION_NOT_SET_TIME:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_NOT_SET_TIME, true);
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if (chn == IBP_CHN_2)
        {
            _chnData[chn].calibReply = true;
            switch (static_cast<IBPCalibrationResult>(calibinfo))
            {
            case IBP_CALIBRATION_SUCCESS:
            {
                _chnData[chn].lastCalibResult = true;
                alarmSource->setOneShotAlarm(IBP2_CALIB_SUCCESS, true);
                break;
            }
            case IBP_CALIBRATION_IS_PULSE:
            {
                alarmSource->setOneShotAlarm(IBP2_CALIB_IS_PULSE, true);
                break;
            }
            case IBP_CALIBRATION_BEYOND_RANGE:
            {
                alarmSource->setOneShotAlarm(IBP2_CALIB_BEYOND_RANGE, true);
                break;
            }
            case IBP_CALIBRATION_NOT_ZERO:
            {
                alarmSource->setOneShotAlarm(IBP2_CALIB_NOT_ZERO, true);
                break;
            }
            case IBP_CALIBRATION_FAIL:
            {
                alarmSource->setOneShotAlarm(IBP2_CALIB_FAIL, true);
                break;
            }
            case IBP_CALIBRATION_NOT_SET_TIME:
            {
                alarmSource->setOneShotAlarm(IBP2_CALIB_NOT_SET_TIME, true);
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else
        {
            return;
        }
    }
    else
    {
        return;
    }
}

void IBPParam::setLeadStatus(IBPChannel chn, bool leadOff)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }
    if (_chnData[chn].leadOff != leadOff)
    {
        _chnData[chn].leadOff = leadOff;
        _chnData[chn].waveWidget->setLeadSta(leadOff);
        if (leadOff)
        {
            /* after leadoff, need to zero again, set zero flag to false */
            _chnData[chn].trendWidget->setZeroFlag(false);
            _chnData[chn].needZero = true;
        }
    }
}

bool IBPParam::isIBPLeadOff(IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return true;
    }

    return _chnData[chn].leadOff;
}

/**************************************************************************************************
 * 设置扫描速度。
 *************************************************************************************************/
void IBPParam::setSweepSpeed(IBPSweepSpeed speed)
{
    currentConfig.setNumValue("IBP|SweepSpeed", static_cast<int>(speed));
    _setWaveformSpeed(speed);
}

IBPSweepSpeed IBPParam::getSweepSpeed()
{
    int speed = IBP_SWEEP_SPEED_62_5;
    currentConfig.getNumValue("IBP|SweepSpeed", speed);
    return (IBPSweepSpeed)speed;
}

/**************************************************************************************************
 * 设置标名。
 *************************************************************************************************/
void IBPParam::setEntitle(IBPLabel entitle, IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }
    // Save pressure label
    if (chn == IBP_CHN_1)
    {
        currentConfig.setNumValue("IBP|ChannelPressureEntitle1", static_cast<int> (entitle));
    }
    else if (chn == IBP_CHN_2)
    {
        currentConfig.setNumValue("IBP|ChannelPressureEntitle2", static_cast<int> (entitle));
    }
    _chnData[chn].waveWidget->setEntitle(entitle);
    _chnData[chn].trendWidget->setEntitle(entitle);
    _chnData[chn].trendWidget->updateLimit();
    _chnData[chn].paramData.pressureName = entitle;

    IBPMeasueType measureType[IBP_CHN_NR];
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].paramData.pressureName >= IBP_LABEL_CVP
                && _chnData[i].paramData.pressureName <= IBP_LABEL_ICP)
        {
            measureType[i] = IBP_MEASURE_MAP;
        }
        else
        {
            measureType[i] = IBP_MEASURE_SYS_DIA_MAP;
        }
    }

    _provider->setIndicate(_chnData[IBP_CHN_1].paramData.pressureName,
                           _chnData[IBP_CHN_2].paramData.pressureName,
                           measureType[IBP_CHN_1],
                           measureType[IBP_CHN_2]);
}

UnitType IBPParam::getUnit()
{
    int unit = UNIT_MMHG;
    systemConfig.getNumValue("Unit|IBPUnit", unit);
    return (UnitType)unit;
}

void IBPParam::setUnit(UnitType type)
{
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (_chnData[i].trendWidget)
        {
            _chnData[i].trendWidget->updateUnit(type);
            _chnData[i].trendWidget->updateLimit();
            IBPParamInfo data = _chnData[i].paramData;
            _chnData[i].trendWidget->setData(data.sys, data.dia, data.mean);
        }
    }
}

IBPLabel IBPParam::getEntitle(IBPChannel chn) const
{
    // Get the pressure name from the current configuration
    int entitle = IBP_LABEL_NR;
    if (chn == IBP_CHN_1)
    {
        currentConfig.getNumValue("IBP|ChannelPressureEntitle1", entitle);
    }
    else if (chn == IBP_CHN_2)
    {
        currentConfig.getNumValue("IBP|ChannelPressureEntitle2", entitle);
    }
    return static_cast<IBPLabel>(entitle);
}

IBPLabel IBPParam::getEntitle(IBPLimitAlarmType alarmType) const
{
    switch (alarmType)
    {
    case ART_LIMIT_ALARM_SYS_LOW:
    case ART_LIMIT_ALARM_SYS_HIGH:
    case ART_LIMIT_ALARM_DIA_LOW:
    case ART_LIMIT_ALARM_DIA_HIGH:
    case ART_LIMIT_ALARM_MEAN_LOW:
    case ART_LIMIT_ALARM_MEAN_HIGH:
    case ART_LIMIT_ALARM_PR_LOW:
    case ART_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_ART;
    case PA_LIMIT_ALARM_SYS_LOW:
    case PA_LIMIT_ALARM_SYS_HIGH:
    case PA_LIMIT_ALARM_DIA_LOW:
    case PA_LIMIT_ALARM_DIA_HIGH:
    case PA_LIMIT_ALARM_MEAN_LOW:
    case PA_LIMIT_ALARM_MEAN_HIGH:
    case PA_LIMIT_ALARM_PR_LOW:
    case PA_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_PA;
    case CVP_LIMIT_ALARM_MEAN_LOW:
    case CVP_LIMIT_ALARM_MEAN_HIGH:
    case CVP_LIMIT_ALARM_PR_LOW:
    case CVP_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_CVP;
    case LAP_LIMIT_ALARM_MEAN_LOW:
    case LAP_LIMIT_ALARM_MEAN_HIGH:
    case LAP_LIMIT_ALARM_PR_LOW:
    case LAP_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_LAP;
    case RAP_LIMIT_ALARM_MEAN_LOW:
    case RAP_LIMIT_ALARM_MEAN_HIGH:
    case RAP_LIMIT_ALARM_PR_LOW:
    case RAP_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_RAP;
    case ICP_LIMIT_ALARM_MEAN_LOW:
    case ICP_LIMIT_ALARM_MEAN_HIGH:
    case ICP_LIMIT_ALARM_PR_LOW:
    case ICP_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_ICP;
    case AUXP1_LIMIT_ALARM_SYS_LOW:
    case AUXP1_LIMIT_ALARM_SYS_HIGH:
    case AUXP1_LIMIT_ALARM_DIA_LOW:
    case AUXP1_LIMIT_ALARM_DIA_HIGH:
    case AUXP1_LIMIT_ALARM_MEAN_LOW:
    case AUXP1_LIMIT_ALARM_MEAN_HIGH:
    case AUXP1_LIMIT_ALARM_PR_LOW:
    case AUXP1_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_AUXP1;
    case AUXP2_LIMIT_ALARM_SYS_LOW:
    case AUXP2_LIMIT_ALARM_SYS_HIGH:
    case AUXP2_LIMIT_ALARM_DIA_LOW:
    case AUXP2_LIMIT_ALARM_DIA_HIGH:
    case AUXP2_LIMIT_ALARM_MEAN_LOW:
    case AUXP2_LIMIT_ALARM_MEAN_HIGH:
    case AUXP2_LIMIT_ALARM_PR_LOW:
    case AUXP2_LIMIT_ALARM_PR_HIGH:
        return IBP_LABEL_AUXP2;
    default:
        return IBP_LABEL_NR;
    }
}

/**************************************************************************************************
 * 设置滤波。
 *************************************************************************************************/
void IBPParam::setFilter(IBPFilterMode filter)
{
    currentConfig.setNumValue("IBP|FilterMode", static_cast<int>(filter));
    _provider->setFilter(IBP_CHN_1, filter);
    _provider->setFilter(IBP_CHN_2, filter);
}

IBPFilterMode IBPParam::getFilter()
{
    int filter = IBP_FILTER_MODE_0;
    currentConfig.getNumValue("IBP|FilterMode", filter);
    return (IBPFilterMode)filter;
}

/**************************************************************************************************
 * 设置灵敏度。
 *************************************************************************************************/
void IBPParam::setSensitivity(IBPSensitivity sensitivity)
{
    currentConfig.setNumValue("IBP|Sensitivity", static_cast<int>(sensitivity));
    unsigned char time = IBPSymbol::convertNumber(sensitivity);
    _provider->setAvergTime(IBP_CHN_1, time);
    _provider->setAvergTime(IBP_CHN_2, time);
}

IBPSensitivity IBPParam::getSensitivity()
{
    int sens = IBP_SENSITIVITY_MID;
    currentConfig.getNumValue("IBP|Sensitivity", sens);
    return (IBPSensitivity)sens;
}

/**************************************************************************************************
 * 获取IBP计算结果数据。
 *************************************************************************************************/
IBPParamInfo IBPParam::getParamData(IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return IBPParamInfo();
    }
    return _chnData[chn].paramData;
}

/**************************************************************************************************
 * 设置IBP计算结果数据缓存。
 *************************************************************************************************/
void IBPParam::setParamData(IBPChannel chn, unsigned short sys, unsigned short dia, unsigned short mean,
                            unsigned short pr)
{
    if (chn >= IBP_CHN_NR)
    {
        return;
    }

    IBPParamInfo &data = _chnData[chn].paramData;
    data.sys = sys;
    data.dia = dia;
    data.mean = mean;
    data.pr = pr;

    unsigned t = timeDate->time();
    /* udpate pr per second */
    if (_lastPrUpdateTime != t)
    {
        short val = InvData();
        for (int i = 0; i < IBP_CHN_NR; i++)
        {
            if (_chnData[i].leadOff || _chnData[i].paramData.pr == InvData())
            {
                continue;
            }

            /* use the largest pr value of all the channel */
            if (_chnData[i].paramData.pr > val)
            {
                val = _chnData[i].paramData.pr;
            }
        }

        ecgDupParam.updatePR(val, PR_SOURCE_IBP);
        _lastPrUpdateTime = t;
    }
}

SubParamID IBPParam::getSubParamID(IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return SUB_PARAM_NONE;
    }

    return getSubParamID(_chnData[chn].paramData.pressureName);
}

/**************************************************************************************************
 * 根据参数ID获取压力标名
 *************************************************************************************************/
IBPLabel IBPParam::getPressureName(SubParamID id)
{
    IBPLabel name = IBP_LABEL_NR;
    switch (id)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_PR:
        name = IBP_LABEL_ART;
        break;
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_PA_PR:
        name = IBP_LABEL_PA;
        break;
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_CVP_PR:
        name = IBP_LABEL_CVP;
        break;
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_LAP_PR:
        name = IBP_LABEL_LAP;
        break;
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_RAP_PR:
        name = IBP_LABEL_RAP;
        break;
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_ICP_PR:
        name = IBP_LABEL_ICP;
        break;
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP1_PR:
        name = IBP_LABEL_AUXP1;
        break;
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_AUXP2_PR:
        name = IBP_LABEL_AUXP2;
        break;
    default:
        break;
    }
    return name;
}

IBPLabel IBPParam::getPressureName(WaveformID id)
{
    IBPLabel name;
    switch (id)
    {
    case WAVE_ART:
        name = IBP_LABEL_ART;
        break;
    case WAVE_PA:
        name = IBP_LABEL_PA;
        break;
    case WAVE_CVP:
        name = IBP_LABEL_CVP;
        break;
    case WAVE_LAP:
        name = IBP_LABEL_LAP;
        break;
    case WAVE_RAP:
        name = IBP_LABEL_RAP;
        break;
    case WAVE_ICP:
        name = IBP_LABEL_ICP;
        break;
    case WAVE_AUXP1:
        name = IBP_LABEL_AUXP1;
        break;
    case WAVE_AUXP2:
        name = IBP_LABEL_AUXP2;
        break;
    default:
        break;
    }
    return name;
}

/**************************************************************************************************
 * 根据压力标名获取参数ID
 *************************************************************************************************/
SubParamID IBPParam::getSubParamID(IBPLabel name)
{
    SubParamID id;
    switch (name)
    {
    case IBP_LABEL_ART:
        id = SUB_PARAM_ART_SYS;
        break;
    case IBP_LABEL_PA:
        id = SUB_PARAM_PA_SYS;
        break;
    case IBP_LABEL_CVP:
        id = SUB_PARAM_CVP_MAP;
        break;
    case IBP_LABEL_LAP:
        id = SUB_PARAM_LAP_MAP;
        break;
    case IBP_LABEL_RAP:
        id = SUB_PARAM_RAP_MAP;
        break;
    case IBP_LABEL_ICP:
        id = SUB_PARAM_ICP_MAP;
        break;
    case IBP_LABEL_AUXP1:
        id = SUB_PARAM_AUXP1_SYS;
        break;
    case IBP_LABEL_AUXP2:
        id = SUB_PARAM_AUXP2_SYS;
        break;
    default:
        break;
    }
    return id;
}

WaveformID IBPParam::getWaveformID(IBPLabel name)
{
    WaveformID waveID;
    switch (name)
    {
    case IBP_LABEL_ART:
        waveID = WAVE_ART;
        break;
    case IBP_LABEL_PA:
        waveID = WAVE_PA;
        break;
    case IBP_LABEL_CVP:
        waveID = WAVE_CVP;
        break;
    case IBP_LABEL_LAP:
        waveID = WAVE_LAP;
        break;
    case IBP_LABEL_RAP:
        waveID = WAVE_RAP;
        break;
    case IBP_LABEL_ICP:
        waveID = WAVE_ICP;
        break;
    case IBP_LABEL_AUXP1:
        waveID = WAVE_AUXP1;
        break;
    case IBP_LABEL_AUXP2:
        waveID = WAVE_AUXP2;
        break;
    default:
        break;
    }
    return waveID;
}

void IBPParam::updateSubParamLimit(SubParamID id)
{
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        if (getSubParamID(_chnData[i].paramData.pressureName) == id)
        {
            _chnData[i].trendWidget->updateLimit();
        }
    }
}

void IBPParam::clearCalibAlarm()
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource == NULL)
    {
        return;
    }
    for (int id = IBP1_ZERO_SUCCESS; id < IBP_ONESHOT_NR; id++)
    {
        alarmSource->setOneShotAlarm(id, false);
    }
}

bool IBPParam::hasIBPZeroReply(IBPChannel chn)
{
    bool ret = false;
    if (chn < IBP_CHN_NR)
    {
        ret = _chnData[chn].zeroReply;
        _chnData[chn].zeroReply = false;
    }
    return ret;
}

bool IBPParam::getLastZeroResult(IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return false;
    }

    return _chnData[chn].lastZeroResult;
}

bool IBPParam::hasIBPCalibReply(IBPChannel chn)
{
    bool ret = false;
    if (chn < IBP_CHN_NR)
    {
        ret = _chnData[chn].calibReply;
        _chnData[chn].calibReply = false;
    }
    return ret;
}

bool IBPParam::getLaseCalibResult(IBPChannel chn)
{
    if (chn >= IBP_CHN_NR)
    {
        return false;
    }

    return _chnData[chn].lastCalibResult;
}

bool IBPParam::channelNeedZero(IBPChannel chn) const
{
    if (chn >= IBP_CHN_NR)
    {
        return false;
    }
    return _chnData[chn].needZero;
}

IBPModuleType IBPParam::getMoudleType() const
{
    if (_provider)
    {
        return _provider->getIBPModuleType();
    }
    return IBP_MODULE_NR;
}

void IBPParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_IBP || !systemManager.isSupport(CONFIG_IBP))
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_IBP));
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        _chnData[i].waveWidget->updatePalette(pal);
        _chnData[i].trendWidget->updatePalette(pal);
    }
}
