#pragma once
#include <QPalette>
#include "Param.h"
#include "ECGSymbol.h"
#include "ECGDefine.h"
#include "SoundManager.h"
#include <QBasicTimer>
#include "SystemDefine.h"

enum
{
    MAJOR_SAMPLE_MODULE_INIT_FAILED = 1,
    MINOR_SAMPLE_MODULE_INIT_FAILED = (1 << 1),
    PD_BLANK_TEST_FAILED            = (1 << 2),
    PACE_SYNC_TEST_FAILED           = (1 << 3),
};

class ECGPVCSTrendWidget;
class ECGSTTrendWidget;
class OxyCRGHRWidget;
class MonitorSoftkeyAction;
class ECGWaveWidget;
class ECGProviderIFace;
class PDProviderIFace;
class ECGParam: public Param
{
    Q_OBJECT
public:
    static ECGParam &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ECGParam();
        }
        return *_selfObj;
    }
    static ECGParam *_selfObj;

    // 析构。
    virtual ~ECGParam();

#ifdef CONFIG_UNIT_TEST
    friend class TestECGParam;
#endif
public:
    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoWaveform(WaveformID id, short data);
    virtual void handDemoTrendData(void);

    // 获取可得的波形控件集。
    virtual void getAvailableWaveforms(QStringList &waveforms,
            QStringList &waveformShowName, int flag);

    // 获取可得的趋势控件集。
    virtual void getTrendWindow(QStringList &trend);

    // 获取子参数值。
    virtual short getSubParamValue(SubParamID id);

    // 显示子参数值。
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(ECGProviderIFace *provider);

    // 设置服务模式升级数据提供对象。
    void setServiceProvider(ECGProviderIFace *provider);

    // 设置界面对象。
    void setECGPVCSTrendWidget(ECGPVCSTrendWidget *trendWidget);
    void setECGSTTrendWidget(ECGSTTrendWidget *trendWidget);
    void setWaveWidget(ECGWaveWidget *waveWidget, ECGLead whichLead);
    void setOxyCRGWaveWidget(OxyCRGHRWidget *waveWidget);

    // 更新波形数据。
    void updateWaveform(int waveform[], bool *leadoff, bool ipaceMark = false,
            bool epaceMark = false, bool rMark = false);

    // 更新HR的数值
    void updateHR(short hr);
    //get HR, if hr invalid, return invData instead of return pr
    short getHR() const;

    // 更新HR的数值
    void updatePVCS(short pvcs);
    short getPVCS() const;

    // 更新HR的数值
    void updateST(ECGST lead,short st);
    short getST(ECGST lead) const;

    // 更新VFVT的数值
    void updateVFVT(bool onoff);

    // 导联脱落状态。
    void setLeadOff(ECGLead lead, bool status);
    char doesLeadOff(int lead);

    // 设置是否过载
    void setOverLoad(bool flag);

    // update ecg notify message
    void updateECGNotifyMesg(ECGLead lead, bool isAlarm);

    // 是否连接
    bool isConnected();

    // 12导模式进入与退出。
    void enter12Lead(void);
    void exit12Lead(void);

    // 设置OneShot报警。
    void setOneShotAlarm(ECGOneShotType t, bool f);

    // 是否超限报警
    void noticeLimitAlarm(int id, bool isAlarm);

    // 获取支持的工作模式。
    ECGLeadMode getAvailableMode(void);

    // 获取Topwaveform的名称。
    const QString &getCalcLeadWaveformName(void);
    const QString &getWaveWidgetName(ECGLead lead);

    // 获取可得的导联集。
    void getAvailableLeads(QList<ECGLead> &leads);

    // waveID to LeadID
    ECGLead waveIDToLeadID(WaveformID id);
    WaveformID leadToWaveID(ECGLead lead);

    // 模块复位
    void reset();

    // enable VF calc
    void enableVFCalc(bool enable);

    static unsigned selfTestResult;
    void handleSelfTestResult();

public:
    enum ECGTimerAction
    {
        ECG_ACTION_CHECK_RHYTHM_SNAPSHOT,
        ECG_ACTION_TRIGGER_ECG_RHYTHM_SNAPSHOT,
        ECG_ACTION_DIA_SOFTKEY_DISABLE,
        ECG_ACTION_DIA_GET_SNAPSHOT_DATA,
    };

