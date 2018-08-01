#pragma once
#include <QString>
#include "CODefine.h"

// 将定义的枚举转换成符号。
class COSymbol
{
public:
    static const char *convert(COLimitAlarmType index)
    {
        static const char *symbol[CO_LIMIT_ALARM_NR] =
        {
            "COCOLow", "COCOHigh",
            "COCILow", "COCIHigh",
            "COTBLow", "COTBHigh"
        };
        return symbol[index];
    }

    static const char *convert(COOneShotType index)
    {
        static const char *symbol[CO_ONESHOT_NR] =
        {
            "TBLeadOff", "TILeadOff"
        };
        return symbol[index];
    }

    static const char *convert(COTiMode index)
    {
        static const char *symbol[CO_TI_MODE_NR] =
        {
            "Auto", "Manual"
        };
        return symbol[index];
    }

    static const char *convert(COInstCtl index)
    {
        static const char *symbol[CO_INST_NR] =
        {
            "COEnd", "COStart", "COInterrupt"
        };
        return symbol[index];
    }
};
