/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "TimeDate.h"
#include "N5Provider.h"
#include "NIBPTrendWidget.h"
#include "NIBPDataTrendWidget.h"
#include "NIBPServiceStateMachine.h"
#include "NIBPMonitorStateMachine.h"
#include "NIBPMonitorMeasureState.h"
#include "NIBPMonitorErrorState.h"
#include "NIBPMonitorGetResultState.h"
#include "NIBPMonitorMeasureState.h"
#include "NIBPMonitorSafeWaitTimeState.h"
#include "NIBPMonitorStandbyState.h"
#include "NIBPState.h"
#include "NIBPMonitorStopState.h"
#include "NIBPMonitorStartingState.h"
#include "EventStorageManager.h"
#include "TrendDataStorageManager.h"
#include "SoundManager.h"
#include "PatientManager.h"
#include "TimeManager.h"
#include "NIBPCountdownTime.h"
#include "AlarmSourceManager.h"
#include "TrendCache.h"

/**************************************************************************************************
 * 病人类型修改。
 *************************************************************************************************/
void NIBPParam::_patientTypeChangeSlot(PatientType type)
{
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        return;
    }
    int enable = 0;
    setSwitchFlagType(true);

    // 模式修改则停止当前的测量。
    handleNIBPEvent(NIBP_EVENT_TRIGGER_PATIENT_TYPE, NULL, 0);

    machineConfig.getModuleInitialStatus("NIBPNEOMeasureEnable", reinterpret_cast<bool *>(&enable));

    if (!_connectedFlag)
    {
        return;
    }
    if (type == PATIENT_TYPE_NEO && !enable)
    {
        _isNeoDisable = true;
        errorDisable();
    }
    else if (_isNeoDisable && !_isNIBPDisable)  // 如果只是新生儿禁用则恢复正常状态。
    {
        _isNeoDisable = false;
        switchState(NIBP_MONITOR_STANDBY_STATE);
        handleNIBPEvent(NIBP_EVENT_MODULE_RESET, NULL, 0);                        // 恢复禁用状态
    }
    //设置病人类型与预充气值
    if (NULL != _provider)
    {
        _provider->setPatientType((unsigned char)patientManager.getType());
        int initVal = getInitPressure();   // 获取初始压力值

        _provider->setInitPressure(initVal);
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void NIBPParam::initParam(void)
{
    if (_provider == NULL)
    {
        return;
    }
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        return;
    }
    int enable = 0;
    machineConfig.getModuleInitialStatus("NIBPNEOMeasureEnable", reinterpret_cast<bool *>(&enable));
    if (patientManager.getType() == PATIENT_TYPE_NEO && !enable)
    {
        _isNeoDisable = true;
        errorDisable();
    }
    _provider->serviceEnter(false);

    //智能充气
    _provider->enableSmartPressure(true);

    //设置病人类型与预充气值
    _provider->setPatientType((unsigned char)patientManager.getType());
    int initVal = getInitPressure();
    _provider->setInitPressure(initVal);

    //智能充气
//    int mode = NIBP_INTELLIGENT_INFLATE_OFF;
//    systemConfig.getNumValue("PrimaryCfg|NIBP|IntelligentInflate", mode);
//    if(mode == 0)
//    {
//        _provider->enableSmartPressure(false);
//    }
//    else
//    {
//        _provider->enableSmartPressure(true);
//    }
}

/**************************************************************************************************
 * NIBP错误,模块禁用。
 *************************************************************************************************/
void NIBPParam::errorDisable(void)
{
    handleNIBPEvent(NIBP_EVENT_MODULE_ERROR, NULL, 0);
}

void NIBPParam::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }
    _connectedProvider = isConnected;
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
static int counter = 300;
static int autoCounter = 0;
static bool firstDemo = true;       // 进入demo的第一次NIBP测量数据标志
static int firstCounter = 1;        // demo模式下第一次NIBP数据
static int manualCounter = 0;       // 手动测量计数器
void NIBPParam::handDemoTrendData(void)
{
    if (autoCounter > counter || manualCounter >= firstCounter)
    {
        _sysValue = qrand() % 30 + 90;
        _diaValue = qrand() % 20 + 60;
        _mapVaule = qrand() % 25 + 75;
        _prVaule = qrand() % 10 + 60;
        setResult(_sysValue, _diaValue, _mapVaule, _prVaule, NIBP_ONESHOT_NONE);
        eventStorageManager.triggerNIBPMeasurementEvent(timeManager.getCurTime(), NIBP_ONESHOT_NONE);

        setMeasureResult(NIBP_MEASURE_SUCCESS);
        autoCounter = 0;
        firstDemo = false;
        manualCounter = 0;
    }

    if (firstDemo)
    {
        manualCounter++;
    }

    NIBPMode mode = getSuperMeasurMode();
    if (mode == NIBP_MODE_AUTO)
    {
        autoCounter++;
        counter = getAutoIntervalTime();
    }
    else
    {
        autoCounter = 0;
    }
}

