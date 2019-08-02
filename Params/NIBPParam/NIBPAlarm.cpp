/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/1/23
 **/


#include "NIBPAlarm.h"
#include "NIBPParam.h"
#include "IConfig.h"
#include "NIBPSymbol.h"
#include "ParamManager.h"
#include "FloatHandle.h"
#include "AlarmConfig.h"

/**************************************************************************************************
 * 获取正确的报警源名字。
 *************************************************************************************************/
void NIBPLimitAlarm::_getPath(int id, QString &path)
{
    switch (id)
    {
        case NIBP_LIMIT_ALARM_SYS_HIGH:
        case NIBP_LIMIT_ALARM_SYS_LOW:
            path = paramInfo.getSubParamName(SUB_PARAM_NIBP_SYS);
            break;

        case NIBP_LIMIT_ALARM_DIA_HIGH:
        case NIBP_LIMIT_ALARM_DIA_LOW:
            path = paramInfo.getSubParamName(SUB_PARAM_NIBP_DIA);
            break;

        default:
            path = paramInfo.getSubParamName(SUB_PARAM_NIBP_MAP);
            break;
    }
}

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString NIBPLimitAlarm::getAlarmSourceName(void)
{
    return paramInfo.getParamName(PARAM_NIBP);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int NIBPLimitAlarm::getAlarmSourceNR(void)
{
    return NIBP_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID NIBPLimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID NIBPLimitAlarm::getSubParamID(int id)
{
    switch (id)
    {
        case NIBP_LIMIT_ALARM_SYS_HIGH:
        case NIBP_LIMIT_ALARM_SYS_LOW:
            return SUB_PARAM_NIBP_SYS;

        case NIBP_LIMIT_ALARM_DIA_HIGH:
        case NIBP_LIMIT_ALARM_DIA_LOW:
            return SUB_PARAM_NIBP_DIA;

        default:
            return SUB_PARAM_NIBP_MAP;
    }
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority NIBPLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int NIBPLimitAlarm::getValue(int id)
{
    switch (id)
    {
        case NIBP_LIMIT_ALARM_SYS_HIGH:
        case NIBP_LIMIT_ALARM_SYS_LOW:
            return nibpParam.getSYS();

        case NIBP_LIMIT_ALARM_DIA_HIGH:
        case NIBP_LIMIT_ALARM_DIA_LOW:
            return nibpParam.getDIA();

        default:
            return nibpParam.getMAP();
    }
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool NIBPLimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int NIBPLimitAlarm::getUpper(int id)
{
    SubParamID subId = getSubParamID(id);
    UnitType unit = paramManager.getSubParamUnit(PARAM_NIBP, subId);
    return alarmConfig.getLimitAlarmConfig(subId, unit).highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int NIBPLimitAlarm::getLower(int id)
{
    SubParamID subId = getSubParamID(id);
    UnitType unit = paramManager.getSubParamUnit(PARAM_NIBP, subId);
    return alarmConfig.getLimitAlarmConfig(subId, unit).lowLimit;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int NIBPLimitAlarm::getCompare(int value, int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType curUnit = paramManager.getSubParamUnit(PARAM_NIBP, subID);
    UnitType defUnit = paramInfo.getUnitOfSubParam(subID);
    int mul = paramInfo.getMultiOfSubParam(subID);

    LimitAlarmConfig limitConfig = alarmConfig.getLimitAlarmConfig(subID, curUnit);

    if (curUnit == defUnit)
    {
        int low = limitConfig.lowLimit;
        int high = limitConfig.highLimit;
        value /= mul;

        if (0 == id % 2)
        {
            if (value < low)
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
            if (value > high)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        float low = static_cast<float>(limitConfig.lowLimit) / limitConfig.scale;
        float high = static_cast<float>(limitConfig.highLimit) / limitConfig.scale;
        float v = 0;
        float v1 = value * 1.0 / mul;
        QString valueStr = Unit::convert(curUnit, defUnit, v1);
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
const char *NIBPLimitAlarm::toString(int id)
{
    return NIBPSymbol::convert((NIBPLimitAlarmType)id);
}

/**************************************************************************************************
 * 报警通知。
 *************************************************************************************************/
void NIBPLimitAlarm::notifyAlarm(int id, bool flag)
{
    int subID = getSubParamID(id);

    switch (id)
    {
        case NIBP_LIMIT_ALARM_SYS_HIGH:
            _isSysAlarm |= flag;
            nibpParam.noticeLimitAlarm(subID, _isSysAlarm);
            _isSysAlarm = false;
            break;

        case NIBP_LIMIT_ALARM_SYS_LOW:
            _isSysAlarm |= flag;
            break;

        case NIBP_LIMIT_ALARM_DIA_HIGH:
            _isDiaAlarm |= flag;
            nibpParam.noticeLimitAlarm(subID, _isDiaAlarm);
            _isDiaAlarm = false;
            break;

        case NIBP_LIMIT_ALARM_DIA_LOW:
            _isDiaAlarm |= flag;
            break;

        case NIBP_LIMIT_ALARM_MAP_LOW:
            _isMapAlarm |= flag;
            break;

        case NIBP_LIMIT_ALARM_MAP_HIGH:
            _isMapAlarm |= flag;
            nibpParam.noticeLimitAlarm(subID, _isMapAlarm);
            _isMapAlarm = false;
            break;

        default:
            break;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPLimitAlarm::NIBPLimitAlarm()
{
    _alarmNum = 0;
    _isSysAlarm = 0;
    _isDiaAlarm = 0;
    _isMapAlarm = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPLimitAlarm::~NIBPLimitAlarm()
{
}

/**************************************************************************************************
 *---*---*---*---*---*---*---*---*---*---*---*----*---*---*---*---*---*---*---*---*---*---*---*---*
 *************************************************************************************************/

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString NIBPOneShotAlarm::getAlarmSourceName(void)
{
    QString str(paramInfo.getParamName(PARAM_NIBP));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int NIBPOneShotAlarm::getAlarmSourceNR(void)
{
    return NIBP_ONESHOT_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID NIBPOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority NIBPOneShotAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
        case NIBP_ONESHOT_ALARM_MODULE_DISABLE:
        case NIBP_ONESHOT_ALARM_COMMUNICATION_STOP:
            return ALARM_PRIO_HIGH;
        default:
            return ALARM_PRIO_MED;
    }
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType NIBPOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *NIBPOneShotAlarm::toString(int id)
{
    return NIBPSymbol::convert((NIBPOneShotType)id);
}

/**************************************************************************************************
 * latch后移除报警显示信息。
 *************************************************************************************************/
bool NIBPOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

bool NIBPOneShotAlarm::isAlarmed(int id)
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
NIBPOneShotAlarm::NIBPOneShotAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPOneShotAlarm::~NIBPOneShotAlarm()
{
}
