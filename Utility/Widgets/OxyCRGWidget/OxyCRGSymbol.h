#pragma once
#include "OxyCRGDefine.h"

// 将定义的枚举转换成符号。
class OxyCRGSymbol
{
public:
    static const char *convert(OxyCRGInterval index)
    {
        static const char *symbol[OxyCRG_Interval_NR] =
        {
            "_1min", "_2min", "_4min"
        };
        return symbol[index];
    }

    static const char *convert(OxyCRGTrend index)
    {
        static const char *symbol[OxyCRG_Trend_NR] =
        {
            "RESP", "RR"
        };
        return symbol[index];
    }
};