void NIBPParam::exitDemo()
{
    counter = 300;
    autoCounter = 0;
    firstDemo = true;
    manualCounter = 0;
    _sysValue = InvData();
    _diaValue = InvData();
    _mapVaule = InvData();
    _prVaule = InvData();
    _lastTime = 0;

    if (_oldState == NIBP_MONITOR_ERROR_STATE)
    {
        // 恢复状态机为进入演示模式前的状态
        switchState(_oldState);
    }
    else
    {
        switchState(NIBP_MONITOR_STANDBY_STATE);
        clearResult();
    }
    clearTrendListData();
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
int16_t NIBPParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_NIBP_SYS:
        return getSYS();

    case SUB_PARAM_NIBP_DIA:
        return getDIA();

    case SUB_PARAM_NIBP_MAP:
        return getMAP();
    case SUB_PARAM_NIBP_PR:
        return getPR();
    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void NIBPParam::showSubParamValue()
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showValue();
    }
    if (_nibpDataTrendWidget != NULL)
    {
        _nibpDataTrendWidget->showValue();
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType NIBPParam::getCurrentUnit(SubParamID /*id*/)
{
    return getUnit();
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void NIBPParam::setProvider(NIBPProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
    _provider->sendSelfTest();
    _provider->setPatientType(patientManager.getType());

    // 监护模式状态机。
    if (!_machines.contains(NIBP_STATE_MACHINE_MONITOR))
    {
        NIBPStateMachine *machine = new NIBPMonitorStateMachine();
        _activityMachine = machine;
        _machines.insert(machine->type(), machine);
    }

    if (!_machines.contains(NIBP_STATE_MACHINE_SERVICE))
    {
        NIBPStateMachine *machine = new NIBPServiceStateMachine();
        _machines.insert(machine->type(), machine);
    }

    if (_activityMachine->isExit())
    {
        _activityMachine->enter();
    }
    unsigned char cmd = 0x00;
    handleNIBPEvent(NIBP_EVENT_TRIGGER_MODEL, &cmd, 1);

    // 进入演示模式时，切换状态机为正常监护
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        _oldState = _activityMachine->curStatusType();
        switchState(NIBP_MONITOR_STANDBY_STATE);
    }
}

/**************************************************************************************************
 * 导出Provider对象。
 *************************************************************************************************/
NIBPProviderIFace &NIBPParam::provider(void)
{
    return *_provider;
}


/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void NIBPParam::reset(void)
{
    if (_provider == NULL || !isConnected())
    {
        return;
    }

    _isNIBPDisable = false;
    _activityMachine = _machines.find(NIBP_STATE_MACHINE_MONITOR).value();
    if (NULL != _activityMachine)
    {
        _activityMachine->enter();
    }

    _provider->sendSelfTest();

    initParam();

    handleNIBPEvent(NIBP_EVENT_MODULE_RESET, NULL, 0);
}

/**************************************************************************************************
 * 切换模式。
 *************************************************************************************************/
void NIBPParam::changeMode(NIBPStateMachineType type)
{
    MachineStateMap::iterator it = _machines.find(type);
    if (it != _machines.end())
    {
        if (it.key() == NIBP_STATE_MACHINE_SERVICE)
        {
            _provider->serviceEnter(true);
        }
        else
        {
            _provider->serviceEnter(false);
        }
        _activityMachine = it.value();
        _activityMachine->enter();
    }
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void NIBPParam::setNIBPTrendWidget(NIBPTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }

    _trendWidget = trendWidget;

    int16_t sys(InvData()), dia(InvData()), map(InvData());
    unsigned time(0);
    _trendWidget->recoverResults(sys, dia, map, time);
    _trendWidget->setResults(sys, dia, map, time);
    _sysValue = sys;
    _diaValue = dia;
    _mapVaule = map;
    _lastTime = time;
}

/**************************************************************************************************
 * 设置界面对象。
 *************************************************************************************************/
void NIBPParam::setNIBPDataTrendWidget(NIBPDataTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _nibpDataTrendWidget = trendWidget;
}

/**************************************************************************************************
 * 解析测量结果。
 *************************************************************************************************/
bool NIBPParam::analysisResult(const unsigned char *packet, int /*len*/, short &sys,
                               short &dia, short &map, short &pr, NIBPOneShotType &err)
{
    NIBPMeasureResultInfo info = *reinterpret_cast<NIBPMeasureResultInfo *>(const_cast<unsigned char *>(packet));
    err = NIBP_ONESHOT_NONE;

    // 测量有错误，获取错误码。
    if (info.errCode != 0x00)
    {
        err = (NIBPOneShotType)_provider->convertErrcode(info.errCode);
        return true;
    }
    // 测量无错，获取测量结果。
    sys = info.sys;
    dia = info.dia;
    map = info.map;
    pr = info.pr;

    if (sys == InvData() || dia == InvData() || map == InvData())
    {
        sys = InvData();
        dia = InvData();
        map = InvData();
    }
    return true;
}

/**************************************************************************************************
 * 设置测量结果。
 *************************************************************************************************/
void NIBPParam::setResult(int16_t sys, int16_t dia, int16_t map, int16_t pr, NIBPOneShotType err)
{
    setLastTime();

    _sysValue = sys;
    _diaValue = dia;
    _mapVaule = map;
    _prVaule = pr;

    if (_nibpDataTrendWidget != NULL)
    {
        _nibpDataTrendWidget->collectNIBPTrendData(_lastTime);
    }

    // 存在错误需要报错。
    if (err != NIBP_ONESHOT_NONE)
    {
        setText(trs("NIBPREADING") + "\n" + trs("NIBPFAILED"));
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(err, true);
        }
        if (getMeasurMode() == NIBP_MODE_MANUAL || getMeasurMode() == NIBP_MODE_AUTO)
        {
            int index = 0;
            systemConfig.getNumValue("PrimaryCfg|NIBP|AutomaticRetry", index);
            if (index)
            {
                if (isAdditionalMeasure())    // 如果已经进行过额外测量置false
                {
                    setAdditionalMeasure(false);
                }
                else
                {
                    setAdditionalMeasure(true);
                }
            }
            else
            {
                if (isAdditionalMeasure())
                {
                    setAdditionalMeasure(false);
                }
            }
        }

        // 额外测量需要隐藏上次测量时间
        clearResult();

        if (!isAdditionalMeasure())
        {
            // set complete even failed
            setMeasureResult(NIBP_MEASURE_FAIL);
        }

        // record nibp snapshot event following a additional measure
        createSnapshot(err);
        return;
    }

    setAdditionalMeasure(false);

    setShowMeasureCount();
    transferResults(_sysValue, _diaValue, _mapVaule, _lastTime);

    // 保存起来。
    setMeasureResult(NIBP_MEASURE_SUCCESS);
    createSnapshot(err);

    soundManager.nibpCompleteTone();

    if (_sysValue != InvData() && _diaValue != InvData() && _mapVaule != InvData())
    {
        unsigned t = timeDate.time();
        trendCache.collectTrendData(t, true);
        trendCache.collectTrendAlarmStatus(t);
        // 测量出结果后，收集一次趋势数据
        trendDataStorageManager.storeData(t, TrendDataStorageManager::CollectStatusNIBP);
    }
}


