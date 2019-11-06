/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/19
 **/

#include "WaveformCache.h"
#include "RescueDataDefine.h"
#include "Debug.h"

#define CONTINUOUS_PRINT_WAVE_CHANNEL_TIME 8

/**************************************************************************************************
 * 申请一个波形缓存,如果缓存对象已经存在且波形数据采样率不同，则修改其容量大小，否则申请新缓存空间。
 * 参数：
 *      id: 波形参数ID。
 *      rate: 波形数据采样率。
 *      timeLen: 缓存波形数据时间长度。
 * 返回:
 *      true - 成功; false - 失败。
 *************************************************************************************************/
void WaveformCache::registerSource(WaveformID id, int rate, int minValue,
                                   int maxValue, const QString &waveTitle, int baseline)
{
    _source.insert(id, WaveformAttr(rate, minValue, maxValue, waveTitle, baseline));

    // add source to channels
    /* TODO: no need to specific channel name, remove the name member in ChnanelDesc later */
    _storageChannel[id] = new ChannelDesc(QString(), rate * DATA_STORAGE_WAVE_TIME);
    _realtimeChannel[id] = new ChannelDesc(QString(), rate * CONTINUOUS_PRINT_WAVE_CHANNEL_TIME);
}

/**************************************************************************************************
 * 添加波形数据。
 * 参数:
 *      id: 波形参数ID。
 *      data: 波形数据。
 *************************************************************************************************/
void WaveformCache::addData(WaveformID id, WaveDataType data)
{
    // add data to sync cache
    _syncCacheMutex.lock();
    WaveSyncCacheMap::iterator listIter = _syncCache.find(id);
    if (listIter != _syncCache.end())
    {
        QList<WaveSyncCache>::iterator  iter;
        for (iter = listIter->begin(); iter != listIter->end(); ++iter)
        {
            if (iter->curCacheLen < iter->bufflen)
            {
                iter->buff[iter->curCacheLen++] = data;
                if (iter->cb)
                {
                    iter->cb(id, iter->id, iter->curCacheLen);
                }
            }
        }
    }
    _syncCacheMutex.unlock();

    // recorder record wavedata
    _recorderMutex.lock();
    WaveformRecorderMap::Iterator recorderListIter = _waveRecorders.find(id);
    if (recorderListIter != _waveRecorders.end())
    {
        QList<WaveformRecorder>::Iterator iter;
        for (iter = recorderListIter->begin(); iter != recorderListIter->end(); iter ++)
        {
            if (iter->curRecWaveNum < iter->totalRecWaveNum)
            {
                iter->buf[iter->curRecWaveNum++] = data;
                if (iter->curRecWaveNum % iter->sampleRate == 0)
                {
                    iter->recordDurationIncreaseCallback(id, iter->recObj);
                }

                if (iter->curRecWaveNum == iter->totalRecWaveNum && iter->recordCompleteCallback)
                {
                    iter->recordCompleteCallback(id, iter->recObj);
                    iter = recorderListIter->erase(iter);
                    if (iter == recorderListIter->end())
                    {
                        break;
                    }
                }
            }
        }
    }
    _recorderMutex.unlock();

    ChannelDesc  *chn = _storageChannel[id];
    if (chn)
    {
        chn->_mutex.lock();
        chn->buff.push(data);
        chn->_mutex.unlock();
    }

    if (_enableRealtimeChannel)
    {
        chn = _realtimeChannel[id];
        if (chn)
        {
            chn->_mutex.lock();
            chn->buff.push(data);
            chn->_mutex.unlock();
        }
    }
}

/**************************************************************************************************
 * 获取波形(通道)数据采样率
 * 参数:
 *      id: 波形参数ID。
 * 返回:
 *      相应波形(通道)参数数据采样率。
 *************************************************************************************************/
int WaveformCache::getSampleRate(WaveformID id)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return 0;
    }
    return it.value().rate;
}

