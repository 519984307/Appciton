#pragma once
#include "PACEDefine.h"

// 将定义的枚举转换成符号。
class PACESymbol
{
public:
    static const char *convert(PACEMode index)
    {
        static const char *symbol[PACE_MODE_NR] =
        {
            "PACEDemand", "PACEFixedRate"
        };
        return symbol[index];
    }
};