/**************************************************************************************************
 * NIBP上次测量时间的显示。
 *************************************************************************************************/
void NIBPParam::setShowMeasureCount(void)
{
    if (NULL != _trendWidget)
    {
        if (nibpParam.curStatusType() == NIBP_MONITOR_ERROR_STATE)
        {
            _trendWidget->setShowMeasureCount(false);
        }
        else
        {
            if (_isManualMeasure)
            {
                if (isAdditionalMeasure())
                {
                    _trendWidget->setShowMeasureCount(false);
                }
                else
                {
                    _trendWidget->setShowMeasureCount(true);
                }
            }
            else
            {
                if (isAdditionalMeasure() || isSTATClose() || isSTATOpenTemp())
                {
                    _trendWidget->setShowMeasureCount(false);
                }
                else
                {
                    //手动模式，或在在自动模式下的手动测量
                    if (getMeasurMode() == NIBP_MODE_MANUAL || isAutoMeasure())
                    {
                        if (_isManualMeasure)
                        {
                            _trendWidget->setShowMeasureCount(true);
                        }
                        else
                        {
                            _trendWidget->setShowMeasureCount(false);
                        }
                    }
                    else
                    {
                        _trendWidget->setShowMeasureCount(false);
                    }
                }
            }
        }
    }
}

/**************************************************************************************************
 * 设置最后测量时间。
 *************************************************************************************************/
void NIBPParam::setLastTime(void)
{
    _lastTime = timeManager.getCurTime();
}

/**************************************************************************************************
 * 获取测试结果的时间，清除显示。
 *************************************************************************************************/
void NIBPParam::clearResult(void)
{
    _sysValue = InvData();
    _diaValue = InvData();
    _mapVaule = InvData();
    _prVaule = InvData();

    //上次测量时间的显示与关闭,
    //因在自动模式的手动测量下，额外触发一次充气时，切换测量模式无法将手动的时间显示
    //（改为手动时，刷新手动模式显示，但刷新时仍处于额外测量状态），故在此刷新时间显示控制
    setShowMeasureCount();

    if (isSwitchType() && !isAutoMeasure())
    {
        transferResults(InvData(), InvData(), InvData(), 0);
    }
    else
    {
        if (curStatusType() == NIBP_MONITOR_ERROR_STATE)
        {
            transferResults(InvData(), InvData(), InvData(), 0);
        }
        else
        {
            transferResults(InvData(), InvData(), InvData(), _lastTime);
        }
    }
}

/**************************************************************************************************
 * 清除显示的数据。
 *************************************************************************************************/
void NIBPParam::invResultData(void)
{
    _sysValue = InvData();
    _diaValue = InvData();
    _mapVaule = InvData();
    _prVaule = InvData();

    transferResults(InvData(), InvData(), InvData(), 0);  // 显示“---”
    setCountdown(-1);  // 倒计时为“0”

    // 将进行启动测量， 清除界面。
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
    if (alarmSource)
    {
        alarmSource->clear();  // 清除所有报警。
    }
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool NIBPParam::isConnected(void)
{
    return _connectedProvider;
}

void NIBPParam::connectedFlag(bool flag)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_NIBP);
    if (flag)
    {
        if (!_isNIBPDisable)
        {
            handleNIBPEvent(NIBP_EVENT_CONNECTION_NORMAL, NULL, 0);
        }

        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_COMMUNICATION_STOP, false);
        }
        _connectedFlag = true;
    }
    else
    {
        if (systemManager.getCurWorkMode() != WORK_MODE_DEMO)
        {
            handleNIBPEvent(NIBP_EVENT_MODULE_ERROR, NULL, 0);
        }
        //通信中断，清除所有报警，只产生通信中断报警
        if (alarmSource)
        {
            alarmSource->clear();
            alarmSource->setOneShotAlarm(NIBP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
        }
        _connectedFlag = false;
        _isNIBPDisable = false;
    }
}

