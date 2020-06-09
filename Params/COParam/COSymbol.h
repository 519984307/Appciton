/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/6/8
 **/

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

    static const char *convert(COTiSource index)
    {
        static const char *symbol[CO_TI_SOURCE_NR] =
        {
            "Auto", "Manual"
        };
        return symbol[index];
    }

    static const char *convert(COMeasureCtrl index)
    {
        static const char *symbol[CO_MEASURE_NR] =
        {
            "COEnd", "COStart"
        };
        return symbol[index];
    }
};
