#include "AGParam.h"
#include "AGWaveWidget.h"
#include "AGTrendWidget.h"
#include "AGAlarm.h"
#include "WindowManager.h"
#include "WaveformCache.h"

#define DEMO_DATA_NUM       180

AGParam *AGParam::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGParam::~AGParam()
{

}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void AGParam::handDemoWaveform(WaveformID id, short data)
{
    data = data * 10;
    switch (id)
    {
    case WAVE_N2O:
        _waveWidgetN2O->addData(data);
        break;
    case WAVE_AA1:
        _waveWidgetAA1->addData(data);
        break;
    case WAVE_AA2:
        _waveWidgetAA2->addData(data);
        break;
    case WAVE_O2:
        _waveWidgetO2->addData(data);
        break;
    default:
        return;
    }
    waveformCache.addData(id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void AGParam::handDemoTrendData()
{
    _etn2o.value = qrand() % 20 + 20;
    _fin2o.value = qrand() % 10 + 10;

    if (NULL != _trendWidgetN2O)
    {
        _trendWidgetN2O->setEtData(_etn2o.value);
        _trendWidgetN2O->setFiData(_fin2o.value);
    }
}

void AGParam::getAvailableWaveforms(QStringList &waveforms, QStringList &waveformShowName, int)
{
    waveforms.clear();
    waveformShowName.clear();

    if (NULL != _waveWidgetN2O && NULL != _waveWidgetO2 &&
            NULL != _waveWidgetAA1 && NULL != _waveWidgetAA2)
    {
        waveforms.append(_waveWidgetN2O->name());
        waveforms.append(_waveWidgetO2->name());
        waveforms.append(_waveWidgetAA1->name());
        waveforms.append(_waveWidgetAA2->name());
        waveformShowName.append(_waveWidgetN2O->getTitle());
        waveformShowName.append(_waveWidgetO2->getTitle());
        waveformShowName.append(_waveWidgetAA1->getTitle());
        waveformShowName.append(_waveWidgetAA2->getTitle());
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType AGParam::getCurrentUnit(SubParamID /*id*/)
{
    return UNIT_PERCENT;
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short AGParam::getSubParamValue(SubParamID id)
{
    short value;
    switch (id)
    {
    case SUB_PARAM_ETN2O:
        value = getEtData(AG_TYPE_N2O);
        break;
    case SUB_PARAM_FIN2O:
        value = getFiData(AG_TYPE_N2O);
        break;
    case SUB_PARAM_ETAA1:
        value = getEtData(AG_TYPE_AA1);
        break;
    case SUB_PARAM_FIAA1:
        value = getFiData(AG_TYPE_AA1);
        break;
    case SUB_PARAM_ETAA2:
        value = getEtData(AG_TYPE_AA2);
        break;
    case SUB_PARAM_FIAA2:
        value = getFiData(AG_TYPE_AA2);
        break;
    case SUB_PARAM_ETO2:
        value = getEtData(AG_TYPE_O2);
        break;
    case SUB_PARAM_FIO2:
        value = getFiData(AG_TYPE_O2);
        break;
    default:
        return InvData();
    }
    return value;
}

/**************************************************************************************************
 * 功能： 判断自参数是否有效。
 *************************************************************************************************/
bool AGParam::isSubParamAvaliable(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ETCO2:
    case SUB_PARAM_FICO2:
        return _config.co2Config;
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_FIN2O:
        return _config.n2oConfig;
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_FIAA1:
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_FIAA2:
        return _config.halConfig || _config.enfConfig || _config.isoConfig ||
                _config.sevConfig || _config.desConfig;
    case SUB_PARAM_ETO2:
    case SUB_PARAM_FIO2:
        return _config.o2Config;
    default:
        break;
    }
    return false;
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void AGParam::setProvider(AGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    if (_waveWidgetN2O == NULL || _waveWidgetAA1 == NULL ||
            _waveWidgetAA2 == NULL || _waveWidgetO2 == NULL)
    {
        return;
    }
    _provider = provider;

    _setWaveformZoom(AG_DISPLAY_ZOOM_4);

    // 注册波形缓存
    QString titleN2O = _waveWidgetN2O->getTitle();
    waveformCache.registerSource(WAVE_N2O, _provider->getN2OWaveformSample(), 0,
                                 _provider->getN2OMaxWaveform(), titleN2O, _provider->getN2OBaseLine());

    QString titleAA1 = _waveWidgetAA1->getTitle();
    waveformCache.registerSource(WAVE_AA1, _provider->getAA1WaveformSample(), 0,
                                 _provider->getAA1MaxWaveform(), titleAA1, _provider->getAA1BaseLine());

    QString titleAA2 = _waveWidgetAA2->getTitle();
    waveformCache.registerSource(WAVE_AA2, _provider->getAA2WaveformSample(), 0,
                                 _provider->getAA2MaxWaveform(), titleAA2, _provider->getAA2BaseLine());

    QString titleO2 = _waveWidgetO2->getTitle();
    waveformCache.registerSource(WAVE_O2, _provider->getO2WaveformSample(), 0,
                                 _provider->getO2MaxWaveform(), titleO2, _provider->getO2BaseLine());

}

/**************************************************************************************************
 * 设置趋势对象。
 *************************************************************************************************/
void AGParam::setTrendWidget(AGTrendWidget *trendWidget, AGTypeGas gasType)
{
    if (trendWidget == NULL)
    {
        return;
    }

    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        break;
    }
    case AG_TYPE_N2O:
    {
        _trendWidgetN2O = trendWidget;
        break;
    }
    case AG_TYPE_AA1:
    {
        _trendWidgetAA1 = trendWidget;
        break;
    }
    case AG_TYPE_AA2:
    {
        _trendWidgetAA2 = trendWidget;
        break;
    }
    case AG_TYPE_O2:
    {
        _trendWidgetO2 = trendWidget;
        break;
    }
    default:
        break;
    }
}

/**************************************************************************************************
 * 设置波形对象。
 *************************************************************************************************/
void AGParam::setWaveWidget(AGWaveWidget *waveWidget, AGTypeGas gasType)
{
    if (waveWidget == NULL)
    {
        return;
    }

    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        break;
    }
    case AG_TYPE_N2O:
    {
        _waveWidgetN2O = waveWidget;
        break;
    }
    case AG_TYPE_AA1:
    {
        _waveWidgetAA1 = waveWidget;
        break;
    }
    case AG_TYPE_AA2:
    {
        _waveWidgetAA2 = waveWidget;
        break;
    }
    case AG_TYPE_O2:
    {
        _waveWidgetO2 = waveWidget;
        break;
    }
    default:
        break;
    }
    waveWidget->setLimit(0, 1500);
}

/**************************************************************************************************
 * set one shot alarm.
 *************************************************************************************************/
void AGParam::setOneShotAlarm(AGOneShotType t, bool status)
{
    agOneShotAlarm.setOneShotAlarm(t, status);
}


/**************************************************************************************************
 * set apnean time.
 *************************************************************************************************/
void AGParam::setApneaTime(ApneaAlarmTime /*t*/)
{
    if (_provider != NULL)
    {
//        _provider->setApneaTimeout(t);
    }
}

/**************************************************************************************************
 * get apnean time.
 *************************************************************************************************/
ApneaAlarmTime AGParam::getApneaTime()
{
    int t = APNEA_ALARM_TIME_20_SEC;
    return (ApneaAlarmTime)t;
}

/**************************************************************************************************
 * check mode and host apnean time difference.
 *************************************************************************************************/
void AGParam::verifyApneanTime(ApneaAlarmTime time)
{
    if (getApneaTime() == APNEA_ALARM_TIME_OFF)
    {
        return;
    }

    if (getApneaTime() != time)
    {
        if (_provider != NULL)
        {
//            _provider->setApneaTimeout(getApneaTime());
        }
    }
}

/**************************************************************************************************
 * check mode and host work mode difference.
 *************************************************************************************************/
void AGParam::verifyWorkMode(AGWorkMode mode)
{
    if (mode == AG_WORK_SELFTEST)
    {
        return;
    }

    if (NULL == _provider)
    {
        return;
    }

    // 主机为打开状态，当模块为sleep。
//    if (getCO2Switch() && (mode == AG_WORK_SLEEP))
//    {
//        _provider->setWorkMode(CO2_WORK_MEASUREMENT);
//    }

    // 主机为关闭状态，当模块为测量。
//    if (!getCO2Switch() && (mode == AG_WORK_MEASUREMENT))
//    {
//        _provider->setWorkMode(C02_WORK_SLEEP);
//    }
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void AGParam::addWaveformData(short wave, bool invalid, AGTypeGas gasType)
{
    int flag = 0;
    if (invalid)
    {
        flag = 0x4000;
        wave = 0;
    }

    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        break;
    }
    case AG_TYPE_N2O:
    {
        if (_waveWidgetN2O != NULL)
        {
            _waveWidgetN2O->addWaveformData(wave, flag);
        }

        if (n2o < DEMO_DATA_NUM)
        {
            _n2oBuf[n2o] = wave / 10;
            n2o ++;
        }
        if (n2o == DEMO_DATA_NUM)
        {
            getDemoWaveformFile(_n2oBuf, n2o, AG_TYPE_N2O);
            n2o = DEMO_DATA_NUM + 1;
        }
        break;
    }
    case AG_TYPE_AA1:
    {
        if (_waveWidgetAA1 != NULL)
        {
            _waveWidgetAA1->addWaveformData(wave, flag);
        }
        break;
    }
    case AG_TYPE_AA2:
    {
        if (_waveWidgetAA2 != NULL)
        {
            _waveWidgetAA2->addWaveformData(wave, flag);
        }
        break;
    }
    case AG_TYPE_O2:
    {
        if (_waveWidgetO2 != NULL)
        {
            _waveWidgetO2->addWaveformData(wave, flag);
        }
        break;
    }
    default:
        break;
    }
}

