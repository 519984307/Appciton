/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/2
 **/

#include "TEMPAlarm.h"
#include "TEMPParam.h"
#include "IConfig.h"
#include "TEMPSymbol.h"
#include "PatientManager.h"
#include "ParamManager.h"
#include "FloatHandle.h"
#include "AlarmConfig.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString TEMPLimitAlarm::getAlarmSourceName(void)
{
    return paramInfo.getParamName(PARAM_TEMP);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int TEMPLimitAlarm::getAlarmSourceNR(void)
{
    return TEMP_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID TEMPLimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID TEMPLimitAlarm::getSubParamID(int id)
{
    switch (id)
    {
    case TEMP_LIMIT_ALARM_T1_HIGH:
    case TEMP_LIMIT_ALARM_T1_LOW:
        return SUB_PARAM_T1;

    case TEMP_LIMIT_ALARM_T2_HIGH:
    case TEMP_LIMIT_ALARM_T2_LOW:
        return SUB_PARAM_T2;

    default:
        return SUB_PARAM_TD;
    }
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority TEMPLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int TEMPLimitAlarm::getValue(int id)
{
    short t1 = tempParam.getTEMP(SUB_PARAM_T1);
    short t2 = tempParam.getTEMP(SUB_PARAM_T2);
    short td = tempParam.getTEMP(SUB_PARAM_TD);

    switch (id)
    {
    case TEMP_LIMIT_ALARM_T1_HIGH:
    case TEMP_LIMIT_ALARM_T1_LOW:
        return t1;

    case TEMP_LIMIT_ALARM_T2_HIGH:
    case TEMP_LIMIT_ALARM_T2_LOW:
        return t2;

    default:
        return td;
    }
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool TEMPLimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int TEMPLimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = tempParam.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int TEMPLimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = tempParam.getCurrentUnit(subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int TEMPLimitAlarm::getCompare(int value, int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType curUnit = paramManager.getSubParamUnit(PARAM_TEMP, subID);
    UnitType defUnit = paramInfo.getUnitOfSubParam(subID);
    int mul = paramInfo.getMultiOfSubParam(subID);

    LimitAlarmConfig limitConfig = alarmConfig.getLimitAlarmConfig(subID, curUnit);
    float low = static_cast<float>(limitConfig.lowLimit) / limitConfig.scale;
    float high = static_cast<float>(limitConfig.highLimit) / limitConfig.scale;
    float v = value * 1.0 / mul;
    float v1 = value * 1.0 / mul;

    QString str = paramInfo.getSubParamName(subID);
    QString valueStr;
    if (curUnit != defUnit)
    {
        valueStr = Unit::convert(curUnit, defUnit, v1);
        v = valueStr.toDouble();

        if (str == "T1")
        {
            _t1 = v;
        }
        else if (str == "T2")
        {
            _t2 = v;
        }
        else if (str == "TD")
        {
            v = fabs(_t1 - _t2);
        }
    }

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

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *TEMPLimitAlarm::toString(int id)
{
    return TEMPSymbol::convert((TEMPLimitAlarmType)id);
}

/**************************************************************************************************
 * 超限报警通知。
 *************************************************************************************************/
void TEMPLimitAlarm::notifyAlarm(int id, bool flag)
{
    int subID = getSubParamID(id);

    switch (id)
    {
    case TEMP_LIMIT_ALARM_T1_HIGH:
        _isT1Alarm |= flag;
        tempParam.noticeLimitAlarm(subID, _isT1Alarm);
        _isT1Alarm = false;
        break;

    case TEMP_LIMIT_ALARM_T1_LOW:
        _isT1Alarm |= flag;
        break;

    case TEMP_LIMIT_ALARM_T2_HIGH:
        _isT2Alarm |= flag;
        tempParam.noticeLimitAlarm(subID, _isT2Alarm);
        _isT2Alarm = false;
        break;

    case TEMP_LIMIT_ALARM_T2_LOW:
        _isT2Alarm |= flag;
        break;

    case TEMP_LIMIT_ALARM_TD_LOW:
        _isTDAlarm |= flag;
        break;

    case TEMP_LIMIT_ALARM_TD_HIGH:
        _isTDAlarm |= flag;
        tempParam.noticeLimitAlarm(subID, _isTDAlarm);
        _isTDAlarm = false;
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TEMPLimitAlarm::TEMPLimitAlarm() : _isT1Alarm(false), _isT2Alarm(false), _isTDAlarm(false),
    _t1(0), _t2(0)
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TEMPLimitAlarm::~TEMPLimitAlarm()
{
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString TEMPOneShotAlarm::getAlarmSourceName(void)
{
    QString str(paramInfo.getParamName(PARAM_TEMP));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int TEMPOneShotAlarm::getAlarmSourceNR(void)
{
    return TEMP_ONESHOT_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID TEMPOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority TEMPOneShotAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
    case TEMP_OVER_RANGR_1:
    case TEMP_OVER_RANGR_2:
    case TEMP_OVER_RANGR_ALL:
    case TEMP_ONESHOT_ALARM_MODULE_DISABLE:
    case TEMP_ONESHOT_ALARM_MODULE_ABNORMAL:
        return ALARM_PRIO_HIGH;
    default:
        return ALARM_PRIO_MED;
    }
}
/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType TEMPOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *TEMPOneShotAlarm::toString(int id)
{
    return TEMPSymbol::convert((TEMPOneShotType)id);
}

/**************************************************************************************************
 * latch后移除报警显示信息。
 *************************************************************************************************/
bool TEMPOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

bool TEMPOneShotAlarm::isAlarmed(int id)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        return false;
    }

    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
TEMPOneShotAlarm::TEMPOneShotAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TEMPOneShotAlarm::~TEMPOneShotAlarm()
{
}
