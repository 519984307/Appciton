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
          alarmTime(t), alarmMessage(message), alarmSource(source), alarmID(id)
    {
        //    alarmType = type;
        //    alarmPriority = priority;
        //    latch = false;
        //    acknowledge = false;
        //    removeAfterLatch = false;
        //    pauseTime = 0;
        //    displayTime = ALARM_INFO_DISPLAY_TIME;
        //    alarmTime = t;
        //    alarmMessage = message;
        //    promptAlarmBeep = false;
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
};

enum AlarmAudioStatus
{
    ALARM_AUDIO_NORMAL,   // 报警音正常。
    ALARM_AUDIO_SUSPEND,  // 报警音挂起。
    ALARM_AUDIO_OFF,      // 报警音关闭。
    ALARM_OFF,            // 报警关闭
};

// 报警暂停时间
enum AlarmPauseTime
{
    ALARM_PAUSE_TIME_60S,
    ALARM_PAUSE_TIME_90S,
    ALARM_PAUSE_TIME_120S,
    ALARM_PAUSE_TIME_150S,
    ALARM_PAUSE_TIME_180S,
    ALARM_PAUSE_TIME_210S,
    ALARM_PAUSE_TIME_240S,
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
