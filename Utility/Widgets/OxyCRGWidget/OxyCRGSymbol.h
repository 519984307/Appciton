/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/



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
            "_1min", "_2min", "_4min", "_8min"
        };
        return symbol[index];
    }

    static const char *convert(OxyCRGTrend index)
    {
        static const char *symbol[OxyCRG_Trend_NR] =
        {
            "RESP", "CO2"
        };
        return symbol[index];
    }
};
