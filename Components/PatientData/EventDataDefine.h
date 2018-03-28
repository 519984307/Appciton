#pragma once
#include "BaseDefine.h"
#include "TrendDataDefine.h"
#include <QVector>

enum EventType{
    EventPhysiologicalAlarm,
    EventCodeMarker,
    EventRealtimePrint,
    EventNIBPMeasurement,
    EventWaveFreeze,
    EventTypeMax
};

enum EventSegmentType {
    EVENT_INFO_SEGMENT = 0x11111111,
    EVENT_TRENDDATA_SEGMENT = 0x22222222,
    EVENT_WAVEFORM_SEGMENT = 0x33333333,
    EVENT_ALARM_INFO_SEGMENT = 0x44444444,
    EVENT_CODEMARKER_SEGMENT = 0x55555555,
};

/* segment record the alarm info */
struct AlarmInfoSegment {
    int subParamID;             /* sub param ID */
    unsigned char isOneShot;    /* alarm type: 1:oneshot, 0: limit alarm*/
    unsigned char alarmType;    /* alarm id of the param */
    short  alarmLimit;          /* alarm limit if it's a limit alarm */
};

/* segment record the code marker info */
struct CodeMarkerSegment {
    char codeName[32];          /* code name */
};

/* waveform data segment, store the wave data in one second */
struct WaveformDataSegment {
    WaveformID waveID;      /* waveform id */
    int sampleRate;         /* sample rate of the waveform */
    int waveNum;            /* the number of wave points */
    WaveDataType waveData[0];   /* start location of the wave points */
};

/* Event storage info */
struct EventInfoSegment {
    EventType type;         /* the event type*/
    unsigned timestamp;     /* timestamp when this event happen */
    unsigned short duration_before;  /* duration before this event happen */
    unsigned short duration_after;   /* duration after this event happen */
};

struct Event {
    EventInfoSegment storageInfo;
    QVector<TrendDataSegment*> trendSegments; //trend segments of every seconds
    QVector<WaveformDataSegment *> waveSegments; //wave segments of every seconds
};
