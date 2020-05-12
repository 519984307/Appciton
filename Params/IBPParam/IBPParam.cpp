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
#include "Framework/TimeDate/TimeDate.h"

IBPParam *IBPParam::_selfObj = NULL;

#define IBP_SCALE_NUM       13

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
IBPParam::IBPParam() : Param(PARAM_IBP),  _provider(NULL), _waveWidgetIBP1(NULL),
    _waveWidgetIBP2(NULL), _trendWidgetIBP1(NULL), _trendWidgetIBP2(NULL),
    _staIBP1(false), _staIBP2(false), _connectedProvider(false), _ibp1ZeroReply(false),
    _ibp2ZeroReply(false), _ibp1HasBeenZero(false), _ibp2HasBeenZero(false)
{
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

    QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();
    if (currentWaveforms.contains(_waveWidgetIBP1->name()) || currentWaveforms.contains(_waveWidgetIBP2->name()))
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

    WaveformID ibp1WaveID = getWaveformID(getEntitle(IBP_INPUT_1));
    WaveformID ibp2WaveID = getWaveformID(getEntitle(IBP_INPUT_2));

    if (id == ibp1WaveID)
    {
        if (NULL != _waveWidgetIBP1)
        {
            _waveWidgetIBP1->addData(data);
        }
    }
    else if (id == ibp2WaveID)
    {
        if (NULL != _waveWidgetIBP2)
        {
            _waveWidgetIBP2->addData(data);
        }
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
    _ibp1.sys = 120;
    _ibp1.dia = 75;
    _ibp1.mean = 90;
    _ibp1.pr = 60;
    _ibp2.sys = 25;
    _ibp2.dia = 9;
    _ibp2.mean = 14;
    _ibp2.pr = 60;

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

void IBPParam::getAvailableWaveforms(QStringList *waveforms, QStringList *waveformShowName, int)
{
    waveforms->clear();
    waveformShowName->clear();

    if (NULL != _waveWidgetIBP1 && NULL != _waveWidgetIBP2)
    {
        waveforms->append(_waveWidgetIBP1->name());
        waveforms->append(_waveWidgetIBP2->name());
        waveformShowName->append(_waveWidgetIBP1->getTitle());
        waveformShowName->append(_waveWidgetIBP2->getTitle());
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

    // 注册波形缓存
    QString title = IBPSymbol::convert(IBP_PRESSURE_ART);
    waveformCache.registerSource(WAVE_ART, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_PA);
    waveformCache.registerSource(WAVE_PA, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_CVP);
    waveformCache.registerSource(WAVE_CVP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_RAP);
    waveformCache.registerSource(WAVE_RAP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_LAP);
    waveformCache.registerSource(WAVE_LAP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_ICP);
    waveformCache.registerSource(WAVE_ICP, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_AUXP1);
    waveformCache.registerSource(WAVE_AUXP1, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());
    title = IBPSymbol::convert(IBP_PRESSURE_AUXP2);
    waveformCache.registerSource(WAVE_AUXP2, _provider->getIBPWaveformSample(),
                                 0, _provider->getIBPMaxWaveform(), title, _provider->getIBPBaseLine());

    _waveWidgetIBP1->setDataRate(provider->getIBPWaveformSample());
    _waveWidgetIBP2->setDataRate(provider->getIBPWaveformSample());
}

void IBPParam::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }
    _connectedProvider = isConnected;
}

bool IBPParam::isConnected()
{
    return _connectedProvider;
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
    /* NOTE: The wave data's unit should be 0.1 mmhg */
    int flag = 0;

    if (invalid)
    {
        flag = 0x4000;
        wave = 0;
    }

    if (IBP == IBP_INPUT_1)
    {
        WaveformID ibp1WaveID = getWaveformID(getEntitle(IBP_INPUT_1));
        if (_waveWidgetIBP1 != NULL)
        {
            _waveWidgetIBP1->addWaveformData(wave, flag);
        }
        waveformCache.addData(ibp1WaveID, (flag << 16) | wave);
    }
    else if (IBP == IBP_INPUT_2)
    {
        WaveformID ibp2WaveID = getWaveformID(getEntitle(IBP_INPUT_2));
        if (_waveWidgetIBP2 != NULL)
        {
            _waveWidgetIBP2->addWaveformData(wave, flag);
        }
        waveformCache.addData(ibp2WaveID, (flag << 16) | wave);
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
    _setWaveformSpeed(getSweepSpeed());
}

QList<SubParamID> IBPParam::getShortTrendList(IBPSignalInput IBP)
{
    QList<SubParamID> paraList;
    if (IBP == IBP_INPUT_1)
    {
        paraList = _trendWidgetIBP1->getShortTrendSubParams();
    }
    else if (IBP == IBP_INPUT_2)
    {
        paraList = _trendWidgetIBP2->getShortTrendSubParams();
    }
    return paraList;
}

IBPScaleInfo IBPParam::getIBPScale(IBPPressureName name)
{
    // TODO(chenbingyun): implement this function
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

void IBPParam::setRulerLimit(IBPRulerLimit ruler, IBPSignalInput ibp)
{
    if (ibp == IBP_INPUT_1)
    {
        currentConfig.setNumValue("IBP|RulerLimit1", static_cast<int>(ruler));
        if (_waveWidgetIBP1 != NULL)
        {
            _waveWidgetIBP1->setRulerLimit(ruler);
        }
    }
    else if (ibp == IBP_INPUT_2)
    {
        currentConfig.setNumValue("IBP|RulerLimit2", static_cast<int>(ruler));
        if (_waveWidgetIBP2 != NULL)
        {
            _waveWidgetIBP2->setRulerLimit(ruler);
        }
    }
}

void IBPParam::setRulerLimit(int low, int high, IBPSignalInput ibp)
{
    if (ibp == IBP_INPUT_1)
    {
        if (_waveWidgetIBP1 != NULL)
        {
            _waveWidgetIBP1->setLimit(low, high);
        }
    }
    else if (ibp == IBP_INPUT_2)
    {
        if (_waveWidgetIBP2 != NULL)
        {
            _waveWidgetIBP2->setLimit(low, high);
        }
    }
}

IBPRulerLimit IBPParam::getRulerLimit(IBPSignalInput ibp)
{
    int ruler = IBP_RULER_LIMIT_0_160;
    if (ibp == IBP_INPUT_1)
    {
        currentConfig.getNumValue("IBP|RulerLimit1", ruler);
    }
    else if (ibp == IBP_INPUT_2)
    {
        currentConfig.getNumValue("IBP|RulerLimit2", ruler);
    }
    return static_cast<IBPRulerLimit>(ruler);
}

IBPRulerLimit IBPParam::getRulerLimit(IBPPressureName name)
{
    IBPRulerLimit ruler = IBP_RULER_LIMIT_0_160;
    switch (name)
    {
    case IBP_PRESSURE_ART:
    case IBP_PRESSURE_AUXP1:
    case IBP_PRESSURE_AUXP2:
        ruler = IBP_RULER_LIMIT_0_160;
        break;
    case IBP_PRESSURE_PA:
    case IBP_PRESSURE_CVP:
        ruler = IBP_RULER_LIMIT_0_30;
        break;
    case IBP_PRESSURE_LAP:
    case IBP_PRESSURE_RAP:
    case IBP_PRESSURE_ICP:
        ruler = IBP_RULER_LIMIT_0_20;
        break;
    default:
        break;
    }
    return ruler;
}

void IBPParam::setScaleInfo(const IBPScaleInfo &info, const IBPPressureName &name)
{
    if (_ibp1.pressureName == name)
    {
        _scale1 = info;
    }
    else if (_ibp2.pressureName == name)
    {
        _scale2 = info;
    }
}

IBPScaleInfo &IBPParam::getScaleInfo(IBPSignalInput ibp)
{
    if (ibp == IBP_INPUT_1)
    {
        return _scale1;
    }
    else
    {
        return _scale2;
    }
}

/**************************************************************************************************
 * 设置校零。
 *************************************************************************************************/
void IBPParam::zeroCalibration(IBPSignalInput IBP)
{
    if (IBP == IBP_INPUT_1)
    {
        _ibp1HasBeenZero = false;
    }
    else if (IBP == IBP_INPUT_2)
    {
        _ibp2HasBeenZero = false;
    }

    clearCalibAlarm();
    unsigned zeroTime = timeDate->time();
    unsigned int year = timeDate->getDateYear(zeroTime) - 2000;
    unsigned int month = timeDate->getDateMonth(zeroTime);
    unsigned int day = timeDate->getDateDay(zeroTime);
    unsigned int hour = timeDate->getTimeHour(zeroTime);
    unsigned int min = timeDate->getTimeMinute(zeroTime);
    unsigned int second = timeDate->getTimeSeconds(zeroTime);
    _provider->setTimeZero(IBP, IBP_CALIBRATION_ZERO,
                           (unsigned int)second, (unsigned int)min,
                           (unsigned int)hour, (unsigned int)day,
                           (unsigned int)month, (unsigned int)year);
    _provider->setZero(IBP, IBP_CALIBRATION_ZERO, 0x00);
}

bool IBPParam::hasBeenZero(IBPSignalInput IBP)
{
    if (IBP == IBP_INPUT_1)
    {
        return _ibp1HasBeenZero;
    }
    else if (IBP == IBP_INPUT_2)
    {
        return _ibp2HasBeenZero;
    }

    return false;
}

/**************************************************************************************************
 * 设置校准。
 *************************************************************************************************/
void IBPParam::setCalibration(IBPSignalInput IBP, unsigned short value)
{
    clearCalibAlarm();
    _provider->setZero(IBP, IBP_CALIBRATION_SET, value);
}

/**************************************************************************************************
 * 校零校准信息。
 *************************************************************************************************/
void IBPParam::calibrationInfo(IBPCalibration calib, IBPSignalInput IBP, int calibinfo)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_IBP);
    if (alarmSource == NULL)
    {
        return;
    }

    if (calib == IBP_CALIBRATION_ZERO)
    {
        if (IBP == IBP_INPUT_1)
        {
            _ibp1ZeroReply = true;
            if (!_ibp1HasBeenZero)
            {
                _ibp1HasBeenZero = true;
            }
            switch (static_cast<IBPZeroResult>(calibinfo))
            {
            case IBP_ZERO_SUCCESS:
            {
                alarmSource->setOneShotAlarm(IBP1_ZERO_SUCCESS, true);
                _trendWidgetIBP1->setZeroFlag(true);
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
        else if (IBP == IBP_INPUT_2)
        {
            _ibp2ZeroReply = true;
            if (!_ibp2HasBeenZero)
            {
                _ibp2HasBeenZero = true;
            }
            switch (static_cast<IBPZeroResult>(calibinfo))
            {
            case IBP_ZERO_SUCCESS:
            {
                alarmSource->setOneShotAlarm(IBP2_ZERO_SUCCESS, true);
                _trendWidgetIBP2->setZeroFlag(true);
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
        if (IBP == IBP_INPUT_1)
        {
            switch (static_cast<IBPCalibrationResult>(calibinfo))
            {
            case IBP_CALIBRATION_SUCCESS:
            {
                alarmSource->setOneShotAlarm(IBP1_CALIB_SUCCESS, true);
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
        else if (IBP == IBP_INPUT_2)
        {
            switch (static_cast<IBPCalibrationResult>(calibinfo))
            {
            case IBP_CALIBRATION_SUCCESS:
            {
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

bool IBPParam::isIBPLeadOff(IBPSignalInput IBP)
{
    if (IBP == IBP_INPUT_1)
    {
        return _staIBP1;
    }
    else if (IBP == IBP_INPUT_2)
    {
        return _staIBP2;
    }
    return true;
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
        _waveWidgetIBP1->setEntitle(entitle);
        _trendWidgetIBP1->setEntitle(entitle);
        _ibp1.pressureName = entitle;
    }
    else if (IBP == IBP_INPUT_2)
    {
        _ibp2.pressureName = entitle;
        _waveWidgetIBP2->setEntitle(entitle);
        _trendWidgetIBP2->setEntitle(entitle);
    }
    else
    {
        return;
    }

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

IBPPressureName IBPParam::getEntitle(IBPLimitAlarmType alarmType) const
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
        return IBP_PRESSURE_ART;
    case PA_LIMIT_ALARM_SYS_LOW:
    case PA_LIMIT_ALARM_SYS_HIGH:
    case PA_LIMIT_ALARM_DIA_LOW:
    case PA_LIMIT_ALARM_DIA_HIGH:
    case PA_LIMIT_ALARM_MEAN_LOW:
    case PA_LIMIT_ALARM_MEAN_HIGH:
    case PA_LIMIT_ALARM_PR_LOW:
    case PA_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_PA;
    case CVP_LIMIT_ALARM_MEAN_LOW:
    case CVP_LIMIT_ALARM_MEAN_HIGH:
    case CVP_LIMIT_ALARM_PR_LOW:
    case CVP_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_CVP;
    case LAP_LIMIT_ALARM_MEAN_LOW:
    case LAP_LIMIT_ALARM_MEAN_HIGH:
    case LAP_LIMIT_ALARM_PR_LOW:
    case LAP_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_LAP;
    case RAP_LIMIT_ALARM_MEAN_LOW:
    case RAP_LIMIT_ALARM_MEAN_HIGH:
    case RAP_LIMIT_ALARM_PR_LOW:
    case RAP_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_RAP;
    case ICP_LIMIT_ALARM_MEAN_LOW:
    case ICP_LIMIT_ALARM_MEAN_HIGH:
    case ICP_LIMIT_ALARM_PR_LOW:
    case ICP_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_ICP;
    case AUXP1_LIMIT_ALARM_SYS_LOW:
    case AUXP1_LIMIT_ALARM_SYS_HIGH:
    case AUXP1_LIMIT_ALARM_DIA_LOW:
    case AUXP1_LIMIT_ALARM_DIA_HIGH:
    case AUXP1_LIMIT_ALARM_MEAN_LOW:
    case AUXP1_LIMIT_ALARM_MEAN_HIGH:
    case AUXP1_LIMIT_ALARM_PR_LOW:
    case AUXP1_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_AUXP1;
    case AUXP2_LIMIT_ALARM_SYS_LOW:
    case AUXP2_LIMIT_ALARM_SYS_HIGH:
    case AUXP2_LIMIT_ALARM_DIA_LOW:
    case AUXP2_LIMIT_ALARM_DIA_HIGH:
    case AUXP2_LIMIT_ALARM_MEAN_LOW:
    case AUXP2_LIMIT_ALARM_MEAN_HIGH:
    case AUXP2_LIMIT_ALARM_PR_LOW:
    case AUXP2_LIMIT_ALARM_PR_HIGH:
        return IBP_PRESSURE_AUXP2;
    default:
        return IBP_PRESSURE_NR;
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

    ecgDupParam.updatePR(static_cast<short>(pr), PR_SOURCE_IBP);
}

SubParamID IBPParam::getSubParamID(IBPSignalInput inputID)
{
    if (inputID == IBP_INPUT_1)
    {
        return getSubParamID(_ibp1.pressureName);
    }
    else if (inputID == IBP_INPUT_2)
    {
        return getSubParamID(_ibp2.pressureName);
    }
    return SUB_PARAM_NONE;
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

IBPPressureName IBPParam::getPressureName(WaveformID id)
{
    IBPPressureName name;
    switch (id)
    {
    case WAVE_ART:
        name = IBP_PRESSURE_ART;
        break;
    case WAVE_PA:
        name = IBP_PRESSURE_PA;
        break;
    case WAVE_CVP:
        name = IBP_PRESSURE_CVP;
        break;
    case WAVE_LAP:
        name = IBP_PRESSURE_LAP;
        break;
    case WAVE_RAP:
        name = IBP_PRESSURE_RAP;
        break;
    case WAVE_ICP:
        name = IBP_PRESSURE_ICP;
        break;
    case WAVE_AUXP1:
        name = IBP_PRESSURE_AUXP1;
        break;
    case WAVE_AUXP2:
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

WaveformID IBPParam::getWaveformID(IBPPressureName name)
{
    WaveformID waveID;
    switch (name)
    {
    case IBP_PRESSURE_ART:
        waveID = WAVE_ART;
        break;
    case IBP_PRESSURE_PA:
        waveID = WAVE_PA;
        break;
    case IBP_PRESSURE_CVP:
        waveID = WAVE_CVP;
        break;
    case IBP_PRESSURE_LAP:
        waveID = WAVE_LAP;
        break;
    case IBP_PRESSURE_RAP:
        waveID = WAVE_RAP;
        break;
    case IBP_PRESSURE_ICP:
        waveID = WAVE_ICP;
        break;
    case IBP_PRESSURE_AUXP1:
        waveID = WAVE_AUXP1;
        break;
    case IBP_PRESSURE_AUXP2:
        waveID = WAVE_AUXP2;
        break;
    default:
        break;
    }
    return waveID;
}

void IBPParam::updateSubParamLimit(SubParamID id)
{
    if (getSubParamID(_ibp1.pressureName) == id)
    {
        _trendWidgetIBP1->updateLimit();
    }
    else if (getSubParamID(_ibp2.pressureName) == id)
    {
        _trendWidgetIBP2->updateLimit();
    }
}

bool IBPParam::getIBPLeadOff(IBPSignalInput ibp)
{
    if (ibp == IBP_INPUT_1)
    {
        return _staIBP1;
    }
    else if (ibp == IBP_INPUT_2)
    {
        return _staIBP2;
    }
    return true;
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

bool IBPParam::hasIBPZeroReply(IBPSignalInput chn)
{
    bool *reply = NULL;
    if (chn == IBP_INPUT_1)
    {
        reply = &_ibp1ZeroReply;
    }
    else if (chn == IBP_INPUT_2)
    {
        reply = &_ibp2ZeroReply;
    }

    bool ret = false;
    if (reply)
    {
        ret = *reply;
        *reply = false;
    }

    return ret;
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
    _waveWidgetIBP1->updatePalette(pal);
    _waveWidgetIBP2->updatePalette(pal);
    _trendWidgetIBP1->updatePalette(pal);
    _trendWidgetIBP2->updatePalette(pal);
}
