/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/22
 **/

#include "BatteryAlarm.h"
#include "BatteryDefine.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 报警源的名字。
 *************************************************************************************************/
QString BatteryOneShotAlarm::getAlarmSourceName(void)
{
    return "BatteryOneShotAlarm";
}

/**************************************************************************************************
 * 报警源的个数。
 *************************************************************************************************/
int BatteryOneShotAlarm::getAlarmSourceNR(void)
{
    return BAT_ONESHOT_NR;
}

/**************************************************************************************************
 * 技术报警报警级别。
 *************************************************************************************************/
AlarmPriority BatteryOneShotAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
        case BAT_ONESHOT_REPLACE_BATTERY:
            return ALARM_PRIO_HIGH;
        case BAT_ONESHOT_LOW_BATTERY:
            return ALARM_PRIO_MED;

        case BAT_ONESHOT_NO_BATTERY:
            return ALARM_PRIO_PROMPT;

        default:
            return ALARM_PRIO_LOW;
    }
}

/**************************************************************************************************
 * 该报警是否为生命报警，技术报警和生理/生命报警分开存放。
 *************************************************************************************************/
AlarmType BatteryOneShotAlarm::getAlarmType(int id)
{
    Q_UNUSED(id)
    return ALARM_TYPE_TECH;
}

/**************************************************************************************************
 * 将报警ID转换成字串。
 *************************************************************************************************/
const char *BatteryOneShotAlarm::toString(int id)
{
    return BatterySymbol::convert((BatOneShotType)id);
}

/**************************************************************************************************
 * 获取报警对应的波形ID，该波形将被存储。
 *************************************************************************************************/
WaveformID BatteryOneShotAlarm::getWaveformID(int id)
{
    Q_UNUSED(id)
    return WAVE_NONE;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
BatteryOneShotAlarm::BatteryOneShotAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
BatteryOneShotAlarm::~BatteryOneShotAlarm()
{
}

