/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/11
 **/

#pragma once
#include "BaseDefine.h"
#include "ParamDefine.h"

typedef void (*SyncCacheCallback)(WaveformID waveid, long cacheid, int cachelen);

struct WaveformRecorder
{
    int curRecWaveNum;
    int totalRecWaveNum;
    int sampleRate;
    WaveDataType *buf;
    void *recObj;
    void (*recordDurationIncreaseCallback)(WaveformID id, void *obj);
    void (*recordCompleteCallback)(WaveformID id, void *obj);
};

class WaveformCacheInterface
{
public:
    virtual ~WaveformCacheInterface() {}

    // 注册一个数据源。
    virtual void registerSource(WaveformID id, int rate, int minValue, int maxValue, QString &waveTitle,
                                int baseline = 0) = 0;

    // 添加一个波形数据。
    virtual void addData(WaveformID id, WaveDataType data) = 0;

    // 获取波形的数据采样率。
    virtual int getSampleRate(WaveformID id) = 0;

    // 获取值的范围。
    virtual void getRange(WaveformID id, int &minValue, int &maxValue) = 0;

    // 获取基线值。
    virtual void getBaseline(WaveformID id, int &baseline) = 0;

    // 获取标识值。
    virtual void getTitle(WaveformID id, QString &waveTitle) = 0;


public: // 通道相关。
    // get the time duration of the channels;
    virtual unsigned channelDuration() const = 0;

    // start the realtime channel
    virtual void startRealtimeChannel() = 0;

    // stop the realtime channel
    virtual void stopRealtimeChannel() = 0;

    // read data from the storage channel
    virtual int readStorageChannel(WaveformID id, WaveDataType *buff, int time, bool alignToSecond = true,
                                   bool startRealtimeChannel = false) = 0;

    // read the oldest data from the realtime channel, the channel will remove the read data
    virtual int readRealtimeChannel(WaveformID id, WaveDataType *buff, int time) = 0;

    // read the oldest data from the realtime channel, the channel will remove the read data
    virtual int readRealtimeChannel(WaveformID id, int num, WaveDataType *buff) = 0;

    // TODO: deprecated, remove later
    // register a user buffer as sync cache
    virtual bool registerSyncCache(WaveformID id, long cacheID, WaveDataType *buff, int buflen,
                                   SyncCacheCallback cb = NULL) = 0;
    // TODO: deprecated, remove later
    // unregister sync cache
    virtual void unRegisterSyncCache(WaveformID id, long cacheID) = 0;
    // TODO: deprecated, remove later
    // check whether the sync cache complete
    virtual bool isSyncCacheCompleted(WaveformID id, long cacheID) = 0;

    // register a waveform recorder, for each recorder object, it can not record the same waveform twice
    // recorder will be deleted automatically when complete
    virtual bool registerWaveformRecorder(WaveformID id, const WaveformRecorder &recorder) = 0;

    // unregister a waveform recorder
    virtual void unRegisterWaveformRecorder(WaveformID id, void *recObj) = 0;

public:
    /**
     * @brief registerWaveformCache register a WaveformCache object
     * @param interface the new WaveformCache interface
     * @return the old WaveformCache interafce
     */
    static WaveformCacheInterface *registerWaveformCache(WaveformCacheInterface *interface);

    /**
     * @brief getWaveformCache get the WaveformCache interface
     * @return the interface or NULL if no any interface
     */
    static WaveformCacheInterface *getWaveformCache();
};
