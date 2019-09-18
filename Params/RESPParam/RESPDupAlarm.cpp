/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by tongzhou fang <fangtongzhou@blmed.cn>, 2019/1/31
 **/
#include "RESPDupAlarm.h"
#include "RESPDupParam.h"
#include "IConfig.h"
#include "PatientManager.h"
#include "ParamInfo.h"
#include "CO2Param.h"
#include "AlarmConfig.h"
#include "ECGParam.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString RESPDupLimitAlarm::getAlarmSourceName(void)
{
    return paramInfo.getParamName(PARAM_DUP_RESP);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
ParamID RESPDupLimitAlarm::getParamID(void)
{
    return PARAM_DUP_RESP;
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int RESPDupLimitAlarm::getAlarmSourceNR(void)
{
    return RESP_DUP_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID RESPDupLimitAlarm::getWaveformID(int id)
{
    switch (id)
    {
        case RESP_DUP_LIMIT_ALARM_RR_HIGH:
        case RESP_DUP_LIMIT_ALARM_RR_LOW:
            return WAVE_RESP;

        case RESP_DUP_LIMIT_ALARM_BR_HIGH:
        case RESP_DUP_LIMIT_ALARM_BR_LOW:
            return WAVE_CO2;

        default:
            return WAVE_NONE;
    }
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID RESPDupLimitAlarm::getSubParamID(int id)
{
    Q_UNUSED(id);
    return SUB_PARAM_RR_BR;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority RESPDupLimitAlarm::getAlarmPriority(int paramID)
{
    Q_UNUSED(paramID);
    return alarmConfig.getLimitAlarmPriority(SUB_PARAM_RR_BR);
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int RESPDupLimitAlarm::getValue(int paramIndex)
{
    Q_UNUSED(paramIndex);
    return respDupParam.getRR();
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool RESPDupLimitAlarm::isAlarmEnable(int paramID)
{
    if (!alarmConfig.isLimitAlarmEnable(SUB_PARAM_RR_BR))
    {
        return false;
    }

    switch (paramID)
    {
        case RESP_DUP_LIMIT_ALARM_RR_HIGH:
        case RESP_DUP_LIMIT_ALARM_RR_LOW:
            return respDupParam.isEnabled();

        case RESP_DUP_LIMIT_ALARM_BR_HIGH:
        case RESP_DUP_LIMIT_ALARM_BR_LOW:
            return co2Param.isEnabled();

        default:
            return false;
    }
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int RESPDupLimitAlarm::getUpper(int paramID)
{
    Q_UNUSED(paramID);
    UnitType unit = respDupParam.getCurrentUnit(SUB_PARAM_RR_BR);
    return alarmConfig.getLimitAlarmConfig(SUB_PARAM_RR_BR, unit).highLimit;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int RESPDupLimitAlarm::getLower(int paramID)
{
    Q_UNUSED(paramID);
    UnitType unit = respDupParam.getCurrentUnit(SUB_PARAM_RR_BR);
    return alarmConfig.getLimitAlarmConfig(SUB_PARAM_RR_BR, unit).lowLimit;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int RESPDupLimitAlarm::getCompare(int value, int id)
{
    if ((respDupParam.getBrSource() == RESPDupParam::BR_SOURCE_ECG && id > 1)
            || (respDupParam.getBrSource() == RESPDupParam::BR_SOURCE_CO2 && id < 2))
    {
        return 0;
    }
    switch (id)
    {
        case RESP_DUP_LIMIT_ALARM_RR_HIGH:
            if (!ecgParam.isConnected())
            {
                return 0;
            }
            else if (value > getUpper(id))
            {
                return 1;
            }
            else
            {
                return 0;
            }

        case RESP_DUP_LIMIT_ALARM_RR_LOW:
            if (!ecgParam.isConnected())
            {
                return 0;
            }
            else if (value < getLower(id))
            {
                return -1;
            }
            else
            {
                return 0;
            }

        case RESP_DUP_LIMIT_ALARM_BR_HIGH:
            if (!co2Param.isConnected())
            {
                return 0;
            }
            else if (value > getUpper(id))
            {
                return 1;
            }
            else
            {
                return 0;
            }

        case RESP_DUP_LIMIT_ALARM_BR_LOW:
            if (!co2Param.isConnected())
            {
                return 0;
            }
            else if (value < getLower(id))
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

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *RESPDupLimitAlarm::toString(int id)
{
    return RESPSymbol::convert((RESPDupLimitAlarmType)id);
}

/**************************************************************************************************
 * 发生报警。
 *************************************************************************************************/
void RESPDupLimitAlarm::notifyAlarm(int id, bool isAlarm)
{
    _isRRAlarm |= isAlarm;
    if (id == RESP_DUP_LIMIT_ALARM_BR_HIGH)
    {
        respDupParam.isAlarm(_isRRAlarm, true);
        _isRRAlarm = false;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
RESPDupLimitAlarm::RESPDupLimitAlarm()
{
    _isRRAlarm = false;
    _isBRAlarm = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
RESPDupLimitAlarm::~RESPDupLimitAlarm()
{
}
