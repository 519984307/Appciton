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
    Q_UNUSED(id);
    return SUB_PARAM_HR_PR;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority ECGDupLimitAlarm::getAlarmPriority(int paramID)
{
    Q_UNUSED(paramID);
    return alarmConfig.getLimitAlarmPriority(SUB_PARAM_HR_PR);
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int ECGDupLimitAlarm::getValue(int paramIndex)
{
    Q_UNUSED(paramIndex);
    return ecgDupParam.getHR();
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool ECGDupLimitAlarm::isAlarmEnable(int paramID)
{
    Q_UNUSED(paramID);
    return alarmConfig.isLimitAlarmEnable(SUB_PARAM_HR_PR);
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int ECGDupLimitAlarm::getUpper(int paramID)
{
    Q_UNUSED(paramID);
    UnitType unit = ecgDupParam.getCurrentUnit(SUB_PARAM_HR_PR);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_HR_PR, unit);
    return config.highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int ECGDupLimitAlarm::getLower(int paramID)
{
    Q_UNUSED(paramID);
    UnitType unit = ecgDupParam.getCurrentUnit(SUB_PARAM_HR_PR);
    LimitAlarmConfig config = alarmConfig.getLimitAlarmConfig(SUB_PARAM_HR_PR, unit);
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
    _isAlarmLimit |= isAlarm;
    if (id == ECG_DUP_LIMIT_ALARM_PR_HIGH)
    {
        ecgDupParam.isAlarm(_isAlarmLimit, true);
        _isAlarmLimit = false;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGDupLimitAlarm::ECGDupLimitAlarm()
{
    _isAlarmLimit = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGDupLimitAlarm::~ECGDupLimitAlarm()
{
}
