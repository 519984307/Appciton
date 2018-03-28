#include "AlarmPauseState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmPauseState::AlarmPauseState() : AlarmState(ALARM_PAUSE_STATE)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmPauseState::~AlarmPauseState()
{

}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmPauseState::enter()
{
    alarmIndicator.setAudioStatus(ALARM_AUDIO_SUSPEND);
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmPauseState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
        {
            // 有栓锁的报警和新的技术报警
            bool ret = alarmIndicator.techAlarmPauseStatusHandle();
            if (alarmIndicator.hasLatchPhyAlarm())
            {
                alarmIndicator.delLatchPhyAlarm();
                ret |= true;
            }

            // 有处于未暂停的报警
            if (alarmIndicator.hasNonPausePhyAlarm())
            {
                alarmIndicator.phyAlarmPauseStatusHandle();
                ret |= true;
            }

            if (ret)
            {
                return;
            }

            alarmIndicator.phyAlarmPauseStatusHandle();
            alarmStateMachine.switchState(ALARM_NORMAL_STATE);
            break;
        }

        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME:
            if (alarmStateMachine.isEnableAlarmAudioOff())
            {
                alarmStateMachine.switchState(ALARM_AUDIO_OFF_STATE);
            }
            break;

        case ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED:
        case ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM:
            alarmStateMachine.switchState(ALARM_NORMAL_STATE);
            break;

        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME:
            if (alarmStateMachine.isEnableAlarmOff())
            {
                alarmStateMachine.switchState(ALARM_OFF_STATE);
            }
            break;

        default:
            break;
    }
}




