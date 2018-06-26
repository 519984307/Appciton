#include "IBPAlarm.h"
#include "ParamInfo.h"
#include "AlarmConfig.h"
#include "IBPParam.h"

IBPLimitAlarm *IBPLimitAlarm::_selfObj = NULL;

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
    case IBP1_LIMIT_ALARM_SYS_LOW:
    case IBP1_LIMIT_ALARM_SYS_HIGH:
    case IBP1_LIMIT_ALARM_DIA_LOW:
    case IBP1_LIMIT_ALARM_DIA_HIGH:
    case IBP1_LIMIT_ALARM_MEAN_LOW:
    case IBP1_LIMIT_ALARM_MEAN_HIGH:
    case IBP1_LIMIT_ALARM_PR_LOW:
    case IBP1_LIMIT_ALARM_PR_HIGH:
        return WAVE_IBP1;
    case IBP2_LIMIT_ALARM_SYS_LOW:
    case IBP2_LIMIT_ALARM_SYS_HIGH:
    case IBP2_LIMIT_ALARM_DIA_LOW:
    case IBP2_LIMIT_ALARM_DIA_HIGH:
    case IBP2_LIMIT_ALARM_MEAN_LOW:
    case IBP2_LIMIT_ALARM_MEAN_HIGH:
    case IBP2_LIMIT_ALARM_PR_LOW:
    case IBP2_LIMIT_ALARM_PR_HIGH:
        return WAVE_IBP2;
    default:
        return WAVE_NONE;
    }
}

/**************************************************************************************************
 * alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID IBPLimitAlarm::getSubParamID(int id)
{
    IBPPressureName ibp1 = ibpParam.getParamData(IBP_INPUT_1).pressureName;
    IBPPressureName ibp2 = ibpParam.getParamData(IBP_INPUT_2).pressureName;
    switch (id)
    {
    case IBP1_LIMIT_ALARM_SYS_LOW:
    case IBP1_LIMIT_ALARM_SYS_HIGH:
    {
        switch (ibp1)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_SYS;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_SYS;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_SYS;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_SYS;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP1_LIMIT_ALARM_DIA_LOW:
    case IBP1_LIMIT_ALARM_DIA_HIGH:
    {
        switch (ibp1)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_DIA;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_DIA;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_DIA;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_DIA;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP1_LIMIT_ALARM_MEAN_LOW:
    case IBP1_LIMIT_ALARM_MEAN_HIGH:
    {
        switch (ibp1)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_MAP;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_MAP;
        case IBP_PRESSURE_CVP:
            return SUB_PARAM_CVP_MAP;
        case IBP_PRESSURE_LAP:
            return SUB_PARAM_LAP_MAP;
        case IBP_PRESSURE_RAP:
            return SUB_PARAM_RAP_MAP;
        case IBP_PRESSURE_ICP:
            return SUB_PARAM_ICP_MAP;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_MAP;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_MAP;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP1_LIMIT_ALARM_PR_LOW:
    case IBP1_LIMIT_ALARM_PR_HIGH:
    {
        switch (ibp1)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_PR;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_PR;
        case IBP_PRESSURE_CVP:
            return SUB_PARAM_CVP_PR;
        case IBP_PRESSURE_LAP:
            return SUB_PARAM_LAP_PR;
        case IBP_PRESSURE_RAP:
            return SUB_PARAM_RAP_PR;
        case IBP_PRESSURE_ICP:
            return SUB_PARAM_ICP_PR;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_PR;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_PR;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP2_LIMIT_ALARM_SYS_LOW:
    case IBP2_LIMIT_ALARM_SYS_HIGH:
    {
        switch (ibp2)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_SYS;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_SYS;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_SYS;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_SYS;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP2_LIMIT_ALARM_DIA_LOW:
    case IBP2_LIMIT_ALARM_DIA_HIGH:
    {
        switch (ibp2)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_DIA;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_DIA;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_DIA;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_DIA;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP2_LIMIT_ALARM_MEAN_LOW:
    case IBP2_LIMIT_ALARM_MEAN_HIGH:
    {
        switch (ibp2)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_MAP;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_MAP;
        case IBP_PRESSURE_CVP:
            return SUB_PARAM_CVP_MAP;
        case IBP_PRESSURE_LAP:
            return SUB_PARAM_LAP_MAP;
        case IBP_PRESSURE_RAP:
            return SUB_PARAM_RAP_MAP;
        case IBP_PRESSURE_ICP:
            return SUB_PARAM_ICP_MAP;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_MAP;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_MAP;
        default:
            return SUB_PARAM_NONE;
        }
    }
    case IBP2_LIMIT_ALARM_PR_LOW:
    case IBP2_LIMIT_ALARM_PR_HIGH:
    {
        switch (ibp2)
        {
        case IBP_PRESSURE_ART:
            return SUB_PARAM_ART_PR;
        case IBP_PRESSURE_PA:
            return SUB_PARAM_PA_PR;
        case IBP_PRESSURE_CVP:
            return SUB_PARAM_CVP_PR;
        case IBP_PRESSURE_LAP:
            return SUB_PARAM_LAP_PR;
        case IBP_PRESSURE_RAP:
            return SUB_PARAM_RAP_PR;
        case IBP_PRESSURE_ICP:
            return SUB_PARAM_ICP_PR;
        case IBP_PRESSURE_AUXP1:
            return SUB_PARAM_AUXP1_PR;
        case IBP_PRESSURE_AUXP2:
            return SUB_PARAM_AUXP2_PR;
        default:
            return SUB_PARAM_NONE;
        }
    }
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
    switch (id)
    {
    case IBP1_LIMIT_ALARM_SYS_LOW:
    case IBP1_LIMIT_ALARM_SYS_HIGH:
        return ibpParam.getParamData(IBP_INPUT_1).sys;
    case IBP1_LIMIT_ALARM_DIA_LOW:
    case IBP1_LIMIT_ALARM_DIA_HIGH:
        return ibpParam.getParamData(IBP_INPUT_1).dia;
    case IBP1_LIMIT_ALARM_MEAN_LOW:
    case IBP1_LIMIT_ALARM_MEAN_HIGH:
        return ibpParam.getParamData(IBP_INPUT_1).mean;
    case IBP1_LIMIT_ALARM_PR_LOW:
    case IBP1_LIMIT_ALARM_PR_HIGH:
        return ibpParam.getParamData(IBP_INPUT_1).pr;
    case IBP2_LIMIT_ALARM_SYS_LOW:
    case IBP2_LIMIT_ALARM_SYS_HIGH:
        return ibpParam.getParamData(IBP_INPUT_2).sys;
    case IBP2_LIMIT_ALARM_DIA_LOW:
    case IBP2_LIMIT_ALARM_DIA_HIGH:
        return ibpParam.getParamData(IBP_INPUT_2).dia;
    case IBP2_LIMIT_ALARM_MEAN_LOW:
    case IBP2_LIMIT_ALARM_MEAN_HIGH:
        return ibpParam.getParamData(IBP_INPUT_2).mean;
    case IBP2_LIMIT_ALARM_PR_LOW:
    case IBP2_LIMIT_ALARM_PR_HIGH:
        return ibpParam.getParamData(IBP_INPUT_2).pr;
    default:
        return -1;
    }
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
    UnitType unit;
    if (id == IBP1_LIMIT_ALARM_PR_LOW || id == IBP1_LIMIT_ALARM_PR_HIGH ||
            id == IBP2_LIMIT_ALARM_PR_LOW || id == IBP2_LIMIT_ALARM_PR_HIGH)
    {
        unit = UNIT_BPM;
    }
    else
    {
        unit = UNIT_MMHG;
    }
    return alarmConfig.getLimitAlarmConfig(subID, unit).highLimit;
}

/**************************************************************************************************
 * get lower limit of physiological parameter alarm.
 *************************************************************************************************/
