#include "DemoProvider.h"
#include "SystemManager.h"
#include "Param.h"
#include "NIBPParam.h"
#include "NIBPParamService.h"
#include "SPO2Param.h"
#include "RESPParam.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "IBPParam.h"
#include "COParam.h"
#include "AGParam.h"

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
    {"SPO2",      WAVE_SPO2,      NULL, 50, 0},
    {"CO2",       WAVE_CO2,       NULL, 20, 0},
    {"N2O",       WAVE_N2O,       NULL, 50, 0},
    {"AA1",       WAVE_AA1,       NULL, 50, 0},
    {"AA2",       WAVE_AA2,       NULL, 50, 0},
    {"O2",        WAVE_O2,        NULL, 50, 0},
    {"IBP1",      WAVE_IBP1,      NULL, 50, 0},
    {"IBP2",      WAVE_IBP2,      NULL, 50, 0},
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
void DemoProvider::timerEvent(QTimerEvent */*event*/)
{
    static int elapseSum = 0;
    int elapsed = _time.elapsed();
    _time.restart();
    elapseSum += elapsed;

    if (elapsed == 0)
    {
        return;
    }

    int len = 0;
    char data;
    unsigned char d;
    int ret = 0;
    for (int i = 0; i < WAVE_NR; i++)
    {
        len = _demoWaveData[i].sampleRate * elapsed + _demoWaveData[i].error;
        _demoWaveData[i].error = len % 1000;
        len /= 1000;
        if(len <= 0)
        {
            continue;
        }

        if (elapseSum >= 2000)
        {
            elapseSum = 0;
            for ( int i = 0; i < PARAM_NR; i++)
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
                d = (unsigned char)(data);
                _demoWaveData[i].param->handDemoWaveform(_demoWaveData[i].waveID, d);
            }
        }
    }
}

/**************************************************************************************************
 * 与参数绑定。
 *************************************************************************************************/
bool DemoProvider::attachParam(Param &param)
{
    const QString &name = param.getName();

    if (name == paramInfo.getParamName(PARAM_ECG))
    {
        for (int i = WAVE_ECG_I; i <= WAVE_ECG_V6; i++)
        {
            _demoWaveData[i].param = &param;
        }
        ecgParam.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_SPO2))
    {
        _demoWaveData[WAVE_SPO2].param = &param;
        spo2Param.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_RESP))
    {
        _demoWaveData[WAVE_RESP].param = &param;
        respParam.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_NIBP))
    {
        nibpParam.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_CO2))
    {
        _demoWaveData[WAVE_CO2].param = &param;
        co2Param.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_IBP))
    {
        ibpParam.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_CO))
    {
        coParam.setProvider(this);
    }
    else if (name == paramInfo.getParamName(PARAM_AG))
    {
        agParam.setProvider(this);
    }

    _demoTrendData[param.getParamID()].param = &param;

    if (_timerID == 0)
    {
        _timerID = startTimer(4); // 250Hz,4ms.
        _time.start();
    }

    return true;
}

/**************************************************************************************************
 * 与参数脱离。
 *************************************************************************************************/
void DemoProvider::detachParam(Param &param)
{
    ParamID id = param.getParamID();

    if (id == PARAM_ECG)
    {
        _demoTrendData[PARAM_ECG].param = NULL;
        for (int i = WAVE_ECG_I; i <= WAVE_ECG_V6; i++)
        {
            _demoWaveData[i].param = NULL;
        }
    }
    else if (id == PARAM_SPO2)
    {
        _demoTrendData[PARAM_SPO2].param = NULL;
        _demoWaveData[WAVE_SPO2].param = &param;
    }
    else if (id == PARAM_NIBP)
    {
        _demoTrendData[PARAM_NIBP].param = NULL;
    }
    else if (id == PARAM_CO2)
    {
        _demoTrendData[PARAM_CO2].param = NULL;
        _demoWaveData[WAVE_CO2].param = &param;
    }

    // 删除定时器。
    if (_timerID != 0)
    {
        killTimer(_timerID);
    }
}

/**************************************************************************************************
 * 从端口读取数据。
 *************************************************************************************************/
void DemoProvider::dataArrived(void)
{

}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
DemoProvider::DemoProvider() : Provider("DemoProvider")
{
    _timerID = 0;

    _waveSampleRate = 250;

    QString path("/usr/local/nPM/demodata/");

    for (int i = 0; i < WAVE_NR; i++)
    {
        _demoFile[i].setFileName(path + _demoWaveData[i].waveformName);
        _demoFile[i].open(QFile::ReadOnly);
    }
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
