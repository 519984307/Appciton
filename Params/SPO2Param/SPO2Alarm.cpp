/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/17
 **/

#include "SPO2Alarm.h"
#include "SPO2Param.h"
#include "IConfig.h"
#include "SPO2Symbol.h"
#include "PatientManager.h"
#include "AlarmConfig.h"
#include "SystemManager.h"
#include "NIBPParam.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString SPO2LimitAlarm::getAlarmSourceName(void)
{
    return paramInfo.getParamName(PARAM_SPO2);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int SPO2LimitAlarm::getAlarmSourceNR(void)
{
    return SPO2_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID SPO2LimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_SPO2;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID SPO2LimitAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_SPO2;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority SPO2LimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int SPO2LimitAlarm::getValue(int id)
{
    Q_UNUSED(id)
    return spo2Param.getSPO2();
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool SPO2LimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int SPO2LimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = spo2Param.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int SPO2LimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = spo2Param.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int SPO2LimitAlarm::getCompare(int value, int id)
{
    if (spo2Param.isNibpSameSide() && nibpParam.isMeasuring())
    {
        // 如果打开同侧功能，且nibp正在测量，则不设置报警
        return 0;
    }
    switch (id)
    {
        case SPO2_LIMIT_ALARM_SPO2_HIGH:
            if (value > getUpper(id))
            {
                return 1;
            }
            else
            {
                return 0;
            }
            break;
        default:
            if (value < getLower(id))
            {
                return -1;
            }
            else
            {
                return 0;
            }
            break;
    }
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *SPO2LimitAlarm::toString(int id)
{
    return SPO2Symbol::convert((SPO2LimitAlarmType)id);
}

/**************************************************************************************************
 * 发生报警。
 *************************************************************************************************/
void SPO2LimitAlarm::notifyAlarm(int id, bool isAlarm)
{
    switch (id)
    {
    case SPO2_LIMIT_ALARM_SPO2_LOW:
        _isAlarmLimit |= isAlarm;
        break;
    case SPO2_LIMIT_ALARM_SPO2_HIGH:
        _isAlarmLimit |= isAlarm;
        spo2Param.noticeLimitAlarm(_isAlarmLimit);
        _isAlarmLimit = false;
        break;
    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2LimitAlarm::SPO2LimitAlarm()
{
    _isAlarmLimit = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2LimitAlarm::~SPO2LimitAlarm()
{
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString SPO2OneShotAlarm::getAlarmSourceName(void)
{
    QString str(paramInfo.getParamName(PARAM_SPO2));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int SPO2OneShotAlarm::getAlarmSourceNR(void)
{
    return SPO2_ONESHOT_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID SPO2OneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority SPO2OneShotAlarm::getAlarmPriority(int id)
{
    if (id == SPO2_ONESHOT_ALARM_COMMUNICATION_STOP)
    {
        return ALARM_PRIO_HIGH;
    }
    else if (id == SPO2_ONESHOT_ALARM_LOW_PERFUSION)
    {
        return ALARM_PRIO_PROMPT;
    }
    else
    {
        return ALARM_PRIO_MED;
    }
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType SPO2OneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * check whether alarmed。
 *************************************************************************************************/
bool SPO2OneShotAlarm::isAlarmed(int id)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        return false;
    }

    if (id == SPO2_ONESHOT_ALARM_CHECK_SENSOR)
    {
        return AlarmOneShotIFace::isAlarmed(id) && spo2Param.getEverCheckFinger();
    }

    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *SPO2OneShotAlarm::toString(int id)
{
    return SPO2Symbol::convert((SPO2OneShotType)id);
}

/**************************************************************************************************
 * remove alarm message after latch。
 *************************************************************************************************/
bool SPO2OneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2OneShotAlarm::SPO2OneShotAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2OneShotAlarm::~SPO2OneShotAlarm()
{
}