/**************************************************************************************************
 * 获取波形(通道)数据值的范围
 * 参数:
 *      id: 波形参数ID。
 *      minValue: 带回最小值。
 *      maxValue: 带回最大只。
 *************************************************************************************************/
void WaveformCache::getRange(WaveformID id, int *minValue, int *maxValue)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return;
    }

    if (minValue)
    {
        *minValue = it.value().minValue;
    }
    if (maxValue)
    {
        *maxValue = it.value().maxValue;
    }
}

/**************************************************************************************************
 * 获取波形(通道)基线值
 * 参数:
 *      id: 波形参数ID。
 *      baseline: 带回基线值。
 *************************************************************************************************/
int WaveformCache::getBaseline(WaveformID id)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return 0;
    }

    return it.value().baseline;
}

/**************************************************************************************************
 * 获取波形(通道)标识
 * 参数:
 *      id: 波形参数ID。
 *      waveTitle: 带回标识值。
 *************************************************************************************************/
QString WaveformCache::getTitle(WaveformID id)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return QString();
    }

    return it.value().waveTitle;
}

unsigned WaveformCache::channelDuration() const
{
    return DATA_STORAGE_WAVE_TIME;
}

/**
 * @brief WaveformCache::startRealtimeChannel enable the realtime channel
 */
void WaveformCache::startRealtimeChannel()
{
    if (!_enableRealtimeChannel)
    {
        _enableRealtimeChannel = true;
    }
}

/**************************************************************************************************
 * 从当前时间点向前去读time秒的数据。
 * 参数:
 *      id: 波形参数ID。
 *      channelName: 通道名称。
 *      buff: 存放读取数据的缓冲区。
 *      time: 欲读取数据的秒数。
 *      alignToSecond: data align to second edge
 *      startRealtimeChannel: whether start the realtime channel after reading the storage channel,
 *                            used in continuous printing
 * 返回：
 *      读到的数据个数。
 *************************************************************************************************/
int WaveformCache::readStorageChannel(WaveformID id, WaveDataType *buff, int time, bool alignToSecond,
                                      bool startRealtimeChannel)
{
    if ((buff == NULL) || (time <= 0))
    {
        return 0;
    }

    ChannelDesc *channel = _storageChannel[id];
    if (channel == NULL)
    {
        qdebug("channel is empty.\n");
        return 0;
    }

    int size = 0;
    int rate = getSampleRate(id);
    int len = rate * time;

    channel->_mutex.lock();
    RingBuff<WaveDataType> &pool = channel->buff;
    size = pool.dataSize();

    if (len <= size)
    {
        int index = size % rate;
        if (len <= (size - index) && alignToSecond)
        {
            index = size - len - index;
        }
        else
        {
            index = size - len;
        }

        pool.copy(index, buff, len);
        size = len;
    }
    else
    {
        // no enough data, fill the head of the the buff with invalid data
        int baseLine = getBaseline(id);
        qFill(buff, buff + len - size, 0x40000000 | baseLine);
        pool.copy(0, &buff[len - size], size);
    }

    if (startRealtimeChannel)
    {
        _enableRealtimeChannel = true;
    }

    channel->_mutex.unlock();


    return size;
}


/***************************************************************************************************
 * read oldest data from the realtime channel, the channel remove the read data
 **************************************************************************************************/
int WaveformCache::readRealtimeChannel(WaveformID id, WaveDataType *buff, int time)
{
    if ((buff == NULL) || (time <= 0))
    {
        return 0;
    }

    ChannelDesc *channel = _realtimeChannel[id];
    if (channel == NULL)
    {
        qdebug("channel is empty.\n");
        return 0;
    }

    int size = 0;
    int rate = getSampleRate(id);
    int len = rate * time;

    channel->_mutex.lock();
    RingBuff<WaveDataType> &pool = channel->buff;
    size = pool.dataSize();

    if (len <= size)
    {
        size = len;
    }
    pool.copy(0, &buff[0], size);

    // remove the read data
    pool.pop(size);
    channel->_mutex.unlock();

    return size;
}