/***************************************************************************************************
 * check whether nibp is in measuring state
 **************************************************************************************************/
bool NIBPParam::isMeasuring() const
{
    if (!_activityMachine)
    {
        return false;
    }

    return curStatusType() == NIBP_MONITOR_MEASURE_STATE;
}

/**************************************************************************************************
 * 通信超时。
 *************************************************************************************************/
void NIBPParam::connectedTimeout()
{
}

/**************************************************************************************************
 * 设置测量结果的数据。
 *************************************************************************************************/
void NIBPParam::transferResults(int16_t sys, int16_t dia, int16_t map, unsigned time)
{
    if (NULL != _trendWidget)
    {
        if (sys == 0 || dia == 0 || map == 0)
        {
            sys = InvData();
            dia = InvData();
            map = InvData();
        }
        _trendWidget->setResults(sys, dia, map, time);
    }
}

/**************************************************************************************************
 * 设置袖带压力。
 *************************************************************************************************/
void NIBPParam::setCuffPressure(int pressure)
{
    if (NULL != _trendWidget)
    {
        _trendWidget->setCuffPressure(pressure);
    }
}

/**************************************************************************************************
 * 设置自动或者连续测量时的倒计时。
 *************************************************************************************************/
void NIBPParam::setCountdown(int t)
{
    if (NULL != _trendWidget)
    {
        _trendWidget->setCountdown(t);
    }
}

/**************************************************************************************************
 * NIBP状态信息显示。
 *************************************************************************************************/
void NIBPParam::setText(QString text)
{
    _text = text;
    setText();
}

/**************************************************************************************************
 * NIBP状态信息显示。
 *************************************************************************************************/
void NIBPParam::setText(void)
{
    if (NULL != _trendWidget)
    {
        if (nibpParam.curStatusType() == NIBP_MONITOR_ERROR_STATE)
        {
            if (_isNIBPDisable || !_connectedProvider)
            {
                _trendWidget->showText(trs("NIBPModule") + "\n" + trs("NIBPDisable"));
            }
            else
            {
                 _trendWidget->showText(trs("Neonate") + "\n" + trs("NIBPStop"));
            }
        }
        else
        {
            _trendWidget->showText(_text);
        }
    }
}

/**************************************************************************************************
 * NIBP STAT模式显示。
 *************************************************************************************************/
void NIBPParam::setModelText(QString text)
{
    if (NULL != _trendWidget)
    {
        _trendWidget->showModelText(text);
        if (text == trs("NIBPManual"))
        {
            _isManualMeasure = true;
        }
        else
        {
            _isManualMeasure = false;
        }
        setShowMeasureCount();
    }
}

/**************************************************************************************************
 * 获取SYS。
 *************************************************************************************************/
int16_t NIBPParam::getSYS(void)
{
    return _sysValue;
}

/**************************************************************************************************
 * 获取DIA。
 *************************************************************************************************/
int16_t NIBPParam::getDIA(void)
{
    return _diaValue;
}

/**************************************************************************************************
 * 获取MAP。
 *************************************************************************************************/
int16_t NIBPParam::getMAP(void)
{
    return _mapVaule;
}

/**************************************************************************************************
 * 获取MAP。
 *************************************************************************************************/
int16_t NIBPParam::getPR(void)
{
    return _prVaule;
}

/**************************************************************************************************
 * 获取测量完成标志。
 *************************************************************************************************/
NIBPMeasureResult NIBPParam::getMeasureResult(void)
{
    return _measureResult;
}

/**************************************************************************************************
 * 设置测量完成标志。
 *************************************************************************************************/
void NIBPParam::setMeasureResult(NIBPMeasureResult flag)
{
    _measureResult = flag;
}

void NIBPParam::recoverInitTrendWidgetData()
{
    int16_t sys(InvData());
    int16_t dia(InvData());
    int16_t map(InvData());
    unsigned time(0);

    _trendWidget->recoverResults(sys, dia, map, time);
    if ((sys == InvData()) || (dia == InvData()) || (map == InvData()))
    {
        nibpParam.setText(InvStr());
    }
    _trendWidget->setResults(sys, dia, map, time);
}

/**************************************************************************************************
 * 获取自动模式时间变化切换标志。
 *************************************************************************************************/
bool NIBPParam::isSwitchTime()
{
    return _SwitchFlagTime;
}

/**************************************************************************************************
 * 设置自动模式时间变化标志。
 *************************************************************************************************/
void NIBPParam::setSwitchFlagTime(bool flag)
{
    _SwitchFlagTime = flag;
}

