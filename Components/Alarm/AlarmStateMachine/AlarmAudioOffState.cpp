#include <QTimerEvent>
#include "AlarmAudioOffState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "IConfig.h"
#include "AlarmDefine.h"
#include "SoundManager.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmAudioOffState::AlarmAudioOffState() : AlarmState(ALARM_AUDIO_OFF_STATE)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmAudioOffState::~AlarmAudioOffState()
{

}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmAudioOffState::enter()
{
    // 定时发出报警声音关闭提示音
    int time = ALARM_CLOSE_PROMPT_OFF;
    superConfig.getNumValue("Alarm|AlarmOffPrompting", time);
    if (time != ALARM_CLOSE_PROMPT_OFF && time < ALARM_CLOSE_PROMPT_NR)
    {
        beginTimer(time * 5 * 60 * 1000);
    }

    alarmIndicator.setAudioStatus(ALARM_AUDIO_OFF);
    alarmIndicator.clearAlarmPause();
}


/**************************************************************************************************
 * 状态退出。
 *************************************************************************************************/
void AlarmAudioOffState::exit()
{
    endTimer();
}

/**************************************************************************************************
 * 定时器事件处理。
 *************************************************************************************************/
void AlarmAudioOffState::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == getTimerID())
    {
        soundManager.playSound(SOUND_TYPE_TECH_ALARM, ALARM_PRIO_LOW);
    }
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmAudioOffState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
        {
            bool ret = alarmIndicator.techAlarmPauseStatusHandle();
            if (alarmIndicator.hasLatchPhyAlarm())
            {
                alarmIndicator.delLatchPhyAlarm();
                ret |= true;
            }
            break;
        }

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


