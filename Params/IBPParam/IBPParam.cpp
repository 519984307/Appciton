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
#include "WindowManager.h"
#include "IBPMenu.h"
#include "TimeDate.h"
#include "AlarmLimitMenu.h"
#include "TrendTableWindow.h"
#include "TrendGraphWidget.h"
#include "TrendGraphSetWidget.h"
#include "WaveformCache.h"
#include "IConfig.h"

IBPParam *IBPParam::_selfObj = NULL;

#define IBP_SCALE_NUM       13

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPParam::IBPParam() : Param(PARAM_IBP), _staIBP1(true), _staIBP2(true)
{

    _provider = NULL;
    _waveWidgetIBP1 = NULL;
    _waveWidgetIBP2 = NULL;

    _trendWidgetIBP1 = NULL;
    _trendWidgetIBP2 = NULL;

    _ibp1.pressureName = IBP_PRESSURE_ART;
    _ibp2.pressureName = IBP_PRESSURE_PA;

    ibpScaleList.clear();
    IBPScaleInfo manualScale;
    ibpScaleList.append(manualScale);
    int lowLimit[IBP_SCALE_NUM] = { -10, 0, 0, 0, 0, 0, 60, 30, 0, 0, 0, 0, 0};
    int highLimit[IBP_SCALE_NUM] = {10, 20, 30, 40, 60, 80, 140, 140, 140, 160, 200, 240, 300};
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
    if (_waveWidgetIBP1 == NULL || _waveWidgetIBP2 == NULL)
    {
        return;
    }
    switch (speed)
    {
    case IBP_SWEEP_SPEED_62_5:
        _waveWidgetIBP1->setWaveSpeed(6.25);
        _waveWidgetIBP2->setWaveSpeed(6.25);
        break;

    case IBP_SWEEP_SPEED_125:
        _waveWidgetIBP1->setWaveSpeed(12.5);
        _waveWidgetIBP2->setWaveSpeed(12.5);
        break;

    case IBP_SWEEP_SPEED_250:
        _waveWidgetIBP1->setWaveSpeed(25);
        _waveWidgetIBP2->setWaveSpeed(25);
        break;

    case IBP_SWEEP_SPEED_500:
        _waveWidgetIBP1->setWaveSpeed(50);
        _waveWidgetIBP2->setWaveSpeed(50);
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
        if (currentWaveforms[i] == "IBP1WaveWidget" || currentWaveforms[i] == "IBP2WaveWidget")
        {
            windowManager.resetWave();
            break;
        }
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
    data = data * 10 + 1000;
    if (id != WAVE_IBP1 && id != WAVE_IBP2)
    {
        return;
    }
    switch (id)
    {
    case WAVE_IBP1:
        if (NULL != _waveWidgetIBP1)
        {
            _waveWidgetIBP1->addData(data);
        }
        break;
    case WAVE_IBP2:
        if (NULL != _waveWidgetIBP2)
        {
            _waveWidgetIBP2->addData(data);
        }
        break;
    default:
        break;
    }
    waveformCache.addData(id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void IBPParam::handDemoTrendData()
{
    _trendWidgetIBP1->setZeroFlag(true);
    _trendWidgetIBP2->setZeroFlag(true);
    _ibp1.sys = qrand() % 15 + 105;
    _ibp1.dia = qrand() % 15 + 75;
    _ibp1.mean = qrand() % 15 + 90;
    _ibp1.pr = qrand() % 15 + 70;
    _ibp2.sys = qrand() % 15 + 105;
    _ibp2.dia = qrand() % 15 + 75;
    _ibp2.mean = qrand() % 15 + 90;
    _ibp2.pr = qrand() % 15 + 70;

    if (NULL != _trendWidgetIBP1 && NULL != _trendWidgetIBP2)
    {
        _trendWidgetIBP1->setData(_ibp1.sys, _ibp1.dia, _ibp1.mean);
        _trendWidgetIBP2->setData(_ibp2.sys, _ibp2.dia, _ibp2.mean);
    }
}

void IBPParam::exitDemo()
{
    _ibp1.sys = InvData();
    _ibp1.dia = InvData();
    _ibp1.mean = InvData();
    _ibp1.pr = InvData();
    _ibp2.sys = InvData();
    _ibp2.dia = InvData();
    _ibp2.mean = InvData();
    _ibp2.pr = InvData();
    if (NULL != _trendWidgetIBP1 && NULL != _trendWidgetIBP2)
    {
        _trendWidgetIBP1->setData(InvData(), InvData(), InvData());
        _trendWidgetIBP2->setData(InvData(), InvData(), InvData());
    }
}

void IBPParam::showSubParamValue()
{
    if (NULL != _trendWidgetIBP1)
    {
        _trendWidgetIBP1->showValue();
    }

    if (NULL != _trendWidgetIBP2)
    {
        _trendWidgetIBP2->showValue();
    }
}

void IBPParam::noticeLimitAlarm(int id, bool isAlarm, IBPSignalInput ibp)
{
    if (ibp == IBP_INPUT_1 && NULL != _trendWidgetIBP1)
    {
        _trendWidgetIBP1->isAlarm(id, isAlarm);
    }

    if (ibp == IBP_INPUT_2 && NULL != _trendWidgetIBP2)
    {
        _trendWidgetIBP2->isAlarm(id, isAlarm);
    }
}

void IBPParam::getAvailableWaveforms(QStringList &waveforms, QStringList &waveformShowName, int)
{
    waveforms.clear();
    waveformShowName.clear();

    if (NULL != _waveWidgetIBP1 && NULL != _waveWidgetIBP2)
    {
        waveforms.append(_waveWidgetIBP1->name());
        waveforms.append(_waveWidgetIBP2->name());
        waveformShowName.append(_waveWidgetIBP1->getTitle());
        waveformShowName.append(_waveWidgetIBP2->getTitle());
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
        return UNIT_MMHG;
    }
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short IBPParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ART_SYS:
        if (_ibp1.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_1).sys;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_2).sys;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_ART_DIA:
        if (_ibp1.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_1).dia;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_2).dia;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_ART_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_ART_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_ART)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_PA_SYS:
        if (_ibp1.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_1).sys;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_2).sys;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_PA_DIA:
        if (_ibp1.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_1).dia;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_2).dia;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_PA_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_PA_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_PA)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_CVP_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_CVP)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_CVP)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_CVP_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_CVP)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_CVP)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_LAP_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_LAP)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_LAP)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_LAP_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_LAP)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_LAP)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_RAP_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_RAP)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_RAP)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_RAP_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_RAP)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_RAP)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_ICP_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_ICP)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_ICP)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_ICP_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_ICP)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_ICP)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP1_SYS:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_1).sys;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_2).sys;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP1_DIA:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_1).dia;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_2).dia;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP1_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP1_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP1)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP2_SYS:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_1).sys;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_2).sys;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP2_DIA:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_1).dia;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_2).dia;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP2_MAP:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_1).mean;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_2).mean;
        }
        else
        {
            return InvData();
        }
    case SUB_PARAM_AUXP2_PR:
        if (_ibp1.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_1).pr;
        }
        else if (_ibp2.pressureName == IBP_PRESSURE_AUXP2)
        {
            return getParamData(IBP_INPUT_2).pr;
        }
        else
        {
            return InvData();
        }
    default:
        return InvData();
    }
}

