/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/10/22
 **/

#pragma once
#include "EventDataDefine.h"
#include <QScopedPointer>
#include <QByteArray>
#include <QObject>


class EventStorageItemPrivate;
class EventStorageItem : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief EventStorageItem
     * @param type event type
     * @param storeWaveforms waveforms this event to store
     */
    EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms);

    /**
     * @brief EventStorageItem user for alarm event
     * @param type alarm type
     * @param storeWaveforms waveforms this event to store
     * @param almInfo alarm info
     */
    EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, const AlarmInfoSegment &almInfo);

    /**
     * @brief EventStorageItem user for codemarker event
     * @param type alarm type
     * @param storeWaveforms waveforms this event to store
     * @param codeName name of the code
     */
    EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, const char *codeName);

    /**
     * @brief EventStorageItem user for OxyCRG event
     * @param type event type
     * @param storeWaveforms waveforms this event to store
     * @param oxyCRGtype oxyCRG event type
     * @param almInfo alarm info
     */
    EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, OxyCRGEventType oxyCRGtype, const AlarmInfoSegment &almInfo);

    /**
     * @brief EventStorageItem user for OxyCRG event
     * @param type event type
     * @param storeWaveforms waveforms this event to store
     * @param oxyCRGtype oxyCRG event type
     */
    EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, OxyCRGEventType oxyCRGtype);

    /**
     * @brief EventStorageItem user for NIBPMeasure event
     * @param type event type
     * @param storeWaveforms waveforms this event to store
     * @param measureResult measure result
     */
    EventStorageItem(EventType type, const QList<WaveformID> &storeWaveforms, NIBPOneShotType measureResult);

    virtual ~EventStorageItem();

    // get the event type
    virtual EventType getType() const;

    // get extra data
    virtual quint32 getExtraData() const;

    // start collect trend and waveform data, return false if already start
    virtual bool startCollectTrendAndWaveformData(unsigned t);

    // check whether collect data complete
    virtual bool checkCompleted();

    // get the storage data of the event storage item
    virtual QByteArray getStorageData() const;

    /**
     * @brief getStoreWaveforms get the store wave forms
     * @return list of waveform ids
     */
    QList<WaveformID> getStoreWaveforms() const;

    /**
     * @brief getCurWaveCacheDuration get teh current wave cache duration
     * @param waveId the wave id
     * @return  return the actual duration in unit of second if the wave exists, otherwise, return -1
     */
    int getCurWaveCacheDuration(WaveformID waveId) const;

    /**
     * @brief getOneSecondWaveform get one second waveform
     * @param waveId the waveform id
     * @param waveBuf the wave buffer to store the wave data
     * @param startSecond the start second of the the waveform
     * @return true if has enough data, otherwise, return false
     */
    bool getOneSecondWaveform(WaveformID waveId, WaveDataType *waveBuf, int startSecond);

    /**
     * @brief getTotalWaveCacheDuration get the total wave cache duration of this item
     * @return the wave duration
     */
    int getTotalWaveCacheDuration() const;

    /**
     * @brief getTrendData get the trend data package
     * @return
     */
    TrendDataPackage getTrendData() const;

    /**
     * @brief setForTriggerPrintFlag wait for the trigger print finished before this item complete
     * @param flag
     */
    void setWaitForTriggerPrintFlag(bool flag);

    /**
     * @brief getEventTitle get the event title string
     * @return  title string, might be empty
     */
    QString getEventTitle() const;

    /**
     * @brief getEventFolderName get event storage folder name
     * @return folder name
     */
    QString getEventFolderName() const;
private slots:
    // callback when the trigger page generator is stopped
    void onTriggerPrintStopped();

private:
    QScopedPointer<EventStorageItemPrivate> d_ptr;
};