/**************************************************************************************************
 * 获取病人类型修改标志。
 *************************************************************************************************/
bool NIBPParam::isSwitchType(void)
{
    return _SwitchFlagType;
}

/**************************************************************************************************
 * 设置病人类型修改标志。
 *************************************************************************************************/
void NIBPParam::setSwitchFlagType(bool flag)
{
    _SwitchFlagType = flag;
}

/**************************************************************************************************
 * 额外一次充气标志。
 *************************************************************************************************/
void NIBPParam::setAdditionalMeasure(bool flag)
{
    _additionalMeasureFlag = flag;
}

/**************************************************************************************************
 * 额外一次充气标志。
 *************************************************************************************************/
bool NIBPParam::isAdditionalMeasure()
{
    return _additionalMeasureFlag;
}

/**************************************************************************************************
 * STAT模式在5分钟时间未到的情况下被手动关闭的标志。
 *************************************************************************************************/
void NIBPParam::setSTATClose(bool flag)
{
    _statCloseFlag = flag;
}

/**************************************************************************************************
 * STAT模式在5分钟时间未到的情况下被手动关闭的标志。
 *************************************************************************************************/
bool NIBPParam::isSTATClose(void)
{
    return _statCloseFlag;
}

/**************************************************************************************************
 * 在安全间隔期内打开STAT标志
 * 当STAT关闭时，标志位取消
 *************************************************************************************************/
void NIBPParam::setSTATOpenTemp(bool flag)
{
    _statOpenTemp = flag;
}

/**************************************************************************************************
 * 在安全间隔期内打开STAT标志。
 *************************************************************************************************/
bool NIBPParam::isSTATOpenTemp(void)
{
    return _statOpenTemp;
}

/**************************************************************************************************
 * 以STAT启动测量的第一次启动标志位
 * 注：若测量中转STAT的不算第一次，之后的算第一次
 *************************************************************************************************/
void NIBPParam::setSTATFirst(bool flag)
{
    _statFirst = flag;
}

/**************************************************************************************************
 * 以STAT启动测量的第一次启动标志位
 *************************************************************************************************/
bool NIBPParam::isSTATFirst(void)
{
    return _statFirst;
}

/**************************************************************************************************
 * 自动测量模式中的手动触发。在转为手动和转为STAT时会置false
 *************************************************************************************************/
void NIBPParam::setAutoMeasure(bool flag)
{
    _autoMeasureFlag = flag;
}

/**************************************************************************************************
 * 自动测量模式中的手动触发。
 *************************************************************************************************/
bool NIBPParam::isAutoMeasure()
{
    return _autoMeasureFlag;
}

/**************************************************************************************************
 * 超限报警通知。
 *************************************************************************************************/
void NIBPParam::noticeLimitAlarm(int id, bool flag)
{
    if (NULL != _trendWidget)
    {
        _trendWidget->isAlarm(id, flag);
    }
}

/***************************************************************************************************
 * create a snapshot flag
 **************************************************************************************************/
void NIBPParam::setSnapshotFlag(bool flag)
{
    _isCreateSnapshotFlag = flag;
}

/***************************************************************************************************
 * return create a snapshot flag
 **************************************************************************************************/
bool NIBPParam::isSnapshotFlag()
{
    return _isCreateSnapshotFlag;
}

/***************************************************************************************************
 * create a snapshot
 **************************************************************************************************/
void NIBPParam::createSnapshot(NIBPOneShotType err)
{
    if (isSnapshotFlag())
    {
        if (err == NIBP_ONESHOT_ABORT)
        {
            setLastTime();
            nibpParam.setMeasureResult(NIBP_MEASURE_FAIL);
        }
        setSnapshotFlag(false);
        // summaryStorageManager.addNIBP(_lastTime, err);
    }
}

/**************************************************************************************************
 * 设置预充气值。
 *************************************************************************************************/
void NIBPParam::setInitPressure(int index)
{
    if (_provider != NULL)
    {
        _provider->setInitPressure(index);
    }
}

/**************************************************************************************************
 * 获取不同病人类型的初始压力值
 *************************************************************************************************/
int NIBPParam::getInitPressure()
{
    int initVal;
    currentConfig.getNumValue("NIBP|InitialCuffInflation", initVal);
    PatientType patienType = patientManager.getType();
    if (patienType == PATIENT_TYPE_ADULT)
    {
        initVal = 80 + initVal * 10;
    }
    else if (patienType == PATIENT_TYPE_PED)
    {
        initVal = 80 + initVal * 10;
    }
    else if (patienType == PATIENT_TYPE_NEO)
    {
        initVal = 60 + initVal * 10;
    }
    return initVal;
}

/**************************************************************************************************
 * 设置测量模式。
 * STAT模式不存配置列表
 *************************************************************************************************/
void NIBPParam::setMeasurMode(NIBPMode mode)
{
    unsigned char cmd;
    if (mode == NIBP_MODE_STAT)
    {
        nibpParam.setSTATFirst(true);
        nibpParam.setFirstAuto(true);
        cmd = 0x01;
        handleNIBPEvent(NIBP_EVENT_TRIGGER_MODEL, &cmd, 1);
    }
    else
    {
        systemConfig.setNumValue("PrimaryCfg|NIBP|MeasureMode", static_cast<int>(mode));
        cmd = 0x00;
        handleNIBPEvent(NIBP_EVENT_TRIGGER_MODEL, &cmd, 1);
    }
}

