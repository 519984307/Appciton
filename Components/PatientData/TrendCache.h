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
#include "TrendCacheInterface.h"
#include <QMutex>
#include <QList>

#define MAX_TREND_CACHE_NUM (240)   // 缓存的趋势数据条数

struct TrendAlarmStatus
{
    QMap<SubParamID, bool> alarms;
};

struct TrendRecorder
{
    int toTimestamp;        /* record trend until this timestamp */
    void *obj;              /* object that the trend data record to */

    /* call when trend data ready*/
    void (*completeCallback)(unsigned timestamp, const TrendCacheData &data,
                             const TrendAlarmStatus &almStatus, void *obj);
};

// 子参数ID、数值映射。
typedef QMap<unsigned, TrendCacheData> TrendCacheMap;
typedef QMap<unsigned, TrendAlarmStatus> TrendAlarmStatusCacheMap;

/**************************************************************************************************
 * Trend数据缓存
 *************************************************************************************************/
class TrendCache : public TrendCacheInterface
{
public:
    static TrendCache &getInstance(void);
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
    bool getTrendData(unsigned t, TrendCacheData &data);    /* NOLINT */
    bool getTrendAlarmStatus(unsigned t, TrendAlarmStatus &alarmStatus);    /* NOLINT */

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

    /* override */
    void clearTrendCache();

    /* override */
    void stopDataCollect(quint32 times);

private:
    TrendCache();

private:
    TrendCacheMap _trendCacheMap;
    TrendAlarmStatusCacheMap _trendAlarmStatusCacheMap;
    QList<TrendRecorder> _recorders;
    unsigned _nibpMeasureTime;
    unsigned _nibpMeasureSuccessTime;
    quint32 _stopCollectTimes;
    QMutex _mutex;
};

#define trendCache (TrendCache::getInstance())
