/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/3/27
 **/


#include "TrendCache.h"
#include "ParamManager.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "NIBPParam.h"
#include "RESPParam.h"
#include "SPO2Param.h"
#include "TEMPParam.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "Alarm.h"
#include <QMutexLocker>


/**************************************************************************************************
 * 函数说明：收集趋势数据。
 * 函数说明：收集趋势数据。
 * 函数参数：
 *      @t: timestamp of the data
 *      @overwrite: overwrite the exist data if data for @t is collocted
 *************************************************************************************************/
void TrendCache::collectTrendData(unsigned t, bool overwrite)
{
    QMutexLocker locker(&_mutex);
    if (_trendCacheMap.contains(t) && !overwrite)
    {
        return;
    }

    if (_stopCollectTimes > 0)
    {
        _stopCollectTimes--;
        return;
    }
    TrendCacheData data;
    QList<ParamID> paramIDList = paramManager.getParamIDs();
    ParamID paramID;

    data.co2baro = co2Param.getBaro();
    NIBPMeasureResult nibpResult = nibpParam.getMeasureResult();
    switch (nibpResult)
    {
    case NIBP_MEASURE_SUCCESS:
        _nibpMeasureSuccessTime = t;    // fall through
    case NIBP_MEASURE_FAIL:
        _nibpMeasureTime = t;
        nibpParam.setMeasureResult(NIBP_MEASURE_RESULT_NONE);
    case NIBP_MEASURE_RESULT_NONE:
    default:
        break;
    }

    data.lastNibpMeasureTime = _nibpMeasureTime;
    data.lastNibpMeasureSuccessTime = _nibpMeasureSuccessTime;
    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        paramID = paramInfo.getParamID((SubParamID)i);

        if (static_cast<SubParamID>(i) == SUB_PARAM_NIBP_PR)  // 收集nibp PR趋势数据
        {
            paramID = PARAM_NIBP;
        }
        if (-1 == paramIDList.indexOf(paramID))
        {
            continue;
        }

        if (!paramManager.isSubParamAvaliable(paramID, (SubParamID)i))
        {
            continue;
        }

        data.values[(SubParamID)i] = paramManager.getSubParamValue(paramID, (SubParamID)i);
    }

    if (MAX_TREND_CACHE_NUM == _trendCacheMap.count())
    {
        _trendCacheMap.remove(_trendCacheMap.begin().key());
    }

    _trendCacheMap.insert(t, data);
}

/**************************************************************************************************
 * 函数说明：collect subparam alarm status。
 * 函数参数：
 *      @t: timestamp of the subparam alarm status
 *************************************************************************************************/
void TrendCache::collectTrendAlarmStatus(unsigned t)
{
    if (_trendAlarmStatusCacheMap.contains(t))
    {
        return;
    }

    TrendAlarmStatus alarmStatus;
    QList<ParamID> paramIDList = paramManager.getParamIDs();
    ParamID paramID;

    for (int i = 0; i < SUB_PARAM_NR; ++i)
    {
        paramID = paramInfo.getParamID((SubParamID)i);
        if (-1 == paramIDList.indexOf(paramID))
        {
            continue;
        }

        if (!paramManager.isSubParamAvaliable(paramID, (SubParamID)i))
        {
            continue;
        }

        alarmStatus.alarms[(SubParamID)i] = alertor.getAlarmSourceStatus(paramInfo.getParamName(paramID),
                                            (SubParamID)i);
    }

    if (MAX_TREND_CACHE_NUM == _trendAlarmStatusCacheMap.count())
    {
        _trendAlarmStatusCacheMap.remove(_trendAlarmStatusCacheMap.begin().key());
    }

    _trendAlarmStatusCacheMap.insert(t, alarmStatus);

    // handle the trend recorders
    QList<TrendRecorder>::Iterator iter = _recorders.begin();
    for (; iter != _recorders.end(); iter++)
    {
        iter->completeCallback(t, _trendCacheMap.find(t).value(), alarmStatus, iter->obj);
        if (iter->toTimestamp <= static_cast<int>(t))
        {
            iter = _recorders.erase(iter);
            if (iter == _recorders.end())
            {
                break;
            }
        }
    }
}

