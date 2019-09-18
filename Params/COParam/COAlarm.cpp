/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/22
 **/

#include "COAlarm.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "COParam.h"
#include "SystemManager.h"

/**************************************************************************************************
 * alarm source name.
 *************************************************************************************************/
QString COLimitAlarm::getAlarmSourceName()
{
    return paramInfo.getParamName(PARAM_CO);
}

/**************************************************************************************************
 * alarm source number.
 *************************************************************************************************/
int COLimitAlarm::getAlarmSourceNR()
{
    return CO_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID COLimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID COLimitAlarm::getSubParamID(int id)
{
    switch (id)
    {
    case CO_LIMIT_ALARM_CO_LOW:
    case CO_LIMIT_ALARM_CO_HIGH:
        return SUB_PARAM_CO_CO;
    case CO_LIMIT_ALARM_CI_LOW:
    case CO_LIMIT_ALARM_CI_HIGH:
        return SUB_PARAM_CO_CI;
    case CO_LIMIT_ALARM_TB_LOW:
    case CO_LIMIT_ALARM_TB_HIGH:
        return SUB_PARAM_CO_TB;
    default:
        return SUB_PARAM_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to alarm priority
 *************************************************************************************************/
AlarmPriority COLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * get alarm measure data of physiological parameter
 *************************************************************************************************/
int COLimitAlarm::getValue(int id)
{
    switch (id)
    {
    case CO_LIMIT_ALARM_CO_LOW:
    case CO_LIMIT_ALARM_CO_HIGH:
        return coParam.getCOData();
    case CO_LIMIT_ALARM_CI_LOW:
    case CO_LIMIT_ALARM_CI_HIGH:
        return coParam.getCIData();
    case CO_LIMIT_ALARM_TB_LOW:
    case CO_LIMIT_ALARM_TB_HIGH:
        return coParam.getTBData();
    default:
        return -1;
    }
}

/**************************************************************************************************
 * physiological parameter alarm enable.
 *************************************************************************************************/
bool COLimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * get upper limit of physiological parameter alarm.
 *************************************************************************************************/
int COLimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = UNIT_LPM;
    if (subID == SUB_PARAM_CO_CO)
    {
        unit = UNIT_LPM;
    }
    else if (subID == SUB_PARAM_CO_CI)
    {
        unit = UNIT_LPMPSQM;
    }
    else if (subID == SUB_PARAM_CO_TB)
    {
        unit = UNIT_TDC;
    }
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

/**************************************************************************************************
 * get lower limit of physiological parameter alarm.
 *************************************************************************************************/
int COLimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = UNIT_LPM;
    if (subID == SUB_PARAM_CO_CO)
    {
        unit = UNIT_LPM;
    }
    else if (subID == SUB_PARAM_CO_CI)
    {
        unit = UNIT_LPMPSQM;
    }
    else if (subID == SUB_PARAM_CO_TB)
    {
        unit = UNIT_TDC;
    }
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * compare physiological parameter alarm.
 *************************************************************************************************/
int COLimitAlarm::getCompare(int value, int id)
{
    if (0 == id % 2)
    {
        if (value < getLower(id))
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
        if (value > getUpper(id))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

/**************************************************************************************************
 * physiological parameter alarm convert string.
 *************************************************************************************************/
const char *COLimitAlarm::toString(int id)
{
    return COSymbol::convert((COLimitAlarmType)id);
}

/**************************************************************************************************
 * alarm notify.
 *************************************************************************************************/
void COLimitAlarm::notifyAlarm(int id, bool flag)
{
    Q_UNUSED(id)
    Q_UNUSED(flag)
}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
COLimitAlarm::~COLimitAlarm()
{
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
COLimitAlarm::COLimitAlarm()
{
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * one shot alarm source name.
 *************************************************************************************************/
QString COOneShotAlarm::getAlarmSourceName()
{
    QString str(paramInfo.getParamName(PARAM_CO));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * one shot alarm source number.
 *************************************************************************************************/
int COOneShotAlarm::getAlarmSourceNR()
{
    return CO_ONESHOT_NR;
}

/**************************************************************************************************
 * one shot alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID COOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * one shot alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID COOneShotAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * one shot alarm priority.
 *************************************************************************************************/
AlarmPriority COOneShotAlarm::getAlarmPriority(int id)
{
    Q_UNUSED(id)
    return ALARM_PRIO_LOW;
}

/**************************************************************************************************
 * one shot alarm type.
 *************************************************************************************************/
AlarmType COOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * alarm enable.
 *************************************************************************************************/
bool COOneShotAlarm::isAlarmed(int id)
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
void COOneShotAlarm::notifyAlarm(int /*id*/, bool /*isAlarm*/)
{
}

/**************************************************************************************************
 * one shot alarm convert string.
 *************************************************************************************************/
const char *COOneShotAlarm::toString(int id)
{
    return COSymbol::convert((COOneShotType)id);
}

/**************************************************************************************************
 * enable one shot alarm.
 *************************************************************************************************/
bool COOneShotAlarm::isAlarmEnable(int id)
{
    Q_UNUSED(id)
    return true;
}

/**************************************************************************************************
 *  remove alarm display information later latch.
 *************************************************************************************************/
bool COOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
COOneShotAlarm::~COOneShotAlarm()
{
}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
COOneShotAlarm::COOneShotAlarm()
{
}
