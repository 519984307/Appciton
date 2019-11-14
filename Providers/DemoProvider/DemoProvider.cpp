/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#include "DemoProvider.h"
#include "SystemManager.h"
#include "Param.h"
#include "NIBPParam.h"
#include "SPO2Param.h"
#include "RESPParam.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "IBPParam.h"
#include "COParam.h"
#include "AGParam.h"
#include "ParamManager.h"
#include "IConfig.h"
#include "BLMCO2Provider.h"

#define DEMO_DATA_DIR  "/usr/local/nPM/demodata/"

struct DemoWaveDataDesc
{
    const char *waveformName;       // 对应的波形名称。
    const WaveformID waveID;        // 波形标识。
    Param *param;                   // 处理数据的Param对象。
    short sampleRate;               // 采样率。
    int error;                      // 数据产生时的误差记录。
};

static DemoWaveDataDesc _demoWaveData[WAVE_NR] =
{
    {"ECG-I",     WAVE_ECG_I,     NULL, 250, 0},
    {"ECG-II",    WAVE_ECG_II,    NULL, 250, 0},
    {"ECG-III",   WAVE_ECG_III,   NULL, 250, 0},
    {"ECG-aVR",   WAVE_ECG_aVR,   NULL, 250, 0},
    {"ECG-aVL",   WAVE_ECG_aVL,   NULL, 250, 0},
    {"ECG-aVF",   WAVE_ECG_aVF,   NULL, 250, 0},
    {"ECG-V1",    WAVE_ECG_V1,    NULL, 250, 0},
    {"ECG-V2",    WAVE_ECG_V2,    NULL, 250, 0},
    {"ECG-V3",    WAVE_ECG_V3,    NULL, 250, 0},
    {"ECG-V4",    WAVE_ECG_V4,    NULL, 250, 0},
    {"ECG-V5",    WAVE_ECG_V5,    NULL, 250, 0},
    {"ECG-V6",    WAVE_ECG_V6,    NULL, 250, 0},
    {"RESP",      WAVE_RESP,      NULL, 250, 0},
    {"SPO2",      WAVE_SPO2,      NULL, 250, 0},
    {"SPO2_2",    WAVE_SPO2_2,    NULL, 250, 0},
    {"CO2",       WAVE_CO2,       NULL, 20, 0},
    {"N2O",       WAVE_N2O,       NULL, 125, 0},
    {"AA1",       WAVE_AA1,       NULL, 125, 0},
    {"AA2",       WAVE_AA2,       NULL, 125, 0},
    {"O2",        WAVE_O2,        NULL, 125, 0},
    {"ART",       WAVE_ART,       NULL, 250, 0},
    {"PA",        WAVE_PA,        NULL, 250, 0},
    {"CVP",       WAVE_CVP,       NULL, 250, 0},
    {"LAP",       WAVE_LAP,       NULL, 250, 0},
    {"RAP",       WAVE_RAP,       NULL, 250, 0},
    {"ICP",       WAVE_ICP,       NULL, 250, 0},
    {"AUXP1",     WAVE_AUXP1,     NULL, 250, 0},
    {"AUXP2",     WAVE_AUXP2,     NULL, 250, 0},
};

struct DemoTrendDesc
{
    DemoTrendDesc()
    {
        param = NULL;
    }

    Param *param;
};
static DemoTrendDesc _demoTrendData[PARAM_NR];

/**************************************************************************************************
 * 定时器Slot函数。
 *************************************************************************************************/
void DemoProvider::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event)
    static int elapseSum = 0;
    int elapsed = _time.elapsed();
    _time.restart();
    elapseSum += elapsed;

    if (elapsed == 0)
    {
        return;
    }

//    int len = 0;
    char data;
    unsigned char d;
    int ret = 0;
    for (int i = 0; i < WAVE_NR; i++)
    {
        int len = _demoWaveData[i].sampleRate * elapsed + _demoWaveData[i].error;
        _demoWaveData[i].error = len % 1000;
        len /= 1000;
        if (len <= 0)
        {
            continue;
        }

        if (elapseSum >= 1000)
        {
            elapseSum = 0;
            for (int i = 0; i < PARAM_NR; i++)
            {
                if (_demoTrendData[i].param != NULL)
                {
                    _demoTrendData[i].param->handDemoTrendData();
                }
            }
        }

        if (!_demoFile[i].isOpen())
        {
            continue;
        }

        for (int j = 0; j < len; j++)
        {
            ret = _demoFile[i].read(&data, 1);
            if (ret <= 0)
            {
                _demoFile[i].seek(0);
                _demoFile[i].read(&data, 1);
            }

            if (_demoWaveData[i].param != NULL)
            {
                d = static_cast<unsigned char>(data);
                _demoWaveData[i].param->handDemoWaveform(_demoWaveData[i].waveID, d);
            }
        }
    }
}

/**************************************************************************************************
 * 与参数绑定。
 *************************************************************************************************/
