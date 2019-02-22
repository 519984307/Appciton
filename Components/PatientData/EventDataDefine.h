/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/1/16
 **/

#pragma once
#include "BaseDefine.h"
#include "TrendDataDefine.h"
#include <QVector>
#include "NIBPDefine.h"

enum EventType
{
    EventAll,
    EventPhysiologicalAlarm,
    EventCodeMarker,
    EventRealtimePrint,
    EventNIBPMeasurement,
    EventWaveFreeze,
    EventOxyCRG,
    EventTypeMax
};

/* ECG gain */
enum ECGEventGain
{
    ECG_EVENT_GAIN_X0125,       // x0.125
    ECG_EVENT_GAIN_X025,        // x0.25
    ECG_EVENT_GAIN_X05,         // x0.5
    ECG_EVENT_GAIN_X10,         // x1
    ECG_EVENT_GAIN_X20,         // x2
    ECG_EVENT_GAIN_X40,         // x4
    ECG_EVENT_GAIN_NR
};

enum EventSegmentType
{
    EVENT_INFO_SEGMENT = 0x11111111,
    EVENT_TRENDDATA_SEGMENT = 0x22222222,
    EVENT_WAVEFORM_SEGMENT = 0x33333333,
    EVENT_ALARM_INFO_SEGMENT = 0x44444444,
    EVENT_CODEMARKER_SEGMENT = 0x55555555,
    EVENT_OXYCRG_SEGMENT = 0x66666666,
    EVENT_NIBPMEASURE_SEGMENT = 0x77777777,
};

enum EventLevel
{
    EVENT_LEVEL_ALL,
    EVENT_LEVEL_HINT,
    EVENT_LEVEL_LOW,
    EVENT_LEVEL_MED,
    EVENT_LEVEL_High,
    EVENT_LEVEL_NR
};

/* segment record the alarm info */
struct AlarmInfoSegment
{
    int subParamID;             /* sub param ID */
    unsigned char alarmInfo;    /* alarm info: 1bit  1:oneshot, 0: limit alarm
                                               2bit  1:upper,   0: lowers*/
    unsigned char alarmType;    /* alarm id of the param */
    short  alarmLimit;          /* alarm limit if it's a limit alarm */
};

/* segment record the code marker info */
struct CodeMarkerSegment
{
    char codeName[32];          /* code name */
};

enum OxyCRGEventType
{
    OxyCRGEventAll,
    OxyCRGEventManual,
    OxyCRGEventApnea,
    OxyCRGEventECG,
    OxyCRGEventSpO2,
    OxyCRGEventResp,
    OxyCRGEventTypeMax
};

/* segment record the oxyCRG info */
struct OxyCRGSegment
{
    OxyCRGEventType type;
};

/* segment record the nibp measure */
struct NIBPMeasureSegment
{
    NIBPOneShotType measureResult;
};

/* waveform data segment, store the wave data in one second */
struct WaveformDataSegment
{
    WaveformID waveID;      /* waveform id */
    int sampleRate;         /* sample rate of the waveform */
    int waveNum;            /* the number of wave points */
    char remarks[MAX_WAVE_SEG_INFO_REMARK];        /* record remarks infomation*/
    WaveDataType waveData[0];   /* start location of the wave points */
};

/* Event storage info */
struct EventInfoSegment
{
    EventType type;         /* the event type*/
    unsigned timestamp;     /* timestamp when this event happen */
    unsigned short duration_before;  /* duration before this event happen */
    unsigned short duration_after;   /* duration after this event happen */
};

struct Event
{
    EventInfoSegment storageInfo;
    QVector<TrendDataSegment *> trendSegments; // trend segments of every seconds
    QVector<WaveformDataSegment *> waveSegments; // wave segments of every seconds
};
