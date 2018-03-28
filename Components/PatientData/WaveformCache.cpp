#include "WaveformCache.h"
#include "RescueDataDefine.h"
#include "Debug.h"

#define CONTINUOUS_PRINT_WAVE_CHANNEL_TIME 6

WaveformCache *WaveformCache::_selfObj = NULL;

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
        int maxValue, QString &waveTitle, int baseline)
{
    SourceMap::iterator it = _source.find(id);

    // 已经存在。
    if (it != _source.end())
    {
        return;
    }

    _source.insert(id, WaveformAttr(rate, minValue, maxValue, waveTitle, baseline));

    //add source to channels
    //TODO: no need to specific channel name, remove the name member in ChnanelDesc later
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
    //add data to sync cache
    _syncCacheMutex.lock();
    WaveSyncCacheMap::iterator listIter = _syncCache.find(id);
    if(listIter != _syncCache.end())
    {
        QList<WaveSyncCache>::iterator  iter;
        for(iter = listIter->begin(); iter != listIter->end(); iter ++)
        {
            if(iter->curCacheLen < iter->bufflen)
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

    //recorder record wavedata
    _recorderMutex.lock();
    WaveformRecorderMap::Iterator recorderListIter = _waveRecorders.find(id);
    if(recorderListIter != _waveRecorders.end())
    {
        QList<WaveformRecorder>::Iterator iter;
        for(iter = recorderListIter->begin(); iter != recorderListIter->end(); iter ++)
        {
            if(iter->curRecWaveNum < iter->totalRecWaveNum)
            {
                iter->buf[iter->curRecWaveNum++] = data;
                if(iter->recordCompleteCallback)
                {
                    iter->recordCompleteCallback(id, iter->recObj);
                }
            }
        }
    }
    _recorderMutex.unlock();

    int count = 0;
    ChannelDesc  *chn = _storageChannel[id];
    if(chn)
    {
        if (!chn->_mutex.tryLock())
        {
            qdebug("get lock fail\n");
            chn->buffBk.append(data);
        }
        count = chn->buffBk.count();
        for (int j = 0; j < count; ++j)
        {
            chn->buff.push(chn->buffBk.at(j));
        }

        chn->buff.push(data);
        chn->buffBk.clear();
        chn->_mutex.unlock();
    }

    if(_enableRealtimeChannel)
    {
        chn = _realtimeChannel[id];
        if(chn)
        {
            if (!chn->_mutex.tryLock())
            {
                qdebug("get lock fail\n");
                chn->buffBk.append(data);
            }
            count = chn->buffBk.count();
            for (int j = 0; j < count; ++j)
            {
                chn->buff.push(chn->buffBk.at(j));
            }

            chn->buff.push(data);
            chn->buffBk.clear();
            chn->_mutex.unlock();
        }
    }

#if 0 //TODO: remove
    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        return;
    }


    int size = channel.size();
    int count = 0;
    for (int i = 0; i < size; i++)
    {
        if (!channel[i]->_mutex.tryLock())
        {
            qdebug("get lock fail\n");
            channel[i]->buffBk.append(data);
            continue;
        }
        count = channel[i]->buffBk.count();
        for (int j = 0; j < count; ++j)
        {
            channel[i]->buff.push(channel[i]->buffBk.at(j));
        }

        channel[i]->buff.push(data);
        channel[i]->buffBk.clear();
        channel[i]->_mutex.unlock();
    }
#endif
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
void WaveformCache::getRange(WaveformID id, int &minValue, int &maxValue)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return;
    }

    minValue = it.value().minValue;
    maxValue = it.value().maxValue;
}

/**************************************************************************************************
 * 获取波形(通道)基线值
 * 参数:
 *      id: 波形参数ID。
 *      baseline: 带回基线值。
 *************************************************************************************************/
void WaveformCache::getBaseline(WaveformID id, int &baseline)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return;
    }

    baseline = it.value().baseline;
}

