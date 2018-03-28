#include "AlarmNormalState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmNormalState::AlarmNormalState() : AlarmState(ALARM_NORMAL_STATE)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmNormalState::~AlarmNormalState()
{

}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmNormalState::enter()
{
    alarmIndicator.setAudioStatus(ALARM_AUDIO_NORMAL);
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmNormalState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
        {
            //删除栓锁报警
            alarmIndicator.delLatchPhyAlarm();
            alarmIndicator.techAlarmPauseStatusHandle();

            if (alarmIndicator.phyAlarmPauseStatusHandle())
            {
                alarmStateMachine.switchState(ALARM_PAUSE_STATE);
            }
            break;
        }

        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME:
            if (alarmStateMachine.isEnableAlarmAudioOff())
            {
                alarmStateMachine.switchState(ALARM_AUDIO_OFF_STATE);
            }
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
