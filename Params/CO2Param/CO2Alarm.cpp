/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/29
 **/

#include "CO2Alarm.h"
#include "CO2Param.h"
#include "RESPDupParam.h"
#include "IConfig.h"
#include "CO2Symbol.h"
#include "PatientManager.h"
#include "ParamManager.h"
#include "FloatHandle.h"
#include "Debug.h"
#include "AlarmConfig.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString CO2LimitAlarm::getAlarmSourceName(void)
{
    return paramInfo.getParamName(PARAM_CO2);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int CO2LimitAlarm::getAlarmSourceNR(void)
{
    return CO2_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID CO2LimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_CO2;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID CO2LimitAlarm::getSubParamID(int id)
{
    switch (id)
    {
        case CO2_LIMIT_ALARM_ETCO2_LOW:
        case CO2_LIMIT_ALARM_ETCO2_HIGH:
            return SUB_PARAM_ETCO2;
        case CO2_LIMIT_ALARM_FICO2_LOW:
        case CO2_LIMIT_ALARM_FICO2_HIGH:
            return SUB_PARAM_FICO2;
        case CO2_LIMIT_ALARM_AWRR_LOW:
        case CO2_LIMIT_ALARM_AWRR_HIGH:
            return SUB_PARAM_AWRR;
        default:
            return SUB_PARAM_NONE;
    }
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority CO2LimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int CO2LimitAlarm::getValue(int id)
{
    switch (id)
    {
        case CO2_LIMIT_ALARM_ETCO2_LOW:
        case CO2_LIMIT_ALARM_ETCO2_HIGH:
            return co2Param.getEtCO2();

        case CO2_LIMIT_ALARM_FICO2_LOW:
        case CO2_LIMIT_ALARM_FICO2_HIGH:
            return co2Param.getFiCO2();
        case CO2_LIMIT_ALARM_AWRR_LOW:
        case CO2_LIMIT_ALARM_AWRR_HIGH:
            return co2Param.getAWRR();
        default:
            return InvData();
    }
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool CO2LimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id)) && co2Param.isEnabled();
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int CO2LimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = co2Param.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int CO2LimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = co2Param.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int CO2LimitAlarm::getCompare(int value, int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType curUnit = co2Param.getCurrentUnit(subID);
    UnitType defUnit = paramInfo.getUnitOfSubParam(subID);

    LimitAlarmConfig limitConfig = alarmConfig.getLimitAlarmConfig(subID, curUnit);

    int mul = paramInfo.getMultiOfSubParam(subID);
    QString valueStr;
    if (curUnit == UNIT_MMHG)
    {
        int low = limitConfig.lowLimit * limitConfig.scale;
        int high = limitConfig.highLimit * limitConfig.scale;
        int v = 0;
        float v1 = value * 1.0 / mul;
        valueStr = Unit::convert(curUnit, defUnit, v1, co2Param.getBaro());
        v = valueStr.toInt();
        if (0 == id % 2)
        {
            return v < low ? -1 : 0;
        }
        else
        {
            return v > high ? 1 : 0;
        }
    }
    else
    {
        float low = static_cast<float>(limitConfig.lowLimit) / limitConfig.scale;
        float high = static_cast<float>(limitConfig.highLimit) / limitConfig.scale;
        float v = 0;
        float v1 = value * 1.0 / mul;
        valueStr = Unit::convert(curUnit, defUnit, v1, co2Param.getBaro());
        v = valueStr.toDouble();
        if (0 == id % 2)
        {
            if (isUpper(low, v))
            {
                return -1;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            if (isUpper(v, high))
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *CO2LimitAlarm::toString(int id)
{
    return CO2Symbol::convert((CO2LimitAlarmType)id);
}

/**************************************************************************************************
 * 额外测量通知。
 *************************************************************************************************/
void CO2LimitAlarm::notifyAlarm(int id, bool flag)
{
    int subID = getSubParamID(id);

    switch (id)
    {
        case CO2_LIMIT_ALARM_ETCO2_HIGH:
            _isEtco2Alarm |= flag;
            co2Param.noticeLimitAlarm(subID, _isEtco2Alarm);
            _isEtco2Alarm = false;
            break;

        case CO2_LIMIT_ALARM_ETCO2_LOW:
            _isEtco2Alarm |= flag;
            break;

        case CO2_LIMIT_ALARM_FICO2_HIGH:
            _isFico2Alarm |= flag;
            co2Param.noticeLimitAlarm(subID, _isFico2Alarm);
            _isFico2Alarm = false;
            break;

        case CO2_LIMIT_ALARM_FICO2_LOW:
            _isFico2Alarm |= flag;
            break;
        case CO2_LIMIT_ALARM_AWRR_HIGH:
            _isAwrrAlarm |= flag;
            co2Param.noticeLimitAlarm(subID, _isAwrrAlarm);
            _isAwrrAlarm = false;
            break;

        case CO2_LIMIT_ALARM_AWRR_LOW:
            _isAwrrAlarm |= flag;
            break;

        default:
            break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2LimitAlarm::CO2LimitAlarm()
{
    _isEtco2Alarm = false;
    _isFico2Alarm = false;
    _isAwrrAlarm = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2LimitAlarm::~CO2LimitAlarm()
{
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString CO2OneShotAlarm::getAlarmSourceName(void)
{
    QString str(paramInfo.getParamName(PARAM_CO2));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int CO2OneShotAlarm::getAlarmSourceNR(void)
{
    return CO2_ONESHOT_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID CO2OneShotAlarm::getWaveformID(int id)
{
    if (id == CO2_ONESHOT_ALARM_APNEA)
    {
        return WAVE_CO2;
    }

    return WAVE_NONE;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID CO2OneShotAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_ETCO2;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority CO2OneShotAlarm::getAlarmPriority(int id)
{
    if (id == CO2_ONESHOT_ALARM_APNEA)
    {
        return ALARM_PRIO_HIGH;
    }

    if (id == CO2_ONESHOT_ALARM_ZEROING_COMPLETED || id == CO2_ONESHOT_ALARM_ZERO_IN_PROGRESS)
    {
        return ALARM_PRIO_PROMPT;
    }

    if (id == CO2_ONESHOT_ALARM_NO_ADAPTER)
    {
        return ALARM_PRIO_MED;
    }

    return ALARM_PRIO_LOW;
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType CO2OneShotAlarm::getAlarmType(int id)
{
    if (id == CO2_ONESHOT_ALARM_APNEA)
    {
        return ALARM_TYPE_PHY;
    }

    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * notify alarm。
 *************************************************************************************************/
void CO2OneShotAlarm::notifyAlarm(int id, bool isAlarm)
{
    if (id == CO2_ONESHOT_ALARM_APNEA && isAlarm)
    {
        respDupParam.isAlarm(isAlarm, false);
    }
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *CO2OneShotAlarm::toString(int id)
{
    return CO2Symbol::convert((CO2OneShotType)id);
}

/**************************************************************************************************
 * 报警使能。
 *************************************************************************************************/
bool CO2OneShotAlarm::isAlarmEnable(int id)
{
    if (id == CO2_ONESHOT_ALARM_ZERO_IN_PROGRESS ||
            id == CO2_ONESHOT_ALARM_STANDBY)
    {
        return true;
    }

    return (co2Param.isEnabled() && co2Param.getCO2Switch());
}

/**************************************************************************************************
 * 报警使能。
 *************************************************************************************************/
bool CO2OneShotAlarm::isAlarmed(int id)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        return false;
    }

    if (id == CO2_ONESHOT_ALARM_STANDBY)
    {
        return AlarmOneShotIFace::isAlarmed(id);
    }

    if (id != CO2_ONESHOT_ALARM_APNEA)
    {
        if (!co2Param.isConnected() || !co2Param.getCO2Switch())
        {
            return false;
        }
    }

    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * latch后移除报警显示信息。
 *************************************************************************************************/
bool CO2OneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
CO2OneShotAlarm::CO2OneShotAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
CO2OneShotAlarm::~CO2OneShotAlarm()
{
}
