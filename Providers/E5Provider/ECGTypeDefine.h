/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 **/

#pragma once

namespace ECGAlg
{
// 各电极的按位对应关系
#define ECG_POLE_RLD    0x0200
#define ECG_POLE_LA     0x0100
#define ECG_POLE_LL     0x0080
#define ECG_POLE_RA     0x0040
#define ECG_POLE_V1     0x0020
#define ECG_POLE_V2     0x0010
#define ECG_POLE_V3     0x0008
#define ECG_POLE_V4     0x0004
#define ECG_POLE_V5     0x0002
#define ECG_POLE_V6     0x0001

// E5心电模块基本参数设置
#define ECG_ORG_SAMPLERATE   (2000)     // ECG原始数据采样频率
#define ECG_CAL_SAMPLERATE   (500)      // ECG算法分析采样频率
#define ECG_DATA_GAIN        (816)      // ECG采样增益：816LSB/mV
#define ECG_DOWNSAMPLE_RATIO (ECG_ORG_SAMPLERATE / ECG_CAL_SAMPLERATE)     // ECG降采样比率

#define ECG_ALG_ALLDATA_LEN  (12)       // ECG算法分析的最大数据导联个数
#define ECG_ALG_INDATA_LEN   (9)        // ECG前端采集的最大数据导联个数

#define ECG_ALLDISP_DATALEN  (ECG_ALG_ALLDATA_LEN)// 用于ECG显示的数据导联个数
#define ECG_DISP_DATALEN     (ECG_ALG_INDATA_LEN) // 用于ECG显示的数据导联个数
#define ECG_ALLCALC_DATALEN  (ECG_ALG_ALLDATA_LEN)// 用于ECG计算的数据导联个数
#define ECG_CALC_DATALEN     (ECG_ALG_INDATA_LEN) // 用于ECG计算的数据导联个数

#define ECG_INVALID_VALUE    (65536) // ECG数据无效值
#define ECG_HR_INVALID_VALUE (-1)    // HR无效值

// ECG数据包
struct ECGDataPack
{
    int     I;      // 各数据导联的数据
    int     II;
    int     III;
    int     V1;
    int     V2;
    int     V3;
    int     V4;
    int     V5;
    int     V6;
    int     pacestate; // 体内起搏状态
    short   leadoff;   // 电极导联脱落标志(参见上文中的各电极的按位对应关系)
    bool    pdblank;     // PDBLANK起始标记, 仅在PDBLANK起始时置1
    bool    pdblankepoch;// PDBLANK时段标记, 处于PDBLANK时期内均置1
};

// ECG工作模式:监护模式/起搏模式/除颤模式
enum ECGWorkMode
{
    ECG_WORK_MONITOR,
    ECG_WORK_PACER,
    ECG_WORK_DEFIB
};

// ECG分析模式:监护模式/诊断模式/手术模式/ST模式
enum ECGCalcMode
{
    ECG_CALC_MONITOR,
    ECG_CALC_DIAGNOSIS,
    ECG_CALC_SURGERY,
    ECG_CALC_ST
};

// 导联模式:3导联/5导联/12导联
enum ECGLeadMode
{
    ECG_MODE_3_LEAD,
    ECG_MODE_5_LEAD,
    ECG_MODE_12_LEAD
};

// 导联线类型:3导联/5导联/12导联
enum ECGLineType
{
    ECG_TYPE_NONE,
    ECG_TYPE_3_LINE,
    ECG_TYPE_5_LINE,
    ECG_TYPE_12_LINE
};

// 陷波器选项: OFF/50Hz/60Hz/50&60Hz
enum ECGNotchFilter
{
    ECG_NOTCH_OFF,
    ECG_NOTCH_50HZ,
    ECG_NOTCH_60HZ,
    ECG_NOTCH_5060HZ
};

// 患者模式:成人/小儿/新生儿
enum PatientType
{
    PATIENT_ADULT,
    PATIENT_PEDIATRIC,
    PATIENT_NEONATE
};

// 心电导联编号
enum ECGLeads
{
    I,
    II,
    III,
    V1,
    V2,
    V3,
    V4,
    V5,
    V6,
    aVR,
    aVL,
    aVF
};

// ARR报警相关的阈值设置项
enum ARRPara
{
    ASYS_THRESHOLD,            // 停搏报警延迟时间, 设置范围:5~10s, 默认:5。输入值:5~10。
    PAUSE_THRESHOLD,           // 心脏暂停时间, 设置范围:1.5/2.0/2.5s, 默认:2.0。输入值:0/1/2。
    TACHY_THRESHOLD,           // 心动过速阈值, 设置范围:60~300bpm, 默认:120(成人)/160(小儿)。输入值:60~300。
    BRADY_THRESHOLD,           // 心动过缓阈值, 设置范围:15~120bpm, 默认:50(成人)/75(小儿)。输入值:15~120。
    EXTRETACHY_THRESHOLD,      // 极度心动过速阈值, 设置范围:120~300bpm, 默认:160(成人)/180(小儿)。输入值:120~300。
    EXTREBRADY_THRESHOLD,      // 极度心动过缓阈值, 设置范围:15~60bpm, 默认:35(成人)/50(小儿)。输入值:15~60。
    VENT_TACHY_THRESHOLD,      // 室速心率阈值, 设置范围:100~200bpm, 默认:130。输入值:100~200。
    VENT_BRADY_THRESHOLD,      // 室缓心率阈值, 设置范围:15~60bpm, 默认:40。输入值:15~60。
    VENT_TACHY_PVC_THRESHOLD,  // 室速心率阈值, 设置范围:3~99/min, 默认:6。输入值:3~99。
    VENT_BRADY_PVC_THRESHOLD,  // 室缓心率阈值, 设置范围:3~99/min, 默认:5。输入值:3~99。
    MULTIF_PVC_WINDOW          // 多形PVC窗宽, 设置范围:3~31, 默认:15。输入值:3~31。
};

enum ARRCODE
{
    NONE = 0,
    ASYS = 1,  // Asystole
    VFVT = 2,  // VFib/VTac
    VT = 3,  // Vtac
    VEBRA = 4,  // Vent. Brady
    EXTRETA = 5,  // Extreme Tachy
    EXTREBRA = 6,  // Extreme Brady
    NONSVT = 7,  // Nonsus. Vtac
    VENTRHY = 8,  // Vent. Rhythm
    SRUNVT = 9,  // Short run PVCs
    COUPLET = 10, // Couplet
    VRONT = 11, // R on T
    VBIG = 12, // Bigeminy
    VTRIG = 13, // Trigeminy
    VMULTI = 14, // Multif. PVCs
    SPVC = 15, // single PVC
    PAUSE = 16, // Pause
    PNP = 17, // Pacer Not Pacing
    PNC = 18, // Pacer Not Capture
    MISSED = 19, // Missed Beat
    TACHY = 20, // Tachycardia
    BRADY = 21, // Bradycardia
    IRR = 22, // Irr.Rhythm
};

struct ECGDetInfo
{
    short	WorkMode;   // 工作模式
    short	PatientType;// 患者类型
    short	LeadMode;   // 导联模式
    short   CalcLead;   // 计算导联设置，参照ECGLeads中的编号
    bool    PaceOnoff;  // 起搏信号检测开关
};

struct UserSetMode
{
    short WorkMode;
    short CalcMode;
    short NotchType;
    short LeadMode;
    short CalcLead;
    short PatientType;
    bool  PaceOnoff;
};

} // namespace ECGAlg
