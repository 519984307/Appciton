/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/13
 **/

#include "O2Alarm.h"
#include "ParamInfo.h"
#include "O2Define.h"
#include "AlarmConfig.h"
#include "O2Param.h"
#include "O2Symbol.h"

QString O2LimitAlarm::getAlarmSourceName()
{
    return paramInfo.getParamName(PARAM_O2);
}

int O2LimitAlarm::getAlarmSourceNR()
{
    return O2_LIMIT_ALARM_NR;
}

WaveformID O2LimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

AlarmPriority O2LimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

int O2LimitAlarm::getValue(int id)
{
    int16_t o2 = o2Param.getO2Concentration();
    switch (id)
    {
    case O2_LIMIT_ALARM_O2_LOW:
    case O2_LIMIT_ALARM_O2_HIGH:
        return o2;
    default:
        return o2;
    }
}

bool O2LimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

int O2LimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = o2Param.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

int O2LimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = o2Param.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

int O2LimitAlarm::getCompare(int value, int id)
{
    switch (id)
    {
    case O2_LIMIT_ALARM_O2_HIGH:
        if (value > getUpper(id))
        {
            return 1;
        }
        else
        {
            return 0;
        }
    case O2_LIMIT_ALARM_O2_LOW:
        if (value < getLower(id))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    default:
        return 0;
    }
}

const char *O2LimitAlarm::toString(int id)
{
    return O2Symbol::convert((O2LimitAlarmType)id);
}

void O2LimitAlarm::notifyAlarm(int id, bool flag)
{
    switch (id)
    {
    case O2_LIMIT_ALARM_O2_LOW:
        _isO2Alarm |= flag;
        break;
    case O2_LIMIT_ALARM_O2_HIGH:
        _isO2Alarm |= flag;
        o2Param.noticeLimitAlarm(_isO2Alarm);
        _isO2Alarm = false;
        break;
    default:
        break;
    }
}

O2LimitAlarm::O2LimitAlarm() : _isO2Alarm(false)
{
}

O2LimitAlarm::~O2LimitAlarm()
{
}

QString O2OneShotAlarm::getAlarmSourceName()
{
    QString str(paramInfo.getParamName(PARAM_O2));
    return (str + "OneShotAlarm");
}

int O2OneShotAlarm::getAlarmSourceNR()
{
    return O2_ONESHOT_ALARM_NR;
}

WaveformID O2OneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

AlarmPriority O2OneShotAlarm::getAlarmPriority(int id)
{
    if (id == O2_ONESHOT_ALARM_MOTOR_NOT_IN_POSITION ||
            id == O2_ONESHOT_ALARM_SENSOR_OFF)
    {
        return ALARM_PRIO_HIGH;
    }
    else if (id == O2_ONESHOT_ALARM_SEND_COMMUNICATION_STOP)
    {
        return ALARM_PRIO_MED;
    }
    return ALARM_PRIO_PROMPT;
}

AlarmType O2OneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

const char *O2OneShotAlarm::toString(int id)
{
    return O2Symbol::convert((O2OneShotAlarmType)id);
}

bool O2OneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

bool O2OneShotAlarm::isAlarmed(int id)
{
    return AlarmOneShotIFace::isAlarmed(id);
}

O2OneShotAlarm::O2OneShotAlarm()
{
}

O2OneShotAlarm::~O2OneShotAlarm()
{
}
