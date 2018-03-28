#pragma once
#include <cstring>
#include <QString>
#include "BaseDefine.h"
#include "ParamDefine.h"
#include "RescueDataDefine.h"
#include "ECGDefine.h"
#include "UnitManager.h"

// Summary 类型
enum SummaryType
{
    SUMMARY_ECG_RHYTHM,               // ECG节率。
    SUMMARY_ECG_FOREGROUND_ANALYSIS,  // ECG前景分析。
    SUMMARY_SHOCK_DELIVERY,           // SHOCK动作。
    SUMMARY_CHECK_PATIENT_ALARM,      // "check patient"报警。
    SUMMARY_PACER_STARTUP,            // 进入pacer模式。
    SUMMARY_PHY_ALARM,                // 生理报警。
    SUMMARY_PRINTER_ACTION,           // 打印机激活。
    SUMMARY_CODE_MAKER,               // 临床事件。
    SUMMARY_NIBP,                     // NIBP Messure event
    SUMMARY_DIAG_ECG,                 // Diagnostic ECG
    SUMMARY_MONITOR,                  // monitor snapshot, store in full disclosure snapshot report only
    SUMMARY_PRINTER_ACTION_2_TRACE,   // print action with two trace
    SUMMARY_PRINTER_ACTION_3_TRACE,   // print action with three trace
    SUMMARY_PRINTER_ACTION_4_TRACE,   // print action with four trace
    SUMMARY_NR
};

const char *getSummaryName(SummaryType type);

// Summary 中的参数值
struct SummaryParamValue
{
    SummaryParamValue()
    {
        value = InvData();
        mul = 1;
        type = UNIT_NONE;
    }

    unsigned int calSum()
    {
        return value + mul + type;
    }

    short value;
    int mul;
    UnitType type;
}__attribute__((packed));

// Summary 项目描述
struct SummaryItemDesc
{
    SummaryItemDesc()
    {
        type = 0;
        time = 0;
    }

    unsigned int calSum()
    {
        unsigned int sum = 0;

        sum += type;
        sum += time;
        return sum;
    }

    unsigned char type;           // Summary类型
    unsigned time;                // Summary记录时间
}__attribute__((packed));

//summary 头部信息
struct SummaryHead
{
    void sum()
    {
        checkSum = lastItemTime + totalItems + moduleConfig;

        for (int i = 0; i < MAX_DEVICE_ID_LEN; ++i)
        {
            checkSum += deviceID[i];
        }

        for (int i = 0; i < MAX_PATIENT_NAME_LEN; ++i)
        {
            checkSum += patientName[i];
        }

        int serialNumCount = sizeof(serialNum) / sizeof(serialNum[0]);
        for (int i = 0; i < serialNumCount; ++i)
        {
            checkSum += serialNum[i];
        }
    }

    SummaryHead()
    {
        lastItemTime = 0;
        totalItems = 0;
        moduleConfig = 0;
        ::memset(deviceID, 0, sizeof(deviceID));
        ::memset(patientName, 0, sizeof(patientName));
        ::memset(serialNum, 0, MAX_SERIAL_NUM_LEN);
        checkSum = 0;
    }
    unsigned lastItemTime;                     // 最后一条summary的时间
    unsigned short totalItems;                 // summary总数
    short moduleConfig;                        // module Config
    char deviceID[MAX_DEVICE_ID_LEN];                         // 设备标识符
    char patientName[MAX_PATIENT_NAME_LEN];                     // 病人姓名
    char serialNum[MAX_SERIAL_NUM_LEN];                        // 设备序列号
    char reserved[50];                         // 预留
    char checkSum;                             // 累加和
}__attribute__((packed));

