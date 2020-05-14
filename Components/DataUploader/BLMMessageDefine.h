/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/28
 **/

#pragma once
#include <QtGlobal>
#include <QString>
#include "PatientDefine.h"

enum BLMLanguage
{
    BLM_LANG_English = 0,
    BLM_LANG_ChineseSimplified,
    BLM_LANG_Language_NR,
};

enum BLMDateFormat
{
    BLM_DATE_FM_Y_M_D = 0,
    BLM_DATE_FM_M_D_Y,
    BLM_DATE_FM_D_M_Y,
    BLM_DATE_FM_FORMAT_NR,
};

enum BLMTimeFormat
{
    BLM_TIME_FM_12 = 0,
    BLM_TIME_FM_24,
    BLM_TIME_FM_NR,
};

enum BLMEcgConvention
{
    BLM_ECG_CONV_AAMI,
    BLM_ECG_CONV_IEC,
    BLM_ECG_CONV_NR,
};

enum BLMUnit
{
    BLM_UNIT_NONE,                  /* no unit */
    BLM_UNIT_C,                     /* Celsius °C */
    BLM_UNIT_F,                     /* Fahrenheit °F */
    BLM_UNIT_MMHG,                  /* mmHg */
    BLM_UNIT_KPA,                   /* kPa */
    BLM_UNIT_PERCENT,               /* % */
    BLM_UNIT_RPM,                   /* respirations per minute */
    BLM_UNIT_BPM,                   /* beats per minute */
    BLM_UNIT_UV,                    /* μV */
    BLM_UNIT_MV,                    /* mV */
    BLM_UNIT_KG,                    /* kg */
    BLM_UNIT_LB,                    /* lb */
    BLM_UNIT_CM,                    /* cm */
    BLM_UNIT_MM,                    /* mm */
    BLM_UNIT_INCH,                  /* inch */
    BLM_UNIT_MS,                    /* ms */

    BLM_UNIT_NR
};

enum BLMWaveformID
{
    BLM_WAVE_NONE = -1,             /* no valid wave */
    BLM_WAVE_I,                     /* ECG LEAD I */
    BLM_WAVE_II,                    /* ECG LEAD II */
    BLM_WAVE_III,                   /* ECG LEAD III */
    BLM_WAVE_AVR,                   /* ECG LEAD aVR */
    BLM_WAVE_AVL,                   /* ECG LEAD aVL */
    BLM_WAVE_AVF,                   /* ECG LEAD aVF */
    BLM_WAVE_V1,                    /* ECG LEAD V1 */
    BLM_WAVE_V2,                    /* ECG LEAD V2 */
    BLM_WAVE_V3,                    /* ECG LEAD V3 */
    BLM_WAVE_V4,                    /* ECG LEAD V4 */
    BLM_WAVE_V5,                    /* ECG LEAD V5 */
    BLM_WAVE_V6,                    /* ECG LEAD V6 */
    BLM_WAVE_RESP,                  /* RESP */
    BLM_WAVE_SPO2,                  /* SPO2 */
    BLM_WAVE_CO2,                   /* CO2 */

    BLM_WAVE_NR,
};

enum BLMParamID
{
    BLM_PARAM_NONE                      = 0,           /* no valid param */
    BLM_PARAM_ECG                       = 1,           /* ECG */
    BLM_PARAM_RESP                      = 2,           /* RESP */
    BLM_PARAM_SPO2                      = 3,           /* SPO2 */
    BLM_PARAM_NIBP                      = 4,           /* NIBP */
    BLM_PARAM_TEMP                      = 5,           /* TEMP */
    BLM_PARAM_CO2                       = 6,           /* CO2 */
    BLM_PARAM_NR
};

enum BLMTrendParamID
{
    BLM_TREND_PARAM_NONE                = 0,            /* no valid trend param*/
    BLM_TREND_PARAM_HR_PR               = 1,            /* HR/PR */
    BLM_TREND_PARAM_RR_BR               = 2,            /* RR/BR */
    BLM_TREND_PARAM_SPO2                = 3,            /* SPO2 */
    BLM_TREND_PARAM_PI                  = 4,            /* PI */
    BLM_TREND_PARAM_NIBP_SYS            = 5,            /* NIBP sys */
    BLM_TREND_PARAM_NIBP_DIA            = 6,            /* NIBP DIA */
    BLM_TREND_PARAM_NIBP_MAP            = 7,            /* NIBP MAP */
    BLM_TREND_PARAM_TEMP_T1             = 8,            /* temp T1 */
    BLM_TREND_PARAM_TEMP_T2             = 9,            /* temp T2 */
    BLM_TREND_PARAM_TEMP_TD             = 10,           /* temp TD */
    BLM_TREND_PARAM_ETCO2               = 11,           /* EtCO2 */
    BLM_TREND_PARAM_FICO2               = 12,           /* FiCO2 */
    BLM_TREND_PARAM_NR,                                 /* NR */
};

