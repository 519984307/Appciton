#pragma once
#include "TimeDefine.h"

class TimeSymbol
{
public:
    static const char *convert(DateFormat index)
    {
        const char *symbol[DATE_FORMAT_NR] =
        {
            "YMD",
            "MDY",
            "DMY"
        };

        return symbol[index];
    }

    static const char *convert(TimeFormat index)
    {
        const char *symbol[TIME_FORMAT_NR] =
        {
            "12 Hour", "24 Hour"
        };

        return symbol[index];
    }
};

