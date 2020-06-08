/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/28
 **/

#include "IBPAlarm.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "IBPParam.h"
#include "SystemManager.h"
#include "ParamManager.h"
#include "FloatHandle.h"

/**************************************************************************************************
 * alarm source name.
 *************************************************************************************************/
QString IBPLimitAlarm::getAlarmSourceName()
{
    return paramInfo.getParamName(PARAM_IBP);
}

/**************************************************************************************************
 * alarm source number.
 *************************************************************************************************/
int IBPLimitAlarm::getAlarmSourceNR()
{
    return IBP_LIMIT_ALARM_NR;
}

/**************************************************************************************************
 * alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID IBPLimitAlarm::getWaveformID(int id)
{
    switch (id)
    {
    case ART_LIMIT_ALARM_SYS_LOW:
    case ART_LIMIT_ALARM_SYS_HIGH:
    case ART_LIMIT_ALARM_DIA_LOW:
    case ART_LIMIT_ALARM_DIA_HIGH:
    case ART_LIMIT_ALARM_MEAN_LOW:
    case ART_LIMIT_ALARM_MEAN_HIGH:
    case ART_LIMIT_ALARM_PR_LOW:
    case ART_LIMIT_ALARM_PR_HIGH:
        return WAVE_ART;
    case PA_LIMIT_ALARM_SYS_LOW:
    case PA_LIMIT_ALARM_SYS_HIGH:
    case PA_LIMIT_ALARM_DIA_LOW:
    case PA_LIMIT_ALARM_DIA_HIGH:
    case PA_LIMIT_ALARM_MEAN_LOW:
    case PA_LIMIT_ALARM_MEAN_HIGH:
    case PA_LIMIT_ALARM_PR_LOW:
    case PA_LIMIT_ALARM_PR_HIGH:
        return WAVE_PA;
    case CVP_LIMIT_ALARM_MEAN_LOW:
    case CVP_LIMIT_ALARM_MEAN_HIGH:
    case CVP_LIMIT_ALARM_PR_LOW:
    case CVP_LIMIT_ALARM_PR_HIGH:
        return WAVE_CVP;
    case LAP_LIMIT_ALARM_MEAN_LOW:
    case LAP_LIMIT_ALARM_MEAN_HIGH:
    case LAP_LIMIT_ALARM_PR_LOW:
    case LAP_LIMIT_ALARM_PR_HIGH:
        return WAVE_LAP;
    case RAP_LIMIT_ALARM_MEAN_LOW:
    case RAP_LIMIT_ALARM_MEAN_HIGH:
    case RAP_LIMIT_ALARM_PR_LOW:
    case RAP_LIMIT_ALARM_PR_HIGH:
        return WAVE_RAP;
    case ICP_LIMIT_ALARM_MEAN_LOW:
    case ICP_LIMIT_ALARM_MEAN_HIGH:
    case ICP_LIMIT_ALARM_PR_LOW:
    case ICP_LIMIT_ALARM_PR_HIGH:
        return WAVE_ICP;
    case AUXP1_LIMIT_ALARM_SYS_LOW:
    case AUXP1_LIMIT_ALARM_SYS_HIGH:
    case AUXP1_LIMIT_ALARM_DIA_LOW:
    case AUXP1_LIMIT_ALARM_DIA_HIGH:
    case AUXP1_LIMIT_ALARM_MEAN_LOW:
    case AUXP1_LIMIT_ALARM_MEAN_HIGH:
    case AUXP1_LIMIT_ALARM_PR_LOW:
    case AUXP1_LIMIT_ALARM_PR_HIGH:
        return WAVE_AUXP1;
    case AUXP2_LIMIT_ALARM_SYS_LOW:
    case AUXP2_LIMIT_ALARM_SYS_HIGH:
    case AUXP2_LIMIT_ALARM_DIA_LOW:
    case AUXP2_LIMIT_ALARM_DIA_HIGH:
    case AUXP2_LIMIT_ALARM_MEAN_LOW:
    case AUXP2_LIMIT_ALARM_MEAN_HIGH:
    case AUXP2_LIMIT_ALARM_PR_LOW:
    case AUXP2_LIMIT_ALARM_PR_HIGH:
        return WAVE_AUXP2;
    default:
        return WAVE_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID IBPLimitAlarm::getSubParamID(int id)
{
    IBPLimitAlarmType alarmID = static_cast<IBPLimitAlarmType>(id);
    switch (alarmID)
    {
    case ART_LIMIT_ALARM_SYS_LOW:
    case ART_LIMIT_ALARM_SYS_HIGH:
        return SUB_PARAM_ART_SYS;
    case PA_LIMIT_ALARM_SYS_LOW:
    case PA_LIMIT_ALARM_SYS_HIGH:
        return SUB_PARAM_PA_SYS;
    case AUXP1_LIMIT_ALARM_SYS_LOW:
    case AUXP1_LIMIT_ALARM_SYS_HIGH:
        return SUB_PARAM_AUXP1_SYS;
    case AUXP2_LIMIT_ALARM_SYS_LOW:
    case AUXP2_LIMIT_ALARM_SYS_HIGH:
        return SUB_PARAM_AUXP2_SYS;
    case ART_LIMIT_ALARM_DIA_LOW:
    case ART_LIMIT_ALARM_DIA_HIGH:
        return SUB_PARAM_ART_DIA;
    case PA_LIMIT_ALARM_DIA_LOW:
    case PA_LIMIT_ALARM_DIA_HIGH:
        return SUB_PARAM_PA_DIA;
    case AUXP1_LIMIT_ALARM_DIA_LOW:
    case AUXP1_LIMIT_ALARM_DIA_HIGH:
        return SUB_PARAM_AUXP1_DIA;
    case AUXP2_LIMIT_ALARM_DIA_LOW:
    case AUXP2_LIMIT_ALARM_DIA_HIGH:
        return SUB_PARAM_AUXP2_DIA;
    case ART_LIMIT_ALARM_MEAN_LOW:
    case ART_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_ART_MAP;
    case PA_LIMIT_ALARM_MEAN_LOW:
    case PA_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_PA_MAP;
    case CVP_LIMIT_ALARM_MEAN_LOW:
    case CVP_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_CVP_MAP;
    case LAP_LIMIT_ALARM_MEAN_LOW:
    case LAP_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_LAP_MAP;
    case RAP_LIMIT_ALARM_MEAN_LOW:
    case RAP_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_RAP_MAP;
    case ICP_LIMIT_ALARM_MEAN_LOW:
    case ICP_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_ICP_MAP;
    case AUXP1_LIMIT_ALARM_MEAN_LOW:
    case AUXP1_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_AUXP1_MAP;
    case AUXP2_LIMIT_ALARM_MEAN_LOW:
    case AUXP2_LIMIT_ALARM_MEAN_HIGH:
        return SUB_PARAM_AUXP2_MAP;
    case ART_LIMIT_ALARM_PR_LOW:
    case ART_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_ART_PR;
    case PA_LIMIT_ALARM_PR_LOW:
    case PA_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_PA_PR;
    case CVP_LIMIT_ALARM_PR_LOW:
    case CVP_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_CVP_PR;
    case LAP_LIMIT_ALARM_PR_LOW:
    case LAP_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_LAP_PR;
    case RAP_LIMIT_ALARM_PR_LOW:
    case RAP_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_RAP_PR;
    case ICP_LIMIT_ALARM_PR_LOW:
    case ICP_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_ICP_PR;
    case AUXP1_LIMIT_ALARM_PR_LOW:
    case AUXP1_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_AUXP1_PR;
    case AUXP2_LIMIT_ALARM_PR_LOW:
    case AUXP2_LIMIT_ALARM_PR_HIGH:
        return SUB_PARAM_AUXP2_PR;
    default:
        return SUB_PARAM_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to alarm priority
 *************************************************************************************************/