// summary公共信息
struct SummaryCommonInfo
{
    unsigned int calSum()
    {
        unsigned int sum = 0;
        sum = moduleConfig;
        sum += co2bro + co2Zoom + spo2Gain + respGain +lastMeasureTime;
        sum += (ECGLeadName + ECGGain + ECGBandwidth + normalEcgGain + normalEcgBandWidth + ECGNotchFilter + waveNum);
        sum += patientType;
        for (int i = 0; i < SUB_PARAM_NR; ++i)
        {
            sum += paramValue[i].calSum();
        }

        int ecgWaveCount = sizeof(ECGWave) / sizeof(ECGWave[0]);
        for (int i = 0; i < ecgWaveCount; ++i)
        {
            sum += ECGWave[i];
        }

        for (int i = 0; i < MAX_DEVICE_ID_LEN; ++i)
        {
            sum += deviceID[i];
        }

        for (int i = 0; i < MAX_PATIENT_NAME_LEN; ++i)
        {
            sum += patientName[i];
        }

        int serialNumCount = sizeof(serialNum) / sizeof(serialNum[0]);
        for (int i = 0; i < serialNumCount; ++i)
        {
            sum += serialNum[i];
        }

        return sum;
    }

    SummaryCommonInfo()
    {
        moduleConfig = 0;
        co2bro = 0;
        co2Zoom = 0;
        spo2Gain = 0;
        respGain = 0;
        ECGLeadName = 0;
        ECGGain = 0;
        ECGBandwidth = 0;
        normalEcgGain = 0;
        normalEcgBandWidth = 0;
        ECGNotchFilter = 0;
        patientType = 0;
        lastMeasureTime = 0;
        waveNum = 0;
        ::memset(ECGWave, 0, sizeof(ECGWave));
//        SummaryParamValue paramValue[SUB_PARAM_NR];
        ::memset(deviceID, 0, sizeof(serialNum));
        ::memset(patientName, 0, sizeof(serialNum));
        ::memset(serialNum, 0, sizeof(serialNum));
    }

    short moduleConfig;                        //module config
    short co2bro;
    char co2Zoom;                              // CO2放大倍数
    char spo2Gain;                             // spo2 gain
    char respGain;                             // resp gain
    char ECGLeadName;                          // 主要ECG导联名称
    char ECGGain;                              // 主要ECG大小
    char ECGBandwidth;                         // 主要ECG带宽
    char normalEcgBandWidth;                   // Normal Ecg lead (not calculate lead) bandwidth
    char ECGNotchFilter;                       //notch filter
    char patientType;                           //patient type
    unsigned normalEcgGain;                     // store the other 3 Normal Ecg lead (not calculate lead) gain,
                                                // the most significant byte is set to 1 if we need draw all the gain
    unsigned lastMeasureTime;          // 上一次有效的测量完成时间
    int waveNum;                               // 波形数据个数，触发打印时使用
    int ECGWave[500 * DATA_STORAGE_WAVE_TIME];                     // 主要ECG18s波形
    SummaryParamValue paramValue[SUB_PARAM_NR];// 参数值
    char deviceID[MAX_DEVICE_ID_LEN];                         // 设备标识符
    char patientName[MAX_PATIENT_NAME_LEN];                     // 病人姓名
    char serialNum[MAX_SERIAL_NUM_LEN];                        // 设备序列号
}__attribute__((packed));

//summary ecg rhythm
struct SummaryECGRhythm
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
}__attribute__((packed));

//summary foreground ecg analysis
enum FgAnalysisResult
{
    FG_ANALYSIS_NO_SHOCK_ADVISED = 0,
    FG_ANALYSIS_SHOCK_ADVISED,
    FG_ANALYSIS_NOISY_ECG,
    FG_ANALYSIS_HALTED,
};


struct SummaryForegroundECGAnalysis
{
    SummaryForegroundECGAnalysis()
    {
        startTime = 0;
        endTime = 0;
        segmentNum = 0;
        analysisResult = 0;
        for(int i = 0; i< 3; i++)
        {
            segmentShockable[i] = 0;
            segmentStartTime[i] = 0;
        }
    }

    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
        checkSum += startTime + endTime + segmentNum + analysisResult;
        for(int i = 0; i< 3; i++)
        {
            checkSum +=segmentShockable[i] + segmentStartTime[i];
        }
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned startTime;
    unsigned endTime;
    unsigned char segmentNum;      //2,3,4, when equal to 4, the segments are the last 3 segments
    unsigned char analysisResult;       //0: no shock; 1: shock; 2: nosiy ecg; 3: analysis halted
    unsigned char segmentShockable[3];
    unsigned segmentStartTime[3];
    unsigned int checkSum;
}__attribute__((packed));

