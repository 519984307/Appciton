/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#pragma once
#include "Param.h"
#include "NIBPSymbol.h"
#include "PatientDefine.h"
#include "NIBPStateMachine.h"
#include "NIBPEventDefine.h"
#include <QMap>
#include <QTimer>
#include "NIBPParamInterface.h"

struct NIBPMeasureResultInfo
{
    NIBPMeasureResultInfo()
    {
        sys = InvData();
        dia = InvData();
        map = InvData();
        pr = InvData();
        errCode = 0;
    }
    short sys;
    short dia;
    short map;
    short pr;
    short errCode;
};

class NIBPProviderIFace;
class NIBPTrendWidget;
class NIBPDataTrendWidget;
class NIBPParam: public Param , public NIBPParamInterface
{
    Q_OBJECT
#ifdef CONFIG_UNIT_TEST
    friend class TestNIBPParam;
#endif
public:
    static NIBPParam &getInstance(void);
    ~NIBPParam();

public:
    // 解析数据包。
    void unPacket(unsigned char *packet, int len);

    // 处理NIBP模块的事件。
    void handleNIBPEvent(NIBPEvent event, unsigned char args[], int argLen, unsigned nibpTimestamp = 0);

    // 状态机运行。
    void machineRun(void);

    // 当前的状态机类型及对应的状态类型
    NIBPStateMachineType curMachineType() const;
    int curStatusType() const;

    void switchState(unsigned char newStateID);

public:
    // 初始化参数。
    virtual void initParam(void);

    // 处理DEMO数据。
    virtual void handDemoTrendData(void);

    /* reimplement */
    virtual void exitDemo();

    // 获取子参数值
    virtual int16_t getSubParamValue(SubParamID id);

    // 显示
    virtual void showSubParamValue();

    // 获取当前的单位。
    virtual UnitType getCurrentUnit(SubParamID id);

    // 设置数据提供对象。
    void setProvider(NIBPProviderIFace *provider);

    // 导出Provider给状态机和状态对象使用。
    NIBPProviderIFace &provider(void);

    // 模块复位
    void reset();

    // 切换模式
    void changeMode(NIBPStateMachineType type);

    // NIBP错误,模块禁用
    void errorDisable(void);
    bool isErrorDisable(void)
    {
        return _isNIBPDisable;
    }

    // 设置连接，供给对象调用。
    void setConnected(bool isConnected);

    // 是否连接
    bool isConnected();

    // 通信中断。
    void connectedFlag(bool flag);

    //通信错误标志
    bool getConnectedState()
    {
        return _connectedFlag;
    }

    // check whether current is in measure state
    bool isMeasuring() const;

    // 通信超时
    void connectedTimeout(void);

    // 设置界面对象。
    void setNIBPTrendWidget(NIBPTrendWidget *trendWidget);
    void setNIBPDataTrendWidget(NIBPDataTrendWidget *trendWidget);

    // 解析测量结果。
    bool analysisResult(const unsigned char *packet, int len, short &sys,
                        short &dia, short &map, short &pr, NIBPOneShotType &err);

    // 设置测量结果。
    void setResult(int16_t sys, int16_t dia, int16_t map, int16_t pr, NIBPOneShotType err);

    // 设置最后测量时间。
    void setLastTime(void);

    // 清除测量结果。
    void clearResult(void);

    // 清除显示值。
    void invResultData(void);

    // 袖带压力。
    void setCuffPressure(int pressure);

    // 更新倒计时信息，秒为单位。
    void setCountdown(int t);

    // NIBP状态信息显示。
    void setText(QString text);
    void setText(void);

    // NIBP 模式显示。
    void setModelText(QString text);

    //上次测量时间的显示
    void setShowMeasureCount(void);

    // 获得NIBP的测量数据。
    int16_t getSYS(void);
    int16_t getDIA(void);
    int16_t getMAP(void);
    int16_t getPR(void);
    NIBPMeasureResult getMeasureResult(void);
    void setMeasureResult(NIBPMeasureResult flag);
    void recoverInitTrendWidgetData();

    // 测量时间切换标志
    bool isSwitchTime(void);
    void setSwitchFlagTime(bool flag);

    // NIBP按钮标志
    bool isSwitchType(void);
    void setSwitchFlagType(bool flag);

    // 额外触发
    void setAdditionalMeasure(bool flag);
    bool isAdditionalMeasure(void);

    // STAT模式在5分钟时间未到的情况下被手动关闭的标志
    void setSTATClose(bool flag);
    bool isSTATClose(void);

    // 自动模式中的手动触发测量
    void setAutoMeasure(bool flag);
    bool isAutoMeasure(void);

    // 在安全模式时,打开STAT模式标志,但模式未运行,在安全模式下临时打开
    void setSTATOpenTemp(bool flag);
    bool isSTATOpenTemp(void);

    // 以STAT启动测量的第一次（测量中转STAT的不算第一次）
    void setSTATFirst(bool flag);
    bool isSTATFirst(void);

    // STAT测量模式
    void setSTATMeasure(bool flag);
    bool isSTATMeasure(void);

    // 超限报警通知
    void noticeLimitAlarm(int id, bool flag);

    // create a nibp snapshot
    void setSnapshotFlag(bool flag);
    bool isSnapshotFlag(void);
    void createSnapshot(NIBPOneShotType err);

public:
    // 设置预充气值。。
    void setInitPressure(int index);

