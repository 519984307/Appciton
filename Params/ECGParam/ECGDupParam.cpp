/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang fangtongzhou@blmed.cn, 2019/1/19
 **/

#include "ECGDupParam.h"
#include "BaseDefine.h"
#include "ConfigManager.h"
#include "ECGTrendWidget.h"
#include "Debug.h"
#include "ECGParam.h"
#include "SoundManager.h"
#include "SPO2Param.h"
#include "IBPParam.h"
#include "O2ParamInterface.h"
#include "RESPAlarm.h"
#include "Components/DataUploader/BLMMessageDefine.h"

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void ECGDupParam::initParam(void)
{
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void ECGDupParam::handDemoWaveform(WaveformID /*id*/, short /*data*/)
{
}

/**************************************************************************************************
 * 产生DEMO数据。
 *************************************************************************************************/
void ECGDupParam::handDemoTrendData(void)
{
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short ECGDupParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_HR_PR:
        return getHR();

    case SUB_PARAM_PLUGIN_PR:
        return getPluginPR();
    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void ECGDupParam::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void ECGDupParam::setProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
}


/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType ECGDupParam::getCurrentUnit(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
        return UNIT_MV;
    default:
        break;
    }

    return UNIT_BPM;
}

QVariantMap ECGDupParam::getTrendVariant(int id)
{
    QVariantMap map;

    if (id == BLM_TREND_PARAM_HR_PR)
    {
        map["ParamID"] = BLM_PARAM_ECG;
        map["TrendID"] = BLM_TREND_PARAM_HR_PR;
        map["Value"] = _hrValue;
        map["Status"] = _hrValue == InvData() ? 1 : 0;
    }
    return map;
}

/**************************************************************************************************
 * 设置趋势窗体。
 *************************************************************************************************/
void ECGDupParam::setTrendWidget(ECGTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _trendWidget = trendWidget;
}

void ECGDupParam::updatePR(short pr, PRSourceType type, bool isUpdatePr)
{
    // PR为负数时,置为无效值
    if (pr < 0)
    {
        pr = InvData();
    }

    if (isUpdatePr)
    {
        if (type == PR_SOURCE_SPO2)
        {
            _prValueFromSPO2 = pr;
        }
        else if (type == PR_SOURCE_IBP)
        {
            _prValueFromIBP = pr;
        }
    }

    bool isIBP1LeadOff = ibpParam.isIBPLeadOff(IBP_CHN_1);
    bool isIBP2LeadOff = ibpParam.isIBPLeadOff(IBP_CHN_2);
    bool isSPO2Valid = spo2Param.isValid();

    // 演示模式下，强制设定相关标志位
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        isIBP1LeadOff = isIBP2LeadOff = false;
        isSPO2Valid = true;
    }

    // 更新pr值
    switch (_prSource)
    {
        case PR_SOURCE_AUTO:
        {
            if (isSPO2Valid && _prValueFromSPO2 != InvData())
            {
                _prValue = _prValueFromSPO2;
                _currentHRSource = HR_SOURCE_SPO2;
            }
            else if ((!isIBP1LeadOff || !isIBP2LeadOff)
                 && (_prValueFromIBP != InvData()))
            {
                _prValue = _prValueFromIBP;
                _currentHRSource = HR_SOURCE_IBP;
            }
            else
            {
                _prValue = InvData();
            }
        }
        break;
        case PR_SOURCE_SPO2:
        {
            if (isSPO2Valid)
            {
                _prValue = _prValueFromSPO2;
                _currentHRSource = HR_SOURCE_SPO2;
            }
            else
            {
                _prValue = InvData();
            }
        }
        break;
        case PR_SOURCE_IBP:
        {
            if (!isIBP1LeadOff || !isIBP2LeadOff)
            {
                _prValue = _prValueFromIBP;
                _currentHRSource = HR_SOURCE_IBP;
            }
            else
            {
                _prValue = InvData();
            }
        }
        break;
    }

    if (_trendWidget == NULL)
    {
        return;
    }

    // 参数界面上更新pr值
    switch (_hrSource)
    {
        case HR_SOURCE_AUTO:
        {
            if (_hrValue != InvData())
            {
                return;
            }
            if (_prValue != InvData())
            {
                _trendWidget->setHRValue(_prValue, _currentHRSource);
            }
            else
            {
                _trendWidget->setHRValue(_prValue, HR_SOURCE_AUTO);
            }
            _hrBeatFlag = false;
        }
        break;
        case HR_SOURCE_IBP:
        case HR_SOURCE_SPO2:
        {
            _hrBeatFlag = false;
            _trendWidget->setHRValue(_prValue, _hrSource);
        }
        break;
        case HR_SOURCE_ECG:
        case HR_SOURCE_NR:
        break;
    }
}