/**************************************************************************************************
 * 设置波形放大标尺。
 *************************************************************************************************/
void AGParam::setDisplayZoom(AGDisplayZoom zoom)
{
    _setWaveformZoom(zoom);
}

/**************************************************************************************************
 * 获取波形放大标尺。
 *************************************************************************************************/
AGDisplayZoom AGParam::getDisplayZoom()
{
    return AG_DISPLAY_ZOOM_4;
}

/**************************************************************************************************
 * get et value.
 *************************************************************************************************/
short AGParam::getEtData(AGTypeGas gasType)
{
    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        return -1;
    }
    case AG_TYPE_N2O:
    {
        return _etn2o.value;
    }
    case AG_TYPE_AA1:
    {
        return _etaa1.value;
    }
    case AG_TYPE_AA2:
    {
        return _etaa2.value;
    }
    case AG_TYPE_O2:
    {
        return _eto2.value;
    }
    default:
        return -1;
    }
}

/**************************************************************************************************
 * set et value.
 *************************************************************************************************/
void AGParam::setEtData(short etValue, AGTypeGas gasType)
{
    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        break;
    }
    case AG_TYPE_N2O:
    {
        if (_trendWidgetN2O != NULL)
        {
            _etn2o.value = etValue;
            _trendWidgetN2O->setEtData(etValue);
        }
        break;
    }
    case AG_TYPE_AA1:
    {
        if (_trendWidgetAA1 != NULL)
        {
            _etaa1.value = etValue;
            _trendWidgetAA1->setEtData(etValue);
        }
        break;
    }
    case AG_TYPE_AA2:
    {
        if (_trendWidgetAA2 != NULL)
        {
            _etaa2.value = etValue;
            _trendWidgetAA2->setEtData(etValue);
        }
        break;
    }
    case AG_TYPE_O2:
    {
        if (_trendWidgetO2 != NULL)
        {
            _eto2.value = etValue;
            _trendWidgetO2->setEtData(etValue);
        }
        break;
    }
    default:
        break;
    }
}

