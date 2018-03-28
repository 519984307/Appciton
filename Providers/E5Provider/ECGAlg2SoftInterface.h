#pragma once
#include "RTypeDefine.h"
using namespace E5ECGALG;

class ECGInterfaceIFace
{
public:

	virtual ~ECGInterfaceIFace();

	
	/* 算法初始化设置 */
	// 设置ECG工作模式：Monitor/Diagnostic/ST/Surgery
	virtual ECGUsrFlt setEcgMode(ECGUsrFlt ecgcalmode) = 0;

	// 导联模式切换 3/5/12
	virtual ECGMode setLeadMode(ECGMode ecgmode) = 0;

	// 陷波频率设置
	virtual ECGNotch setNotchFilt(ECGNotch notch) = 0;

	// 计算导联设置
	virtual ECGLeads setCalLead(ECGLeads calcLead) = 0;

	// 患者类型设置
	virtual PatientTyp setPatientType(PatientTyp ptype) = 0;

	// Pace检测开关设置
	virtual bool setPaceOnOff(bool onoff) = 0;

	/* 填数据给算法，调用计算 */
	//1. 送进来的数据解释：iPaceIn   - pace信号的标记，这个是ADAS1000计算得到的体内PACE检测的结果
	//                     leadoff[] - 导联脱落标志，每一个bit代表一个电极,共9bits
	//                     ecgData[] - 9导联的心电电极数据,24bit，500Hz
	virtual void setHandleData(int iPaceIn, short leadoff, int ecgData[]) = 0;

	/* 算法输出 */
	virtual short getHR() = 0;        //输出心率值，返回值为hr,值的范围[0,n],-100范围值计算输入为无效
	virtual ECG_ARR_ALARM getArr() = 0;       //输出ARR

	//算法输出心电标识
	virtual void getWaveform(bool &qrsDisp, bool &overload, short &leadoff, short &iPace, int ecgDisplayData[]) = 0;

	/*算法重启*/
	virtual void reSet() = 0;
};

ECGInterfaceIFace *getEcgInterface(void);
