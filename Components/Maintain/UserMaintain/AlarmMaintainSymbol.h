#pragma once
#include "AlarmMaintainDefine.h"
#include "LanguageManager.h"
// 将定义的枚举转换成符号。
class AlarmMaintainSymbol
{
public:
    static const char *convert(MaintainAlarmPauseTime index)
    {
        static const char *symbol[ALARM_PAUSE_NR] =
        {
            "1min", "2min", "3min", "5min", "10min",
            "15min", "forever"
        };
        return symbol[index];
    }

    static const char *convert(AlarmSTDelay index)
    {
        static const char *symbol[ALARM_ST_DELAY_NR] =
        {
            "30s", "45s", "1min", "1.5min",
            "2min", "3min"
        };
        return symbol[index];
    }

};
