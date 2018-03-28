#pragma once
namespace E5ECGALG
{
	// ECG module setting
#define ECG_ORG_SAMPLERATE  (500)     // the sampling rate of the raw ecg
#define ECG_CAL_SAMPLERATE  (500)      // the sampling rate for qrs detection
#define ECG_DATA_GAIN       (6524)     // the value for 1mV, the sampling precision is 0.15327uV/LSB

#define ECG_ALG_ALLDATA_LEN (12)       // the maximum number of data lead supported in the device, the 12-lead ECG consists of limb leads and chest leads
#define ECG_ALG_INDATA_LEN  (9)        // the totoal number of data lead taken in from the front-end chip

#define ECG_INVALID_VALUE    (16777216) // invalid value for ecg data, as the ecg data is 24-bit signed integer
#define ECG_HR_INVALID_VALUE (-1)       // invalid value for HR output

	// 心电数据包结构体
	struct ECGRawDataDesc
	{
		int     I;
		int     II;
		int     III;
		int     aVR;
		int     aVL;
		int     aVF;
		int     v1;
		int     v2;
		int     v3;
		int     v4;
		int     v5;
		int     v6;
		int     iPace;       // 内部Pace标志，每一个bit代表一个导联。
		short   nLeadOff;    // 导联脱落标志，每一个bit代表一个电极，详见ECG_POLE_XXX的定义。
	};



	enum ECGWorkMode
	{
		ECG_WORK_MONITOR,
		ECG_WORK_PACER,
		ECG_WORK_DEFIB
	};

	// 导联模式
	enum ECGMode
	{
		ECG_MODE_NONE = -1,
		ECG_MODE_3_LEAD,
		ECG_MODE_5_LEAD,
		ECG_MODE_12_LEAD
	};

	//// 导联线类型
	//enum ECGLineType
	//{
	//	ECG_TPYE_NONE_LINE = 0,
	//	ECG_TYPE_3_LINE,
	//	ECG_TYPE_5_LINE,
	//	ECG_TYPE_12_LINE
	//};

	// 陷波器
	enum ECGNotch
	{
		ECG_NOTCH_NONE = -1,
		ECG_NOTCH_OFF,
		ECG_NOTCH_50_HZ,
		ECG_NOTCH_60_HZ,
		ECG_NOTCH_5060_HZ
	};

	// 滤波类型
	enum ECGUsrFlt
	{
		ECG_FILTER_NONE = -1,
		ECG_FILTER_MONITOR,
		ECG_FILTER_DIAGNOSTIC,
		ECG_FILTER_SURGERY,
		ECG_FILTER_ST,
	};

	//患者模式
	enum PatientTyp
	{
		PATIENT_NONE = -1,
		PATIENT_AUDLT,
		PATIENT_PEDIATRIC,
		PATIENT_NEONATE,
	};

	// 导联名称
	enum ECGLeads
	{
		ECG_LEADS_NONE = -1,
		I = 0,
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

	enum ECG_ARR_ALARM
	{
		ECG_ALARM_NONE,

	};
	struct RDetDataInfo {
		short	cPatientType;   // 病人类型
		// 0: 成人
		// 1: 幼儿/儿童
		// 2: 婴儿/新生儿

		short	nLeadNum;       // 导联数目
		short   nCalcuLead;     // 计算导联序号，
		// 赋值为计算导联的位置，
		// 若计算导联为I，nCalcuLead  = 0;
		// 若计算导联为II，nCalcuLead = 1;

		short    bPaceFlag;     // Pace检测开关
		// 0: 关闭Pace检测
		// 1: 使能Pace检测

		short    nWorkMode;     // 工作模式
		// 0: Pacer
		// 1: Moniter
		// 2: Defibrillator
	};

	struct UsrSetMode
	{
		short _nNotchType;
		short _nWorkMode;  //代表不同的带宽模式：监护/诊断/手术/ST

		short _nCalLead;
		short _nLeadMode;
		short _nPatientType;

		bool  _bPaceOn;
	};

}
