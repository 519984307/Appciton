/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/22
 **/

#include "PrintAlarm.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString PrintOneShotAlarm::getAlarmSourceName(void)
{
    return "PrintOneShotAlarm";
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int PrintOneShotAlarm::getAlarmSourceNR(void)
{
    return PRINT_ONESHOT_NR;
}

/**************************************************************************************************
 * 技术报警报警级别。
 *************************************************************************************************/
AlarmPriority PrintOneShotAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
    case PRINT_ONESHOT_ALARM_FAULT:
        return ALARM_PRIO_HIGH;
    default:
        return ALARM_PRIO_MED;
    }
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType PrintOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *PrintOneShotAlarm::toString(int id)
{
    return PrintSymbol::convert((PrintOneShotType)id);
}

/**************************************************************************************************
 * is remove message after acknowledge alarm。
 *************************************************************************************************/
bool PrintOneShotAlarm::isRemoveAfterLatch(int id)
{
    Q_UNUSED(id)
    return false;
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID PrintOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
PrintOneShotAlarm::PrintOneShotAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
PrintOneShotAlarm::~PrintOneShotAlarm()
{
}
