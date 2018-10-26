/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/28
 **/


#pragma once
#include "TimeDefine.h"

class TimeSymbol
{
public:
    static const char *convert(DateFormat index)
    {
        const char *symbol[DATE_FORMAT_NR] =
        {
            "YearMonthDay",
            "MonthDayYear",
            "DayMonthYear"
        };

        return symbol[index];
    }

    static const char *convert(TimeFormat index)
    {
        const char *symbol[TIME_FORMAT_NR] =
        {
            "_12Hour", "_24Hour"
        };

        return symbol[index];
    }
};

