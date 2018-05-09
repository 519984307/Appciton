#include <QTimerEvent>
#include "AlarmOffState.h"
#include "AlarmIndicator.h"
#include "AlarmStateMachine.h"
#include "IConfig.h"
#include "AlarmDefine.h"
#include "SoundManager.h"
#include "SystemManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AlarmOffState::AlarmOffState() : AlarmState(ALARM_OFF_STATE)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AlarmOffState::~AlarmOffState()
{

}

/**************************************************************************************************
 * 状态进入。
 *************************************************************************************************/
void AlarmOffState::enter()
{
    // 定时发出报警关闭提示音
    int time = ALARM_CLOSE_PROMPT_OFF;
    currentConfig.getNumValue("Alarm|AlarmOffPrompting", time);
    if (time > ALARM_CLOSE_PROMPT_OFF && time < ALARM_CLOSE_PROMPT_NR)
    {
        beginTimer(time * 5 * 60 * 1000);
    }

    alarmIndicator.setAudioStatus(ALARM_OFF);
    alarmIndicator.delAllPhyAlarm();
}

/**************************************************************************************************
 * 状态退出。
 *************************************************************************************************/
void AlarmOffState::exit()
{
    endTimer();
}

/**************************************************************************************************
 * 定时器事件处理。
 *************************************************************************************************/
void AlarmOffState::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == getTimerID())
    {
    //    soundManager.playSound(SOUND_TYPE_ALARM_OFF_PROMPT, ALARM_PRIO_PROMPT);
    }
}

/**************************************************************************************************
 * 事件处理。
 *************************************************************************************************/
void AlarmOffState::handAlarmEvent(AlarmStateEvent event, unsigned char */*data*/, unsigned /*len*/)
{
    switch (event)
    {
        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED:
        {
            if (alarmIndicator.techAlarmPauseStatusHandle())
            {
                return;
            }
            alarmStateMachine.switchState(ALARM_NORMAL_STATE);
            break;
        }

        case ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME:
            if (alarmStateMachine.isEnableAlarmAudioOff())
            {
                alarmStateMachine.switchState(ALARM_AUDIO_OFF_STATE);
            }
            break;

        default:
            break;
    }
}






