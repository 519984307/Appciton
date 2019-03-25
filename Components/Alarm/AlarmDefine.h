/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/


#pragma once
#include <stddef.h>
#include <string.h>
#define ALARM_INFO_DISPLAY_TIME (3)//每条报警信息显示时间

enum AlarmType
{
    ALARM_TYPE_PHY,
    ALARM_TYPE_LIFE,
    ALARM_TYPE_TECH
};

enum LimitAlarmType
{
    NotLimitAlarm,
    LowLimitAlarm,
    HighLimitAlarm
};

enum AlarmPriority
{
    ALARM_PRIO_PROMPT = -1,     // 指示提示信息。
    ALARM_PRIO_LOW,        // 低级。
    ALARM_PRIO_MED,        // 中级。
    ALARM_PRIO_HIGH,       // 高级。
};
class AlarmParamIFace;
struct AlarmInfoNode
{
    AlarmInfoNode()
    {
        reset();
    }

    AlarmInfoNode(unsigned t, AlarmType type, AlarmPriority priority,
                  const char *message, AlarmParamIFace *source, int id)
        : alarmType(type), alarmPriority(priority), latch(false), acknowledge(false),
          removeAfterLatch(false), promptAlarmBeep(false), pauseTime(0), displayTime(ALARM_INFO_DISPLAY_TIME),
          alarmTime(t), alarmMessage(message), alarmSource(source), alarmID(id), removeLigthAfterConfirm(false)
    {
    }

    void reset()
    {
        alarmType = ALARM_TYPE_TECH;
        alarmPriority = ALARM_PRIO_LOW;
        latch = false;
        acknowledge = false;
        removeAfterLatch = false;
        promptAlarmBeep = false;
        pauseTime = 0;
        displayTime = ALARM_INFO_DISPLAY_TIME;
        alarmTime = 0;
        alarmMessage = NULL;
        alarmSource = NULL;
        alarmID = 0;
        removeLigthAfterConfirm = false;
    }

    bool operator == (const AlarmInfoNode &other) const
    {
        if (this->alarmMessage == NULL || other.alarmMessage == NULL)
        {
            return false;
        }

        if (this->alarmType == other.alarmType && this->alarmPriority == other.alarmPriority &&
                this->latch == other.latch && this->acknowledge == other.acknowledge &&
                this->removeAfterLatch == other.removeAfterLatch &&
                this->promptAlarmBeep == other.promptAlarmBeep &&
                this->pauseTime == other.pauseTime && this->displayTime == other.displayTime &&
                this->alarmTime == other.alarmTime &&
                (strcmp(this->alarmMessage, other.alarmMessage) == 0) &&
                this->alarmID == other.alarmID &&
                this->removeLigthAfterConfirm == other.removeLigthAfterConfirm)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    AlarmType alarmType;
    AlarmPriority alarmPriority;
    bool latch;
    bool acknowledge;
    bool removeAfterLatch;
    bool promptAlarmBeep;  // new prompt need to beep
    unsigned pauseTime;
    unsigned displayTime;
    unsigned alarmTime;
    const char *alarmMessage;
    AlarmParamIFace *alarmSource;
    int alarmID;
    bool removeLigthAfterConfirm;
};

enum AlarmStatus
{
    ALARM_STATUS_NORMAL,   // 报警正常。
    ALARM_STATUS_PAUSE,    // alarm pause。
    ALARM_STATUS_AUDIO_OFF,// 报警音关闭。
    ALARM_STATUS_OFF,      // 报警关闭
    ALARM_STATUS_RESET,    // alarm reset
};

// 报警暂停时间
enum AlarmPauseTime
{
    ALARM_PAUSE_TIME_1MIN,
    ALARM_PAUSE_TIME_2MIN,
    ALARM_PAUSE_TIME_3MIN,
    ALARM_PAUSE_TIME_5MIN,
    ALARM_PAUSE_TIME_10MIN,
    ALARM_PAUSE_TIME_15MIN,
    ALARM_PAUSE_TIME_NR
};

// 报警关闭/报警音关闭提示机制
enum AlarmClosePromptTime
{
    ALARM_CLOSE_PROMPT_OFF,
    ALARM_CLOSE_PROMPT_5MIN,
    ALARM_CLOSE_PROMPT_10MIN,
    ALARM_CLOSE_PROMPT_15MIN,
    ALARM_CLOSE_PROMPT_NR
};

/**************************************************************************************************
 * 窒息报警时间。
 *************************************************************************************************/
enum ApneaAlarmTime
{
    APNEA_ALARM_TIME_OFF,
    APNEA_ALARM_TIME_20_SEC,
    APNEA_ALARM_TIME_25_SEC,
    APNEA_ALARM_TIME_30_SEC,
    APNEA_ALARM_TIME_35_SEC,
    APNEA_ALARM_TIME_40_SEC,
    APNEA_ALARM_TIME_45_SEC,
    APNEA_ALARM_TIME_50_SEC,
    APNEA_ALARM_TIME_55_SEC,
    APNEA_ALARM_TIME_60_SEC,
    APNEA_ALARM_TIME_NR
};
