/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/

#pragma once
#include <QList>
#include "BaseDefine.h"
#include "ParamDefine.h"

// 趋势缓存数据
struct TrendCacheData
{
    TrendCacheData()
    {
        lastNibpMeasureTime = 0;
        lastNibpMeasureSuccessTime = 0;
        co2baro = 0;
    }

    TrendCacheData &operator=(const TrendCacheData &data)
    {
        lastNibpMeasureTime = data.lastNibpMeasureTime;
        lastNibpMeasureSuccessTime = data.lastNibpMeasureSuccessTime;
        co2baro = data.co2baro;
        values = data.values;
        return *this;
    }
    unsigned lastNibpMeasureTime;
    unsigned lastNibpMeasureSuccessTime;
    TrendDataType co2baro;
    QMap<SubParamID, TrendDataType> values;
};


class TrendCacheInterface
{
public:
    virtual ~TrendCacheInterface(){}

    static TrendCacheInterface *registerTrendCache(TrendCacheInterface *instance);

    static TrendCacheInterface *getTrendCache(void);

    /**
     * @brief getTrendData 获取趋势数据
     * @param t
     * @param data
     * @return
     */
    virtual bool getTrendData(unsigned t, TrendCacheData &data) = 0;
};
