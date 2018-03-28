#pragma once
#include "TrendDataDefine.h"

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
};
