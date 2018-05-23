#pragma once
#include "BaseDefine.h"
#include "ParamDefine.h"

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
    TrendValueSegment values[0];        /* followed by the sub param value  */
};

/* data description of the trend storage file */
struct TrendDataDescription
{
    unsigned startTime;                 /* rescue event start time */
    unsigned moduleConfig;              /* module config */
    char patientAge;                    /* patient age */
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
        alarmFlag = false;
        co2Baro  = 0;
    }
    unsigned time;
    QMap<SubParamID, TrendDataType> subparamValue;
    QMap<SubParamID, bool> subparamAlarm;
    short co2Baro;
    unsigned char alarmFlag;
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
    RESOLUTION_RATIO_NR
};

/* 趋势组 */
enum TrendGroup
{
    TREND_GROUP_RESP = 0,
    TREND_GROUP_IBP,
    TREND_GROUP_AG
};

/* 趋势图类型 */
enum TrendGraphType
{
    TREND_GRAPH_TYPE_NORMAL,
    TREND_GRAPH_TYPE_NIBP,
    TREND_GRAPH_TYPE_ART_IBP,           // IBP动脉类型
    TREND_GRAPH_TYPE_AG_TEMP,
};

/* 趋势图子窗口信息*/
struct TrendSubWidgetInfo
{
    TrendSubWidgetInfo()
    {
        downRuler = 0;
        upRuler = 100;
    }
    SubParamID id;
    TrendGraphType type;
    int xHead;
    int xTail;
    int yTop;
    int yBottom;
    int downRuler;
    int upRuler;
};
