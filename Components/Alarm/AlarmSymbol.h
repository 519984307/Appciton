/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/1/11
 **/


#pragma once
#include "AlarmDefine.h"

class AlarmSymbol
{
public:
    AlarmSymbol() {}
    ~AlarmSymbol() {}

    static const char *convert(AlarmPauseTime index)
    {
        const char *pauseTime[] =
        {
            "_1min", "_2min", "_3min", "_5min", "_10min", "_15min"
        };

        return pauseTime[index];
    }

    static const char *convert(AlarmClosePromptTime index)
    {
        const char *closePromptTime[] =
        {
            "Off", "_5min", "_10min", "_15min"
        };

        return closePromptTime[index];
    }

    static const char *convert(ApneaAlarmTime index)
    {
        static const char *symbol[APNEA_ALARM_TIME_NR] =
        {
            "Off",
            "_20sec", "_25sec",
            "_30sec", "_35sec", "_40sec", "_45sec",
            "_50sec", "_55sec", "_60sec"
        };
        return symbol[index];
    }
};

