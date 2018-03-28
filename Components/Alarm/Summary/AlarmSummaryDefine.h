#pragma once
#include <time.h>
#include "BaseDefine.h"
#include "AlarmDefine.h"
#include "ParamDefine.h"
#include "ECGDefine.h"

#define ArrayEntries(Array)     (sizeof(Array) / sizeof(Array[0]))

struct AlarmSummaryItem
{
    AlarmSummaryItem()
    {
        alarmTime = 0;
        oneShotID = InvData();
        alarmPriority = ALARM_PRIO_LOW;
        ::memset(alarmMessage, 0, sizeof(alarmMessage));

        paramID = PARAM_NONE;
        subParamID = SUB_PARAM_NONE;
        for (unsigned i = 0; i < ArrayEntries(trendData); i++)
        {
            trendData[i] = InvData();
        }

        waveformID = WAVE_NONE;
        waveformRate = 0;
        waveformLen = 0;
        for (unsigned i = 0; i < ArrayEntries(waveData); i++)
        {
            waveData[i] = InvData();
        }

        lead = ECG_LEAD_II;
        gain = ECG_GAIN_X10;
        for (unsigned i = 0; i < ArrayEntries(ecgNoise); i++)
        {
            ecgNoise[i] = ECG_ONESHOT_NONE;
        }

        co2Baro = 0;
    }

    // 通用属性。
    time_t alarmTime;                      // 报警发生的时间。
    char alarmMessage[MAX_SUMMARY_TITLE_LEN];  // 报警信息，显示在报警条目上的信息。
    int oneShotID;                         // OneShot报警标记，当它为无效值时表示该报警不是OneShot报警。
    AlarmPriority alarmPriority;           // 报警等级。

    // 参数属性
    ParamID paramID;                       // 报警的参数。
    SubParamID subParamID;                 // 报警的子参数。
    TrendDataType trendData[SUB_PARAM_NR]; // 报警时刻的参数值。

    // 波形属性。
    WaveformID waveformID;                 // 波形参数ID。
    short waveformRate;                    // 波形数据的采样率，单位时个/秒。
    short waveformLen;                     // 实际存储的波形数据个数。
    WaveDataType waveData[MAX_WAVEFORM_CACHE_LEN];   // 波形数据区。

    // 心电属性。
    ECGLead lead;                          // 计算导联。
    ECGGain gain;                          // 波形幅度(增益)。
    ECGOneShotType ecgNoise[16];           // ECG报警事件。

    // 二氧化碳属性。
    int co2Baro;                           // CO2的实时压力,hPa为单位。
};