void ECGDupParam::updatePluginPR(short pr)
{
    // PR为负数时,置为无效值
    if (pr < 0)
    {
        pr = InvData();
    }

    if (_pluginPRValue == pr)
    {
        return;
    }

    _pluginPRValue = pr;
    _trendWidget->setPluginPR(pr);
}

/**************************************************************************************************
 * 更新VFVT数值。
 *************************************************************************************************/
void ECGDupParam::updateVFVT(bool onoff)
{
    Q_UNUSED(onoff)
    return;
}

/**************************************************************************************************
 * 更新HR心跳图标。
 *************************************************************************************************/
void ECGDupParam::updateHRBeatIcon()
{
    // 加入hr来源判断为限制条件--避免spo2\ibp模块的pr数据为无效值时造成误判
    if (_hrBeatFlag
            && (_hrSource == HR_SOURCE_ECG
                || _hrSource == HR_SOURCE_AUTO))
    {
        if (NULL != _trendWidget && _hrValue != InvData())
        {
            _trendWidget->blinkBeatPixmap();
        }
    }
}

/**************************************************************************************************
 * 更新PR心跳图标。
 *************************************************************************************************/
void ECGDupParam::updatePRBeatIcon()
{
    // 加入hr来源判断为限制条件--避免ecg模块的hr数据为无效值时造成误判
    if (!_hrBeatFlag
            && (_hrSource == HR_SOURCE_SPO2
                || _hrSource == HR_SOURCE_IBP
                || _hrSource == HR_SOURCE_AUTO))
    {
        if (NULL != _trendWidget && _prValue != InvData())
        {
            _trendWidget->blinkBeatPixmap();
        }
    }
}

/**************************************************************************************************
 * 更新HR的值。
 *************************************************************************************************/