AlarmPriority IBPLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * get alarm measure data of physiological parameter
 *************************************************************************************************/
int IBPLimitAlarm::getValue(int id)
{
    IBPLabel ibp1 = ibpParam.getEntitle(IBP_CHN_1);
    IBPLabel ibp2 = ibpParam.getEntitle(IBP_CHN_2);
    IBPLabel idName = ibpParam.getEntitle(static_cast<IBPLimitAlarmType>(id));
    switch (id)
    {
    case ART_LIMIT_ALARM_SYS_LOW:
    case ART_LIMIT_ALARM_SYS_HIGH:
    case PA_LIMIT_ALARM_SYS_LOW:
    case PA_LIMIT_ALARM_SYS_HIGH:
    case AUXP1_LIMIT_ALARM_SYS_LOW:
    case AUXP1_LIMIT_ALARM_SYS_HIGH:
    case AUXP2_LIMIT_ALARM_SYS_LOW:
    case AUXP2_LIMIT_ALARM_SYS_HIGH:
        if (idName == ibp1)
        {
            return ibpParam.getParamData(IBP_CHN_1).sys;
        }
        else if (idName == ibp2)
        {
            return ibpParam.getParamData(IBP_CHN_2).sys;
        }
        break;
    case ART_LIMIT_ALARM_DIA_LOW:
    case ART_LIMIT_ALARM_DIA_HIGH:
    case PA_LIMIT_ALARM_DIA_LOW:
    case PA_LIMIT_ALARM_DIA_HIGH:
    case AUXP1_LIMIT_ALARM_DIA_LOW:
    case AUXP1_LIMIT_ALARM_DIA_HIGH:
    case AUXP2_LIMIT_ALARM_DIA_LOW:
    case AUXP2_LIMIT_ALARM_DIA_HIGH:
        if (idName == ibp1)
        {
            return ibpParam.getParamData(IBP_CHN_1).dia;
        }
        else if (idName == ibp2)
        {
            return ibpParam.getParamData(IBP_CHN_2).dia;
        }
        break;
    case ART_LIMIT_ALARM_MEAN_LOW:
    case ART_LIMIT_ALARM_MEAN_HIGH:
    case PA_LIMIT_ALARM_MEAN_LOW:
    case PA_LIMIT_ALARM_MEAN_HIGH:
    case CVP_LIMIT_ALARM_MEAN_LOW:
    case CVP_LIMIT_ALARM_MEAN_HIGH:
    case LAP_LIMIT_ALARM_MEAN_LOW:
    case LAP_LIMIT_ALARM_MEAN_HIGH:
    case RAP_LIMIT_ALARM_MEAN_LOW:
    case RAP_LIMIT_ALARM_MEAN_HIGH:
    case ICP_LIMIT_ALARM_MEAN_LOW:
    case ICP_LIMIT_ALARM_MEAN_HIGH:
    case AUXP1_LIMIT_ALARM_MEAN_LOW:
    case AUXP1_LIMIT_ALARM_MEAN_HIGH:
    case AUXP2_LIMIT_ALARM_MEAN_LOW:
    case AUXP2_LIMIT_ALARM_MEAN_HIGH:
        if (idName == ibp1)
        {
            return ibpParam.getParamData(IBP_CHN_1).mean;
        }
        else if (idName == ibp2)
        {
            return ibpParam.getParamData(IBP_CHN_2).mean;
        }
        break;
    case ART_LIMIT_ALARM_PR_LOW:
    case ART_LIMIT_ALARM_PR_HIGH:
    case PA_LIMIT_ALARM_PR_LOW:
    case PA_LIMIT_ALARM_PR_HIGH:
    case CVP_LIMIT_ALARM_PR_LOW:
    case CVP_LIMIT_ALARM_PR_HIGH:
    case LAP_LIMIT_ALARM_PR_LOW:
    case LAP_LIMIT_ALARM_PR_HIGH:
    case RAP_LIMIT_ALARM_PR_LOW:
    case RAP_LIMIT_ALARM_PR_HIGH:
    case ICP_LIMIT_ALARM_PR_LOW:
    case ICP_LIMIT_ALARM_PR_HIGH:
    case AUXP1_LIMIT_ALARM_PR_LOW:
    case AUXP1_LIMIT_ALARM_PR_HIGH:
    case AUXP2_LIMIT_ALARM_PR_LOW:
    case AUXP2_LIMIT_ALARM_PR_HIGH:
        if (idName == ibp1)
        {
            return ibpParam.getParamData(IBP_CHN_1).pr;
        }
        else if (idName == ibp2)
        {
            return ibpParam.getParamData(IBP_CHN_2).pr;
        }
        break;
    default:
        break;
    }
    return -1;
}

