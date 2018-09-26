/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/25
 **/

#pragma once
#include <QString>
#include "ParamInfo.h"
#include "UnitManager.h"
#include <QMutex>
#include <QList>

#define MAX_TREND_CACHE_NUM (240)//缓存的趋势数据条数

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

struct TrendAlarmStatus
{
    QMap<SubParamID, bool> alarms;
};

struct TrendRecorder
{
    int toTimestamp;        /* record trend until this timestamp */
    void *obj;              /* object that the trend data record to */

    /* call when trend data ready*/
    void (*completeCallback)(unsigned timestamp, const TrendCacheData &data, const TrendAlarmStatus &almStatus, void *obj);
};

// 子参数ID、数值映射。
typedef QMap<unsigned, TrendCacheData> TrendCacheMap;
typedef QMap<unsigned, TrendAlarmStatus> TrendAlarmStatusCacheMap;

/**************************************************************************************************
 * Trend数据缓存
 *************************************************************************************************/
class TrendCache
{
public:
    static TrendCache &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new TrendCache();
        }

        return *_selfObj;
    }
    static TrendCache *_selfObj;
    ~TrendCache();

public:
    // 收集趋势数据
    void collectTrendData(unsigned t, bool overwrite = false);
    void collectTrendAlarmStatus(unsigned t);


    /**
     * @brief getTrendData get a list of trend cache data
     * @param start start timestamp
     * @param stop stop timestamp
     * @return trend cache data list
     */
    QList<TrendCacheData> getTrendData(unsigned start, unsigned stop);

    /**
     * @brief getTrendAlarmStatus get a list of trend alarm status
     * @param start start timestamp
     * @param stop stop timestamp
     * @return trend alarm status data list
     */
    QList<TrendAlarmStatus> getTrendAlarmStatus(unsigned start, unsigned stop);

    // 获取趋势数据
    bool getTendData(unsigned t, TrendCacheData &data);
    bool getTrendAlarmStatus(unsigned t, TrendAlarmStatus &alarmStatus);

    unsigned getLastNibpMeasureTime() const
    {
        return _nibpMeasureTime;
    }
    unsigned getLastNibpSuccessMeasureTime() const
    {
        return _nibpMeasureSuccessTime;
    }

    /* register a recorder to record the trend data, it will deleted when timestamp expired */
    void registerTrendRecorder(const TrendRecorder &recorder);

    /* unregister a recorder, use the record object to find the recorder, return true if the recorder is remove */
    bool unregisterTrendRecorder(void *recordObj);

private:
    TrendCache();

private:
    TrendCacheMap _trendCacheMap;
    TrendAlarmStatusCacheMap _trendAlarmStatusCacheMap;
    QList<TrendRecorder> _recorders;
    unsigned _nibpMeasureTime;
    unsigned _nibpMeasureSuccessTime;
    QMutex _mutex;
};

#define trendCache (TrendCache::construction())
#define deleteTrendCache() (delete TrendCache::_selfObj)
