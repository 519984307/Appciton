#pragma once
#include "CPRDefine.h"

// 将定义的枚举转换成符号。
class CPRSymbol
{
public:
    static const char *convert(CPREnable index)
    {
        static const char *symbol[CPR_ENABLE_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    static const char *convert(CPRVoicePrompt index)
    {
        static const char *symbol[CPR_VOCIE_PROMPT_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }

    static const char *convert(CPRMetronome index)
    {
        static const char *symbol[CPR_METRONOME_NR] =
        {
            "Off", "On"
        };
        return symbol[index];
    }
};
