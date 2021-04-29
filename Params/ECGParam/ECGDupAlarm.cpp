/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by fangtongzhou <fangtongzhou@blmed.cn>, 2019/1/19
 **/
#include "ECGDupAlarm.h"
#include "ECGDupParam.h"
#include "IConfig.h"
#include "ECGSymbol.h"
#include "PatientManager.h"
#include "AlarmConfig.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString ECGDupLimitAlarm::getAlarmSourceName(void)
{
    return paramInfo.getParamName(PARAM_DUP_ECG);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
ParamID ECGDupLimitAlarm::getParamID(void)
{
    return PARAM_DUP_ECG;
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int ECGDupLimitAlarm::getAlarmSourceNR(void)
{
    return ECG_DUP_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID ECGDupLimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id);
    bool isHR = ecgDupParam.isHRValid();
    if (isHR)
    {
        return ecgParam.leadToWaveID(ecgParam.getCalcLead());;
    }
    else
    {
        return WAVE_SPO2;
    }
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID ECGDupLimitAlarm::getSubParamID(int id)
{
    ECGDupLimitAlarmType alarmID = static_cast<ECGDupLimitAlarmType> (id);
    switch (alarmID)
    {
        case ECG_DUP_LIMIT_ALARM_HR_LOW:
        case ECG_DUP_LIMIT_ALARM_HR_HIGH:
        case ECG_DUP_LIMIT_ALARM_PR_LOW:
        case ECG_DUP_LIMIT_ALARM_PR_HIGH:
            return SUB_PARAM_HR_PR;
            break;
        case ECG_DUP_LIMIT_ALARM_PLUGIN_PR_LOW:
        case ECG_DUP_LIMIT_ALARM_PLUGIN_PR_HIGH:
            return SUB_PARAM_HR_PR;
            break;
        default:
            break;
    }
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority ECGDupLimitAlarm::getAlarmPriority(int paramID)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(paramID));
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int ECGDupLimitAlarm::getValue(int id)
{
    switch (id)
    {
        case ECG_DUP_LIMIT_ALARM_PLUGIN_PR_LOW:
        case ECG_DUP_LIMIT_ALARM_PLUGIN_PR_HIGH:
            return ecgDupParam.getPluginPR();
        default:
            return ecgDupParam.getHR();
    }
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool ECGDupLimitAlarm::isAlarmEnable(int paramID)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(paramID));
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int ECGDupLimitAlarm::getUpper(int paramID)
{
    SubParamID subId = getSubParamID(paramID);
    UnitType unit = ecgDupParam.getCurrentUnit(subId);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subId, unit);
    return config.highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int ECGDupLimitAlarm::getLower(int paramID)
{
    SubParamID subId = getSubParamID(paramID);
    UnitType unit = ecgDupParam.getCurrentUnit(subId);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(subId, unit);
    return config.lowLimit;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int ECGDupLimitAlarm::getCompare(int value, int id)
{
    HRSourceType isHR = ecgDupParam.getCurHRSource();
    if (isHR == HR_SOURCE_ECG)
    {
        if (id == ECG_DUP_LIMIT_ALARM_HR_HIGH)
        {
            if (value > getUpper(id))
            {
                return 1;
            }
        }
        else if (id == ECG_DUP_LIMIT_ALARM_HR_LOW)
        {
            if (value < getLower(id))
            {
                return -1;
            }
        }
    }
    else
    {
        if (id == ECG_DUP_LIMIT_ALARM_PR_HIGH)
        {
            if (value > getUpper(id))
            {
                return 1;
            }
        }
        else if (id == ECG_DUP_LIMIT_ALARM_PR_LOW)
        {
            if (value < getLower(id))
            {
                return -1;
            }
        }
    }

    if (id == ECG_DUP_LIMIT_ALARM_PLUGIN_PR_LOW)
    {
        if (value > getUpper(id))
        {
            return 1;
        }
    }
    else if (id == ECG_DUP_LIMIT_ALARM_PLUGIN_PR_HIGH)
    {
        if (value < getLower(id))
        {
            return -1;
        }
    }
    return 0;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *ECGDupLimitAlarm::toString(int id)
{
    return ECGSymbol::convert((ECGDupLimitAlarmType)id);
}

/**************************************************************************************************
 * 通知报警。
 *************************************************************************************************/
void ECGDupLimitAlarm::notifyAlarm(int id, bool isAlarm)
{
    switch (id)
    {
        case ECG_DUP_LIMIT_ALARM_HR_LOW:
        case ECG_DUP_LIMIT_ALARM_HR_HIGH:
        case ECG_DUP_LIMIT_ALARM_PR_LOW:
            _isAlarmLimit |= isAlarm;
            break;
        case ECG_DUP_LIMIT_ALARM_PR_HIGH:
            ecgDupParam.isAlarm(_isAlarmLimit, getSubParamID(id));
            _isAlarmLimit = false;
            break;
        case ECG_DUP_LIMIT_ALARM_PLUGIN_PR_LOW:
            _isPluginAlarm |= isAlarm;
            break;
        case ECG_DUP_LIMIT_ALARM_PLUGIN_PR_HIGH:
            ecgDupParam.isAlarm(_isPluginAlarm, getSubParamID(id));
            break;
        default:
            break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGDupLimitAlarm::ECGDupLimitAlarm()
{
    _isAlarmLimit = false;
    _isPluginAlarm = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGDupLimitAlarm::~ECGDupLimitAlarm()
{
}
