#pragma once
#include <QMap>
#include <QString>
#include <QMutex>
#include "RingBuff.h"
#include "BaseDefine.h"
#include "ParamDefine.h"

typedef void (*SyncCacheCallback) (WaveformID waveid, long cacheid, int cachelen);

struct WaveformRecorder
{
    int curRecWaveNum;
    int totalRecWaveNum;
    WaveDataType *buf;
    void *recObj;
    void (*recordCompleteCallback)(WaveformID id, void *obj);
};


/**************************************************************************************************
 * 波形数据缓存
 *************************************************************************************************/
class WaveformCache
{
public:
    static WaveformCache &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new WaveformCache();
        }

        return *_selfObj;
    }
    static WaveformCache *_selfObj;
    ~WaveformCache();

public:
    // 注册一个数据源。
    void registerSource(WaveformID id, int rate, int minValue, int maxValue, QString &waveTitle,
            int baseline = 0);

    // 添加一个波形数据。
    void addData(WaveformID id, WaveDataType data);

    // 获取波形的数据采样率。
    int getSampleRate(WaveformID id);

    //acerrro
    //设置波形(通道)数据采样率
    //void getSampleRate(WaveformID id, int rate);
    //end

    // 获取值的范围。
    void getRange(WaveformID id, int &minValue, int &maxValue);

    // 获取基线值。
    void getBaseline(WaveformID id, int &baseline);

    // 获取标识值。
    void getTitle(WaveformID id, QString &waveTitle);

public: // 通道相关。
#if 0 //TODO: remove
    // 在所有波形源上申请一个波形通道数据缓存。
    void channelRequest(const QString &name, int timeLen = MAX_TIME_LEN);

    // 获取波形(通道)数据的个数。
    int channelSize(WaveformID id, const QString &channelName);

    // 清空指定波形通道数据。
    void channelClear(WaveformID id, const QString &channelName);

    // 清空所有波形通道的数据。
    void channelClear(WaveformID id);

    // 丢弃通道中的部分数据，保存最新的len个/second的数据，也即这之前的数据被丢弃。
    void channelDicard(WaveformID id, const QString &channelName, int len);

    // 丢弃通道中的部分旧数据。
    void channelDicardOld(WaveformID id, const QString &channelName, int len);

    // 读取通道的数据，原始数据不保留，如果当前数据个数比申请个数少，则将数据放到缓冲区后部分带回。。
    //int channelRead(WaveformID id, const QString &channelName, WaveDataType *buff,
    //                int len, bool del = true);
    int channelRead(WaveformID id, WaveDataType *buff, int time, const QString &channelName, bool alignToSecond = true);

    int channelReadRealTimeData(WaveformID id, WaveDataType *buff, int time,
        const QString &channelName, bool isInit, bool readFromHead);
#endif

    // enable or disable the realtime channel
    void stopRealtimeChannel();

    //read data from the storage channel
    int readStorageChannel(WaveformID id, WaveDataType *buff, int time, bool alignToSecond = true, bool startRealtimeChannel = false);


    //read the oldest data from the realtime channel, the channel will remove the read data
    int readRealtimeChannel(WaveformID id, WaveDataType *buff, int time);

    //TODO: deprecated, remove later
    //register a user buffer as sync cache
    bool registerSyncCache(WaveformID id, long cacheID, WaveDataType *buff, int buflen, SyncCacheCallback cb=NULL);
    //TODO: deprecated, remove later
    //unregister sync cache
    void unRegisterSyncCache(WaveformID id, long cacheID);
    //TODO: deprecated, remove later
    //check whether the sync cache complete
    bool isSyncCacheCompleted(WaveformID id, long cacheID);

    //register a waveform recorder, for each recorder object, it can not record the same waveform twice
    // recorder will be deleted automatically when complete
    bool registerWaveformRecorder(WaveformID id, const WaveformRecorder &recorder);

    //unregister a waveform recorder
    void unRegisterWaveformRecorder(WaveformID id, void *recObj);


private:
    inline bool _channelExisted(WaveformID id, const QString &name);

private:
    WaveformCache();

    // 波形的属性。
    struct WaveformAttr
    {
        WaveformAttr(int r, int min, int max, QString &tile, int base)
        {
            rate = r;
            minValue = min;
            maxValue = max;
            waveTitle = tile;
            baseline = base;
        }

        int rate;                 // 采样率。
        int minValue;             // 最小值。
        int maxValue;             // 最大值。
        QString waveTitle;        // 波形标识。
        int baseline;             // 基线值。
    };

    // 波形数据采样率、值范围映射。
    typedef QMap<WaveformID, WaveformAttr> SourceMap;
    SourceMap _source;

    // 通道数据。
    struct ChannelDesc
    {
        ChannelDesc(const QString &n, int buffLen)
        {
            name = n;
            buff.resize(buffLen);
            buffBk.clear();
        }

        ~ChannelDesc()
        {
            buffBk.clear();
        }

        QString name;
        RingBuff<WaveDataType> buff;
        QList<WaveDataType> buffBk;
        QMutex _mutex;
    };

#if 0  //TODO: remove
    // 波形数据缓存映射。
    typedef QMultiMap<WaveformID, ChannelDesc*> ChannelMap;
    ChannelMap _channel;
#endif

    ChannelDesc *_storageChannel[WAVE_NR];
    ChannelDesc *_realtimeChannel[WAVE_NR];
    bool _enableRealtimeChannel;

    //Wave Sync Cache, Cache the wave data directly to user's buffer
    struct WaveSyncCache
    {
        WaveSyncCache(long cacheId, WaveDataType *buffer, int bufferLen, SyncCacheCallback cb)
            :id(cacheId), buff(buffer), bufflen(bufferLen), curCacheLen(0), cb(cb)
        {}
        long id;
        WaveDataType *buff;
        int bufflen;
        int curCacheLen;
        SyncCacheCallback cb;
    };
    typedef QMap<WaveformID, QList<WaveSyncCache> > WaveSyncCacheMap;
    WaveSyncCacheMap _syncCache;
    QMutex _syncCacheMutex;

    typedef QMap<WaveformID, QList<WaveformRecorder> > WaveformRecorderMap;
    WaveformRecorderMap _waveRecorders;
    QMutex _recorderMutex;

};
#define waveformCache (WaveformCache::construction())
#define deleteWaveformCache() (delete WaveformCache::_selfObj)
