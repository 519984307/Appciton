/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/17
 **/

#pragma once

// 报警状态事件
enum AlarmStateEvent
{
    ALARM_STATE_EVENT_MUTE_BTN_PRESSED,
    ALARM_STATE_EVENT_MUTE_BTN_RELEASED,
    ALARM_STATE_EVENT_MUTE_BTN_PRESSED_SHORT_TIME,
    ALARM_STATE_EVENT_MUTE_BTN_PRESSED_LONG_TIME,
    ALARM_STATE_EVENT_ALL_PHY_ALARM_LATCHED,
    ALARM_STATE_EVENT_NO_PAUSED_PHY_ALARM,
    ALARM_STATE_EVENT_RESET_BTN_PRESSED,
    ALARM_STATE_EVENT_RESET_BTN_RELEASED,
    ALARM_STATE_EVENT_NEW_PHY_ALARM,
    ALARM_STATE_EVENT_NEW_TECH_ALARM,
    ALARM_STATE_EVENT_NO_ACKNOWLEDG_ALARM,
    ALARM_STATE_EVENT_NR
};

// 报警状态类型
enum ALarmStateType
{
    ALARM_STATE_NONE,
    ALARM_NORMAL_STATE,
    ALARM_PAUSE_STATE,
    ALARM_AUDIO_OFF_STATE,
    ALARM_OFF_STATE,
    ALARM_RESET_STATE,
};

