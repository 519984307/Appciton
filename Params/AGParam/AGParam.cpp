/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "AGParam.h"
#include "AGWaveWidget.h"
#include "AGTrendWidget.h"
#include "AGAlarm.h"
#include "WaveformCache.h"
#include "ConfigManager.h"
#include "LayoutManager.h"
#include "SystemManager.h"
#include "AlarmSourceManager.h"

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
    _etn2o.value = 6;
    _fin2o.value = 2;
    _etaa1.value = 3;
    _fiaa1.value = 3;
    _etaa2.value = 3;
    _fiaa2.value = 3;
    _eto2.value = 6;
    _fio2.value = 2;

    if (NULL != _trendWidgetN2O)
    {
        _trendWidgetN2O->setEtData(_etn2o.value);
        _trendWidgetN2O->setFiData(_fin2o.value);
    }
    if (NULL != _trendWidgetO2)
    {
        _trendWidgetO2->setEtData(_eto2.value);
        _trendWidgetO2->setFiData(_fio2.value);
    }
    if (NULL != _trendWidgetAA1)
    {
        _trendWidgetAA1->setEtData(_etaa1.value);
        _trendWidgetAA1->setFiData(_fiaa1.value);
    }
    if (NULL != _trendWidgetAA2)
    {
        _trendWidgetAA2->setEtData(_etaa2.value);
        _trendWidgetAA2->setFiData(_fiaa2.value);
    }
}

void AGParam::exitDemo()
{
    _etn2o.value = InvData();
    _fin2o.value = InvData();
    if (NULL != _trendWidgetN2O)
    {
        _trendWidgetN2O->setEtData(InvData());
        _trendWidgetN2O->setFiData(InvData());
    }
}

void AGParam::showSubParamValue()
{
    if (NULL != _trendWidgetN2O)
    {
        _trendWidgetN2O->showValue();
    }

    if (NULL != _trendWidgetAA1)
    {
        _trendWidgetAA1->showValue();
    }

    if (NULL != _trendWidgetAA2)
    {
        _trendWidgetAA2->showValue();
    }

    if (NULL != _trendWidgetO2)
    {
        _trendWidgetO2->showValue();
    }
}

void AGParam::noticeLimitAlarm(int id, bool flag)
{
    SubParamID subID = (SubParamID)id;
    switch (subID)
    {
    case SUB_PARAM_ETN2O:
    case SUB_PARAM_FIN2O:
    {
        if (NULL != _trendWidgetN2O)
        {
            _trendWidgetN2O->isAlarm(id, flag);
        }
        break;
    }
    case SUB_PARAM_ETAA1:
    case SUB_PARAM_FIAA1:
    {
        if (NULL != _trendWidgetAA1)
        {
            _trendWidgetAA1->isAlarm(id, flag);
        }
        break;
    }
    case SUB_PARAM_ETAA2:
    case SUB_PARAM_FIAA2:
    {
        if (NULL != _trendWidgetAA2)
        {
            _trendWidgetAA2->isAlarm(id, flag);
        }
        break;
    }
    case SUB_PARAM_ETO2:
    case SUB_PARAM_FIO2:
    {
        if (NULL != _trendWidgetO2)
        {
            _trendWidgetO2->isAlarm(id, flag);
        }
        break;
    }
    default:
        break;
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
    Q_UNUSED(id)
    return true;
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

    _setWaveformZoom(AG_TYPE_N2O, getDisplayZoom(AG_TYPE_N2O));
    _setWaveformZoom(AG_TYPE_AA1, getDisplayZoom(AG_TYPE_AA1));
    _setWaveformZoom(AG_TYPE_AA2, getDisplayZoom(AG_TYPE_AA2));
    _setWaveformZoom(AG_TYPE_O2, getDisplayZoom(AG_TYPE_O2));

    // 注册波形缓存
    QString titleN2O = _waveWidgetN2O->getTitle();
    _waveWidgetN2O->setDataRate(provider->getN2OWaveformSample());
    waveformCache.registerSource(WAVE_N2O, _provider->getN2OWaveformSample(), 0,
                                 _provider->getN2OMaxWaveform(), titleN2O, _provider->getN2OBaseLine());

    QString titleAA1 = _waveWidgetAA1->getTitle();
    _waveWidgetAA1->setDataRate(provider->getAA1WaveformSample());
    waveformCache.registerSource(WAVE_AA1, _provider->getAA1WaveformSample(), 0,
                                 _provider->getAA1MaxWaveform(), titleAA1, _provider->getAA1BaseLine());

    QString titleAA2 = _waveWidgetAA2->getTitle();
    _waveWidgetAA2->setDataRate(provider->getAA2WaveformSample());
    waveformCache.registerSource(WAVE_AA2, _provider->getAA2WaveformSample(), 0,
                                 _provider->getAA2MaxWaveform(), titleAA2, _provider->getAA2BaseLine());

    QString titleO2 = _waveWidgetO2->getTitle();
    _waveWidgetO2->setDataRate(provider->getO2WaveformSample());
    waveformCache.registerSource(WAVE_O2, _provider->getO2WaveformSample(), 0,
                                 _provider->getO2MaxWaveform(), titleO2, _provider->getO2BaseLine());
}

void AGParam::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }
    _connectedProvider = isConnected;
}

bool AGParam::isConnected()
{
    return _connectedProvider;
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
    _setWaveformSpeed(getSweepSpeed());
}

/**************************************************************************************************
 * set one shot alarm.
 *************************************************************************************************/