/**************************************************************************************************
 * 获取波形(通道)标识
 * 参数:
 *      id: 波形参数ID。
 *      waveTitle: 带回标识值。
 *************************************************************************************************/
void WaveformCache::getTitle(WaveformID id, QString &waveTitle)
{
    SourceMap::iterator it = _source.find(id);
    if (it == _source.end())
    {
        return;
    }

    waveTitle = it.value().waveTitle;
}

#if 0 //TODO: remove
/**************************************************************************************************
 * 查找通道是否已经存在。
 * 参数：
 *      id: 数据源。
 *      name: 通道名。
 *************************************************************************************************/
inline bool WaveformCache::_channelExisted(WaveformID id, const QString &name)
{
    // 查找是否已经存在同名的通道。
    QList<ChannelDesc*> descs = _channel.values(id);
    int n = descs.size();
    for (int i = 0; i < n; i++)
    {
        if (name == descs[i]->name)
        {
            return true;
        }
    }

    return false;
}

/**************************************************************************************************
 * 申请一个通道。
 * 参数：
 *      name: 通道名称。
 *      outRate: 需要的速率。
 *      timeLen: 时间长度。
 *************************************************************************************************/
void WaveformCache::channelRequest(const QString &name, int timeLen)
{
    SourceMap::iterator it = _source.begin();
    for (; it != _source.end(); ++it)
    {
        if (_channelExisted(it.key(), name))
        {
            continue;
        }

        _channel.insert(it.key(), new ChannelDesc(name, getSampleRate(it.key()) * timeLen));
    }
}

/**************************************************************************************************
 * 获取波形（通道)数据的个数。
 * 参数:
 *      id: 波形参数ID。
 *      channelName: 波形缓存通道名称。
 * 返回:
 *      相应波形(通道)缓存数据大小。
 *************************************************************************************************/
int WaveformCache::channelSize(WaveformID id, const QString &channelName)
{
    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        return 0;
    }

    int size = channel.size();
    int len = 0;
    for (int i = 0; i < size; i++)
    {
        if (channel[i]->name == channelName)
        {
            channel[i]->_mutex.lock();
            len =  channel[i]->buff.dataSize();
            channel[i]->_mutex.unlock();
        }
    }

    return len;
}

/**************************************************************************************************
 * 清空波形通道数据。
 * 参数:
 *      id: 波形参数ID。
 *      channelName: 波形缓存通道名称。
 *************************************************************************************************/
void WaveformCache::channelClear(WaveformID id, const QString &channelName)
{
    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        return;
    }

    int size = channel.size();
    for (int i = 0; i < size; i++)
    {
        if (channel[i]->name == channelName)
        {
            channel[i]->_mutex.lock();
            channel[i]->buff.clear();
            channel[i]->_mutex.unlock();
            break;
        }
    }
}

/**************************************************************************************************
 * 清空所有波形通道的数据。
 * 参数:
 *      id: 波形参数ID。
 *************************************************************************************************/
void WaveformCache::channelClear(WaveformID id)
{
    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        return;
    }

    int size = channel.size();
    for (int i = 0; i < size; i++)
    {
        channel[i]->_mutex.lock();
        channel[i]->buff.clear();
        channel[i]->_mutex.unlock();
    }
}

/**************************************************************************************************
 * 功能：丢弃通道中的部分数据，保存最新的len个数据。
 * 参数：
 *      id: 波形参数ID。
 *      channelName: 通道名称。
 *      len: 欲保留的个数。
 *************************************************************************************************/
void WaveformCache::channelDicard(WaveformID id, const QString &channelName, int len)
{
    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        return;
    }

    int size = channel.size();
    for (int i = 0; i < size; i++)
    {
        if (channel[i]->name == channelName)
        {
            int size = channelSize(id, channelName) - len;
            if (size > 0)
            {
                channel[i]->_mutex.lock();
                channel[i]->buff.pop(size);
                channel[i]->_mutex.unlock();
            }
            break;
        }
    }
}