/**************************************************************************************************
 * 获取当前的测量模式。
 *************************************************************************************************/
NIBPMode NIBPParam::getMeasurMode(void)
{
    int mode = NIBP_MODE_MANUAL;
    systemConfig.getNumValue("PrimaryCfg|NIBP|MeasureMode", mode);
    if (_statModelFlag)
    {
        mode = NIBP_MODE_STAT;
    }
    return (NIBPMode)mode;
}

/**************************************************************************************************
 * 获取超级管理员配置中的测量模式。
 *************************************************************************************************/
NIBPMode NIBPParam::getSuperMeasurMode(void)
{
    int mode = NIBP_MODE_MANUAL;
    systemConfig.getNumValue("PrimaryCfg|NIBP|MeasureMode", mode);
    return (NIBPMode)mode;
}

/**************************************************************************************************
 * STAT打开关闭设置。
 * true:进入STAT模式
 * false:退出
 *************************************************************************************************/
void NIBPParam::setSTATMeasure(bool flag)
{
    if (flag)
    {
        setAutoMeasure(false);
        setAdditionalMeasure(false);
    }
    _statModelFlag = flag;
    emit statBtnState(flag);
}

/**************************************************************************************************
 * STAT打开关闭设置。
 *************************************************************************************************/
bool NIBPParam::isSTATMeasure()
{
    return _statModelFlag;
}

/**************************************************************************************************
 * 设置自动测量时的时间间隔。
 *************************************************************************************************/
void NIBPParam::setAutoInterval(NIBPAutoInterval interv)
{
    systemConfig.setNumValue("PrimaryCfg|NIBP|AutoInterval", static_cast<int>(interv));

    // 测量间隔修改，停止当前的测量，只在自动模式生效。
    if (getSuperMeasurMode() == NIBP_MODE_AUTO)
    {
        setSwitchFlagTime(true);
        unsigned char cmd = 0x00;
        handleNIBPEvent(NIBP_EVENT_TRIGGER_MODEL, &cmd, 1);
    }
}

/**************************************************************************************************
 * 获取自动测量时的时间间隔。
 *************************************************************************************************/
NIBPAutoInterval NIBPParam::getAutoInterval(void)
{
    int interv = NIBP_AUTO_INTERVAL_30;
    systemConfig.getNumValue("PrimaryCfg|NIBP|AutoInterval", interv);
    return (NIBPAutoInterval)interv;
}

/**************************************************************************************************
 * 获取自动测量时的时间间隔。
 *************************************************************************************************/
int NIBPParam::getAutoIntervalTime(void)
{
    int _timing = 0;
    switch (getAutoInterval())
    {
    case NIBP_AUTO_INTERVAL_2_5:
        _timing = 150;
//        _timing = 50;
        break;

    case NIBP_AUTO_INTERVAL_5:
        _timing = 5 * 60;
        break;

    case NIBP_AUTO_INTERVAL_10:
        _timing = 10 * 60;
        break;

    case NIBP_AUTO_INTERVAL_15:
        _timing = 15 * 60;
        break;

    case NIBP_AUTO_INTERVAL_20:
        _timing = 20 * 60;
        break;

    case NIBP_AUTO_INTERVAL_30:
        _timing = 30 * 60;
        break;

    case NIBP_AUTO_INTERVAL_45:
        _timing = 45 * 60;
        break;

    case NIBP_AUTO_INTERVAL_60:
        _timing = 60 * 60;
        break;

    case NIBP_AUTO_INTERVAL_90:
        _timing = 90 * 60;
        break;

    case NIBP_AUTO_INTERVAL_120:
        _timing = 120 * 60;
        break;

    default:
        break;
    };
    return _timing;
}

/**************************************************************************************************
 * 设置智能充气模式。
 *************************************************************************************************/
void NIBPParam::setIntelligentInflate(NIBPIntelligentInflate mode)
{
    if (NULL == _provider)
    {
        return;
    }
    int on_off = static_cast<int>(mode);
    systemConfig.setNumValue("PrimaryCfg|NIBP|IntelligentInflate", static_cast<int>(mode));
    if (on_off == 0)
    {
        _provider->enableSmartPressure(false);
    }
    else
    {
        _provider->enableSmartPressure(true);
    }
}

/**************************************************************************************************
 * 获取智能充气模式。
 *************************************************************************************************/
NIBPIntelligentInflate NIBPParam::getIntelligentInflate(void)
{
    int mode = NIBP_INTELLIGENT_INFLATE_OFF;
    systemConfig.getNumValue("PrimaryCfg|NIBP|IntelligentInflate", mode);
    return (NIBPIntelligentInflate)mode;
}

/**************************************************************************************************
 * 获取单位。
 *************************************************************************************************/
UnitType NIBPParam::getUnit(void)
{
    int unit = UNIT_MMHG;
    systemConfig.getNumValue("Unit|PressureUnit", unit);
    return (UnitType)unit;
}

