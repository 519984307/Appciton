#pragma once
#include "EventDataDefine.h"
#include <QScopedPointer>
#include <QByteArray>


class EventStorageItemPrivate;
class EventStorageItem
{
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


    virtual ~EventStorageItem();

    //get the event type
    virtual EventType getType() const;

    //start collect trend and waveform data, return false if already start
    virtual bool startCollectTrendAndWaveformData();

    //check whether collect data complete
    virtual bool checkCompleted();

    //get the storage data of the event storage item
    virtual QByteArray getStorageData() const;

private:
    QScopedPointer<EventStorageItemPrivate> d_ptr;
};