/**************************************************************************************************
 * 功能：丢弃通道中的部分旧数据。
 * 参数:
 *      id: 波形参数ID。
 *      channelName: 通道名称。
 *      led: 欲丢弃旧数据的长度。
 *************************************************************************************************/
void WaveformCache::channelDicardOld(WaveformID id, const QString &channelName, int len)
{
    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        return;
    }

    if (len <= 0)
    {
        return;
    }

    int size = channel.size();
    for (int i = 0; i < size; i++)
    {
        if (channel[i]->name == channelName)
        {
            channel[i]->_mutex.lock();
            channel[i]->buff.pop(len);
            channel[i]->_mutex.unlock();
            break;
        }
    }
}


///**************************************************************************************************
// * 读取波形通道的数据，如果当前数据个数比申请个数少，则将数据放到缓冲区后部分带回。
// * 参数:
// *      id: 波形参数ID。
// *      channelName: 通道名称。
// *      buff: 存放读取数据的缓冲区。
// *      len: 欲读取数据的个数。
// *      del: 是否保留原始数据，默认不保留
// * 返回：
// *      读到的数据个数。
// *************************************************************************************************/
//int WaveformCache::channelRead(WaveformID id, const QString &channelName,
//        WaveDataType *buff, int len, bool del)
//{
//    if ((buff == NULL) || (len <= 0))
//    {
//        return 0;
//    }
//
//    QList<ChannelDesc*> channel = _channel.values(id);
//    if (channel.isEmpty())
//    {
//        return 0;
//    }
//
//    int size = 0;
//    int channelNR = channel.size();
//    for (int i = 0; i < channelNR; i++)
//    {
//        if (channel[i]->name != channelName)
//        {
//            continue;
//        }
//
//        channel[i]->_mutex.lock();
//        RingBuff<WaveDataType> &pool = channel[i]->buff;
//        size = pool.dataSize();
//
//        if (len <= size)
//        {
//            pool.copy(0, buff, len);
//            size = len;
//        }
//        else
//        {
//            pool.copy(0, &buff[len - size], size);
//        }
//
//        // 清除。
//        if (del)
//        {
//            pool.pop(size);
//        }
//        channel[i]->_mutex.unlock();
//
//        break;
//    }
//
//    return size;
//}

/**************************************************************************************************
 * 从当前时间点向前去读time秒的数据。
 * 参数:
 *      id: 波形参数ID。
 *      channelName: 通道名称。
 *      buff: 存放读取数据的缓冲区。
 *      time: 欲读取数据的秒数。
 *      alignToSecond: data align to second edge
 * 返回：
 *      读到的数据个数。
 *************************************************************************************************/
int WaveformCache::channelRead(WaveformID id, WaveDataType *buff, int time,
        const QString &channelName, bool alignToSecond)
{
    if ((buff == NULL) || (time <= 0))
    {
        return 0;
    }

    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        qdebug("channel is empty.\n");
        return 0;
    }

    int size = 0;
    int rate = getSampleRate(id);
    int len = rate * time;
    int channelNR = channel.size();
    for (int i = 0; i < channelNR; i++)
    {
        if (channel[i]->name != channelName)
        {
            continue;
        }

        channel[i]->_mutex.lock();
        RingBuff<WaveDataType> &pool = channel[i]->buff;
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
            //no enough data, fill the head of the the buff with invalid data
            int baseLine = 0;
            getBaseline(id, baseLine);
            qFill(buff, buff + len - size, 0x40000000 | baseLine);
            pool.copy(0, &buff[len - size], size);
        }
        channel[i]->_mutex.unlock();
        break;
    }

    return size;
}