void NIBPParam::setUnit(UnitType type)
{
    int unit = UNIT_MMHG;
    if (type == UNIT_KPA)
    {
        unit = UNIT_KPA;
    }
    systemConfig.setNumValue("Unit|PressureUnit", unit);
    if (_trendWidget)
    {
        _trendWidget->updateUnit(static_cast<UnitType>(unit));
        _trendWidget->updateLimit();
        _trendWidget->setResults(_sysValue, _diaValue, _mapVaule, _lastTime);
        _nibpDataTrendWidget->updateUnit(static_cast<UnitType>(unit));
    }
}

/**************************************************************************************************
 * 按钮释放。
 *************************************************************************************************/
void NIBPParam::keyReleased(void)
{
    _btnTimer->stop();
    //长按事件已触发，则不再触发短按事件
    if (!_toggleMeasureLongFlag)
    {
        toggleMeasureShort();
    }
}

/**************************************************************************************************
 * 按钮按下。
 *************************************************************************************************/
void NIBPParam::keyPressed(void)
{
    int index = NIBP_PR_DISPLAY_NR;
    systemConfig.getNumValue("PrimaryCfg|NIBP|StatFunction", index);
    // STAT功能关闭，按钮为按下触发，无长按功能
    if (index == NIBP_PR_DISPLAY_OFF)
    {
        _toggleMeasureLongFlag = true;
        toggleMeasureShort();
    }
    // STAT功能打开，按钮为弹起触发，有长按，短按功能
    else
    {
        _btnTimer->start(2 * 1000);
        _toggleMeasureLongFlag = false;
    }
}

/**************************************************************************************************
 * 超时。
 *************************************************************************************************/
void NIBPParam::_btnTimeOut()
{
    _btnTimer->stop();
    _toggleMeasureLongFlag = true;
    toggleMeasureLong();
}

void NIBPParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_NIBP || !systemManager.isSupport(CONFIG_NIBP))
    {
        return;
    }
    QPalette pal = colorManager.getPalette(paramInfo.getParamName(PARAM_NIBP));
    _trendWidget->updatePalette(pal);
    _nibpDataTrendWidget->updatePalette(pal);
}

/**************************************************************************************************
 * 短按NIBP按钮触发测量控制。
 *************************************************************************************************/
void NIBPParam::toggleMeasureShort(void)
{
    handleNIBPEvent(NIBP_EVENT_TRIGGER_BUTTON, NULL, 0);
}

/**************************************************************************************************
 * 长按NIBP按钮触发测量控制，进入STST
 *************************************************************************************************/
void NIBPParam::toggleMeasureLong(void)
{
    nibpParam.setMeasurMode(NIBP_MODE_STAT);
}

/**************************************************************************************************
 * STAT在安全间隔内被关闭,临时STAT状态也被关闭。
 *************************************************************************************************/
void NIBPParam::safeWaitTimeSTATStop()
{
    if (!nibpParam.isSTATOpenTemp())
    {
        nibpCountdownTime.STATMeasureStop();
        nibpParam.setSTATClose(true);
    }
    nibpParam.setSTATMeasure(false);
    nibpParam.setAutoStat(false);
    nibpParam.setText(trs("NIBPMEASURE") + "\n" + trs("NIBPSTOPPED"));
    nibpParam.setModelText(trs("STATSTOPPED"));
}


/**************************************************************************************************
 * 状态转为AUTO后，开始倒计时。
 *************************************************************************************************/
void NIBPParam::switchToAuto(void)
{
    int timing = nibpParam.getAutoIntervalTime();
    //判断是切换的模式还是修改了自动测量的时间
    if (nibpParam.isSwitchTime())
    {
        // 自动测量，获取延时的时间。
        nibpCountdownTime.autoMeasureChange(timing);
    }
    else
    {
        nibpCountdownTime.autoMeasureStart(timing);
    }

    nibpParam.setSwitchFlagTime(false);
    if (nibpParam.curStatusType() != NIBP_MONITOR_ERROR_STATE)
    {
        if (nibpParam.getMeasurMode() != NIBP_MODE_STAT)
        {
            if (nibpParam.isFirstAuto())
            {
                nibpParam.setModelText(trs("NIBPAUTO") + ":" +
                                       trs(NIBPSymbol::convert((NIBPAutoInterval)nibpParam.getAutoInterval())));
            }
            else
            {
                nibpParam.setModelText(trs("NIBPManualStart") + ":" +
                                       trs(NIBPSymbol::convert((NIBPAutoInterval)nibpParam.getAutoInterval())));
            }
        }
    }
}

/**************************************************************************************************
 * 状态转为手动后，倒计时时间停止，且不显示。
 *************************************************************************************************/
void NIBPParam::switchToManual(void)
{
    nibpCountdownTime.autoMeasureStop();
    nibpParam.setCountdown(-1);

    nibpParam.setAutoMeasure(false);
    nibpParam.setFirstAuto(true);
    if (nibpParam.curStatusType() != NIBP_MONITOR_ERROR_STATE)
    {
        if (nibpParam.getMeasurMode() != NIBP_MODE_STAT)
        {
            nibpParam.setModelText(trs("NIBPManual"));
        }
    }
}