//summary shock delivery
struct SummaryShockDelivery
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
        checkSum += (shockCount + selectEngry + deliveryEngry + patientImdepedance +
                     patientCurrent);
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned short shockCount;
    unsigned char selectEngry;
    unsigned short deliveryEngry;
    unsigned short patientImdepedance;
    unsigned short patientCurrent;
    unsigned int checkSum;
}__attribute__((packed));

//summary check patient
struct SummaryCheckPatient
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
        checkSum += systemMode;
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    char systemMode;
    unsigned int checkSum;
}__attribute__((packed));

//summary pacer start up
struct SummaryPacerStart
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
}__attribute__((packed));

struct SummaryAlarmInfo
{
    unsigned int calSum() { return paramid + (isOneshot << 7) + alarmType;}
    unsigned char paramid;          //alarm param
    unsigned char isOneshot:1;      //1: oneshot alarm, 0: limit alarm
    unsigned char alarmType:7;      //alarm type id
};

//summary phy alarm
struct SummaryPhyAlarm
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
        checkSum += waveID + waveGain + waveNum;

        int activeAlarmNum = sizeof(curActiveAlarmInfos) / sizeof(curActiveAlarmInfos[0]);
        for (int i = 0; i < activeAlarmNum; ++i)
        {
            checkSum += curActiveAlarmInfos[i].calSum();
        }
        checkSum += curAlarmInfo.calSum();
        checkSum += curActiveAlarmNum;

        int waveBufNum = sizeof(waveBuf) / sizeof(waveBuf[0]);
        for (int i = 0; i < waveBufNum; ++i)
        {
            checkSum += waveBuf[i];
        }
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    SummaryAlarmInfo curActiveAlarmInfos[(SUB_PARAM_NR+SUB_DUP_PARAM_NR) * 2];  //store current active alarm, should be large enough
    SummaryAlarmInfo curAlarmInfo;                        //current alarm info
    char curActiveAlarmNum;                              //current active alarm counter
    char waveID;
    char waveGain;
    int waveBuf[250 * DATA_STORAGE_WAVE_TIME];
    int waveNum;       // 波形数据个数，触发打印时使用
    unsigned int checkSum;
}__attribute__((packed));

//summary printer action
struct SummaryPrinterAction
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
}__attribute__((packed));

//summary printer action, 2 trace
struct SummaryPrinterAction2Trace
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
    int waveBuf1[500 * DATA_STORAGE_WAVE_TIME];
    int waveNum1;
    int waveID1;
}__attribute__((packed));

//summary printer action, 3 trace
struct SummaryPrinterAction3Trace
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
    int waveBuf1[500 * DATA_STORAGE_WAVE_TIME];
    int waveNum1;
    int waveID1;
    int waveBuf2[500 * DATA_STORAGE_WAVE_TIME];
    int waveNum2;
    int waveID2;
}__attribute__((packed));

//summary printer action, 4 trace
struct SummaryPrinterAction4Trace
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
    int waveBuf1[500 * DATA_STORAGE_WAVE_TIME];
    int waveNum1;
    int waveID1;
    int waveBuf2[500 * DATA_STORAGE_WAVE_TIME];
    int waveNum2;
    int waveID2;
    int waveBuf3[500 * DATA_STORAGE_WAVE_TIME];
    int waveNum3;
    int waveID3;
}__attribute__((packed));

//summary monitor, record all the traces in the screen, only store in full disclosure
struct SummaryMonitor
{
    SummaryMonitor()
        :otherTraceNum(0)
    {
        memset(waveBuf, 0 , sizeof(waveBuf));
        memset(waveID, 0xff, sizeof(waveID));
        memset(waveNum, 0, sizeof(waveNum));
    }
    ~SummaryMonitor(){
        for(int i = 0; i< 3; i++)
        {
            if(waveBuf[i])
            {
                delete [] waveBuf[i];
            }
        }
    }
    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    int otherTraceNum;      //trace number except the calculation lead, 3 at most
    int *waveBuf[3];         //buffer of the trace
    int waveID[3];           //trace id list
    int waveNum[3];          //waveNum
};