bool IBPParam::isSubParamAvaliable(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_ART_PR:
        return _ibp1.pressureName == IBP_PRESSURE_ART || _ibp2.pressureName == IBP_PRESSURE_ART;
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_PA_PR:
        return _ibp1.pressureName == IBP_PRESSURE_PA || _ibp2.pressureName == IBP_PRESSURE_PA;
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_CVP_PR:
        return _ibp1.pressureName == IBP_PRESSURE_CVP || _ibp2.pressureName == IBP_PRESSURE_CVP;
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_LAP_PR:
        return _ibp1.pressureName == IBP_PRESSURE_LAP || _ibp2.pressureName == IBP_PRESSURE_LAP;
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_RAP_PR:
        return _ibp1.pressureName == IBP_PRESSURE_RAP || _ibp2.pressureName == IBP_PRESSURE_RAP;
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_ICP_PR:
        return _ibp1.pressureName == IBP_PRESSURE_ICP || _ibp2.pressureName == IBP_PRESSURE_ICP;
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP1_PR:
        return _ibp1.pressureName == IBP_PRESSURE_AUXP1 || _ibp2.pressureName == IBP_PRESSURE_AUXP1;
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_AUXP2_PR:
        return _ibp1.pressureName == IBP_PRESSURE_AUXP2 || _ibp2.pressureName == IBP_PRESSURE_AUXP2;
    default:
        break;
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
    if (_waveWidgetIBP1 == NULL || _waveWidgetIBP2 == NULL)
    {
        return;
    }

    _provider = provider;

    QString titleIBP1 = _waveWidgetIBP1->getTitle();
    QString titleIBP2 = _waveWidgetIBP2->getTitle();
    // 注册波形缓存
    waveformCache.registerSource(WAVE_IBP1, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), titleIBP1, _provider->getIBPBaseLine());
    waveformCache.registerSource(WAVE_IBP2, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), titleIBP2, _provider->getIBPBaseLine());
}