bool DemoProvider::attachParam(Param *param)
{
    const QString &name = param->getName();

    if (name == paramInfo.getParamName(PARAM_ECG))
    {
        for (int i = WAVE_ECG_I; i <= WAVE_ECG_V6; i++)
        {
            _demoWaveData[i].param = param;
        }
        ecgParam.setProvider(this);
        ecgParam.setConnected(true);
    }
    else if (name == paramInfo.getParamName(PARAM_SPO2))
    {
        _demoWaveData[WAVE_SPO2].param = param;
        _demoWaveData[WAVE_SPO2_2].param = param;
        spo2Param.setProvider(this);
        spo2Param.setConnected(true);
        if (spo2Param.isConnected(true))
        {
            spo2Param.setProvider(this, true);
            spo2Param.setConnected(true, true);
        }
    }
    else if (name == paramInfo.getParamName(PARAM_RESP))
    {
        _demoWaveData[WAVE_RESP].param = param;
        respParam.setProvider(this);
        respParam.setConnected(true);
    }
    else if (name == paramInfo.getParamName(PARAM_NIBP))
    {
        nibpParam.setProvider(this);
        nibpParam.setConnected(true);
    }
    else if (name == paramInfo.getParamName(PARAM_CO2))
    {
        _demoWaveData[WAVE_CO2].param = param;
        co2Param.setProvider(this);
        co2Param.setConnected(true);
    }
    else if (name == paramInfo.getParamName(PARAM_IBP))
    {
        _demoWaveData[WAVE_ART].param = param;
        _demoWaveData[WAVE_PA].param = param;
        _demoWaveData[WAVE_CVP].param = param;
        _demoWaveData[WAVE_LAP].param = param;
        _demoWaveData[WAVE_RAP].param = param;
        _demoWaveData[WAVE_ICP].param = param;
        _demoWaveData[WAVE_AUXP1].param = param;
        _demoWaveData[WAVE_AUXP2].param = param;
        ibpParam.setProvider(this);
        ibpParam.setConnected(true);
    }
    else if (name == paramInfo.getParamName(PARAM_CO))
    {
        coParam.setProvider(this);
        coParam.setConnected(true);
    }
    else if (name == paramInfo.getParamName(PARAM_AG))
    {
        _demoWaveData[WAVE_N2O].param = param;
        _demoWaveData[WAVE_AA1].param = param;
        _demoWaveData[WAVE_AA2].param = param;
        _demoWaveData[WAVE_O2].param = param;
        agParam.setProvider(this);
        agParam.setConnected(true);
    }

    _demoTrendData[param->getParamID()].param = param;

    if (_timerID == 0)
    {
        _timerID = startTimer(4);   // 250Hz,4ms.
        _time.start();
    }

    return true;
}

/**************************************************************************************************
 * 与参数脱离。
 *************************************************************************************************/
void DemoProvider::detachParam(Param *param)
{
    ParamID id = param->getParamID();

    if (id == PARAM_ECG)
    {
        for (int i = WAVE_ECG_I; i <= WAVE_ECG_V6; i++)
        {
            _demoWaveData[i].param = NULL;
        }
    }
    else if (id == PARAM_RESP)
    {
        _demoWaveData[PARAM_RESP].param = NULL;
    }
    else if (id == PARAM_SPO2)
    {
        _demoWaveData[WAVE_SPO2].param = NULL;
        _demoWaveData[WAVE_SPO2_2].param = NULL;
    }
    else if (id == PARAM_CO2)
    {
        _demoWaveData[WAVE_CO2].param = NULL;
        QString str;
        machineConfig.getStrValue("CO2", str);
        BLMCO2Provider *co2Provider = reinterpret_cast<BLMCO2Provider *>(paramManager.getProvider(str));
        if (!co2Provider->isConnectModel())
        {
            co2Param.setConnected(false);
        }
    }
    else if (id == PARAM_IBP)
    {
        _demoWaveData[WAVE_ART].param = NULL;
        _demoWaveData[WAVE_PA].param = NULL;
        _demoWaveData[WAVE_CVP].param = NULL;
        _demoWaveData[WAVE_LAP].param = NULL;
        _demoWaveData[WAVE_RAP].param = NULL;
        _demoWaveData[WAVE_ICP].param = NULL;
        _demoWaveData[WAVE_AUXP1].param = NULL;
        _demoWaveData[WAVE_AUXP2].param = NULL;
    }
    else if (id == PARAM_AG)
    {
        _demoWaveData[WAVE_N2O].param = NULL;
        _demoWaveData[WAVE_AA1].param = NULL;
        _demoWaveData[WAVE_AA2].param = NULL;
        _demoWaveData[WAVE_O2].param = NULL;
    }

    _demoTrendData[id].param = NULL;

    // 删除定时器。
    if (_timerID != 0)
    {
        killTimer(_timerID);
        _timerID = 0;
    }
}

/**************************************************************************************************
 * 从端口读取数据。
 *************************************************************************************************/
void DemoProvider::dataArrived(void)
{
}

QByteArray DemoProvider::getDemoWaveData(WaveformID waveid)
{
    QByteArray content;
    if (waveid > WAVE_NONE && waveid < WAVE_NR)
    {
        QString path(DEMO_DATA_DIR);
        QFile f(path + _demoWaveData[waveid].waveformName);
        if (f.open(QIODevice::ReadOnly))
        {
            content = f.readAll();
        }
    }
    return content;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
DemoProvider::DemoProvider() : Provider("DemoProvider")
{
    _timerID = 0;

    _waveSampleRate = 250;

    QString path(DEMO_DATA_DIR);

    for (int i = 0; i < WAVE_NR; i++)
    {
        _demoFile[i].setFileName(path + _demoWaveData[i].waveformName);
        _demoFile[i].open(QFile::ReadOnly);
    }

    isConnected = true;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
DemoProvider::~DemoProvider()
{
    if (_timerID != 0)
    {
        killTimer(_timerID);
    }
}
