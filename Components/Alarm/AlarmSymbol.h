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
            "_60sec", "_90sec", "_120sec", "_150sec", "_180sec", "_210sec", "_240sec"
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
            "Off", "_20sec", "_25sec",
            "_30sec", "_35sec", "_40sec", "_45sec",
            "_50sec", "_55sec", "_60sec"
        };
        return symbol[index];
    }
};