/**************************************************************************************************
 * physiological parameter alarm enable.
 *************************************************************************************************/
bool IBPLimitAlarm::isAlarmEnable(int id)
{
    return alarmConfig.isLimitAlarmEnable(getSubParamID(id));
}

/**************************************************************************************************
 * get upper limit of physiological parameter alarm.
 *************************************************************************************************/
int IBPLimitAlarm::getUpper(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = paramManager.getSubParamUnit(PARAM_IBP, subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

/**************************************************************************************************
 * get lower limit of physiological parameter alarm.
 *************************************************************************************************/
int IBPLimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit = paramManager.getSubParamUnit(PARAM_IBP, subID);
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * compare physiological parameter alarm.
 *************************************************************************************************/
int IBPLimitAlarm::getCompare(int value, int id)
{
    IBPLabel ibp1 = ibpParam.getEntitle(IBP_CHN_1);
    IBPLabel ibp2 = ibpParam.getEntitle(IBP_CHN_2);
    IBPLimitAlarmType type = static_cast<IBPLimitAlarmType>(id);
    IBPLabel idName = ibpParam.getEntitle(type);
    if (idName != ibp1 && idName != ibp2)
    {
        return 0;
    }

    if (type == ART_LIMIT_ALARM_PR_HIGH || type == ART_LIMIT_ALARM_PR_LOW ||
            type == PA_LIMIT_ALARM_PR_HIGH || type == PA_LIMIT_ALARM_PR_LOW ||
            type == CVP_LIMIT_ALARM_PR_HIGH || type == CVP_LIMIT_ALARM_PR_LOW ||
            type == LAP_LIMIT_ALARM_PR_HIGH || type == LAP_LIMIT_ALARM_PR_LOW ||
            type == RAP_LIMIT_ALARM_PR_HIGH || type == RAP_LIMIT_ALARM_PR_LOW ||
            type == ICP_LIMIT_ALARM_PR_HIGH || type == ICP_LIMIT_ALARM_PR_LOW ||
            type == AUXP1_LIMIT_ALARM_PR_HIGH || type == AUXP1_LIMIT_ALARM_PR_LOW ||
            type == AUXP2_LIMIT_ALARM_PR_HIGH || type == AUXP2_LIMIT_ALARM_PR_LOW)
    {
        return 0;
    }
    // get sub param id
    SubParamID subID = getSubParamID(id);
    // get cur ibp unit
    UnitType curUnit = paramManager.getSubParamUnit(PARAM_IBP, subID);
    // get default unit
    UnitType defUnit = paramInfo.getUnitOfSubParam(subID);
    // get limit config
    LimitAlarmConfig limitConfig = alarmConfig.getLimitAlarmConfig(subID, curUnit);

    if (curUnit == defUnit)
    {
        int low = limitConfig.lowLimit;
        int high = limitConfig.highLimit;

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
        float v = value * 1.0;
        QString valueStr = Unit::convert(curUnit, defUnit, v);
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
const char *IBPLimitAlarm::toString(int id)
{
    return IBPSymbol::convert((IBPLimitAlarmType)id);
}

/**************************************************************************************************
 * alarm notify.
 *************************************************************************************************/
void IBPLimitAlarm::notifyAlarm(int id, bool flag)
{
    SubParamID subID = getSubParamID(id);
    IBPLabel ibp1 = ibpParam.getEntitle(IBP_CHN_1);
    IBPLabel ibp2 = ibpParam.getEntitle(IBP_CHN_2);
    IBPLimitAlarmType type = static_cast<IBPLimitAlarmType>(id);
    IBPLabel idName = ibpParam.getEntitle(type);

    bool isAlarm;
    switch (type)
    {
    case ART_LIMIT_ALARM_SYS_LOW:
        _isARTSYSAlarm |= flag;
        break;
    case ART_LIMIT_ALARM_SYS_HIGH:
        _isARTSYSAlarm |= flag;
        isAlarm = _isARTSYSAlarm;
        _isARTSYSAlarm = false;
        break;
    case ART_LIMIT_ALARM_DIA_LOW:
        _isARTDIAAlarm |= flag;
        break;
    case ART_LIMIT_ALARM_DIA_HIGH:
        _isARTDIAAlarm |= flag;
        isAlarm = _isARTDIAAlarm;
        _isARTDIAAlarm = false;
        break;
    case ART_LIMIT_ALARM_MEAN_LOW:
        _isARTMAPAlarm |= flag;
        break;
    case ART_LIMIT_ALARM_MEAN_HIGH:
        _isARTMAPAlarm |= flag;
        isAlarm = _isARTMAPAlarm;
        _isARTMAPAlarm = false;
        break;

    case PA_LIMIT_ALARM_SYS_LOW:
        _isPASYSAlarm |= flag;
        break;
    case PA_LIMIT_ALARM_SYS_HIGH:
        _isPASYSAlarm |= flag;
        isAlarm = _isPASYSAlarm;
        _isPASYSAlarm = false;
        break;
    case PA_LIMIT_ALARM_DIA_LOW:
        _isPADIAAlarm |= flag;
        break;
    case PA_LIMIT_ALARM_DIA_HIGH:
        _isPADIAAlarm |= flag;
        isAlarm = _isPADIAAlarm;
        _isPADIAAlarm = false;
        break;
    case PA_LIMIT_ALARM_MEAN_LOW:
        _isPAMAPAlarm |= flag;
        break;
    case PA_LIMIT_ALARM_MEAN_HIGH:
        _isPAMAPAlarm |= flag;
        isAlarm = _isPAMAPAlarm;
        _isPAMAPAlarm = false;
        break;

    case CVP_LIMIT_ALARM_MEAN_LOW:
        _isCVPMAPAlarm |= flag;
        break;
    case CVP_LIMIT_ALARM_MEAN_HIGH:
        _isCVPMAPAlarm |= flag;
        isAlarm = _isCVPMAPAlarm;
        _isCVPMAPAlarm = false;
        break;

    case LAP_LIMIT_ALARM_MEAN_LOW:
        _isLAPMAPAlarm |= flag;
        break;
    case LAP_LIMIT_ALARM_MEAN_HIGH:
        _isLAPMAPAlarm |= flag;
        isAlarm = _isLAPMAPAlarm;
        _isLAPMAPAlarm = false;
        break;

    case RAP_LIMIT_ALARM_MEAN_LOW:
        _isRAPMAPAlarm |= flag;
        break;
    case RAP_LIMIT_ALARM_MEAN_HIGH:
        _isRAPMAPAlarm |= flag;
        isAlarm = _isRAPMAPAlarm;
        _isRAPMAPAlarm = false;
        break;

    case ICP_LIMIT_ALARM_MEAN_LOW:
        _isICPMAPAlarm |= flag;
        break;
    case ICP_LIMIT_ALARM_MEAN_HIGH:
        _isICPMAPAlarm |= flag;
        isAlarm = _isICPMAPAlarm;
        _isICPMAPAlarm = false;
        break;

    case AUXP1_LIMIT_ALARM_SYS_LOW:
        _isAUXP1SYSAlarm |= flag;
        break;
    case AUXP1_LIMIT_ALARM_SYS_HIGH:
        _isAUXP1SYSAlarm |= flag;
        isAlarm = _isAUXP1SYSAlarm;
        _isAUXP1SYSAlarm = false;
        break;
    case AUXP1_LIMIT_ALARM_DIA_LOW:
        _isAUXP1DIAAlarm |= flag;
        break;
    case AUXP1_LIMIT_ALARM_DIA_HIGH:
        _isAUXP1DIAAlarm |= flag;
        isAlarm = _isAUXP1DIAAlarm;
        _isAUXP1DIAAlarm = false;
        break;
    case AUXP1_LIMIT_ALARM_MEAN_LOW:
        _isAUXP1MAPAlarm |= flag;
        break;
    case AUXP1_LIMIT_ALARM_MEAN_HIGH:
        _isAUXP1MAPAlarm |= flag;
        isAlarm = _isAUXP1MAPAlarm;
        _isAUXP1MAPAlarm = false;
        break;

    case AUXP2_LIMIT_ALARM_SYS_LOW:
        _isAUXP2SYSAlarm |= flag;
        break;
    case AUXP2_LIMIT_ALARM_SYS_HIGH:
        _isAUXP2SYSAlarm |= flag;
        isAlarm = _isAUXP2SYSAlarm;
        _isAUXP2SYSAlarm = false;
        break;
    case AUXP2_LIMIT_ALARM_DIA_LOW:
        _isAUXP2DIAAlarm |= flag;
        break;
    case AUXP2_LIMIT_ALARM_DIA_HIGH:
        _isAUXP2DIAAlarm |= flag;
        isAlarm = _isAUXP2DIAAlarm;
        _isAUXP2DIAAlarm = false;
        break;
    case AUXP2_LIMIT_ALARM_MEAN_LOW:
        _isAUXP2MAPAlarm |= flag;
        break;
    case AUXP2_LIMIT_ALARM_MEAN_HIGH:
        _isAUXP2MAPAlarm |= flag;
        isAlarm = _isAUXP2MAPAlarm;
        _isAUXP2MAPAlarm = false;
        break;
    default:
        break;
    }
    if (idName == ibp1)
    {
        ibpParam.noticeLimitAlarm(subID, isAlarm, IBP_CHN_1);
    }
    else if (idName == ibp2)
    {
        ibpParam.noticeLimitAlarm(subID, isAlarm, IBP_CHN_2);
    }
}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
IBPLimitAlarm::~IBPLimitAlarm()
{
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
IBPLimitAlarm::IBPLimitAlarm() :
    _isARTSYSAlarm(false), _isARTDIAAlarm(false), _isARTMAPAlarm(false),
    _isPASYSAlarm(false), _isPADIAAlarm(false), _isPAMAPAlarm(false),
    _isCVPMAPAlarm(false), _isLAPMAPAlarm(false), _isRAPMAPAlarm(false),
    _isICPMAPAlarm(false), _isAUXP1SYSAlarm(false), _isAUXP1DIAAlarm(false),
    _isAUXP1MAPAlarm(false), _isAUXP2SYSAlarm(false), _isAUXP2DIAAlarm(false),
    _isAUXP2MAPAlarm(false)
{
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * one shot alarm source name.
 *************************************************************************************************/
QString IBPOneShotAlarm::getAlarmSourceName()
{
    QString str(paramInfo.getParamName(PARAM_IBP));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * one shot alarm source number.
 *************************************************************************************************/
int IBPOneShotAlarm::getAlarmSourceNR()
{
    return IBP_ONESHOT_NR;
}

/**************************************************************************************************
 * one shot alarm id corresponding to waveform ID
 *************************************************************************************************/
WaveformID IBPOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * one shot alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID IBPOneShotAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * one shot alarm priority.
 *************************************************************************************************/
AlarmPriority IBPOneShotAlarm::getAlarmPriority(int id)
{
    if (id == IBP1_LEAD_OFF || id == IBP2_LEAD_OFF)
    {
        return ALARM_PRIO_MED;
    }
    else
    {
        return ALARM_PRIO_PROMPT;
    }
}

/**************************************************************************************************
 * one shot alarm type.
 *************************************************************************************************/
AlarmType IBPOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * alarm enable.
 *************************************************************************************************/
bool IBPOneShotAlarm::isAlarmed(int id)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO && getAlarmType(id) == ALARM_TYPE_TECH)
    {
        return false;
    }

    if (id == IBP1_LEAD_OFF)
    {
        return  ibpParam.isIBPLeadOff(IBP_CHN_1);
    }
    else if (id == IBP2_LEAD_OFF)
    {
        return  ibpParam.isIBPLeadOff(IBP_CHN_2);
    }
    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * notify alarm.
 *************************************************************************************************/
void IBPOneShotAlarm::notifyAlarm(int /*id*/, bool /*isAlarm*/)
{
}

/**************************************************************************************************
 * one shot alarm convert string.
 *************************************************************************************************/
const char *IBPOneShotAlarm::toString(int id)
{
    return IBPSymbol::convert((IBPOneShotType)id);
}

/**************************************************************************************************
 * enable one shot alarm.
 *************************************************************************************************/
bool IBPOneShotAlarm::isAlarmEnable(int id)
{
    Q_UNUSED(id)
    return true;
}

/**************************************************************************************************
 *  remove alarm display information later latch.
 *************************************************************************************************/
bool IBPOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

/**************************************************************************************************
 * constructor
 *************************************************************************************************/
IBPOneShotAlarm::~IBPOneShotAlarm()
{
}

/**************************************************************************************************
 * destructor
 *************************************************************************************************/
IBPOneShotAlarm::IBPOneShotAlarm()
{
}
