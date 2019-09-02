/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#pragma once
#include <QPalette>
#include "Param.h"
#include "ECGSymbol.h"
#include "SoundManager.h"
#include <QBasicTimer>
#include "SystemDefine.h"
#include "ECGAlg2SoftInterface.h"
#include "SystemManager.h"
#include "ECGParamInterface.h"

enum
{
    MAJOR_SAMPLE_MODULE_INIT_FAILED = 1,
    MINOR_SAMPLE_MODULE_INIT_FAILED = (1 << 1),
    PD_BLANK_TEST_FAILED            = (1 << 2),
    PACE_SYNC_TEST_FAILED           = (1 << 3),
};

class ECGPVCSTrendWidget;
class ECGSTTrendWidget;
class MonitorSoftkeyAction;
class ECGWaveWidget;
class ECGProviderIFace;
class PDProviderIFace;
class OxyCRGRRHRWaveWidget;
class ECGParam: public Param, public ECGParamInterface
{
    Q_OBJECT
public:
    static ECGParam &getInstance();

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
    /* reimplement */
    virtual void exitDemo();

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

    /**
     * @brief setOxyCRGHrWaveWidget
     * @param waveWidget
     */
    void setOxyCRGHrWaveWidget(OxyCRGRRHRWaveWidget *waveWidget);

    // 更新波形数据。
    void updateWaveform(int waveform[], bool *leadoff, bool ipaceMark = false,
                        bool epaceMark = false, bool rMark = false);

    // 更新HR的数值
    void updateHR(short hr);
    // get HR, if hr invalid, return invData instead of return pr
    short getHR() const;

    // 更新HR的数值
    void updatePVCS(short pvcs);
    short getPVCS() const;

    // 更新HR的数值
    void updateST(ECGST lead, short st);
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

    // 设置连接,供给对象调用
    void setConnected(bool isConnected);

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
    QString getWaveWidgetName(ECGLead lead);

    // 更新ECG心电标准显示
    void updateECGStandard(int standard);

    // 获取可得的导联集。
    void getAvailableLeads(QList<ECGLead> &leads);

    // waveID to LeadID
    ECGLead waveIDToLeadID(WaveformID id);
    WaveformID leadToWaveID(ECGLead lead);

    // 模块复位
    void reset();

    // enable VF calc
    void enableVFCalc(bool enable);

    void handleSelfTestResult(unsigned selfTestResult);

    /**
     * @brief getHrSourceTypeFromId  从参数id转换获取hr来源类型
     * @param id  参数id
     * @return hr来源类型
     */
   static HRSourceType getHrSourceTypeFromId(ParamID id);

    /**
     * @brief getIdFromHrSourceType  从hr来源类型转换获取参数id
     * @param type hr来源类型
     * @return 参数id
     */
    static ParamID getIdFromHrSourceType(HRSourceType type);

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

    // 设置12导界面下波形的增益。
    void set12LGain(ECGGain gain, ECGLead lead);

    // 设置12导界面下波形的增益。
    ECGGain get12LGain(ECGLead lead);

    // 设置病人类型
    void setPatientType(unsigned char type);
    unsigned char getPatientType(void);

    // 轮转设置计算导联。
    void autoSetCalcLead(void);

    // 设置/获取带宽。
    void setBandwidth(int band);
    ECGBandwidth getCalcLeadBandWidth(void);
    ECGBandwidth getBandwidth(void);
    ECGBandwidth getDisplayBandWidth(void);

    // filter mode
    void setFilterMode(int mode);
    ECGFilterMode getFilterMode() const;

    // 自学习设置
    void setSelfLearn(bool onOff);
    bool getSelfLearn() const;

    // 阀值设置
    void setARRThreshold(ECGAlg::ARRPara parameter, short value);

    // 设置/获取起搏标记。
    void setPacermaker(ECGPaceMode onoff);
    ECGPaceMode getPacermaker(void);
    void updatePacermaker(void);

    // 设置/获取12L起搏标记。
    void set12LPacermaker(ECGPaceMode onoff);
    ECGPaceMode get12LPacermaker(void);

    // 设置/获取波形速度。
    void setSweepSpeed(ECGSweepSpeed onoff);
    ECGSweepSpeed getSweepSpeed(void);

    /**
     * @brief getWaveDataRate  获取波形速率
     * @return
     */
    int getWaveDataRate(void) const;

