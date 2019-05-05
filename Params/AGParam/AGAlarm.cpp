/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/18
 **/


#include "AGAlarm.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "AGParam.h"
#include "FloatHandle.h"
#include "SystemManager.h"

/**************************************************************************************************
 * alarm source name.
 *************************************************************************************************/
QString AGLimitAlarm::getAlarmSourceName()
{
    return paramInfo.getParamName(PARAM_AG);
}

/**************************************************************************************************
 * alarm source number.
 *************************************************************************************************/
int AGLimitAlarm::getAlarmSourceNR()
{
    return AG_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID AGLimitAlarm::getWaveformID(int id)
{
    switch (id)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
    case AG_LIMIT_ALARM_ETN2O_HIGH:
    case AG_LIMIT_ALARM_FIN2O_LOW:
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        return WAVE_N2O;
    case AG_LIMIT_ALARM_ETAA1_LOW:
    case AG_LIMIT_ALARM_ETAA1_HIGH:
    case AG_LIMIT_ALARM_FIAA1_LOW:
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        return WAVE_AA1;
    case AG_LIMIT_ALARM_ETAA2_LOW:
    case AG_LIMIT_ALARM_ETAA2_HIGH:
    case AG_LIMIT_ALARM_FIAA2_LOW:
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        return WAVE_AA2;
    case AG_LIMIT_ALARM_ETO2_LOW:
    case AG_LIMIT_ALARM_ETO2_HIGH:
    case AG_LIMIT_ALARM_FIO2_LOW:
    case AG_LIMIT_ALARM_FIO2_HIGH:
        return WAVE_O2;
    default:
        return WAVE_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID AGLimitAlarm::getSubParamID(int id)
{
    switch (id)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
    case AG_LIMIT_ALARM_ETN2O_HIGH:
        return SUB_PARAM_ETN2O;
    case AG_LIMIT_ALARM_FIN2O_LOW:
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        return SUB_PARAM_FIN2O;
    case AG_LIMIT_ALARM_ETAA1_LOW:
    case AG_LIMIT_ALARM_ETAA1_HIGH:
        return SUB_PARAM_ETAA1;
    case AG_LIMIT_ALARM_FIAA1_LOW:
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        return SUB_PARAM_FIAA1;
    case AG_LIMIT_ALARM_ETAA2_LOW:
    case AG_LIMIT_ALARM_ETAA2_HIGH:
        return SUB_PARAM_ETAA2;
    case AG_LIMIT_ALARM_FIAA2_LOW:
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        return SUB_PARAM_FIAA2;
    case AG_LIMIT_ALARM_ETO2_LOW:
    case AG_LIMIT_ALARM_ETO2_HIGH:
        return SUB_PARAM_ETO2;
    case AG_LIMIT_ALARM_FIO2_LOW:
    case AG_LIMIT_ALARM_FIO2_HIGH:
        return SUB_PARAM_FIO2;
    default:
        return SUB_PARAM_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to alarm priority
 *************************************************************************************************/
AlarmPriority AGLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * get alarm measure data of physiological parameter
 *************************************************************************************************/
int AGLimitAlarm::getValue(int id)
{
    switch (id)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
    case AG_LIMIT_ALARM_ETN2O_HIGH:
        return agParam.getEtData(AG_TYPE_N2O);
    case AG_LIMIT_ALARM_FIN2O_LOW:
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        return agParam.getFiData(AG_TYPE_N2O);
    case AG_LIMIT_ALARM_ETAA1_LOW:
    case AG_LIMIT_ALARM_ETAA1_HIGH:
        return agParam.getEtData(AG_TYPE_AA1);
    case AG_LIMIT_ALARM_FIAA1_LOW:
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        return agParam.getFiData(AG_TYPE_AA1);
    case AG_LIMIT_ALARM_ETAA2_LOW:
    case AG_LIMIT_ALARM_ETAA2_HIGH:
        return agParam.getEtData(AG_TYPE_AA2);
    case AG_LIMIT_ALARM_FIAA2_LOW:
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        return agParam.getFiData(AG_TYPE_AA2);
    case AG_LIMIT_ALARM_ETO2_LOW:
    case AG_LIMIT_ALARM_ETO2_HIGH:
        return agParam.getEtData(AG_TYPE_O2);
    case AG_LIMIT_ALARM_FIO2_LOW:
    case AG_LIMIT_ALARM_FIO2_HIGH:
        return agParam.getFiData(AG_TYPE_O2);
    default:
        return -1;
    }
}

/**************************************************************************************************
 * physiological parameter alarm enable.
 *************************************************************************************************/
bool AGLimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * get upper limit of physiological parameter alarm.
 *************************************************************************************************/
int AGLimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = UNIT_PERCENT;
    int value = alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
    return value;
}

/**************************************************************************************************
 * get lower limit of physiological parameter alarm.
 *************************************************************************************************/
int AGLimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = UNIT_PERCENT;
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * compare physiological parameter alarm.
 *************************************************************************************************/
int AGLimitAlarm::getCompare(int value, int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType curUnit = agParam.getCurrentUnit(subID);
    UnitType defUnit = paramInfo.getUnitOfSubParam(subID);

    LimitAlarmConfig limitConfig = alarmConfig.getLimitAlarmConfig(subID, curUnit);

    int mul = limitConfig.scale;
    QString valueStr;
    if (curUnit == UNIT_MMHG)
    {
        int low = limitConfig.lowLimit;
        int high = limitConfig.highLimit;
        int v = 0;
        valueStr = Unit::convert(curUnit, defUnit, value);
        v = valueStr.toInt();
        v /= mul;
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
        valueStr = Unit::convert(curUnit, defUnit, v1);
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
 * physiological parameter alarm convert string.
 *************************************************************************************************/
const char *AGLimitAlarm::toString(int id)
{
    return AGSymbol::convert((AGLimitAlarmType)id);
}

/**************************************************************************************************
 * alarm notify.
 *************************************************************************************************/
void AGLimitAlarm::notifyAlarm(int id, bool flag)
{
    AGLimitAlarmType alarmID = (AGLimitAlarmType)id;
    SubParamID subID = getSubParamID(id);
    switch (alarmID)
    {
    case AG_LIMIT_ALARM_ETN2O_LOW:
        _isEtN2OAlarm |= flag;
        break;
    case AG_LIMIT_ALARM_ETN2O_HIGH:
        _isEtN2OAlarm |= flag;
        agParam.noticeLimitAlarm(subID, _isEtN2OAlarm);
        _isEtN2OAlarm = false;
        break;

    case AG_LIMIT_ALARM_FIN2O_LOW:
        _isFiN2OAlarm |= flag;
        break;
    case AG_LIMIT_ALARM_FIN2O_HIGH:
        _isFiN2OAlarm |= flag;
        agParam.noticeLimitAlarm(subID, _isFiN2OAlarm);
        _isFiN2OAlarm = false;
        break;

    case AG_LIMIT_ALARM_ETAA1_LOW:
        _isEtAA1Alarm |= flag;
        break;
    case AG_LIMIT_ALARM_ETAA1_HIGH:
        _isEtAA1Alarm |= flag;
        agParam.noticeLimitAlarm(subID, _isEtAA1Alarm);
        _isEtAA1Alarm = false;
        break;

    case AG_LIMIT_ALARM_FIAA1_LOW:
        _isFiAA1Alarm |= flag;
        break;
    case AG_LIMIT_ALARM_FIAA1_HIGH:
        _isFiAA1Alarm |= flag;
        agParam.noticeLimitAlarm(subID, _isFiAA1Alarm);
        _isFiAA1Alarm = false;
        break;

    case AG_LIMIT_ALARM_ETAA2_LOW:
        _isEtAA2Alarm |= flag;
        break;
    case AG_LIMIT_ALARM_ETAA2_HIGH:
        _isEtAA2Alarm |= flag;
        agParam.noticeLimitAlarm(subID, _isEtAA2Alarm);
        _isEtAA2Alarm = false;
        break;

    case AG_LIMIT_ALARM_FIAA2_LOW:
        _isFiAA2Alarm |= flag;
        break;
    case AG_LIMIT_ALARM_FIAA2_HIGH:
        _isFiAA2Alarm |= flag;
        agParam.noticeLimitAlarm(subID, _isFiAA2Alarm);
        _isFiAA2Alarm = false;
        break;

    case AG_LIMIT_ALARM_ETO2_LOW:
        _isEtO2Alarm |= flag;
        break;
    case AG_LIMIT_ALARM_ETO2_HIGH:
        _isEtO2Alarm |= flag;
        agParam.noticeLimitAlarm(subID, _isEtO2Alarm);
        _isEtO2Alarm = false;
        break;

    case AG_LIMIT_ALARM_FIO2_LOW:
        _isFiO2Alarm |= flag;
        break;
    case AG_LIMIT_ALARM_FIO2_HIGH:
        _isFiO2Alarm |= flag;
        agParam.noticeLimitAlarm(subID, _isFiO2Alarm);
        _isFiO2Alarm = false;
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
AGLimitAlarm::~AGLimitAlarm()
{
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
AGLimitAlarm::AGLimitAlarm()
{
    _isEtN2OAlarm = false;
    _isFiN2OAlarm = false;
    _isEtO2Alarm = false;
    _isFiO2Alarm = false;
    _isEtAA1Alarm = false;
    _isFiAA1Alarm = false;
    _isEtAA2Alarm = false;
    _isFiAA2Alarm = false;
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * one shot alarm source name.
 *************************************************************************************************/
QString AGOneShotAlarm::getAlarmSourceName()
{
    QString str(paramInfo.getParamName(PARAM_AG));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * one shot alarm source number.
 *************************************************************************************************/
int AGOneShotAlarm::getAlarmSourceNR()
{
    return AG_ONESHOT_NR;
}

/**************************************************************************************************
 * one shot alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID AGOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * one shot alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID AGOneShotAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * one shot alarm priority.
 *************************************************************************************************/
AlarmPriority AGOneShotAlarm::getAlarmPriority(int id)
{
    Q_UNUSED(id)
    return ALARM_PRIO_LOW;
}

/**************************************************************************************************
 * one shot alarm type.
 *************************************************************************************************/
AlarmType AGOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * alarm enable.
 *************************************************************************************************/
bool AGOneShotAlarm::isAlarmed(int id)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO && getAlarmType(id) == ALARM_TYPE_TECH)
    {
        return false;
    }

    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * notify alarm.
 *************************************************************************************************/
void AGOneShotAlarm::notifyAlarm(int /*id*/, bool /*isAlarm*/)
{
}

/**************************************************************************************************
 * one shot alarm convert string.
 *************************************************************************************************/
const char *AGOneShotAlarm::toString(int id)
{
    return AGSymbol::convert((AGOneShotType)id);
}

/**************************************************************************************************
 * enable one shot alarm.
 *************************************************************************************************/
bool AGOneShotAlarm::isAlarmEnable(int id)
{
    Q_UNUSED(id)
    return true;
}

/**************************************************************************************************
 *  remove alarm display information later latch.
 *************************************************************************************************/
bool AGOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
AGOneShotAlarm::~AGOneShotAlarm()
{
}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
AGOneShotAlarm::AGOneShotAlarm()
{
}