QList<TrendCacheData> TrendCache::getTrendData(unsigned start, unsigned stop)
{
    QList<TrendCacheData> trendDataList;
    for (unsigned t = start; t <= stop; t ++)
    {
        TrendCacheData trendData;
        if (getTrendData(t, &trendData))
        {
            trendDataList.append(trendData);
        }
    }
    return trendDataList;
}

QList<TrendAlarmStatus> TrendCache::getTrendAlarmStatus(unsigned start, unsigned stop)
{
    QList<TrendAlarmStatus> alarmStatusList;
    for (unsigned t = start; t <= stop; t ++)
    {
        TrendAlarmStatus trendAlarmStatus;
        getTrendAlarmStatus(t, &trendAlarmStatus);
        alarmStatusList.append(trendAlarmStatus);
    }
    return alarmStatusList;
}

/**************************************************************************************************
 * 函数说明：获取指定时间的趋势数据。
 * 函数参数：
 *         data:保存趋势数据
 * 返回:
 * true，读取成功；false，失败
 *************************************************************************************************/
bool TrendCache::getTrendData(unsigned t, TrendCacheData *data)
{
    if (_trendCacheMap.isEmpty())
    {
        return false;
    }

    TrendCacheMap::iterator trendIter = _trendCacheMap.find(t);
    if (trendIter == _trendCacheMap.end())
    {
        --trendIter;
        if (t - trendIter.key() <= 2)
        {
            *data = trendIter.value();
        }
        return false;
    }

    *data = trendIter.value();
    return true;
}

/**************************************************************************************************
 * 函数说明:get timestamp of the trend data alarm status。
 * 函数参数：
 *         data:保存趋势数据
 * 返回:
 * true，读取成功；false，失败
 *************************************************************************************************/
bool TrendCache::getTrendAlarmStatus(unsigned t, TrendAlarmStatus *data)
{
    if (_trendAlarmStatusCacheMap.isEmpty())
    {
        return false;
    }

    TrendAlarmStatusCacheMap::iterator trendIter = _trendAlarmStatusCacheMap.find(t);
    if (trendIter == _trendAlarmStatusCacheMap.end())
    {
        --trendIter;
        if (t - trendIter.key() <= 2)
        {
            *data = trendIter.value();
        }
        return false;
    }

    *data = trendIter.value();
    return true;
}

/* register a recorder to record the trend data, it will deleted when timestamp expired */
void TrendCache::registerTrendRecorder(const TrendRecorder &recorder)
{
    _recorders.append(recorder);
}

/* remove the recorder */
bool TrendCache::unregisterTrendRecorder(void *recordObj)
{
    QList<TrendRecorder>::Iterator iter = _recorders.begin();
    for (; iter != _recorders.end(); iter++)
    {
        if (iter->obj == recordObj)
        {
            _recorders.erase(iter);
            return true;
        }
    }

    return false;
}

void TrendCache::clearTrendCache()
{
    QMutexLocker locker(&_mutex);
    _trendCacheMap.clear();
}

void TrendCache::stopDataCollect(quint32 times)
{
    _stopCollectTimes = times;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
TrendCache::TrendCache()
{
    _trendCacheMap.clear();

    _nibpMeasureSuccessTime = 0;
    systemConfig.getNumValue("PrimaryCfg|NIBP|MeasureTime", _nibpMeasureSuccessTime);
    _nibpMeasureTime = _nibpMeasureSuccessTime;
    _stopCollectTimes = 0;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/

TrendCache::~TrendCache()
{
    _trendCacheMap.clear();
}

TrendCache &TrendCache::getInstance()
{
    static TrendCache *instance = NULL;
    if (instance == NULL)
    {
        instance = new TrendCache();
        TrendCacheInterface *old = registerTrendCache(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}
