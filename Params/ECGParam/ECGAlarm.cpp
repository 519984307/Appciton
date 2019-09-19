/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/12/5
 **/

#include "ECGAlarm.h"
#include "ECGParam.h"
#include "ECGDupParam.h"
#include "NIBPParam.h"
#include "IConfig.h"
#include "ECGSymbol.h"
#include "PatientManager.h"
#include "SystemManager.h"
#include "AlarmConfig.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString ECGLimitAlarm::getAlarmSourceName(void)
{
    return QString(paramInfo.getParamName(PARAM_ECG));
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int ECGLimitAlarm::getAlarmSourceNR(void)
{
    return 1; // 只有心率报警。
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID ECGLimitAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return static_cast<WaveformID>(ecgParam.getCalcLead());
}

/**************************************************************************************************
 * 获取id对应的参数ID。
 *************************************************************************************************/
SubParamID ECGLimitAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_HR_PR;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority ECGLimitAlarm::getAlarmPriority(int id)
{
    return alarmConfig.getLimitAlarmPriority(getSubParamID(id));
}

/**************************************************************************************************
 * 获取指定的生理参数测量数据。
 *************************************************************************************************/
int ECGLimitAlarm::getValue(int paramIndex)
{
    Q_UNUSED(paramIndex)
    return 0;
}

/**************************************************************************************************
 * 生理参数报警使能。
 *************************************************************************************************/
bool ECGLimitAlarm::isAlarmEnable(int paramID)
{
    Q_UNUSED(paramID)
    return true;
}

/**************************************************************************************************
 * 生理参数报警上限。
 *************************************************************************************************/
int ECGLimitAlarm::getUpper(int paramID)
{
    Q_UNUSED(paramID)
    return 0;
}

/**************************************************************************************************
 * 生理参数报警下限。
 *************************************************************************************************/
int ECGLimitAlarm::getLower(int paramID)
{
    Q_UNUSED(paramID)
    return 0;
}

/**************************************************************************************************
 * 生理参数报警比较。
 *************************************************************************************************/
int ECGLimitAlarm::getCompare(int value, int id)
{
    Q_UNUSED(value)
    Q_UNUSED(id)
    return 0;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *ECGLimitAlarm::toString(int paramID)
{
    Q_UNUSED(paramID)
    return NULL;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGLimitAlarm::ECGLimitAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGLimitAlarm::~ECGLimitAlarm()
{
}

/**************************************************************************************************
 *************************************************************************************************/

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString ECGOneShotAlarm::getAlarmSourceName(void)
{
    QString str(paramInfo.getParamName(PARAM_ECG));
    return (str + "OneShotAlarm");
}

/**************************************************************************************************
 * 是否发生报警。
 *************************************************************************************************/
bool ECGOneShotAlarm::isAlarmed(int id)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        return false;
    }

    if (id >= ECG_ONESHOT_ALARM_LEADOFF && id <= ECG_ONESHOT_ALARM_V6_LEADOFF && ecgParam.getFristConnect())
    {
        switch (id)
        {
        case ECG_ONESHOT_ALARM_LEADOFF:
            for (int i = ECG_LEAD_I; i <= ECG_LEAD_AVF; ++i)
            {
                if (1 == ecgParam.doesLeadOff(i))
                {
                    return ecgParam.getEverLeadOn((ECGLead)i);
                }
            }
            return false;

        default:
        {
            for (int i = ECG_LEAD_I; i <= ECG_LEAD_AVF; ++i)
            {
                if (1 == ecgParam.doesLeadOff(i))
                {
                    return false;
                }
            }

            if (1 == ecgParam.doesLeadOff(ECG_LEAD_V1 + id - ECG_ONESHOT_ALARM_V1_LEADOFF))
            {
                return ecgParam.getEverLeadOn((ECGLead)(ECG_LEAD_V1 + id -
                                                        ECG_ONESHOT_ALARM_V1_LEADOFF));
            }

            return false;
        }
        }
    }
    return AlarmOneShotIFace::isAlarmed(id);
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int ECGOneShotAlarm::getAlarmSourceNR(void)
{
    return ECG_ONESHOT_NR;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID ECGOneShotAlarm::getWaveformID(int id)
{
    // todo:返回计算导联的ID
    if ((id >= ECG_ONESHOT_ARR_ASYSTOLE) && (id <= ECG_ONESHOT_ARR_VFIBVTAC))
    {
        return WAVE_ECG_II;
    }
    return WAVE_NONE;
}

SubParamID ECGOneShotAlarm::getSubParamID(int id)
{
    Q_UNUSED(id)
    return SUB_PARAM_HR_PR;
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
AlarmPriority ECGOneShotAlarm::getAlarmPriority(int id)
{
    if (id < ECG_ONESHOT_ALARM_LEADOFF || id == ECG_ONESHOT_ALARM_COMMUNICATION_STOP)
    {
        return ALARM_PRIO_HIGH;
    }
    else if ((id >= ECG_ONESHOT_ALARM_TRANSFER_SUCCESS) && (id <= ECG_ONESHOT_ALARM_TRANSFER_FAILED))
    {
        return ALARM_PRIO_PROMPT;
    }
//    else if (id > ECG_ONESHOT_ALARM_V6_LEADOFF)
    else if (id > ECG_ONESHOT_ALARM_TRANSFER_FAILED)
    {
        return ALARM_PRIO_MED;
    }
    else
    {

        return ALARM_PRIO_MED;
    }
}

/**************************************************************************************************
 * 是否需要删除。
 *************************************************************************************************/
bool ECGOneShotAlarm::isNeedRemove(int id)
{
    switch (id)
    {
    default:
        return false;
    }
}

/**************************************************************************************************
 * 生理参数报警级别。
 *************************************************************************************************/
void ECGOneShotAlarm::notifyAlarm(int id, bool isAlarm)
{
    if ((id >= ECG_ONESHOT_ARR_ASYSTOLE) && (id <= ECG_ONESHOT_ARR_VFIBVTAC))
    {
        _isPhyAlarm |= isAlarm;
    }
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType ECGOneShotAlarm::getAlarmType(int id)
{
    if ((id >= ECG_ONESHOT_ARR_ASYSTOLE) && (id <= ECG_ONESHOT_ARR_VFIBVTAC))
    {
        return ALARM_TYPE_PHY;
    }

    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *ECGOneShotAlarm::toString(int id)
{
    bool is12Lead = ecgParam.getLeadMode() < ECG_LEAD_MODE_12 ? 0 : 1;
    return ECGSymbol::convert((ECGOneShotType)id, ecgParam.getLeadConvention(), is12Lead);
}

/**************************************************************************************************
 * 报警使能。
 *************************************************************************************************/
bool ECGOneShotAlarm::isAlarmEnable(int id)
{
    if ((id >= ECG_ONESHOT_ARR_ASYSTOLE) && (id <= ECG_ONESHOT_ARR_VFIBVTAC))
    {
        return alarmConfig.isLimitAlarmEnable(SUB_PARAM_HR_PR);
    }

    return true;
}

/**************************************************************************************************
 * latch后移除报警显示信息。
 *************************************************************************************************/
bool ECGOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

bool ECGOneShotAlarm::isRemoveLightAfterConfirm(int id)
{
    if (id >= ECG_ONESHOT_ALARM_LEADOFF && id <= ECG_ONESHOT_ALARM_OVERLOAD)
    {
        return false;
    }

    return true;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGOneShotAlarm::ECGOneShotAlarm()
{
    _isPhyAlarm = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGOneShotAlarm::~ECGOneShotAlarm()
{
}

