/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/11/8
 **/

#pragma once
#include "AlarmMaintainDefine.h"
#include "Framework/Language/LanguageManager.h"
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
