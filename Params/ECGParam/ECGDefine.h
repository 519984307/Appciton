#pragma once

/**************************************************************************************************
 * ECG导联系统。
 *************************************************************************************************/
enum ECGLeadMode
{
    ECG_LEAD_MODE_3,       // 3导联
    ECG_LEAD_MODE_5,       // 5导联
    ECG_LEAD_MODE_12,      // 12导联
    ECG_LEAD_MODE_NR
};

/**************************************************************************************************
 * ECG显示模式。
 *************************************************************************************************/
enum ECGDisplayMode
{
    ECG_DISPLAY_NORMAL,
    ECG_DISPLAY_12_LEAD_FULL,
    ECG_DISPLAY_NR
};

/**************************************************************************************************
 * ECG alarm source
 *************************************************************************************************/
enum ECGAlarmSource
{
    ECG_ALARM_SRC_HR,
    ECG_ALARM_SRC_PR,
    ECG_ALARM_SRC_AUTO,
    ECG_ALARM_SRC_NR,
};

// 12导显示格式。
enum Display12LeadFormat
{
    DISPLAY_12LEAD_STAND,
    DISPLAY_12LEAD_CABRELA,
    DISPLAY_12LEAD_NR
};

/**************************************************************************************************
 * ECG导联
 *************************************************************************************************/
enum ECGLead
{
    ECG_LEAD_I,
    ECG_LEAD_II,
    ECG_LEAD_III,
    ECG_LEAD_AVR,
    ECG_LEAD_AVL,
    ECG_LEAD_AVF,
    ECG_LEAD_V1,
    ECG_LEAD_V2,
    ECG_LEAD_V3,
    ECG_LEAD_V4,
    ECG_LEAD_V5,
    ECG_LEAD_V6,
    ECG_LEAD_NR
};

/**************************************************************************************************
 * ECG_ST
 *************************************************************************************************/
enum ECGST
{
    ECG_ST_I,
    ECG_ST_II,
    ECG_ST_III,
    ECG_ST_aVR,
    ECG_ST_aVL,
    ECG_ST_aVF,
    ECG_ST_V1,
    ECG_ST_V2,
    ECG_ST_V3,
    ECG_ST_V4,
    ECG_ST_V5,
    ECG_ST_V6,
    ECG_ST_NR
};

/**************************************************************************************************
 * ECG增益
 *************************************************************************************************/
enum ECGGain
{
    ECG_GAIN_X0125,      // x0.125
    ECG_GAIN_X025,      // x0.25
    ECG_GAIN_X05,      // x0.5
    ECG_GAIN_X10,      // x1
    ECG_GAIN_X15,      // x1.5
    ECG_GAIN_X20,      // x2
    ECG_GAIN_X30,      // x3
    ECG_GAIN_AUTO,     // AUTO
    ECG_GAIN_NR
};

/**************************************************************************************************
 * ECG起搏分析开关
 *************************************************************************************************/
enum ECGPaceMode
{
    ECG_PACE_OFF,     // 关
    ECG_PACE_ON,      // 开
    ECG_PACE_NR
};

/**************************************************************************************************
 * ECG波形速度。
 *************************************************************************************************/
enum ECGSweepSpeed
{
    ECG_SWEEP_SPEED_125,
    ECG_SWEEP_SPEED_250,
    ECG_SWEEP_SPEED_500,
    ECG_SWEEP_SPEED_NR
};

/**************************************************************************************************
 * ECG文件传输格式。
 *************************************************************************************************/
enum ECGTransferFileFormat
{
    ECG_TRANSFER_FILE_FORMAT_PDF = 0,
    ECG_TRANSFER_FILE_FORMAT_JSON,
    ECG_TRANSFER_FILE_FORMAT_NR
};

/**************************************************************************************************
 * ECG带宽
 *************************************************************************************************/
enum ECGBandwidth
{
    ECG_BANDWIDTH_067_20HZ = 0,
    ECG_BANDWIDTH_067_40HZ,
    ECG_BANDWIDTH_0525_40HZ,
    ECG_BANDWIDTH_0525_150HZ,
    ECG_BANDWIDTH_NR,
    ECG_BANDWIDTH_MULTIPLE = ECG_BANDWIDTH_NR,   //multiple bandwidth, equal to the bandwidth NR
};

/**************************************************************************************************
 * ECG filter mode
 *************************************************************************************************/
enum ECGFilterMode
{
    ECG_FILTERMODE_MONITOR,
    ECG_FILTERMODE_DIAGNOSTIC,
    ECG_FILTERMODE_SURGERY,
    ECG_FILTERMODE_ST,
    ECG_FILTERMODE_NR,
};

/**************************************************************************************************
 * ECG工频滤波频率
 *************************************************************************************************/