public: // 用于访问配置相关信息。
    // 设置/获取导联模式。
    void setLeadMode(ECGLeadMode mode);
    ECGLeadMode getLeadMode(void) const;

    // 设置/获取显示模式。
    void setDisplayMode(ECGDisplayMode mode, bool refresh = true);
    ECGDisplayMode getDisplayMode(void);

    // 设置/获取计算导联。
    void setCalcLead(const QString &leadWaveform);
    void setCalcLead(ECGLead lead);
    void setLeadMode3DisplayLead(ECGLead lead);
    void setLeadMode3DisplayLead(const QString &leadWaveform);
    ECGLead getCalcLead(void);

    //设置12导界面下波形的增益。
    void set12LGain(ECGGain gain, ECGLead lead);

    //设置12导界面下波形的增益。
    ECGGain get12LGain(ECGLead lead);

    // 设置病人类型
    void setPatientType(unsigned char type);
    unsigned char getPatientType(void);

    // 轮转设置计算导联。
    void autoSetCalcLead(void);

    // 设置/获取带宽。
    void setBandwidth(int band);
    void setDiagBandwidth();
    void restoreDiagBandwidth(int isCompleted = false);
    ECGBandwidth getDiagBandwidth() {return ECG_BANDWIDTH_0525_40HZ;}
    ECGBandwidth getCalcLeadBandWidth(void);
    ECGBandwidth getBandwidth(void);
    ECGBandwidth getMFCBandwidth(void);
    ECGBandwidth getDisplayBandWidth(void);
    //get the last diag mode timestamp
    unsigned lastDiagModeTimestamp() const { return _lastDiagModeTimestamp;}

    //filter mode
    void setFilterMode(int mode);
    ECGFilterMode getFilterMode() const;

    // 获取计算导联带宽字符串
    QString getCalBandWidthStr();

    // 设置/获取起搏标记。
    void setPacermaker(ECGPaceMode onoff);
    ECGPaceMode getPacermaker(void);

    // 设置/获取12L起搏标记。
    void set12LPacermaker(ECGPaceMode onoff);
    ECGPaceMode get12LPacermaker(void);

    // 设置/获取波形速度。
    void setSweepSpeed(ECGSweepSpeed onoff);
    ECGSweepSpeed getSweepSpeed(void);

    // 设置/获取增益。
    void setGain(ECGGain gain, ECGLead lead);
    void setGain(ECGGain gain, int waveID);
    ECGGain getGain(ECGLead lead);
    void setAutoGain(ECGLead lead, int flag);
    bool getAutoGain(ECGLead lead);

    // 非监护模式下修改自动增益。
    void changeAutoGain(void);

    // 获取最大的增益值。
    int getMaxGain(void);

    // 设置/获取QRS音量。
    void setQRSToneVolume(int vol);
    int getQRSToneVolume(void);

    // 设置/获取工频滤波。
    void setNotchFilter(int filter);
    ECGNotchFilter getNotchFilter();
    ECGNotchFilter getCalcLeadNotchFilter();

    // 获取导联命令方式。
    ECGLeadNameConvention getLeadConvention(void) const;

    // 获取12L显示格式
    Display12LeadFormat get12LDisplayFormat(void) const;

    // 对导联线类型做相应处理
    void handleECGLeadCabelType(unsigned char cabelType);

    // 通过导联线类型获取导联模式
    ECGLeadMode getECGModeByECGCabelType(unsigned char cabelType);

    //get is ever lead on
    bool getEverLeadOn(ECGLead lead);

    //设置计算导联字串
    void setECGTrendWidgetCalcName(ECGLead calLead);

    //set/get is VF signal
    void setCheckPatient(bool flag);
    bool getCheckPatient() {return _isCheckPatient;}

signals:
    void calcLeadChanged();

private slots:
    //presenting rhythm, 6 seconds before the first primary lead on
    void presentRhythm();

private:
    // 构造。
    ECGParam();

    // 获取禁用的波形控件。
    void _getDisabledWaveforms(QStringList &waveforms);

    ECGProviderIFace *_provider;
    ECGPVCSTrendWidget *_pvcsTrendWidget;
    ECGSTTrendWidget *_ecgSTTrendWidget;
    ECGWaveWidget *_waveWidget[ECG_LEAD_NR];
    OxyCRGHRWidget *_waveOxyCRGWidget;
    QBasicTimer _timer;
    short _hrValue;
    short _pvcsValue;
    short _stValue[ECG_ST_NR];
    ECGLead _calcLead;
    ECGLeadMode _curLeadMode;
    ECGDisplayMode _displayMode;
    ECGPaceMode _12LeadPacerMarker;
    ECGBandwidth _chestFreqBand;
    ECGBandwidth _12LeadFreqBand;
    ECGFilterMode _filterMode;
    Display12LeadFormat _12LeadDispFormat;

    unsigned _lastDiagModeTimestamp;

    char _leadOff[ECG_LEAD_NR];
    MonitorSoftkeyAction *_monitorSoftkey;

    bool _isDisableDiaSoftKey;   //进入DIA模式后，1s内禁用此软按键

    bool _waveDataInvalid;

    bool _isPowerOnNewSession;              //用于设置计算导联，开机时只判断一次

    bool _isEverLeadOn[ECG_LEAD_NR];    // is ever lead on
    bool _isCheckPatient;               // VF signal
};
#define ecgParam (ECGParam::construction())