void NIBPParam::setResult(bool result)
{
    _reply = true;
    _result = result;
}

bool NIBPParam::geReply()
{
    bool reply = _reply;
    if (reply)
    {
        _reply = false;
    }
    return reply;
}

bool NIBPParam::getResult()
{
    return _result;
}

void NIBPParam::setManometerPressure(int16_t value)
{
    _manometerPressure = value;
}

int16_t NIBPParam::getManometerPressure()
{
    return _manometerPressure;
}

void NIBPParam::updateSubParamLimit(SubParamID id)
{
    if (id == SUB_PARAM_NIBP_SYS)
    {
        _trendWidget->updateLimit();
    }
}

void NIBPParam::enterMaintain(bool enter)
{
    _isMaintain = enter;
}

bool NIBPParam::isMaintain()
{
    return _isMaintain;
}

void NIBPParam::clearTrendListData()
{
    if (systemManager.isSupport(PARAM_NIBP) && _nibpDataTrendWidget)
    {
        _nibpDataTrendWidget->clearListData();
        _nibpDataTrendWidget->update();
    }
}

void NIBPParam::setFirstAuto(bool flag)
{
    _firstAutoFlag = flag;
}

bool NIBPParam::isFirstAuto()
{
    return _firstAutoFlag;
}

void NIBPParam::setAutoStat(bool flag)
{
    _autoStatFlag = flag;
}

bool NIBPParam::isAutoStat()
{
    return _autoStatFlag;
}

void NIBPParam::setZeroSelfTestState(bool flag)
{
    _zeroSelfTestFlag = flag;
}

bool NIBPParam::isZeroSelfTestState()
{
    return _zeroSelfTestFlag;
}

void NIBPParam::setDisableState(bool flag)
{
    _isNIBPDisable = flag;
}

bool NIBPParam::getNeoDisState()
{
    return _isNeoDisable;
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void NIBPParam::stopMeasure(void)
{
    handleNIBPEvent(NIBP_EVENT_MONITOR_STOP, NULL, 0);
}

/**************************************************************************************************
 * 解析包。
 *************************************************************************************************/
void NIBPParam::handleNIBPEvent(NIBPEvent event, unsigned char args[], int argLen, unsigned /*nibpTimestamp*/)
{
    if (_activityMachine != NULL)
    {
        _activityMachine->handleNIBPEvent(event, args, argLen);
    }
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
void NIBPParam::machineRun(void)
{
    if (_activityMachine == NULL)
    {
        return;
    }

    _activityMachine->run();
    nibpCountdownTime.run();
}

/**************************************************************************************************
 * 当前状态机类型。
 *************************************************************************************************/
NIBPStateMachineType NIBPParam::curMachineType() const
{
    if (NULL != _activityMachine)
    {
        return _activityMachine->type();
    }

    return NIBP_STATE_MACHINE_NONE;
}

/**************************************************************************************************
 * 当前状态机对应的状态。
 *************************************************************************************************/
int NIBPParam::curStatusType() const
{
    if (NULL != _activityMachine)
    {
//        debug("%d\n", _activityMachine->curStatusType());
        return _activityMachine->curStatusType();
    }

    return -1;
}

void NIBPParam::switchState(unsigned char newStateID)
{
    _activityMachine->switchToState(newStateID);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPParam::NIBPParam()
    : Param(PARAM_NIBP), _provider(NULL),
      _trendWidget(NULL), _nibpDataTrendWidget(NULL),
      _sysValue(InvData()), _diaValue(InvData()),
      _mapVaule(InvData()), _prVaule(InvData()),
      _lastTime(0), _measureResult(NIBP_MEASURE_RESULT_NONE),
      _SwitchFlagTime(false), _SwitchFlagType(false),
      _additionalMeasureFlag(false), _autoMeasureFlag(false),
      _statModelFlag(false), _statCloseFlag(false),
      _statFirst(true), _toggleMeasureLongFlag(false),
      _statOpenTemp(false), _isCreateSnapshotFlag(false),
      _isNIBPDisable(false), _isManualMeasure(false),
      _connectedFlag(false), _connectedProvider(false),
      _text(InvStr()),
      _reply(false), _result(false), _manometerPressure(InvData()), _isMaintain(false), _firstAutoFlag(false),
      _autoStatFlag(false), _zeroSelfTestFlag(false), _isNeoDisable(false),
      _activityMachine(NULL), _oldState(0)
{
    nibpCountdownTime.getInstance();

    connect(&patientManager, SIGNAL(signalPatientType(PatientType)),
            this, SLOT(_patientTypeChangeSlot(PatientType)));

    _btnTimer = new QTimer();
    connect(_btnTimer, SIGNAL(timeout()), this, SLOT(_btnTimeOut()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPParam &NIBPParam::getInstance()
{
    static NIBPParam *instance = NULL;
    if (instance == NULL)
    {
        instance = new NIBPParam();
        NIBPParamInterface *old = registerNIBPParam(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

NIBPParam::~NIBPParam()
{
    if (NULL != _btnTimer)
    {
        delete _btnTimer;
        _btnTimer = NULL;
    }
}