void AGParam::setOneShotAlarm(AGOneShotType t, bool status)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_AG);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(t, status);
    }
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
            n2o++;
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
void AGParam::setDisplayZoom(AGTypeGas type, AGDisplayZoom zoom)
{
    _setWaveformZoom(type, zoom);
}

/**************************************************************************************************
 * 获取波形放大标尺。
 *************************************************************************************************/
AGDisplayZoom AGParam::getDisplayZoom(AGTypeGas type)
{
    int value = 0;
    currentConfig.getNumValue("AG|DisplayZoom", value);
    int zoom = 0;
    switch (type)
    {
    case AG_TYPE_N2O:
        zoom  = (value >> 2) & 0x3;
        break;
    case AG_TYPE_AA1:
        zoom  = (value >> 4) & 0x3;
        break;
    case AG_TYPE_AA2:
        zoom  = (value >> 6) & 0x3;
        break;
    case AG_TYPE_O2:
        zoom  = (value >> 8) & 0x3;
        break;
    default:
        break;
    }
    return static_cast<AGDisplayZoom>(zoom);
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
    currentConfig.setNumValue("AG|SweepSpeed", static_cast<int>(speed));
    _setWaveformSpeed(speed);
}

AGSweepSpeed AGParam::getSweepSpeed()
{
    int speed = AG_SWEEP_SPEED_250;
    currentConfig.getNumValue("AG|SweepSpeed", speed);
    return (AGSweepSpeed)speed;
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

    num = file.write(buf, len);

    if (num != len)
    {
        return false;
    }
    return true;
}

QList<SubParamID> AGParam::getShortTrendList(SubParamID type)
{
    QList<SubParamID> subParams;
    switch (type) {
    case SUB_PARAM_ETN2O:
        subParams = _trendWidgetN2O->getShortTrendSubParams();
        break;
    case SUB_PARAM_ETAA1:
        subParams = _trendWidgetAA1->getShortTrendSubParams();
        break;
    case SUB_PARAM_ETAA2:
        subParams = _trendWidgetAA2->getShortTrendSubParams();
        break;
    case SUB_PARAM_ETO2:
        subParams = _trendWidgetO2->getShortTrendSubParams();
        break;
    default:
        break;
    }
    return subParams;
}

void AGParam::updateSubParamLimit(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ETN2O:
        _trendWidgetN2O->updateLimit();
        break;
    case SUB_PARAM_ETAA1:
        _trendWidgetAA1->updateLimit();
        break;
    case SUB_PARAM_ETAA2:
        _trendWidgetAA2->updateLimit();
        break;
    case SUB_PARAM_ETO2:
        _trendWidgetO2->updateLimit();
        break;
    default:
        break;
    }
}

void AGParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_AG || !systemManager.isSupport(CONFIG_AG))
    {
        return;
    }
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_AG));
    _waveWidgetAA1->updatePalette(psrc);
    _waveWidgetAA2->updatePalette(psrc);
    _waveWidgetN2O->updatePalette(psrc);
    _waveWidgetO2->updatePalette(psrc);
    _trendWidgetAA1->updatePalette(psrc);
    _trendWidgetAA2->updatePalette(psrc);
    _trendWidgetN2O->updatePalette(psrc);
    _trendWidgetO2->updatePalette(psrc);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGParam::AGParam() : Param(PARAM_AG), _connectedProvider(false)
{
    _provider = NULL;

    _waveWidgetN2O = NULL;
    _waveWidgetAA1 = NULL;
    _waveWidgetAA2 = NULL;
    _waveWidgetO2 = NULL;

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

    QStringList currentWaveforms = layoutManager.getDisplayedWaveforms();
    if (currentWaveforms.contains(_waveWidgetAA1->name()) || currentWaveforms.contains(_waveWidgetAA2->name())
            || currentWaveforms.contains(_waveWidgetN2O->name()) || currentWaveforms.contains(_waveWidgetO2->name()))
    {
        layoutManager.resetWave();
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void AGParam::_setWaveformZoom(AGTypeGas type, AGDisplayZoom zoom)
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

    int upLimit = 0;
    switch (zoom)
    {
    case AG_DISPLAY_ZOOM_4:
        upLimit = 400;
        break;
    case AG_DISPLAY_ZOOM_8:
        upLimit = 800;
        break;
    case AG_DISPLAY_ZOOM_15:
        upLimit = 1500;
        break;
    default:
        break;
    }

    int value = 0;
    currentConfig.getNumValue("AG|DisplayZoom", value);
    int n2o = (value >> 2) & 0x3;
    int aa1 = (value >> 4) & 0x3;
    int aa2 = (value >> 6) & 0x3;
    int o2 = (value >> 8) & 0x3;

    switch (type)
    {
    case AG_TYPE_N2O:
        n2o = zoom;
        _waveWidgetN2O->setValueRange(0, upLimit);
        _waveWidgetN2O->setRuler(zoom);
        break;
    case AG_TYPE_AA1:
        aa1 = zoom;
        _waveWidgetAA1->setValueRange(0, upLimit);
        _waveWidgetAA1->setRuler(zoom);
        break;
    case AG_TYPE_AA2:
        aa2 = zoom;
        _waveWidgetAA2->setValueRange(0, upLimit);
        _waveWidgetAA2->setRuler(zoom);
        break;
    case AG_TYPE_O2:
        o2 = zoom;
        _waveWidgetO2->setValueRange(0, upLimit);
        _waveWidgetO2->setRuler(zoom);
        break;
    default:
        break;
    }
    value = (n2o << 2) | (aa1 << 4) | (aa2 << 6) | (o2 << 8);
    currentConfig.setNumValue("AG|DisplayZoom", value);
}