/**************************************************************************************************
 * 设置实时数据。
 *************************************************************************************************/
void IBPParam::setRealTimeData(unsigned short sys, unsigned short dia, unsigned short map,
                               unsigned short pr, IBPSignalInput IBP)
{
    setParamData(IBP, sys, dia, map, pr);
    if (IBP == IBP_INPUT_1)
    {
        if (NULL != _trendWidgetIBP1)
        {
            _trendWidgetIBP1->setData(sys, dia, map);
        }
    }
    else if (IBP == IBP_INPUT_2)
    {
        if (NULL != _trendWidgetIBP2)
        {
            _trendWidgetIBP2->setData(sys, dia, map);
        }
    }
    else
    {
        return;
    }
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void IBPParam::addWaveformData(short wave, bool invalid, IBPSignalInput IBP)
{
    int flag = 0;

    if (invalid)
    {
        flag = 0x4000;
        wave = 0;
    }

    if (IBP == IBP_INPUT_1)
    {
        if (_waveWidgetIBP1 != NULL)
        {
            _waveWidgetIBP1->addWaveformData(wave, flag);
        }
        waveformCache.addData(WAVE_IBP1, (flag << 16) | wave);
    }
    else if (IBP == IBP_INPUT_2)
    {
        if (_waveWidgetIBP2 != NULL)
        {
            _waveWidgetIBP2->addWaveformData(wave, flag);
        }
        waveformCache.addData(WAVE_IBP2, (flag << 16) | wave);
    }
}

/**************************************************************************************************
 * 设置趋势对象。
 *************************************************************************************************/
void IBPParam::setIBPTrendWidget(IBPTrendWidget *trendWidget, IBPSignalInput IBP)
{
    if (trendWidget == NULL)
    {
        return;
    }
    if (IBP == IBP_INPUT_1)
    {
        _trendWidgetIBP1 = trendWidget;
    }
    else if (IBP == IBP_INPUT_2)
    {
        _trendWidgetIBP2 = trendWidget;
    }
}

/**************************************************************************************************
 * 设置波形对象。
 *************************************************************************************************/
void IBPParam::setWaveWidget(IBPWaveWidget *waveWidget, IBPSignalInput IBP)
{
    if (waveWidget == NULL)
    {
        return;
    }

    if (IBP == IBP_INPUT_1)
    {
        _waveWidgetIBP1 = waveWidget;
    }
    else if (IBP == IBP_INPUT_2)
    {
        _waveWidgetIBP2 = waveWidget;
    }

//    waveWidget->setLimit(limit[waveWidget->getEntitle()].low, limit[waveWidget->getEntitle()].high);
    waveWidget->setLimit(getIBPScale(waveWidget->getEntitle()).low, getIBPScale(waveWidget->getEntitle()).high);
}

/**************************************************************************************************
 * 设置提示信息对象。
 *************************************************************************************************/
void IBPParam::setInfobarWidget(PromptInfoBarWidget *infoBarWidget)
{
    _infoBarWidget = infoBarWidget;
}


IBPScaleInfo IBPParam::getIBPScale(IBPPressureName name)
{
    // TODO implement this function
    IBPScaleInfo info;
    int highLimit = 0;
    switch (name)
    {
    case IBP_PRESSURE_ART:
    case IBP_PRESSURE_AUXP1:
    case IBP_PRESSURE_AUXP2:
        highLimit = 160;
        break;
    case IBP_PRESSURE_PA:
    case IBP_PRESSURE_CVP:
        highLimit = 30;
        break;
    case IBP_PRESSURE_LAP:
    case IBP_PRESSURE_RAP:
    case IBP_PRESSURE_ICP:
        highLimit = 20;
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

/**************************************************************************************************
 * 设置校零。
 *************************************************************************************************/
void IBPParam::zeroCalibration(IBPSignalInput IBP)
{
    unsigned zeroTime = timeDate.time();
    unsigned int year = timeDate.getDateYear(zeroTime) - 2000;
    unsigned int month = timeDate.getDateMonth(zeroTime);
    unsigned int day = timeDate.getDateDay(zeroTime);
    unsigned int hour = timeDate.getTimeHour(zeroTime);
    unsigned int min = timeDate.getTimeMinute(zeroTime);
    unsigned int second = timeDate.getTimeSenonds(zeroTime);
    _provider->setTimeZero(IBP, IBP_CALIBRATION_ZERO,
                           (unsigned int)second, (unsigned int)min,
                           (unsigned int)hour, (unsigned int)day,
                           (unsigned int)month, (unsigned int)year);
    _provider->setZero(IBP, IBP_CALIBRATION_ZERO, 0x00);
}

/**************************************************************************************************
 * 设置校准。
 *************************************************************************************************/
void IBPParam::setCalibration(IBPSignalInput IBP, unsigned short value)
{
    _provider->setZero(IBP, IBP_CALIBRATION_SET, value);
}

/**************************************************************************************************
 * 校零校准信息。
 *************************************************************************************************/
void IBPParam::calibrationInfo(IBPCalibration calib, IBPSignalInput IBP, int calibinfo)
{
    QString info;
    if (calib == IBP_CALIBRATION_ZERO)
    {
        if (IBP == IBP_INPUT_1)
        {
            switch (calibinfo)
            {
            case 0:
            {
                info = "IBP" + trs("Channel") + "1" + trs("ZeroCalibSuccess");
                _trendWidgetIBP1->setZeroFlag(true);
                break;
            }
            case 1:
            {
                info = "IBP" + trs("Channel") + "1" + trs("MeasurePressureIsPulse");
                break;
            }
            case 2:
            {
                info = "IBP" + trs("Channel") + "1" + trs("ZeroCalibPressureBeyondMeasureRange");
                break;
            }
            case 4:
            {
                info = "IBP" + trs("Channel") + "1" + trs("LeadOffZeroCalibFail");
                break;
            }
            case 5:
            {
                info = "IBP" + trs("Channel") + "1" + trs("NoSetZeroCalibTime");
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if (IBP == IBP_INPUT_2)
        {
            switch (calibinfo)
            {
            case 0:
            {
                info = "IBP" + trs("Channel") + "2" + trs("ZeroCalibSuccess");
                _trendWidgetIBP2->setZeroFlag(true);
                break;
            }
            case 1:
            {
                info = "IBP" + trs("Channel") + "2" + trs("MeasurePressureIsPulse");
                break;
            }
            case 2:
            {
                info = "IBP" + trs("Channel") + "2" + trs("ZeroCalibPressureBeyondMeasureRange");
                break;
            }
            case 4:
            {
                info = "IBP" + trs("Channel") + "2" + trs("LeadOffZeroCalibFail");
                break;
            }
            case 5:
            {
                info = "IBP" + trs("Channel") + "2" + trs("NoSetZeroCalibTime");
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
        if (IBP == IBP_INPUT_1)
        {
            switch (calibinfo)
            {
            case 0:
            {
                info = "IBP" + trs("Channel") + "1" + trs("CalibrationSuccess");
                break;
            }
            case 1:
            {
                info = "IBP" + trs("Channel") + "1" + trs("MeasurePressureIsPulse");
                break;
            }
            case 2:
            {
                info = "IBP" + trs("Channel") + "1" + trs("CalibrationPressureBeyondMeasureRange");
                break;
            }
            case 3:
            {
                info = "IBP" + trs("Channel") + "1" + trs("NoZeroCalib");
                break;
            }
            case 4:
            {
                info = "IBP" + trs("Channel") + "1" + trs("LeadOffCalibrationFail");
                break;
            }
            case 6:
            {
                info = "IBP" + trs("Channel") + "1" + trs("NoSetCalibrationTime");
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if (IBP == IBP_INPUT_2)
        {
            switch (calibinfo)
            {
            case 0:
            {
                info = "IBP" + trs("Channel") + "2" + trs("CalibrationSuccess");
                break;
            }
            case 1:
            {
                info = "IBP" + trs("Channel") + "2" + trs("MeasurePressureIsPulse");
                break;
            }
            case 2:
            {
                info = "IBP" + trs("Channel") + "2" + trs("CalibrationPressureBeyondMeasureRange");
                break;
            }
            case 3:
            {
                info = "IBP" + trs("Channel") + "2" + trs("NoZeroCalib");
                break;
            }
            case 4:
            {
                info = "IBP" + trs("Channel") + "2" + trs("LeadOffCalibrationFail");
                break;
            }
            case 6:
            {
                info = "IBP" + trs("Channel") + "2" + trs("NoSetCalibrationTime");
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
    ibpMenu.displayZeroRev(info);
    _infoBarWidget->display(info);
}

/**************************************************************************************************
 * 导联状态。
 *************************************************************************************************/
void IBPParam::leadStatus(bool staIBP1, bool staIBP2)
{
    if (staIBP1 != _staIBP1)
    {
        _waveWidgetIBP1->setLeadSta(staIBP1);
        if (staIBP1)
        {
            _trendWidgetIBP1->setZeroFlag(false);
        }
        _staIBP1 = staIBP1;
    }

    if (staIBP2 != _staIBP2)
    {
        _waveWidgetIBP2->setLeadSta(staIBP2);
        if (staIBP1)
        {
            _trendWidgetIBP2->setZeroFlag(false);
        }
        _staIBP2 = staIBP2;
    }
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
void IBPParam::setEntitle(IBPPressureName entitle, IBPSignalInput IBP)
{
    if (IBP == IBP_INPUT_1)
    {
        if (entitle == _ibp2.pressureName)
        {
            ibpMenu.comboListChangeIBP2(_ibp1.pressureName);
        }
        _waveWidgetIBP1->setEntitle(entitle);
        _trendWidgetIBP1->setEntitle(entitle);
        _ibp1.pressureName = entitle;
    }
    else if (IBP == IBP_INPUT_2)
    {
        if (entitle == _ibp1.pressureName)
        {
            ibpMenu.comboListChangeIBP1(_ibp2.pressureName);
        }
        _ibp2.pressureName = entitle;
        _waveWidgetIBP2->setEntitle(entitle);
        _trendWidgetIBP2->setEntitle(entitle);
    }
    else
    {
        return;
    }

    alarmLimitMenu.setIBPAlarmItem(_ibp1.pressureName, _ibp2.pressureName);
    trendGraphSetWidget.upDateTrendGroup();
    trendGraphWidget.updateTrendGraph();

    if (((_ibp1.pressureName >= IBP_PRESSURE_CVP) && (_ibp1.pressureName <= IBP_PRESSURE_ICP))
            && ((_ibp2.pressureName >= IBP_PRESSURE_CVP) && (_ibp2.pressureName <= IBP_PRESSURE_ICP)))
    {
        _provider->setIndicate(_ibp1.pressureName, _ibp2.pressureName, IBP_MEASURE_CALC_1, IBP_MEASURE_CALC_1);
    }
    else if (((_ibp1.pressureName >= IBP_PRESSURE_CVP) && (_ibp1.pressureName <= IBP_PRESSURE_ICP))
             && ((_ibp2.pressureName < IBP_PRESSURE_CVP) || (_ibp2.pressureName > IBP_PRESSURE_ICP)))
    {
        _provider->setIndicate(_ibp1.pressureName, _ibp2.pressureName, IBP_MEASURE_CALC_1, IBP_MEASURE_CALC_3);
    }
    else if (((_ibp1.pressureName < IBP_PRESSURE_CVP) || (_ibp1.pressureName > IBP_PRESSURE_ICP))
             && ((_ibp2.pressureName >= IBP_PRESSURE_CVP) && (_ibp2.pressureName <= IBP_PRESSURE_ICP)))
    {
        _provider->setIndicate(_ibp1.pressureName, _ibp2.pressureName, IBP_MEASURE_CALC_3, IBP_MEASURE_CALC_1);
    }
    else
    {
        _provider->setIndicate(_ibp1.pressureName, _ibp2.pressureName, IBP_MEASURE_CALC_3, IBP_MEASURE_CALC_3);
    }
}

IBPPressureName IBPParam::getEntitle(IBPSignalInput signal) const
{
    if (signal == IBP_INPUT_1)
    {
        return _ibp1.pressureName;
    }
    else
    {
        return _ibp2.pressureName;
    }
}

/**************************************************************************************************
 * 设置滤波。
 *************************************************************************************************/
void IBPParam::setFilter(IBPFilterMode filter)
{
    currentConfig.setNumValue("IBP|FilterMode", static_cast<int>(filter));
    _provider->setFilter(IBP_INPUT_1, filter);
    _provider->setFilter(IBP_INPUT_2, filter);
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
    _provider->setAvergTime(IBP_INPUT_1, time);
    _provider->setAvergTime(IBP_INPUT_2, time);
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
IBPParamInfo IBPParam::getParamData(IBPSignalInput IBP)
{
    if (IBP == IBP_INPUT_1)
    {
        return _ibp1;
    }
    else
    {
        return _ibp2;
    }
}

/**************************************************************************************************
 * 设置IBP计算结果数据缓存。
 *************************************************************************************************/
void IBPParam::setParamData(IBPSignalInput IBP, unsigned short sys, unsigned short dia, unsigned short mean,
                            unsigned short pr)
{
    if (IBP == IBP_INPUT_1)
    {
        _ibp1.sys = sys;
        _ibp1.dia = dia;
        _ibp1.mean = mean;
        _ibp1.pr = pr;
    }
    else
    {
        _ibp2.sys = sys;
        _ibp2.dia = dia;
        _ibp2.mean = mean;
        _ibp2.pr = pr;
    }
}

/**************************************************************************************************
 * 获取ibp两通道标名对应的参数ID。
 *************************************************************************************************/
void IBPParam::getSubParamID(SubParamID &ibp1, SubParamID &ibp2)
{
    ibp1 = getSubParamID(_ibp1.pressureName);
    ibp2 = getSubParamID(_ibp2.pressureName);
}

/**************************************************************************************************
 * 根据参数ID获取压力标名
 *************************************************************************************************/
IBPPressureName IBPParam::getPressureName(SubParamID id)
{
    IBPPressureName name;
    switch (id)
    {
    case SUB_PARAM_ART_SYS:
    case SUB_PARAM_ART_MAP:
    case SUB_PARAM_ART_DIA:
    case SUB_PARAM_ART_PR:
        name = IBP_PRESSURE_ART;
        break;
    case SUB_PARAM_PA_SYS:
    case SUB_PARAM_PA_DIA:
    case SUB_PARAM_PA_MAP:
    case SUB_PARAM_PA_PR:
        name = IBP_PRESSURE_PA;
        break;
    case SUB_PARAM_CVP_MAP:
    case SUB_PARAM_CVP_PR:
        name = IBP_PRESSURE_CVP;
        break;
    case SUB_PARAM_LAP_MAP:
    case SUB_PARAM_LAP_PR:
        name = IBP_PRESSURE_LAP;
        break;
    case SUB_PARAM_RAP_MAP:
    case SUB_PARAM_RAP_PR:
        name = IBP_PRESSURE_RAP;
        break;
    case SUB_PARAM_ICP_MAP:
    case SUB_PARAM_ICP_PR:
        name = IBP_PRESSURE_ICP;
        break;
    case SUB_PARAM_AUXP1_SYS:
    case SUB_PARAM_AUXP1_DIA:
    case SUB_PARAM_AUXP1_MAP:
    case SUB_PARAM_AUXP1_PR:
        name = IBP_PRESSURE_AUXP1;
        break;
    case SUB_PARAM_AUXP2_SYS:
    case SUB_PARAM_AUXP2_DIA:
    case SUB_PARAM_AUXP2_MAP:
    case SUB_PARAM_AUXP2_PR:
        name = IBP_PRESSURE_AUXP2;
        break;
    default:
        break;
    }
    return name;
}

/**************************************************************************************************
 * 根据压力标名获取参数ID
 *************************************************************************************************/
SubParamID IBPParam::getSubParamID(IBPPressureName name)
{
    SubParamID id;
    switch (name)
    {
    case IBP_PRESSURE_ART:
        id = SUB_PARAM_ART_SYS;
        break;
    case IBP_PRESSURE_PA:
        id = SUB_PARAM_PA_SYS;
        break;
    case IBP_PRESSURE_CVP:
        id = SUB_PARAM_CVP_MAP;
        break;
    case IBP_PRESSURE_LAP:
        id = SUB_PARAM_LAP_MAP;
        break;
    case IBP_PRESSURE_RAP:
        id = SUB_PARAM_RAP_MAP;
        break;
    case IBP_PRESSURE_ICP:
        id = SUB_PARAM_ICP_MAP;
        break;
    case IBP_PRESSURE_AUXP1:
        id = SUB_PARAM_AUXP1_SYS;
        break;
    case IBP_PRESSURE_AUXP2:
        id = SUB_PARAM_AUXP2_SYS;
        break;
    default:
        break;
    }
    return id;
}
