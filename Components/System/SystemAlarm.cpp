#include "SystemAlarm.h"
#include "SystemManager.h"
#include "KeyActionManager.h"

SystemAlarm * SystemAlarm::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemAlarm::SystemAlarm()
{
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SystemAlarm::~SystemAlarm()
{

}

/**************************************************************************************************
 * 获取报警类型。
 *************************************************************************************************/
QString SystemAlarm::getAlarmSourceName(void)
{
    return "SystemOneAlarm";
}

/**************************************************************************************************
 * 获取报警类型。
 *************************************************************************************************/
AlarmPriority SystemAlarm::getAlarmPriority(int id)
{
    switch (id)
    {
    case SYSTEM_ONE_SHOT_ALARM_COMMUNICATION_STOP:
        return ALARM_PRIO_MED;

    case POWERUP_PANEL_SYNC_PRESSED:
    case POWERUP_PANEL_RECORD_PRESSED:
        return ALARM_PRIO_LOW;

    case SOME_LIMIT_ALARM_DISABLED:
        return ALARM_PRIO_PROMPT;

    default:
        return ALARM_PRIO_LOW;
    }
}

/**************************************************************************************************
 * 是否报警。
 *************************************************************************************************/
bool SystemAlarm::isAlarmed(int id)
{
    bool keyPressed = false;
//    static bool keyStunk[POWERUP_PANEL_ENERGY_DN_PRESSED - POWERUP_PANEL_SYNC_PRESSED + 1];

    switch (id)
    {
        case POWERUP_PANEL_SYNC_PRESSED:
//            if (systemManager.getWorkMode() == WORK_MODE_MDEFIB)
//            {
//                keyPressed = keyActionManager.checkKeyPressed(KEY_F2_PRESSED);
//            }
            break;

        case POWERUP_PANEL_RECORD_PRESSED:
            keyPressed = keyActionManager.checkKeyPressed(KEY_F9_PRESSED);
            break;

        default:
            return AlarmOneShotIFace::isAlarmed(id);
    }

    bool bret = false;

        bret = keyPressed;

    return bret;
}

/**************************************************************************************************
 * 获取报警类型。
 *************************************************************************************************/
AlarmType SystemAlarm::getAlarmType(int /*id*/)
{
    return ALARM_TYPE_TECH;
}

const static char* alarmLimitOneshotStr[] =
{
    "SomeLimitAlarmDisabled",
    "PowerupSyncPressed",
    "PowerupChargePressed",
    "PowerupShockPressed",
    "PowerupRecorderPressed",
    "PowerupEnergyUpPressed",
    "PowerupEnergyDnPressed",
    "PowerupCommunicationStop"
};

/**************************************************************************************************
 * 获取报警字串。
 *************************************************************************************************/
const char* SystemAlarm::toString(int id)
{
    if (id >= SYSTEM_ONE_SHOT_ALARM_NR)
    {
        return NULL;
    }

    return alarmLimitOneshotStr[id];
}

