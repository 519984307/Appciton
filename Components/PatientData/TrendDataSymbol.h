#pragma once
#include "TrendDataDefine.h"
#include "EventDataDefine.h"

class TrendDataSymbol
{
public:
    static const char *convert(ResolutionRatio index)
    {
        static const char *symbol[RESOLUTION_RATIO_NR] =
        {
            "5s", "30s", "1min", "5min", "10min",
            "15min", "30min", "1h", "2h", "3h"
        };
        return symbol[index];
    }

    static const int &convertValue(ResolutionRatio index)
    {
        static const int symbol[RESOLUTION_RATIO_NR] =
        {
            5, 30, 60, 300, 600, 900, 1800, 3600, 7200, 10800
        };
        return symbol[index];
    }
};