/**************************************************************************************************
 * get fi value.
 *************************************************************************************************/
short AGParam::getFiData(AGTypeGas gasType)
{
    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        return -1;
    }
    case AG_TYPE_N2O:
    {
        return _fin2o.value;
    }
    case AG_TYPE_AA1:
    {
        return _fiaa1.value;
    }
    case AG_TYPE_AA2:
    {
        return _fiaa2.value;
    }
    case AG_TYPE_O2:
    {
        return _fio2.value;
    }
    default:
        return -1;
    }
}

/**************************************************************************************************
 * set fi value.
 *************************************************************************************************/
void AGParam::setFiData(short fiValue, AGTypeGas gasType)
{
    switch (gasType)
    {
    case AG_TYPE_CO2:
    {
        break;
    }
    case AG_TYPE_N2O:
    {
        if (_trendWidgetN2O != NULL)
        {
            _fin2o.value = fiValue;
            _trendWidgetN2O->setFiData(fiValue);
        }
        break;
    }
    case AG_TYPE_AA1:
    {
        if (_trendWidgetAA1 != NULL)
        {
            _fiaa1.value = fiValue;
            _trendWidgetAA1->setFiData(fiValue);
        }
        break;
    }
    case AG_TYPE_AA2:
    {
        if (_trendWidgetAA2 != NULL)
        {
            _fiaa2.value = fiValue;
            _trendWidgetAA2->setFiData(fiValue);
        }
        break;
    }
    case AG_TYPE_O2:
    {
        if (_trendWidgetO2 != NULL)
        {
            _fio2.value = fiValue;
            _trendWidgetO2->setFiData(fiValue);
        }
        break;
    }
    default:
        break;
    }
}

/**************************************************************************************************
 * 设置主麻醉和次麻醉的麻醉剂类型。
 *************************************************************************************************/
void AGParam::setAnestheticType(AGAnaestheticType primary, AGAnaestheticType secondary)
{
    _waveWidgetAA1->setAnestheticType(primary);
    _waveWidgetAA2->setAnestheticType(secondary);

    _trendWidgetAA1->setAnestheticType(primary);
    _trendWidgetAA2->setAnestheticType(secondary);
}

/**************************************************************************************************
 * 设置扫描速度。
 *************************************************************************************************/