/**************************************************************************************************
 * 读取实时波形数据。
 * 参数:
 *      id: 波形参数ID。
 *      channelName: 通道名称。
 *      buff: 存放读取数据的缓冲区。
 *      time: 欲读取数据的秒数。
 *      isInit: 是否初始化读位置
 *      readFromHead:从波形头开始读取
 * 返回：
 *      读到的数据个数。
 *************************************************************************************************/
int WaveformCache::channelReadRealTimeData(WaveformID id, WaveDataType *buff, int time,
            const QString &channelName, bool isInit, bool readFromHead)
{
    if ((buff == NULL) || (time <= 0))
    {
        return 0;
    }

    QList<ChannelDesc*> channel = _channel.values(id);
    if (channel.isEmpty())
    {
        qdebug("channel is empty.\n");
        return 0;
    }

    int size = 0;
    int rate = getSampleRate(id);
    int len = rate * time;
    int channelNR = channel.size();
    for (int i = 0; i < channelNR; i++)
    {
        if (channel[i]->name != channelName)
        {
            continue;
        }

        channel[i]->_mutex.lock();
        RingBuff<WaveDataType> &pool = channel[i]->buff;

        if (isInit)
        {
            pool.resetToTail();
        }
        size = pool.dataSize();

        if (len <= size)
        {
            int index = size - len;
            if (readFromHead)
            {
                index = 0;
            }

            pool.copy(index, buff, len);
            size = len;
        }
        else
        {
            //no enough data, fill the head of the the buff with invalid data
            int baseLine = 0;
            getBaseline(id, baseLine);
            qFill(buff, buff + len - size, 0x40000000 | baseLine);
            pool.copy(0, &buff[len - size], size);
        }

        if (isInit)
        {
            pool.pop(pool.dataSize());
        }
        else
        {
            pool.pop(size);
        }
        channel[i]->_mutex.unlock();
        break;
    }

    return size;
}
#endif

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
int WaveformCache::readStorageChannel(WaveformID id, WaveDataType *buff, int time, bool alignToSecond, bool startRealtimeChannel)
{
    if ((buff == NULL) || (time <= 0))
    {
        return 0;
    }

    ChannelDesc *channel = _storageChannel[id];
    if(channel == NULL)
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
        //no enough data, fill the head of the the buff with invalid data
        int baseLine = 0;
        getBaseline(id, baseLine);
        qFill(buff, buff + len - size, 0x40000000 | baseLine);
        pool.copy(0, &buff[len - size], size);
    }

    if(startRealtimeChannel)
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
    if(channel == NULL)
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

    if(len <= size)
    {
        size = len;
    }
    pool.copy(0, &buff[0], size);

    //remove the read data
    pool.pop(size);
    channel->_mutex.unlock();

    return size;
}

/***************************************************************************************************
 * disable the realtime print channel
 **************************************************************************************************/
void WaveformCache::stopRealtimeChannel()
{
    if(!_enableRealtimeChannel)
    {
        return;
    }

    _enableRealtimeChannel = false;

    //clear the channel
    ChannelDesc *chn = NULL;
    for (int i = 0; i < WAVE_NR; i++)
    {
        chn = _realtimeChannel[i];
        if(chn)
        {
            chn->_mutex.lock();
            chn->buff.clear();
            chn->_mutex.unlock();
        }
    }
}

/***************************************************************************************************
 * registerSyncCache : register a sync cache
 *          @id     : wave id
 *          @cacheID : the unique id defined by user
 *          @buff : user buffer
 *          @buflen : buffer length
 **************************************************************************************************/
bool WaveformCache::registerSyncCache(WaveformID id, long cacheID, WaveDataType *buff, int buflen, SyncCacheCallback cb)
{
    if(buff == NULL || buflen <= 0)
    {
        return false;
    }

    QMutexLocker locker(&_syncCacheMutex);

    WaveSyncCacheMap::iterator listIter = _syncCache.find(id);
    if(listIter == _syncCache.end())
    {
        listIter = _syncCache.insert(id, QList<WaveSyncCache>());
    }

    QList<WaveSyncCache>::const_iterator iter;
    for(iter = listIter->constBegin(); iter != listIter->constEnd(); iter++)
    {
        if( iter->id == cacheID)
            return false;
    }

    listIter->append(WaveSyncCache(cacheID, buff, buflen, cb));
    return true;
}

