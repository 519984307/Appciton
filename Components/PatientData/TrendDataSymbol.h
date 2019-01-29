/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/12/20
 **/



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
            "15min", "30min", "1h", "2h", "3h", "NIBP"
        };
        return symbol[index];
    }

    static const int &convertValue(ResolutionRatio index)
    {
        static const int symbol[RESOLUTION_RATIO_NR] =
        {
            5, 30, 60, 300, 600, 900, 1800, 3600, 7200, 10800, InvData()
        };
        return symbol[index];
    }
};