enum BLMWaveStatusFlag
{
    BLM_WAVE_ST_INVALID = 0x0001,   /* invalid wave flag */
    BLM_WAVE_ST_QRS = 0x0002,       /* QRS flag */
    BLM_WAVE_ST_PACE = 0x0004,      /* pace flag */
};

enum BLMMessageID
{
    BLM_MSG_NONE                = 0,            /* invalid message, should never receive this message */
    BLM_MSG_DEV_CONF            = 1,            /* device configuration */
    BLM_MSG_WAVE_CONF           = 2,            /* wave configuration */
    BLM_MSG_CONTINUE_WAVE       = 3,            /* continue wave */
    BLM_MSG_TREND_CONF          = 4,            /* trend configuration */
    BLM_MSG_TREND               = 5,            /* trend */
    BLM_MSG_PATIENT_INFO        = 6,            /* patient info */
    BLM_MSG_NR
};

struct BLMMessageHeader
{
    BLMMessageHeader()
        : msgId(BLM_MSG_NR), elapsedTime(0), devTime(0), msecTime(0)
    {
    }
    BLMMessageID msgId;
    quint32 elapsedTime;
    quint32 devTime;
    qint64 msecTime;
};

enum BLMProductType
{
    BLM_PRODUCT_VITAVUE_10 = 1,
    BLM_PRODUCT_VITAVUE_12 = 2,
    BLM_PRODUCT_VITAVUE_15 = 3,
    BLM_PRODUCT_VITAVUE_GO = 4,
    BLM_PRODUCT_IRIS_TOUCH = 5,
    BLM_PRODUCT_COMPATIBLE = 100,
};

enum BLMModuleConfigFlag
{
    BLM_MOD_CONF_ECG        = 0x00000001,           /* 3/5 lead ecg */
    BLM_MOD_CONF_RESP       = 0x00000002,           /* resp */
    BLM_MOD_CONF_SPO2       = 0x00000004,           /* spo2 */
    BLM_MOD_CONF_NIBP       = 0x00000008,           /* NIBP */
    BLM_MOD_CONF_TEMP       = 0x00000010,           /* temp */
    BLM_MOD_CONF_CO2        = 0x00000020,           /* CO2 */
    BLM_MOD_CONF_ECG12      = 0x00000040,           /* 12 lead ecg */
    BLM_MOD_CONF_IBP        = 0x00000080,           /* IBP */
    BLM_MOD_CONF_CO         = 0x00000100,           /* C.O. */
    BLM_MOD_CONF_RECORDER   = 0x00000200,           /* recorder */
    BLM_MOD_CONF_HDMI       = 0x00000400,           /* HDMI */
    BLM_MOD_CONF_NURSE_CALL = 0x00000800,           /* nurse call */
};

struct BLMSoftwareVersionInfo
{
    QString hostSwVer;          /* host software version */
    QString e5SwVer;            /* E5 sw version */
    QString s5SwVer;            /* S5 sw version */
    QString n5SwVer;            /* N5 sw version */
    QString t5SwVer;            /* T5 sw version */
    QString sysSwVer;           /* system board sw version */
};

struct BLMMessageDevConfig
{
    int msgVer;                         /* version of this message */
    QString serialNumber;               /* device serial number */
    BLMProductType productType;         /* device product type */
    QString bedID;                      /* device bed id */
    BLMLanguage lang;                   /* language */
    BLMDateFormat dateFormat;           /* date format */
    BLMTimeFormat timeFormat;           /* time format */
    BLMEcgConvention ecgConv;           /* ECG lead name convention */
    int moduleConfig;                   /* module config */
    BLMSoftwareVersionInfo swVers;      /* software versions */
};

struct BLMWaveConfig
{
    BLMWaveformID waveId;
    int sampleRate;                 /* wave smaple rate */
    int baseLine;                   /* wave baseline */
    int maxVal;                     /* wave maximum value */
    int minVal;                     /* wave minimum value */
    BLMUnit unit;                   /* wave unit */
    float scale;                    /* scale of wave data */
};

struct BLMTrendConfig
{
    BLMParamID paramID;
    BLMTrendParamID trendID;
    int lowLimit;
    int highLimit;
    int scale;
    BLMUnit unit;
};

struct BLMPatientInfo
{
    QString id;                 /**< patient ID */
    QString name;               /**< patient name */
    int bedId;                  /**< patient bed id, -1 means invalid */
    PatientType type;           /**< patient type */
    PatientSex gender;             /**< patinet gender */
    PatientBloodType blood;     /**< patinet blood type */
    int weight;                 /**< weight, in unit of Kg, scale by 10 */
    int height;                 /**< height, in unit of cm */
    short age;                  /**< age */
};