    /**
     * @brief setGain 设置ECG全屏界面增益
     * @param gain
     */
    void setGain(ECGGain gain);

    /**
     * @brief setGain 设置增益
     * @param gain 增益
     * @param lead 导联
     * @param isAutoGain 自动增益导致的增益设置
     */
    void setGain(ECGGain gain, ECGLead lead, bool isAutoGain = false);
    // 设置/获取增益。
    void setGain(ECGGain gain, int waveID);
    ECGGain getGain(ECGLead lead);
    ECGGain getECGAutoGain(ECGLead lead);
    void setAutoGain(ECGLead lead, int flag);
    bool getAutoGain(ECGLead lead);

    // 非监护模式下修改自动增益。
    void changeAutoGain(void);

    // 获取最大的增益值。
    int getMaxGain(void);

    // 设置/获取QRS音量。
    void setQRSToneVolume(SoundManager::VolumeLevel vol);
    int getQRSToneVolume(void);

    // 设置/获取工频滤波。
    void setNotchFilter(ECGNotchFilter filter);
    void updateEditNotchFilter();
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

    // get is ever lead on
    bool getEverLeadOn(ECGLead lead);

    // 设置计算导联字串
    void setECGTrendWidgetCalcName(ECGLead calLead);

    // set/get is VF signal
    void setCheckPatient(bool flag);
    bool getCheckPatient()
    {
        return _isCheckPatient;
    }

    /**
     * @brief clearOxyCRGWaveNum  //清除呼吸氧和波形更新计数
     */
    void clearOxyCRGWaveNum(void);

    /**
     * @brief updateWaveWidgetStatus  更新其内部第一个心电波形的状态
     */
    void updateWaveWidgetStatus(void);

    /**
     * @brief getLeadNameConvention  获取导联协议
     * @return
     */
    ECGLeadNameConvention getLeadNameConvention() const;

    /**
     * @brief setFristConnect
     */
    void setFristConnect(void);
    bool getFristConnect(void);

    /**
     * @brief setRawDataOnOff set raw data
     */
    void setRawDataOnOff(bool sta);
    bool getRawDataOnOff(void);

    /**
     * @brief adjustPrintWave 调整打印菜单中的设置波形
     * @param preECGLead 调整前的ECG波形
     * @param curECGLead 当前的ECG波形
     */
    void adjustPrintWave(ECGLead preECGLead, ECGLead curECGLead);

signals:
    void calcLeadChanged();

    /**
     * @brief updateNotchFilter 陷波信息更新信号
     */
    void updateNotchFilter(void);
    /**
     * @brief updateFilterMode  滤波模式更新信号
     */
    void updateFilterMode(void);

private slots:
    /**
     * @brief onWorkModeChanged  滤波模式更新
     */
    void onWorkModeChanged(WorkMode mode);

    /**
     * @brief onPaletteChanged change palette
     */
    void onPaletteChanged(ParamID id);
private:
    // 构造。
    ECGParam();

    // 获取禁用的波形控件。
    void _getDisabledWaveforms(QStringList &waveforms);

    ECGProviderIFace *_provider;
    ECGPVCSTrendWidget *_pvcsTrendWidget;
    ECGSTTrendWidget *_ecgSTTrendWidget;
    ECGWaveWidget *_waveWidget[ECG_LEAD_NR];
    OxyCRGRRHRWaveWidget *oxyCRGRrHrTrend;
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
    ECGNotchFilter _notchFilter;
    Display12LeadFormat _12LeadDispFormat;
    ECGLeadNameConvention _ecgStandard;

    char _leadOff[ECG_LEAD_NR];
    MonitorSoftkeyAction *_monitorSoftkey;

    bool _isDisableDiaSoftKey;   // 进入DIA模式后，1s内禁用此软按键

    bool _waveDataInvalid;

    bool _isPowerOnNewSession;              // 用于设置计算导联，开机时只判断一次

    bool _isEverLeadOn[ECG_LEAD_NR];    // is ever lead on
    bool _isCheckPatient;               // VF signal

    int _updateNum;            // 呼吸氧和波形更新标志计数
    bool _connectedProvider;    // 连接Provider标识

    bool _isFristConnect;         // 开机后是否正常连接过导联
    ECGGain _autoGain[ECG_LEAD_NR];      // 自动增益计算出的实际增益

    short getHRMaxValue();
};
#define ecgParam (ECGParam::getInstance())