enum ECGNotchFilter
{
    ECG_NOTCH_OFF1,             // 工频关
    ECG_NOTCH_50HZ,            // 50Hz滤波
    ECG_NOTCH_60HZ,            // 60Hz滤波
    ECG_NOTCH_50_AND_60HZ,     // 50&60Hz滤波
    ECG_NOTCH_NR
};

/**************************************************************************************************
 * ECGOneShot事件定义，包括心率失常类型
 *************************************************************************************************/
enum ECGOneShotType
{
    ECG_ONESHOT_ARR_ASYSTOLE,     // 心脏停搏
    ECG_ONESHOT_ARR_VFIBVTAC,     // 室颤/室速
    ECG_ONESHOT_CHECK_PATIENT_ALARM,       // Check patient alarm

    ECG_ONESHOT_ALARM_LEADOFF,
    ECG_ONESHOT_ALARM_V1_LEADOFF,
    ECG_ONESHOT_ALARM_V2_LEADOFF,
    ECG_ONESHOT_ALARM_V3_LEADOFF,
    ECG_ONESHOT_ALARM_V4_LEADOFF,
    ECG_ONESHOT_ALARM_V5_LEADOFF,
    ECG_ONESHOT_ALARM_V6_LEADOFF,
    ECG_ONESHOT_ALARM_TRANSFER_SUCCESS,
    ECG_ONESHOT_ALARM_TRANSFER_FAILED,
    ECG_ONESHOT_ALARM_OVERLOAD,
    ECG_ONESHOT_ALARM_COMMUNICATION_STOP,
    ECG_ONESHOT_NONE,
    ECG_ONESHOT_NR = ECG_ONESHOT_NONE,
};

/**************************************************************************************************
 * ECG 超限报警定义
 *************************************************************************************************/
enum ECGLimitAlarmType
{
    ECG_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * ECG 复用超限报警定义
 *************************************************************************************************/
enum ECGDupLimitAlarmType
{
    ECG_DUP_LIMIT_ALARM_HR_LOW,
    ECG_DUP_LIMIT_ALARM_HR_HIGH,
    ECG_DUP_LIMIT_ALARM_PR_LOW,
    ECG_DUP_LIMIT_ALARM_PR_HIGH,
    ECG_DUP_LIMIT_ALARM_NR
};

/**************************************************************************************************
 * ECG 导联命名协议
 *************************************************************************************************/
enum ECGLeadNameConvention
{
    ECG_CONVENTION_AAMI,
    ECG_CONVENTION_IEC,
    ECG_CONVENTION_NR
};

/***************************************************************************************************
 * ECG WAVE NOTIFY MESSAGE
 **************************************************************************************************/
enum ECGWaveNotify
{
    ECG_WAVE_NOTIFY_NORMAL,
    ECG_WAVE_NOTIFY_LEAD_OFF,
    ECG_WAVE_NOTIFY_CHECK_PATIENT,
    ECG_WAVE_NOTIFY_NR
};

// ecg 分析结果
struct ECGAnalysisResult
{
    ECGAnalysisResult()
    {
        time = 0;
        analysisType = 0;
        startTime = 0;
        endTime = 0;
        segNumber = 0;
        intervalCode = 0;
        noisyNumber = 0;
        saturatedNumber = 0;
        adviseDection = 0;
    }

    unsigned int calSum()
    {
        unsigned checkSum = 0;
        checkSum += (time + analysisType + startTime + endTime);
        checkSum += (segNumber + intervalCode + noisyNumber + saturatedNumber);
        checkSum += adviseDection;

        return checkSum;
    }

    unsigned time;
    short analysisType;
    unsigned startTime;
    unsigned endTime;
    short segNumber;
    short intervalCode;
    short noisyNumber;
    short saturatedNumber;
    short adviseDection;
}__attribute__((packed));

struct ECGAnalysisMetricsUnit
{
    ECGAnalysisMetricsUnit()
    {
        segType = 0;
        segStartTime = 0;
        segEndTime = 0;
        segMode = 0;
        segNumber = 0;

        for (int i = 0; i < 13; ++i)
        {
            segSequence[i] = 0;
        }
    }

    unsigned int calSum()
    {
        unsigned int checkSum = 0;
        checkSum += (segType + segStartTime + segEndTime + segMode + segNumber);
        for (int i = 0; i < 13; ++i)
        {
            checkSum += segSequence[i];
        }

        return checkSum;
    }

    short segType;
    unsigned segStartTime;
    unsigned segEndTime;
    short segMode;
    short segNumber;
    short segSequence[13];
}__attribute__((packed));

// ecg analysis metrics
struct ECGAnalysisMetrics
{
    ECGAnalysisMetrics()
    {
        time = 0;
    }

    unsigned int calSum()
    {
        unsigned int checkSum = time;
        checkSum += segment[1].calSum();
        checkSum += segment[2].calSum();
        checkSum += segment[3].calSum();

        return checkSum;
    }

    unsigned time;
    ECGAnalysisMetricsUnit segment[3];
}__attribute__((packed));