int IBPLimitAlarm::getLower(int id)
{
    SubParamID subID = getSubParamID(id);
    UnitType unit;
    if (id == IBP1_LIMIT_ALARM_PR_LOW || id == IBP1_LIMIT_ALARM_PR_HIGH ||
            id == IBP2_LIMIT_ALARM_PR_LOW || id == IBP2_LIMIT_ALARM_PR_HIGH)
    {
        unit = UNIT_BPM;
    }
    else
    {
        unit = UNIT_MMHG;
    }
    return alarmConfig.getLimitAlarmConfig(subID, unit).lowLimit;
}

/**************************************************************************************************
 * compare physiological parameter alarm.
 *************************************************************************************************/
int IBPLimitAlarm::getCompare(int value, int id)
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
const char *IBPLimitAlarm::toString(int id)
{
    return IBPSymbol::convert((IBPLimitAlarmType)id);
}

/**************************************************************************************************
 * alarm notify.
 *************************************************************************************************/
void IBPLimitAlarm::notifyAlarm(int id, bool isAlarm)
{
    _isAlarmLimit |= isAlarm;

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
IBPLimitAlarm::IBPLimitAlarm()
{

}

/**************************************************************************************************
 *************************************************************************************************/

IBPOneShotAlarm *IBPOneShotAlarm::_selfObj = NULL;

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
WaveformID IBPOneShotAlarm::getWaveformID(int /*id*/)
{
    return WAVE_NONE;
}

/**************************************************************************************************
 * one shot alarm id corresponding to param ID
 *************************************************************************************************/
SubParamID IBPOneShotAlarm::getSubParamID(int /*id*/)
{
    return SUB_PARAM_NONE;
}

/**************************************************************************************************
 * one shot alarm priority.
 *************************************************************************************************/
AlarmPriority IBPOneShotAlarm::getAlarmPriority(int /*id*/)
{
    return ALARM_PRIO_LOW;
}

/**************************************************************************************************
 * one shot alarm type.
 *************************************************************************************************/
AlarmType IBPOneShotAlarm::getAlarmType(int /*id*/)
{
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * alarm enable.
 *************************************************************************************************/
bool IBPOneShotAlarm::isAlarmed(int id)
{
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
bool IBPOneShotAlarm::isAlarmEnable(int /*id*/)
{
    return true;
}

/**************************************************************************************************
 *  remove alarm display information later latch.
 *************************************************************************************************/
bool IBPOneShotAlarm::isRemoveAfterLatch(int /*id*/)
{
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
