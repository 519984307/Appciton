/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "SPO2TrendWidget.h"
#include "SPO2WaveWidget.h"
#include "SPO2ProviderIFace.h"
#include "IConfig.h"
#include "WaveformCache.h"
#include "ECGDupParam.h"
#include "SystemManager.h"
#include "ComboListPopup.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include <QTimer>
#include "OxyCRGSPO2TrendWidget.h"
#include "NIBPParam.h"

SPO2Param *SPO2Param::_selfObj = NULL;

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void SPO2Param::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId, data, len);
    }
}

void SPO2Param::setAverageTime(AverageTime index)
{
    currentConfig.setNumValue("SPO2|AverageTime", static_cast<int>(index));
    if (NULL != _provider)
    {
        _provider->setAverageTime(index);
    }
}

AverageTime SPO2Param::getAverageTime()
{
    int time = SPO2_AVER_TIME_8SEC;
    currentConfig.getNumValue("SPO2|AverageTime", time);
    return (AverageTime)time;
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void SPO2Param::_setWaveformSpeed(SPO2WaveVelocity speed)
{
    if (_waveWidget == NULL)
    {
        return;
    }

    switch (speed)
    {
    case SPO2_WAVE_VELOCITY_62D5:
        _waveWidget->setWaveSpeed(6.25);
        break;

    case SPO2_WAVE_VELOCITY_125:
        _waveWidget->setWaveSpeed(12.5);
        break;

    case SPO2_WAVE_VELOCITY_250:
        _waveWidget->setWaveSpeed(25.0);
        break;

    case SPO2_WAVE_VELOCITY_500:
        _waveWidget->setWaveSpeed(50.0);
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void SPO2Param::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void SPO2Param::handDemoWaveform(WaveformID id, short data)
{
    if (id != WAVE_SPO2)
    {
        return;
    }
    if (NULL != _waveWidget)
    {
        _waveWidget->addData(data);
    }
    if (NULL != _trendWidget)
    {
        _trendWidget->setBarValue(data * 15 / 255);
    }
    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void SPO2Param::handDemoTrendData(void)
{
    _spo2Value = 98;
    _piValue = 41;
    if (NULL != _trendWidget)
    {
        _trendWidget->setSPO2Value(_spo2Value);
        _trendWidget->setPIValue(_piValue);
    }

    if (NULL != _oxyCRGSPO2Trend)
    {
        _oxyCRGSPO2Trend->addTrendData(_spo2Value);
    }

    int prValue = 60;
    setPR(prValue);
}

void SPO2Param::exitDemo()
{
    _spo2Value = InvData();
    if (NULL != _trendWidget)
    {
        _trendWidget->setSPO2Value(InvData());
        _trendWidget->setPIValue(InvData());
        _trendWidget->setBarValue(InvData());
    }

    setPR(InvData());
}

/**************************************************************************************************
 * 获取可得的波形控件集。
 *************************************************************************************************/
void SPO2Param::getAvailableWaveforms(QStringList &waveforms,
                                      QStringList &waveformShowName, int /*flag*/)
{
    waveforms.clear();
    waveformShowName.clear();

    if (NULL != _waveWidget)
    {
        waveforms.append(_waveWidget->name());
    }
    waveformShowName.append(trs("PLETH"));
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short SPO2Param::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_SPO2:
        return getSPO2();

    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void SPO2Param::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType SPO2Param::getCurrentUnit(SubParamID /*id*/)
{
    return UNIT_PERCENT;
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void SPO2Param::setProvider(SPO2ProviderIFace *provider)
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

    _waveWidget->setDataRate(_provider->getSPO2WaveformSample());

    QString str;
    machineConfig.getStrValue("SPO2", str);
    if (str == "BLM_TS3")
    {
        //设置灵敏度
        _provider->setSensitivityFastSat(static_cast<SensitivityMode>(getSensitivity()), getFastSat());
    }
    else if (str == "MASIMO_SPO2" || str == "RAINBOW_SPO2")
    {
        _provider->setSensitivityFastSat(SPO2_MASIMO_SENS_NORMAL, false);
        _provider->setAverageTime(SPO2_AVER_TIME_8SEC);

        SPO2SMARTPLUSETONE pulseTone = getSmartPulseTone();
        if (pulseTone == SPO2_SMART_PLUSE_TONE_ON)
        {
            _provider->setSmartTone(true);
        }
        else if (pulseTone == SPO2_SMART_PLUSE_TONE_OFF)
        {
            _provider->setSmartTone(false);
        }
    }

    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        _waveWidget->setNotify(false, "");
    }

    //查询状态
    _provider->sendStatus();

    QString tile = _waveWidget->getTitle();
    // 请求波形缓冲区。
    waveformCache.registerSource(WAVE_SPO2, _provider->getSPO2WaveformSample(), 0, _provider->getSPO2MaxValue(),
                                 tile, _provider->getSPO2BaseLine());

    // update spo2 value range
    _waveWidget->setValueRange(0, _provider->getSPO2MaxValue());
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void SPO2Param::reset()
{
    if (NULL == _provider)
    {
        return;
    }

    //设置灵敏度
    _provider->setSensitivityFastSat(static_cast<SensitivityMode>(getSensitivity()), getFastSat());

    //查询状态
    _provider->sendStatus();
}

/**************************************************************************************************
 * 取值范围。
 *************************************************************************************************/
int SPO2Param::getSPO2MaxValue()
{
    if (NULL != _provider)
    {
        return _provider->getSPO2MaxValue();
    }
    else
    {
        return 0;
    }
}

/**************************************************************************************************
 * 设置服务模式升级数据提供对象。
 *************************************************************************************************/
void SPO2Param::setServiceProvider(SPO2ProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void SPO2Param::setTrendWidget(SPO2TrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _trendWidget = trendWidget;
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void SPO2Param::setWaveWidget(SPO2WaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    _waveWidget = waveWidget;
    _setWaveformSpeed((SPO2WaveVelocity)getSweepSpeed());
}

void SPO2Param::setOxyCRGSPO2Trend(OxyCRGSPO2TrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _oxyCRGSPO2Trend = trendWidget;
}

/**************************************************************************************************
 * 获取QRS/PR音量。
 *************************************************************************************************/
SoundManager::VolumeLevel SPO2Param::getPluseToneVolume(void)
{
    int vol = SoundManager::VOLUME_LEV_3;
    currentConfig.getNumValue("ECG|QRSToneVolume", vol);
    return (SoundManager::VolumeLevel)vol;
}

/**************************************************************************************************
 * 设置SPO2的值。
 *************************************************************************************************/
void SPO2Param::setSPO2(short spo2Value)
{
    if (_spo2Value == spo2Value)
    {
        return;
    }
    _spo2Value = spo2Value;
    if (NULL != _trendWidget)
    {
        _trendWidget->setSPO2Value(_spo2Value);
    }

    if (NULL != _oxyCRGSPO2Trend)
    {
        _oxyCRGSPO2Trend->addTrendData(_spo2Value);
    }
}

/**************************************************************************************************
 * 获取SPO2的值。
 *************************************************************************************************/
short SPO2Param::getSPO2(void)
{
    return _spo2Value;
}

/**************************************************************************************************
 * 设置PR的值。
 *************************************************************************************************/
void SPO2Param::setPR(short prValue)
{
    if (_prValue == prValue)
    {
        return;
    }

    if (prValue < 0)
    {
        prValue = InvData();
    }

    _prValue = prValue;
    ecgDupParam.updatePR(prValue);
}

void SPO2Param::updatePIValue(short piValue)
{
    if (_piValue == piValue)
    {
        return;
    }
    _piValue = piValue;
    if (NULL != _trendWidget)
    {
        _trendWidget->setPIValue(_piValue);
    }
}

/**************************************************************************************************
 * 设置波形值。
 *************************************************************************************************/
void SPO2Param::addWaveformData(short wave)
{
    int flag = 0;
    if (!_isValid)
    {
        flag = 0x4000;
    }

    if (NULL != _waveWidget)
    {
        _waveWidget->addData(wave, flag);
    }
    if (NULL != _trendWidget)
    {
        _trendWidget->setBarValue(wave * 15 / 255);
    }
    waveformCache.addData(WAVE_SPO2, (flag << 16) | wave);
}

/**************************************************************************************************
 * 设置棒图值。
 *************************************************************************************************/
void SPO2Param::addBarData(short data)
{
    if (_barValue == data)
    {
        return;
    }
    _barValue = data;
    if (NULL != _trendWidget)
    {
        _trendWidget->setBarValue(data);
    }
}

/**************************************************************************************************
 * 设置脉搏音标志。
 *************************************************************************************************/
void SPO2Param::setPulseAudio(bool pulse)
{
    HRSourceType type = ecgDupParam.getCurHRSource();
    if (pulse && ecgDupParam.getHR() != InvData() &&
        (type == HR_SOURCE_SPO2 || type == HR_SOURCE_AUTO))
    {
        soundManager.pulseTone(getSmartPulseTone() == SPO2_SMART_PLUSE_TONE_ON
                               ? getSPO2()
                               : -1);
        ecgDupParam.updatePRBeatIcon();
    }
}

void SPO2Param::setBeatVol(SoundManager::VolumeLevel vol)
{
    // 将脉搏音与心跳音绑定在一起，形成联动
    currentConfig.setNumValue("SPO2|BeatVol", static_cast<int>(vol));
    currentConfig.setNumValue("ECG|QRSVolume", static_cast<int>(vol));
    soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT, vol);
}

SoundManager::VolumeLevel SPO2Param::getBeatVol() const
{
    int vol = SoundManager::VOLUME_LEV_2;
    currentConfig.getNumValue("SPO2|BeatVol", vol);
    return static_cast<SoundManager::VolumeLevel>(vol);
}

void SPO2Param::setNotify(bool enable, QString str)
{
    if (NULL != _waveWidget)
    {
        if (_isEverCheckFinger)
        {
            _waveWidget->setNotify(enable, str);
        }
    }
}

/**************************************************************************************************
 * 设置搜索脉搏标志。
 *************************************************************************************************/
void SPO2Param::setSearchForPulse(bool isSearching)
{
    // _trendWidget->setSearchForPulse(isSearching);
    if (NULL != _waveWidget)
    {
        _waveWidget->setNotify(isSearching, trs("SPO2PulseSearch"));
    }

    if (isSearching && !_isEverCheckFinger)
    {
        _isEverCheckFinger = true;
        systemConfig.setNumValue("PrimaryCfg|SPO2|EverCheckFinger", true);
    }
}

/**************************************************************************************************
 * 设置报警。
 *************************************************************************************************/
void SPO2Param::setOneShotAlarm(SPO2OneShotType t, bool f)
{
    spo2OneShotAlarm.setOneShotAlarm(t, f);
}

/**************************************************************************************************
 * 设置报警。
 *************************************************************************************************/
void SPO2Param::noticeLimitAlarm(bool isAlarm)
{
    if (isNibpSameSide() && nibpParam.isMeasuring())
    {
        // 如果打开同侧功能，且nibp正在测量，则不设置报警
        return;
    }
    if (NULL != _trendWidget)
    {
        _trendWidget->isAlarm(isAlarm);
    }
}

/**************************************************************************************************
 * 状态0x42。
 *************************************************************************************************/
void SPO2Param::setValidStatus(bool isValid)
{
    _isValid = isValid;
}

/**************************************************************************************************
 * 状态是否有效。
 *************************************************************************************************/
bool SPO2Param::isValid()
{
    return _isValid;
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool SPO2Param::isConnected()
{
    return _connectedProvider;
}

void SPO2Param::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }
    _connectedProvider = isConnected;
}

/**************************************************************************************************
 * set sensor on/off
 *************************************************************************************************/
int SPO2Param::setSensorOff(bool flag)
{
    if (_sensorOff == flag)
    {
        return -1;
    }

    _sensorOff = flag;
    if (!_isEverSensorOn && !_sensorOff)
    {
        _isEverSensorOn = true;
        systemConfig.setNumValue("PrimaryCfg|SPO2|EverSensorOn", _isEverSensorOn);
    }
    return 0;
}

/**************************************************************************************************
 * receive package
 *************************************************************************************************/
void SPO2Param::receivePackage()
{
    if (_recPackageInPowerOn2sec < 5)
    {
        ++_recPackageInPowerOn2sec;
    }
}

/**************************************************************************************************
 * check selftest
 *************************************************************************************************/
void SPO2Param::checkSelftest()
{
    if (!systemManager.isSupport(CONFIG_SPO2))
    {
        return;
    }

    QString str;
    machineConfig.getStrValue("SPO2", str);
    if (str == "BLM_TS3")
    {
        if (_recPackageInPowerOn2sec == 5)
        {
            systemManager.setPoweronTestResult(TS3_MODULE_SELFTEST_RESULT, SELFTEST_SUCCESS);
        }
        else
        {
            systemManager.setPoweronTestResult(TS3_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
            ErrorLogItem *item = new CriticalFaultLogItem();
            item->setName("SPO2 Module SelfTest Failed");
            errorLog.append(item);
        }
    }
    else if (str == "BLM_S5")
    {
        if (_recPackageInPowerOn2sec == 5)
        {
            systemManager.setPoweronTestResult(S5_MODULE_SELFTEST_RESULT, SELFTEST_SUCCESS);
        }
        else
        {
            systemManager.setPoweronTestResult(S5_MODULE_SELFTEST_RESULT, SELFTEST_FAILED);
            ErrorLogItem *item = new CriticalFaultLogItem();
            item->setName("SPO2 Module SelfTest Failed");
            errorLog.append(item);
        }
    }
}

void SPO2Param::onPaletteChanged(ParamID id)
{
    if (id != PARAM_SPO2 || !systemManager.isSupport(CONFIG_SPO2))
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_SPO2));
    _waveWidget->updatePalette(pal);
    _trendWidget->updatePalette(pal);
}

void SPO2Param::setSensitivity(int sens)
{
    currentConfig.setNumValue("SPO2|Sensitivity", static_cast<int>(sens));
    if (NULL != _provider)
    {
        if (_moduleType == MODULE_MASIMO_SPO2 || _moduleType == MODULE_RAINBOW_SPO2)
        {
            _provider->setSensitivityFastSat(static_cast<SensitivityMode>(sens), getFastSat());
        }
        else if (_moduleType != MODULE_SPO2_NR)
        {
            _provider->setSensitive(static_cast<SPO2Sensitive>(sens));
        }
    }
}

int SPO2Param::getSensitivity(void)
{
    int sens = 0;
    currentConfig.getNumValue("SPO2|Sensitivity", sens);
    return sens;
}

void SPO2Param::setFastSat(bool isFast)
{
    currentConfig.setNumValue("SPO2|FastSat", static_cast<int>(isFast));
    if (NULL != _provider)
    {
        _provider->setSensitivityFastSat(static_cast<SensitivityMode>(getSensitivity()), isFast);
    }
}

bool SPO2Param::getFastSat()
{
    int fastSat = false;
    currentConfig.getNumValue("SPO2|FastSat", fastSat);
    return static_cast<bool>(fastSat);
}

/**************************************************************************************************
 * 设置智能音。
 *************************************************************************************************/
void SPO2Param::setSmartPulseTone(SPO2SMARTPLUSETONE sens)
{
    if (_provider)
    {
        if (sens == SPO2_SMART_PLUSE_TONE_ON)
        {
            _provider->setSmartTone(true);
        }
        else if (sens == SPO2_SMART_PLUSE_TONE_OFF)
        {
            _provider->setSmartTone(false);
        }
    }
    currentConfig.setNumValue("SPO2|SmartPluseTone", static_cast<int>(sens));
}

/**************************************************************************************************
 * 获取智能音。
 *************************************************************************************************/
SPO2SMARTPLUSETONE SPO2Param::getSmartPulseTone(void)
{
    int sens = SPO2_SMART_PLUSE_TONE_OFF;
    currentConfig.getNumValue("SPO2|SmartPluseTone", sens);
    return (SPO2SMARTPLUSETONE)sens;
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void SPO2Param::setSweepSpeed(int speed)
{
    currentConfig.setNumValue("SPO2|SweepSpeed", speed);
    _setWaveformSpeed((SPO2WaveVelocity)speed);
}

/**************************************************************************************************
 * 获取波形速度。
 *************************************************************************************************/
int SPO2Param::getSweepSpeed(void)
{
    int speed = SPO2_WAVE_VELOCITY_250;
    currentConfig.getNumValue("SPO2|SweepSpeed", speed);
    return speed;
}

void SPO2Param::updateSubParamLimit(SubParamID id)
{
    if (id == SUB_PARAM_SPO2)
    {
        _trendWidget->updateLimit();
    }
}

void SPO2Param::setModuleType(SPO2ModuleType type)
{
    _moduleType = type;
}

SPO2ModuleType SPO2Param::getModuleType() const
{
    return _moduleType;
}

void SPO2Param::setNibpSameSide(bool flag)
{
    int index = flag;
    currentConfig.setNumValue("SPO2|NIBPSameSide", index);
}

bool SPO2Param::isNibpSameSide(void)
{
    int flag;
    currentConfig.getNumValue("SPO2|NIBPSameSide", flag);
    return flag;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2Param::SPO2Param() : Param(PARAM_SPO2),
                         _oxyCRGSPO2Trend(NULL),
                         _moduleType(MODULE_SPO2_NR)
{
    _provider = NULL;
    _trendWidget = NULL;
    _waveWidget = NULL;

    _spo2Value = InvData();
    _piValue = InvData();
    _prValue = InvData();
    _barValue = InvData();
    _isValid = false;
    _sensorOff = true;
    _recPackageInPowerOn2sec = 0;

    systemConfig.getNumValue("PrimaryCfg|SPO2|EverCheckFinger", _isEverCheckFinger);
    systemConfig.getNumValue("PrimaryCfg|SPO2|EverSensorOn", _isEverSensorOn);

    QTimer::singleShot(2000, this, SLOT(checkSelftest()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2Param::~SPO2Param()
{
}
