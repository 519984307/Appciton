/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/26
 **/


#pragma once
#include "BaseDefine.h"
#include "ParamDefine.h"
#include "Framework/Utility/Unit.h"
#include "RESPDefine.h"
#include "CO2Define.h"
#include <QDate>

/* data structure to store each sub param value */
struct TrendValueSegment
{
    /* sub param id */
    unsigned char subParamId;
    /* this param is whether in alarm status*/
    unsigned char alarmFlag;
    /* sub param value */
    TrendDataType value;
};

/* data structure to store the trend data */
struct TrendDataSegment
{
    unsigned timestamp;                 /* timestamp when the data fetch */
    unsigned status;                    /* status of the fetch data */
    short co2Baro;                      /* current co2 baro */
    short trendValueNum;                /* trend value segment this trend data contians */
    char eventFlag;                     /* event trigger flag */
    char reserved[3];
    TrendValueSegment values[0];        /* followed by the sub param value  */
};

/* data description of the trend storage file */
struct TrendDataDescription
{
    unsigned startTime;                 /* rescue event start time */
    unsigned moduleConfig;              /* module config */
    QDate bornDate;                      /* patient born date */
    char patientGender;                 /* patient gender */
    char patientType;                   /* patient type */
    char patientPacer;                  /* patient wear pacer or not */
    int patientHeight;                /* patient height, mm */
    int patientWeight;                /* patient wieght, g */
    char patientName[MAX_PATIENT_NAME_LEN]; /* patient name */
    char patientID[MAX_PATIENT_ID_LEN];     /* patient id length */
    char deviceID[MAX_DEVICE_ID_LEN];   /* device ID */
    char serialNum[MAX_SERIAL_NUM_LEN]; /* machine serial num */
};

/* trend data package */
struct TrendDataPackage
{
    TrendDataPackage()
    {
        time = 0;
        co2Baro  = 0;
        alarmFlag = false;
        index = 0;
        status = 0;
    }
    unsigned time;
    QMap<SubParamID, TrendDataType> subparamValue;
    QMap<SubParamID, bool> subparamAlarm;
    short co2Baro;
    bool alarmFlag;
    unsigned index;
    unsigned status;
};

/* 分辨率 */
enum ResolutionRatio
{
    RESOLUTION_RATIO_5_SECOND,
    RESOLUTION_RATIO_30_SECOND,
    RESOLUTION_RATIO_1_MINUTE,
    RESOLUTION_RATIO_5_MINUTE,
    RESOLUTION_RATIO_10_MINUTE,
    RESOLUTION_RATIO_15_MINUTE,
    RESOLUTION_RATIO_30_MINUTE,
    RESOLUTION_RATIO_1_HOUR,
    RESOLUTION_RATIO_2_HOUR,
    RESOLUTION_RATIO_3_HOUR,
    RESOLUTION_RATIO_NIBP,
    RESOLUTION_RATIO_NR
};

/* 趋势组 */
enum TrendGroup
{
    TREND_GROUP_INVALID = -1,
    TREND_GROUP_RESP = 0,
    TREND_GROUP_IBP_CO,
    TREND_GROUP_AG
};

/* 趋势图类型 */
enum TrendGraphType
{
    TREND_GRAPH_TYPE_NORMAL,
    TREND_GRAPH_TYPE_NIBP,
    TREND_GRAPH_TYPE_ART_IBP,           // IBP动脉类型
    TREND_GRAPH_TYPE_AG_TEMP,
    TREND_GRAPH_TYPE_CO,
    TREND_GRAPH_TYPE_NR,
};

/* short trend store interval */
enum ShortTrendInterval
{
    SHORT_TREND_INTERVAL_10S,
    SHORT_TREND_INTERVAL_20S,
    SHORT_TREND_INTERVAL_30S,
    SHORT_TREND_INTERVAL_60S,
    SHORT_TREND_INTERVAL_NR
};

/* short trend review duration */
enum ShortTrendDuration
{
    SHORT_TREND_DURATION_30M,
    SHORT_TREND_DURATION_60M,
    SHORT_TREND_DURATION_120M,
    SHORT_TREND_DURATION_240M,
    SHORT_TREND_DURATION_480M,
};

struct TrendGraphData
{
    TrendGraphData() : isAlarm(false), timestamp(0)
    {
    }
    bool isAlarm;
    unsigned timestamp;
    TrendDataType data;
};

struct TrendGraphDataV2     // trend graph draw 2 values
{
    TrendGraphDataV2() : isAlarm(false), timestamp(0)
    {
    }
    bool isAlarm;
    unsigned timestamp;
    TrendDataType data[2];
};

struct TrendGraphDataV3     // trend graph draw 3 values
{
    TrendGraphDataV3() : isAlarm(false), timestamp(0), status(0)
    {
    }
    bool isAlarm;
    unsigned timestamp;
    unsigned status;
    TrendDataType data[3];
};

struct AlarmEventInfo       // alarm event and time
{
    AlarmEventInfo() : isAlarmEvent(false), timestamp(0)
    {
    }
    bool isAlarmEvent;
    unsigned timestamp;
};

struct TrendGraphInfo
{
    SubParamID subParamID;
    unsigned startTime;    // trend graph startTime
    unsigned endTime;      // trend graph endTime
    UnitType unit;
    struct
    {
        int max;
        int min;
        int scale;
    } scale;
    QVector<TrendGraphData> trendData;
    QVector<TrendGraphDataV2> trendDataV2;
    QVector<TrendGraphDataV3> trendDataV3;
    QVector<AlarmEventInfo> alarmInfo;

    void reset()
    {
        trendData.clear();
        trendDataV2.clear();
        trendDataV3.clear();
        alarmInfo.clear();
    }
};

struct OxyCRGWaveInfo
{
    WaveformID id;
    int sampleRate;
    int maxWaveValue;
    int minWaveValue;
    int waveBaseLine;
    QVector<WaveDataType> waveData;
    union
    {
        struct
        {
            RESPZoom zoom;
        } resp;

        struct
        {
            CO2DisplayZoom zoom;
        } co2;
    } waveInfo;

    void reset()
    {
        waveData.clear();
    }
};

/* 趋势图子窗口信息*/
struct TrendSubWidgetInfo
{
    TrendSubWidgetInfo() : xHead(0), xTail(0), yTop(0), yBottom(0)
    {
    }

    int xHead;
    int xTail;
    int yTop;
    int yBottom;
};

/* parse the trend data segment */
TrendDataPackage parseTrendSegment(const TrendDataSegment *dataSegment);

/**
 * @brief convert 翻译时间
 * @param index
 * @return
 */
const char *convert(ShortTrendDuration index);