void ECGDupParam::updateHR(short hr)
{
    _hrValue = hr;

#ifdef ENABLE_O2_APNEASTIMULATION
    O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
    if (o2Param)
    {
        int apneaStimulationHR = 100;
        int motorSta = false;
        currentConfig.getNumValue("ApneaStimulation|HR", apneaStimulationHR);
        if (hr < apneaStimulationHR && hr != InvData())
        {
            motorSta = true;
        }
        else
        {
            motorSta = false;
        }
        o2Param->setVibrationReason(APNEASTIMULATION_REASON_HR, motorSta);
    }
#endif

    if (_trendWidget == NULL)
    {
        return;
    }

    switch (_hrSource)
    {
    case HR_SOURCE_AUTO:
    {
        // HR不为无效时即显示。
        if (_hrValue != InvData())
        {
            _hrBeatFlag = true;
            _trendWidget->setHRValue(_hrValue, HR_SOURCE_ECG);
        }
        else if (_prValue != InvData())
        {
            _hrBeatFlag = false;
            _trendWidget->setHRValue(_prValue, _currentHRSource);
        }
        else  // HR和PR都为无效时。
        {
            _hrBeatFlag = true;
            _trendWidget->setHRValue(_hrValue, HR_SOURCE_ECG);
        }
    }
        break;
    case HR_SOURCE_ECG:
    {
        if (_hrValue != InvData())
        {
            _hrBeatFlag = true;
        }
        else
        {
            _hrBeatFlag = false;
        }
        _trendWidget->setHRValue(_hrValue, _hrSource);
    }
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 获取HR的值。
 *************************************************************************************************/
short ECGDupParam::getHR(bool isGetOriginalHR) const
{
    if (isGetOriginalHR)
    {
        return _hrValue;
    }

    if (_hrSource == HR_SOURCE_AUTO)
    {
        if (InvData() != _hrValue)
        {
            return _hrValue;
        }

        if (InvData() != _prValue)
        {
            return _prValue;
        }
    }
    else if (_hrSource == HR_SOURCE_ECG)
    {
        return _hrValue;
    }
    else
    {
        return _prValue;
    }

    return InvData();
}

short ECGDupParam::getPluginPR() const
{
    return _pluginPRValue;
}

/**************************************************************************************************
 * 是否为HR有效。
 *************************************************************************************************/
bool ECGDupParam::isHRValid(void)
{
    if (_hrValue != InvData())
    {
        return true;
    }

    if (_prValue == InvData())
    {
        return true;
    }

    return false;
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
void ECGDupParam::isAlarm(bool isAlarm, int subId)
{
    switch (subId)
    {
    case SUB_PARAM_HR_PR:
        _isAlarm |= isAlarm;
        if (NULL != _trendWidget)
        {
            _trendWidget->isAlarm(_isAlarm);
            _isAlarm = false;
        }
        break;
    case SUB_PARAM_PLUGIN_PR:
        _isPluginPRAlarm |= isAlarm;
        if (NULL != _trendWidget)
        {
            _trendWidget->isPluginPRAlarm(_isPluginPRAlarm);
            _isPluginPRAlarm = false;
        }
        break;
    default:
        break;
    }
}

/***************************************************************************************************
 * get the hr source
 **************************************************************************************************/
HRSourceType ECGDupParam::getCurHRSource() const
{
    if (_hrSource == HR_SOURCE_AUTO)
    {
        HRSourceType type = HR_SOURCE_AUTO;
        if (_hrValue == InvData() && _prValue != InvData())
        {
            if (_prSource == PR_SOURCE_SPO2)
            {
                type = HR_SOURCE_SPO2;
            }
            else if (_prSource == PR_SOURCE_IBP)
            {
                type = HR_SOURCE_IBP;
            }
        }
        else
        {
            type = HR_SOURCE_ECG;
        }
        return type;
    }

    return _hrSource;
}

PRSourceType ECGDupParam::getCurPRSource() const
{
    return _prSource;
}

void ECGDupParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_ECG)
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    _trendWidget->updatePalette(pal);
}
void ECGDupParam::setHrSource(HRSourceType type)
{
    _hrSource = type;

    // 更新pr来源选择
    if (type == HR_SOURCE_SPO2)
    {
        _prSource = PR_SOURCE_SPO2;
    }
    else if (type == HR_SOURCE_IBP)
    {
        _prSource = PR_SOURCE_IBP;
    }
    else
    {
        _prSource = PR_SOURCE_AUTO;
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    if (_hrSource != HR_SOURCE_ECG && _hrSource != HR_SOURCE_AUTO)
    {
        O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
        if (o2Param)
        {
            o2Param->setVibrationReason(APNEASTIMULATION_REASON_HR, false);
        }
    }
    currentConfig.setNumValue("ECG|HRSource", static_cast<int>(type));
#endif
}

void ECGDupParam::updateHRSource()
{
    int type = 0;
    currentConfig.getNumValue("ECG|HRSource", type);
    _hrSource = static_cast<HRSourceType>(type);
    if (_hrSource == HR_SOURCE_ECG)
    {
        updateHR(_hrValue);
    }
    else if (_hrSource == HR_SOURCE_SPO2)
    {
        updatePR(_prValue);
    }
    else if (_hrSource == HR_SOURCE_AUTO)
    {
        if (InvData() != _hrValue)
        {
            updateHR(_hrValue);
        }

        if (InvData() != _prValue)
        {
            updatePR(_prValue);
        }
    }
}

bool ECGDupParam::isAutoTypeHrSouce() const
{
    if (_hrSource == HR_SOURCE_AUTO)
    {
        return true;
    }
    else
    {
        return false;
    }
}

QList<SubParamID> ECGDupParam::getShortTrendList()
{
    return _trendWidget->getShortTrendSubParams();
}

void ECGDupParam::updateSubParamLimit(SubParamID id)
{
    if (id == SUB_PARAM_HR_PR)
    {
        _trendWidget->updateLimit();
    }
}

void ECGDupParam::restartParamUpdateTime()
{
    paramUpdateTimer->start(PARAM_UPDATE_TIMEOUT);
}

void ECGDupParam::paramUpdateTimeout()
{
    HRSourceType source = getCurHRSource();
    switch (source) {
    case HR_SOURCE_ECG:
        _hrValue = InvData();
        if (_trendWidget != NULL)
        {
            _trendWidget->setHRValue(_hrValue, source);
        }
        break;
    case HR_SOURCE_SPO2:
        _prValue = InvData();
        if (_trendWidget != NULL)
        {
            _trendWidget->setHRValue(_prValue, source);
        }
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGDupParam::ECGDupParam()
    : Param(PARAM_DUP_ECG),
      _provider(NULL),
      _trendWidget(NULL),
      _hrValue(InvData()),
      _prValue(InvData()),
      _pluginPRValue(InvData()),
      _prValueFromSPO2(InvData()),
      _prValueFromIBP(InvData()),
      _hrBeatFlag(true),
      _isAlarm(false),
      _isPluginPRAlarm(false),
      _currentHRSource(HR_SOURCE_AUTO)
{
    // 初始化hr/pr来源
    int type = 0;
    currentConfig.getNumValue("ECG|HRSource", type);
    _hrSource = static_cast<HRSourceType>(type);

    switch (_hrSource)
    {
        case HR_SOURCE_ECG:
        case HR_SOURCE_AUTO:
        case HR_SOURCE_NR:
            _prSource = PR_SOURCE_AUTO;
        break;
        case HR_SOURCE_SPO2:
            _prSource = PR_SOURCE_SPO2;
        break;
        case HR_SOURCE_IBP:
            _prSource = PR_SOURCE_IBP;
        break;
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGDupParam &ECGDupParam::getInstance()
{
    static ECGDupParam *instance = NULL;
    if (instance == NULL)
    {
        instance = new ECGDupParam();
        ECGDupParamInterface *old =  ECGDupParamInterface::registerECGDupParam(instance);
        if (old) {
            delete old;
        }
    }
    return *instance;
}

ECGDupParam::~ECGDupParam()
{
}