/***************************************************************************************************
 * unRegisterSyncCache : unregister the sync cache
 **************************************************************************************************/
void WaveformCache::unRegisterSyncCache(WaveformID id, long cacheID)
{
    QMutexLocker locker(&_syncCacheMutex);
    WaveSyncCacheMap::iterator listIter = _syncCache.find(id);
    if(listIter == _syncCache.end())
    {
        return;
    }

    QList<WaveSyncCache>::iterator iter;
    for(iter = listIter->begin(); iter != listIter->end(); iter++)
    {
        if(iter->id == cacheID)
        {
            listIter->erase(iter);
            return;
        }
    }
}

/***************************************************************************************************
 * isSyncCacheComplete : check whether the sync cache is complete
 **************************************************************************************************/
bool WaveformCache::isSyncCacheCompleted(WaveformID id, long cacheID)
{
    QMutexLocker locker(&_syncCacheMutex);
    WaveSyncCacheMap::iterator listIter = _syncCache.find(id);
    if(listIter == _syncCache.end())
    {
        return false;
    }

    QList<WaveSyncCache>::const_iterator iter;
    for(iter = listIter->constBegin(); iter != listIter->constEnd(); iter++)
    {
        if( iter->id == cacheID)
        {
            return iter->curCacheLen == iter->bufflen;
        }
    }
    return false;
}

bool WaveformCache::registerWaveformRecorder(WaveformID id, const WaveformRecorder &recorder)
{
    QMutexLocker locker(&_recorderMutex);

    WaveformRecorderMap::Iterator listIter = _waveRecorders.find(id);
    if(listIter == _waveRecorders.end())
    {
        listIter = _waveRecorders.insert(id, QList<WaveformRecorder>());
    }

    QList<WaveformRecorder>::ConstIterator iter;
    for(iter = listIter->constBegin(); iter != listIter->constEnd(); iter++)
    {
        // if the record object for the waveform already exist, return false
        if (iter->recObj == recorder.recObj)
            return false;
    }
    listIter->append(recorder);

    return true;
}

void WaveformCache::unRegisterWaveformRecorder(WaveformID id, void *recObj)
{
    QMutexLocker locker(&_recorderMutex);

    WaveformRecorderMap::iterator listIter = _waveRecorders.find(id);
    if(listIter == _waveRecorders.end())
    {
        //no recorder regiser in the waveform
        return;
    }

    QList<WaveformRecorder>::iterator iter;
    for(iter = listIter->begin(); iter != listIter->end(); iter++)
    {
        if(iter->recObj == recObj)
        {
            //fill the left space with invalid wave data
            int baseline = 0;
            getBaseline(id, baseline);
            qFill(iter->buf + iter->curRecWaveNum, iter->buf + iter->totalRecWaveNum, 0x40000000 | baseline);

            //remove
            listIter->erase(iter);
            return;
        }
    }
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
WaveformCache::~WaveformCache()
{
    for(int i = 0; i < WAVE_NR; i++)
    {
        ChannelDesc *chn = _storageChannel[i];
        if(chn)
        {
            delete chn;
        }
        chn = _realtimeChannel[i];
        if(chn)
        {
            delete chn;
        }
    }

#if 0  //TODO:remove
    SourceMap::iterator it = _source.begin();
    for (; it != _source.end(); ++it)
    {
        QList<ChannelDesc*> channel = _channel.values(it.key());
        for (int i = 0; i < channel.size(); i++)
        {
            delete channel[i];
        }
    }
#endif
}
