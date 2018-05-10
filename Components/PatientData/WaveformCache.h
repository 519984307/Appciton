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

    // 获取值的范围。
    void getRange(WaveformID id, int &minValue, int &maxValue);

    // 获取基线值。
    void getBaseline(WaveformID id, int &baseline);

    // 获取标识值。
    void getTitle(WaveformID id, QString &waveTitle);

public: // 通道相关。

    // start the realtime channel
    void startRealtimeChannel();

    // stop the realtime channel
    void stopRealtimeChannel();

    //read data from the storage channel
    int readStorageChannel(WaveformID id, WaveDataType *buff, int time, bool alignToSecond = true, bool startRealtimeChannel = false);

    //read the oldest data from the realtime channel, the channel will remove the read data
    int readRealtimeChannel(WaveformID id, WaveDataType *buff, int time);

    //read the oldest data from the realtime channel, the channel will remove the read data
    int readRealtimeChannel(WaveformID id, int num, WaveDataType *buff);

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
        }

        ~ChannelDesc()
        {
        }

        QString name;
        RingBuff<WaveDataType> buff;
        QMutex _mutex;
    };

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