/***************************************************************************************************
 * read oldest data from the realtime channel, the channel remove the read data
 **************************************************************************************************/
int WaveformCache::readRealtimeChannel(WaveformID id, int num, WaveDataType *buff)
{
    if ((buff == NULL) || (num <= 0))
    {
        return 0;
    }

    ChannelDesc *channel = _realtimeChannel[id];
    if (channel == NULL)
    {
        qdebug("channel is empty.\n");
        return 0;
    }

    int size = 0;

    channel->_mutex.lock();
    RingBuff<WaveDataType> &pool = channel->buff;
    size = pool.dataSize();

    if (num <= size)
    {
        size = num;
    }
    pool.copy(0, &buff[0], size);

    // remove the read data
    pool.pop(size);
    channel->_mutex.unlock();

    return size;
}

/***************************************************************************************************
 * disable the realtime print channel
 **************************************************************************************************/
void WaveformCache::stopRealtimeChannel()
{
    if (!_enableRealtimeChannel)
    {
        return;
    }

    _enableRealtimeChannel = false;

    // clear the channel
    for (int i = 0; i < WAVE_NR; i++)
    {
        ChannelDesc *chn = NULL;
        chn = _realtimeChannel[i];
        if (chn)
        {
            chn->_mutex.lock();
            chn->buff.clear();
            chn->_mutex.unlock();
        }
    }
}

bool WaveformCache::registerWaveformRecorder(WaveformID id, const WaveformRecorder &recorder)
{
    QMutexLocker locker(&_recorderMutex);

    WaveformRecorderMap::Iterator listIter = _waveRecorders.find(id);
    if (listIter == _waveRecorders.end())
    {
        listIter = _waveRecorders.insert(id, QList<WaveformRecorder>());
    }

    QList<WaveformRecorder>::ConstIterator iter;
    for (iter = listIter->constBegin(); iter != listIter->constEnd(); iter++)
    {
        // if the record object for the waveform already exist, return false
        if (iter->recObj == recorder.recObj)
        {
            return false;
        }
    }
    listIter->append(recorder);

    return true;
}

void WaveformCache::unRegisterWaveformRecorder(WaveformID id, void *recObj)
{
    QMutexLocker locker(&_recorderMutex);

    WaveformRecorderMap::iterator listIter = _waveRecorders.find(id);
    if (listIter == _waveRecorders.end())
    {
        // no recorder regiser in the waveform
        return;
    }

    QList<WaveformRecorder>::iterator iter;
    for (iter = listIter->begin(); iter != listIter->end(); ++iter)
    {
        if (iter->recObj == recObj)
        {
            // fill the left space with invalid wave data
            int baseline = getBaseline(id);
            qFill(iter->buf + iter->curRecWaveNum, iter->buf + iter->totalRecWaveNum, 0x40000000 | baseline);

            // remove
            listIter->erase(iter);
            return;
        }
    }
}

void WaveformCache::clear()
{
    memset(_storageChannel, 0, sizeof(_storageChannel));
    memset(_realtimeChannel, 0, sizeof(_realtimeChannel));
    _enableRealtimeChannel = false;
    _source.clear();
    _syncCache.clear();
    _waveRecorders.clear();
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WaveformCache::WaveformCache()
{
    memset(_storageChannel, 0, sizeof(_storageChannel));
    memset(_realtimeChannel, 0, sizeof(_realtimeChannel));
    _enableRealtimeChannel = false;
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WaveformCache &WaveformCache::getInstance()
{
    static WaveformCache *instance = NULL;
    if (instance == NULL)
    {
        instance = new WaveformCache();

        // register the new interface and delete the old one
        WaveformCacheInterface *old = registerWaveformCache(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

WaveformCache::~WaveformCache()
{
    for (int i = 0; i < WAVE_NR; i++)
    {
        ChannelDesc *chn = _storageChannel[i];
        if (chn)
        {
            delete chn;
        }
        chn = _realtimeChannel[i];
        if (chn)
        {
            delete chn;
        }
    }
}