//summary present rhythm
struct SummaryPresentRhythm
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    unsigned int checkSum;
}__attribute__((packed));

//summary code marker
struct SummaryCodeMarker
{
    void sum()
    {
        checkSum = itemDesc.calSum();
        checkSum += (pacerRate + pacerCurrent + pacerMode + moduleConfig);
        checkSum += co2bro + sync + lastMeasureCompleteTime;

        for (int i = 0; i < SUB_PARAM_NR; ++i)
        {
            checkSum += paramValue[i].calSum();
        }

        for (int i = 0; i < MAX_DEVICE_ID_LEN; ++i)
        {
            checkSum += deviceID[i];
        }

        for (int i = 0; i < MAX_PATIENT_NAME_LEN; ++i)
        {
            checkSum += patientName[i];
        }

        int serialNumCount = sizeof(serialNum) / sizeof(serialNum[0]);
        for (int i = 0; i < serialNumCount; ++i)
        {
            checkSum += serialNum[i];
        }
    }

    SummaryItemDesc itemDesc;
    short moduleConfig;                         // module config
    short pacerRate;                           // 脉率
    unsigned char pacerCurrent;                // 电流
    unsigned char pacerMode;                   // 起搏模式
    char workmode;                             // system work mode
    short co2bro;
    char sync;                                 //Defibrillator in sync mode
    SummaryParamValue paramValue[SUB_PARAM_NR];// 参数值,NIBP参数为上一次的有效值
    unsigned lastMeasureCompleteTime;          // 上一次有效的测量完成时间
    char deviceID[MAX_DEVICE_ID_LEN];          // 设备标识符
    char patientName[MAX_PATIENT_NAME_LEN];    // 病人姓名
    char serialNum[MAX_SERIAL_NUM_LEN];        // 设备序列号
    char selectCodeName[20];                   // 选择的code marker名称
    unsigned int checkSum;
}__attribute__((packed));

//NIBP snapshot
struct SummaryNIBP
{
    void sum()
    {
        checkSum = itemDesc.calSum();
        checkSum += moduleConfig;
        checkSum += nibpErrorCode + co2bro + lastMeasureTime;
        checkSum += patientType;

        for (int i = 0; i < SUB_PARAM_NR; ++i)
        {
            checkSum += paramValue[i].calSum();
        }

        for (int i = 0; i < MAX_DEVICE_ID_LEN; ++i)
        {
            checkSum += deviceID[i];
        }

        for (int i = 0; i < MAX_PATIENT_NAME_LEN; ++i)
        {
            checkSum += patientName[i];
        }

        int serialNumCount = sizeof(serialNum) / sizeof(serialNum[0]);
        for (int i = 0; i < serialNumCount; ++i)
        {
            checkSum += serialNum[i];
        }
    }

    SummaryItemDesc itemDesc;
    short moduleConfig;                        // module config
    char patientType;                          // patient type
    char nibpErrorCode;                        // nibp error code, if no error, it will be zero
    short co2bro;
    SummaryParamValue paramValue[SUB_PARAM_NR];// 参数值,NIBP参数为上一次的有效值
    unsigned lastMeasureTime;                 // 上一次有效的测量完成时间
    char deviceID[MAX_DEVICE_ID_LEN];          // 设备标识符
    char patientName[MAX_PATIENT_NAME_LEN];    // 病人姓名
    char serialNum[MAX_SERIAL_NUM_LEN];        // 设备序列号
    unsigned int checkSum;
}__attribute__((packed));

#define DIAG_ECG_SAMPLE_RATE 250
#define DIAG_ECG_DURATION 12
struct SummaryDiagnosticECG
{
    void sum()
    {
        checkSum = itemDesc.calSum() + commonInfo.calSum();
        checkSum += diagBandWidth;
    }

    SummaryItemDesc itemDesc;
    SummaryCommonInfo commonInfo;
    int diagBandWidth;                             //diagnostic filter
    unsigned int checkSum;
}__attribute__((packed));
