/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 **/


#pragma once
#include "ECGTypeDefine.h"

class ECGInterfaceIFace
{
public:
    virtual ~ECGInterfaceIFace();

    // 算法重启
    virtual void reset() = 0;

    //---------------------------- 设置参数 ----------------------------//
    // ECG工作模式设置: 监护/起搏/除颤
    virtual bool setWorkMode(ECGAlg::ECGWorkMode workmode) = 0;

    // ECG分析模式设置: 监护/诊断/手术/ST
    virtual bool setCalcMode(ECGAlg::ECGCalcMode calcmode) = 0;

    // 导联模式设置: 3导联/5导联/12导联
    virtual bool setLeadMode(ECGAlg::ECGLeadMode leadmode) = 0;

    // 陷波频率设置: OFF/50Hz/60Hz/50&60Hz
    virtual bool setNotchType(ECGAlg::ECGNotchFilter notchtype) = 0;

    // 计算导联设置: I/II/III/V1/V2/V3/V4/V5/V6/aVR/aVL/aVF
    virtual bool setCalcLead(ECGAlg::ECGLeads calclead) = 0;

    // 患者类型设置: 成人/少儿/新生儿
    virtual bool setPatientType(ECGAlg::PatientType ptype) = 0;

    // Pace检测开关设置: ON/OFF
    virtual bool setPaceOnOff(bool onoff) = 0;

    // 自学习开关设置: ON/OFF
    virtual bool setSelfLearn(bool onoff) = 0;

    // ARR阈值设置: 设置参数，设置阈值
    virtual bool setARRThreshold(ECGAlg::ARRPara parameter, short value) = 0;

    // ---------------------------- 算法入口 ------------------------------------//
    // 心电算法调用接口, 参数及心电数据输入
    /*
    ecgdisp: 9导心电显示数据, 数组长度为9, 分别对应I/II/III/V1/V2/V3/V4/V5/V6
    ecgcalc: 9导心电计算数据, 数组长度为9, 分别对应I/II/III/V1/V2/V3/V4/V5/V6
    qrsdelay: 常规R波检出延时
    leadoff: 9导电极导联脱落标记, 从高到低按位对应RLD-LA-LL-RA-V1-V2-V3-V4-V5-V6
    paceflag: 体内起搏标记
    qrsflag: 快速R波标记
    overload: 直流过载标记
    pdblank: PDBlank标记
    */
    virtual void setHandleData(int ecgdisp[], int ecgcalc[], int qrsdelay, short leadoff, bool qrsflag, bool paceflag, bool overload, bool pdblank) = 0;

    // ---------------------------- 算法输出项 -----------------------------------//
    // 心律失常报警输出, -1为无效值
    virtual ECGAlg::ARRCODE getARR() = 0;

    // 获取PVC率, -1为无效值
    virtual short getPVCRate() = 0;

    // 心电显示数据及相关指示标记输出
    /*
    ecgdisp: 12导心电显示数据, 数组长度为12, 分别对应I/II/III/aVR/aVL/aVF/V1/V2/V3/V4/V5/V6
    leadoff: 9导电极导联脱落标记, 从高到低按位对应RLD-LA-LL-RA-V1-V2-V3-V4-V5-V6
    qrsflag: 快速R波标记
    paceflag: 体内起搏标记
    overload: 直流过载标记
    pdblank: PDBlank标记
    */
    virtual bool getDispData(int ecgdisp[], short &leadoff, bool &qrsflag, bool &paceflag, bool &overload, bool &pdblank) = 0;
};

ECGInterfaceIFace *getECGInterface(void);