    /**
     * @brief setRawDataSend 设置原始数据发送开关
     * @param onOff
     */
    void enableRawDataSend(bool onOff);

    //获取不同病人类型的初始压力值
    int getInitPressure();

    // 设置/获取测量模式。
    void setMeasurMode(NIBPMode mode);
    NIBPMode getMeasurMode(void);
    NIBPMode getSuperMeasurMode(void);

    // 设置/获取自动测量时的时间间隔。
    void setAutoInterval(NIBPAutoInterval interv);
    NIBPAutoInterval getAutoInterval(void);
    int getAutoIntervalTime(void);

    // 设置/获取智能充气模式。
    void setIntelligentInflate(NIBPIntelligentInflate mode);
    NIBPIntelligentInflate getIntelligentInflate(void);

    // 获取单位。
    UnitType getUnit(void);

    /**
     * @brief setUnit  设置单位
     * @param type 单位类型
     */
    void setUnit(UnitType type);

    // 停止测量。
    void stopMeasure(void);

    //按钮释放
    void keyReleased(void);

    //按钮按下
    void keyPressed(void);

    // 短按NIBP触发 测量起停控制，起停反操作：如果当前处于测量则停止，当前为停止则开始测量。
    void toggleMeasureShort(void);

    // 长按NIBP触发
    void toggleMeasureLong(void);

    // STAT在安全间隔内被关闭
    void safeWaitTimeSTATStop(void);

    // 状态转为AUTO
    void switchToAuto(void);

    // 状态转为MANUAL
    void switchToManual(void);

    //接收定标数据
    void setResult(bool result);

    // 获取校准是否回复
    bool geReply();

    // 获取校准结果
    bool getResult();

    // 设置/获取压力计模式下压力
    void setManometerPressure(int16_t value);
    int16_t getManometerPressure(void);

    // 刷新参数上下限
    virtual void updateSubParamLimit(SubParamID id);

    /**
     * @brief enterMaintain 进入/退出维护
     * @param enter 进入为true,退出为false
     */
    void enterMaintain(bool enter);

    /**
     * @brief isMaintain 是否处于维护
     * @return
     */
    bool isMaintain();

    /**
     * @brief clearTrendListData 清除趋势列表数据
     */
    void clearTrendListData();

    /**
     * @brief setFirstAuto 设置首次AUTO测量
     */
    void setFirstAuto(bool flag);   //

    bool isFirstAuto();

    /**
     * @brief setFirstAuto AUTO倒计时时候进行STAT测量，要有五秒放气期
     */
    void setAutoStat(bool flag);

    bool isAutoStat();

    void setZeroSelfTestState(bool);

    bool isZeroSelfTestState();

    void setDisableState(bool flag);

    bool getNeoDisState();

    void setCalibrateState(bool flag);  // 设置未校准导致禁用状态
    bool isCalibrateState(void);

signals:
    /**
     * @brief statBtnState 设置NIBP菜单中的STAT按键状态
     * @param false：stop stat  true：start stat
     */
    void statBtnState(bool);

private slots:
    void _patientTypeChangeSlot(PatientType type);
    void _btnTimeOut();
    void onPaletteChanged(ParamID id);

private:
    NIBPParam();

    NIBPProviderIFace *_provider;
    NIBPTrendWidget *_trendWidget;
    NIBPDataTrendWidget *_nibpDataTrendWidget;

    // 设置测量结果的数据。
    void transferResults(int16_t sys, int16_t dia, int16_t map, unsigned time);

    int16_t _sysValue;
    int16_t _diaValue;
    int16_t _mapVaule;
    int16_t _prVaule;
    unsigned _lastTime;
    NIBPMeasureResult _measureResult;
    bool _SwitchFlagTime;                   // 切换时间标志
    bool _SwitchFlagType;                   // 切换病人类型标志
    bool _additionalMeasureFlag;            // 额外充气标志
    bool _autoMeasureFlag;                  // 自动模式中的手动测量标志
    bool _statModelFlag;                    // STAT模式打开关闭标志
    bool _statCloseFlag;                    // STAT模式在5分钟时间未到的情况下被手动关闭的标志
    bool _statFirst;                        // 第一次启动STAT
    bool _toggleMeasureLongFlag;            // 长按触发生效标志
    bool _statOpenTemp;                     // 在安全模式时,打开STAT模式标志,但模式未运行,在安全模式下临时打开
    bool _isCreateSnapshotFlag;             // 创建快拍标记；
    bool _isNIBPDisable;                    // 模块禁用；
    bool _isManualMeasure;                  // 手动测量标志
    bool _connectedFlag;                    // NIBP通信是否正常标志
    bool _connectedProvider;                // 是否连接Provider
    QString _text;

    bool _reply;                 // 校准回复
    bool _result;                // 校准结果
    int16_t _manometerPressure;             // 压力计模式下压力

    bool _isMaintain;            // 是否维护模式
    bool _firstAutoFlag;         // 第一次启动AUTO测量标志
    bool _autoStatFlag;         // auto倒计时开启Stat标志
    bool _zeroSelfTestFlag;     //开机较零状态标志
    bool _isNeoDisable;         // 是否新生儿禁用
    bool _CalibrateState;

private:
    typedef QMap<NIBPStateMachineType, NIBPStateMachine *> MachineStateMap;
    MachineStateMap _machines;

    NIBPStateMachine *_activityMachine;       // 当前活跃状态机。

    QTimer *_btnTimer;

    unsigned char _oldState;
};
#define nibpParam (NIBPParam::getInstance())