void AGParam::setSweepSpeed(AGSweepSpeed speed)
{
    _setWaveformSpeed(speed);
}

/**************************************************************************************************
 * 获取AG模块的配置信息。
 *************************************************************************************************/
void AGParam::AGModuleStatus(AGProviderStatus status)
{
    _config = status;
}

bool AGParam::getDemoWaveformFile(const char *buf, int len, AGTypeGas type)
{
    QString path("/usr/local/nPM/demodata/");
    switch (type)
    {
    case AG_TYPE_N2O:
        path += "N2O";
        break;
    case AG_TYPE_AA1:
        path += "AA1";
        break;
    case AG_TYPE_AA2:
        path += "AA2";
        break;
    case AG_TYPE_O2:
        path += "02";
        break;
    default:
        return false;
    }
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly))
    {
        return false;
    }
    int num;

    num = file.write((char *)buf, len);

    if (num != len)
    {
        return false;
    }
    return true;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGParam::AGParam() : Param(PARAM_AG)
{
    _provider = NULL;

    _waveWidgetN2O = NULL;
    _waveWidgetAA1 = NULL;
    _waveWidgetAA2 = NULL;
    _waveWidgetO2 = NULL ;

    _trendWidgetN2O = NULL;
    _trendWidgetAA1 = NULL;
    _trendWidgetAA2 = NULL;
    _trendWidgetO2 = NULL;

    _n2oBuf = new char[DEMO_DATA_NUM];
    n2o = 0;

}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void AGParam::_setWaveformSpeed(AGSweepSpeed speed)
{
    if (_waveWidgetN2O == NULL || _waveWidgetAA1 == NULL || _waveWidgetAA2 == NULL || _waveWidgetO2 == NULL)
    {
        return;
    }
    switch (speed)
    {
    case AG_SWEEP_SPEED_62_5:
        _waveWidgetN2O->setWaveSpeed(6.25);
        _waveWidgetAA1->setWaveSpeed(6.25);
        _waveWidgetAA2->setWaveSpeed(6.25);
        _waveWidgetO2->setWaveSpeed(6.25);
        break;

    case AG_SWEEP_SPEED_125:
        _waveWidgetN2O->setWaveSpeed(12.5);
        _waveWidgetAA1->setWaveSpeed(12.5);
        _waveWidgetAA2->setWaveSpeed(12.5);
        _waveWidgetO2->setWaveSpeed(12.5);
        break;

    case AG_SWEEP_SPEED_250:
        _waveWidgetN2O->setWaveSpeed(25);
        _waveWidgetAA1->setWaveSpeed(25);
        _waveWidgetAA2->setWaveSpeed(25);
        _waveWidgetO2->setWaveSpeed(25);
        break;

    case AG_SWEEP_SPEED_500:
        _waveWidgetN2O->setWaveSpeed(50);
        _waveWidgetAA1->setWaveSpeed(50);
        _waveWidgetAA2->setWaveSpeed(50);
        _waveWidgetO2->setWaveSpeed(50);
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
        if (currentWaveforms[i] == "N2OWaveWidget" || currentWaveforms[i] == "AA1WaveWidget" ||
                currentWaveforms[i] == "AA2WaveWidget" || currentWaveforms[i] == "O2WaveWidget")
        {
            windowManager.resetWave();
            break;
        }
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void AGParam::_setWaveformZoom(AGDisplayZoom zoom)
{
    if (_provider == NULL)
    {
        return;
    }
    if (_waveWidgetN2O == NULL || _waveWidgetAA1 == NULL ||
            _waveWidgetAA2 == NULL || _waveWidgetO2 == NULL)
    {
        return;
    }

    switch (zoom)
    {
    case AG_DISPLAY_ZOOM_4:
        _waveWidgetN2O->setValueRange(0, 400);
        _waveWidgetAA1->setValueRange(0, 400);
        _waveWidgetAA2->setValueRange(0, 400);
        _waveWidgetO2->setValueRange(0, 400);
        break;
    case AG_DISPLAY_ZOOM_8:
        _waveWidgetN2O->setValueRange(0, 800);
        _waveWidgetAA1->setValueRange(0, 800);
        _waveWidgetAA2->setValueRange(0, 800);
        _waveWidgetO2->setValueRange(0, 800);
        break;
    case AG_DISPLAY_ZOOM_15:
        _waveWidgetN2O->setValueRange(0, 1500);
        _waveWidgetAA1->setValueRange(0, 1500);
        _waveWidgetAA2->setValueRange(0, 1500);
        _waveWidgetO2->setValueRange(0, 1500);
        break;
    default:
        break;
    }
    _waveWidgetN2O->setRuler(zoom);
    _waveWidgetAA1->setRuler(zoom);
    _waveWidgetAA2->setRuler(zoom);
    _waveWidgetO2->setRuler(zoom);
}
