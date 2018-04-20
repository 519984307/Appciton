#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPMenu.h"
#include "IConfig.h"
#include "TimeDate.h"
#include "TN3Provider.h"
#include "NIBPTrendWidget.h"
#include "NIBPDataTrendWidget.h"
#include "NIBPErrorState.h"
#include "NIBPGetResultState.h"
#include "NIBPSafeWaitTimeState.h"
#include "NIBPStandbyState.h"
#include "NIBPState.h"
#include "NIBPStopState.h"
#include "NIBPMeasureState.h"
#include "NIBPStartingState.h"
#include "SummaryStorageManager.h"

NIBPParam *NIBPParam::_selfObj = NULL;

/**************************************************************************************************
 * 病人类型修改。
 *************************************************************************************************/
void NIBPParam::_patientTypeChangeSlot(PatientType /*type*/)
{
    setSwitchFlagType(true);

    // 模式修改则停止当前的测量。
    nibpEventTrigger.triggerpatienttype();

    //设置病人类型与预充气值
    if (NULL != _provider)
    {
        _provider->setPatientType((unsigned char)patientManager.getType());
        _provider->setInitPressure(nibpParam.getInitPressure(patientManager.getType()));
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

    _provider->service_Enter(false);

    //智能充气
    _provider->enableSmartPressure(true);

    //设置病人类型与预充气值
    _provider->setPatientType((unsigned char)patientManager.getType());
    _provider->setInitPressure(nibpParam.getInitPressure(patientManager.getType()));

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
    _isNIBPDisable = true;
    nibpOneShotAlarm.clear();
    nibpOneShotAlarm.setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_DISABLE, true);
    nibpEventTrigger.triggerError();
}

/**************************************************************************************************
 * 处理DEMO数据。
 *************************************************************************************************/
void NIBPParam::handDemoTrendData(void)
{
    static int cnt = 50;
    if (cnt++ < 50)
    {
        return;
    }
    cnt = 0;

    _sysValue = qrand() % 30 + 90;
    _diaValue = qrand() % 20 + 60;
    _mapVaule = qrand() % 25 + 75;
    _prVaule = qrand() % 10 + 60;

    setResult(_sysValue, _diaValue, _mapVaule, _prVaule, NIBP_ONESHOT_NONE);

    setMeasureResult(NIBP_MEASURE_SUCCESS);
}

/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short NIBPParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
        case SUB_PARAM_NIBP_SYS:
            return getSYS();

        case SUB_PARAM_NIBP_DIA:
            return getDIA();

        case SUB_PARAM_NIBP_MAP:
            return getMAP();

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
    NIBPState::setProvider(_provider);

    _provider->sendSelfTest();
}


/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void NIBPParam::reset(void)
{
    if (_provider == NULL)
    {
        return;
    }

    _isNIBPDisable = false;

    _provider->sendSelfTest();

    _provider->service_Enter(false);

    //智能充气
    _provider->enableSmartPressure(true);

    //设置病人类型与预充气值
    _provider->setPatientType((unsigned char)patientManager.getType());
    _provider->setInitPressure(nibpParam.getInitPressure(patientManager.getType()));

    nibpEventTrigger.triggerReset();
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

    short sys(InvData()), dia(InvData()), map(InvData());
    unsigned time(0);
    _trendWidget->recoverResults(sys, dia, map, time);
    _trendWidget->setResults(sys, dia, map, time);
    _sysValue = sys;
    _diaValue = dia;
    _mapVaule = map;
    _lastTime = time;

    nibpEventTrigger.triggermodel(false);  // 在加载界面时，加载测量模式，如果是自动模式，则开始倒计时。
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
 * 设置测量结果。
 *************************************************************************************************/
void NIBPParam::setResult(short sys, short dia, short map, short pr, NIBPOneShotType err)
{
    setLastTime();

    // 存在错误需要报错。
    if (err != NIBP_ONESHOT_NONE)
    {
        nibpParam.setText(trs("NIBPREADING") + "\n" + trs("NIBPFAILED"));
        nibpOneShotAlarm.setOneShotAlarm(err, true);
        if (nibpParam.getMeasurMode() == NIBP_MODE_MANUAL || nibpParam.getMeasurMode() == NIBP_MODE_AUTO)
        {
            if(!isAdditionalMeasure())
            {
                int index = NIBP_PR_DISPLAY_NR;
                superConfig.getNumValue("NIBP|AutomaticRetry", index);
                if (index == NIBP_PR_DISPLAY_ON)
                {
                    setAdditionalMeasure(true);
                }
            }
            else
            {
                setAdditionalMeasure(false);
            }
        }

        //额外测量需要隐藏上次测量时间
        clearResult();

        if(!isAdditionalMeasure())
        {
            //set complete even failed
            setMeasureResult(NIBP_MEASURE_FAIL);
        }

        //record nibp snapshot event following a additional measure
        createSnapshot(err);
        return;
    }

    setAdditionalMeasure(false);

    _sysValue = sys;
    _diaValue = dia;
    _mapVaule = map;
    _prVaule = pr;

    if (_nibpDataTrendWidget != NULL)
    {
        _nibpDataTrendWidget->collectNIBPTrendData(_lastTime);
    }

    setShowMeasureCount();
    transferResults(_sysValue, _diaValue, _mapVaule, _lastTime);

    // 保存起来。
    setMeasureResult(NIBP_MEASURE_SUCCESS);
    createSnapshot(err);
}


/**************************************************************************************************
 * NIBP上次测量时间的显示。
 *************************************************************************************************/
void NIBPParam::setShowMeasureCount(void)
{
    if (NULL != _trendWidget)
    {
        if (nibpParam.currentState() == NIBP_STATE_ERROR)
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
        if (nibpParam.currentState() == NIBP_STATE_ERROR)
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

    transferResults(InvData(), InvData(), InvData(), 0);//显示“---”
   setCountdown(-1);//倒计时为“0”

    // 将进行启动测量， 清除界面。
   nibpOneShotAlarm.clear();  // 清除所有报警。
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool NIBPParam::isConnected(void)
{
    Provider *provider = dynamic_cast<Provider*>(_provider);
    if (NULL == provider)
    {
        return false;
    }

    return provider->connected();
}

/***************************************************************************************************
 * check whether nibp is in measuring state
 **************************************************************************************************/
bool NIBPParam::isMeasuring() const
{
    if(!_currentState)
    {
        return false;
    }

    return _currentState->type() == NIBP_STATE_MEASURING;
}

/**************************************************************************************************
 * 通信超时。
 *************************************************************************************************/
void NIBPParam::connectedTimeout()
{
    //通信中断，清除所有报警，只产生通信中断报警
    nibpOneShotAlarm.clear();
    nibpOneShotAlarm.setOneShotAlarm(NIBP_ONESHOT_ALARM_MODULE_DISABLE, _isNIBPDisable);
//    if (isConnected())
//    {
//        nibpOneShotAlarm.setOneShotAlarm(NIBP_ONESHOT_ALARM_COMMUNICATION_TIMEOUT, true);
//    }
//    else
//    {
//        nibpOneShotAlarm.setOneShotAlarm(NIBP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
//    }
    nibpOneShotAlarm.setOneShotAlarm(NIBP_ONESHOT_ALARM_COMMUNICATION_STOP, true);
    nibpEventTrigger.triggerError();
}

/**************************************************************************************************
 * 设置测量结果的数据。
 *************************************************************************************************/
void NIBPParam::transferResults(short sys, short dia, short map, unsigned time)
{
    if (NULL != _trendWidget)
    {
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
        if (nibpParam.currentState() == NIBP_STATE_ERROR)
        {
            _trendWidget->showText(trs("NIBPModule") + "\n" + trs("NIBPDisable"));
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
short NIBPParam::getSYS(void)
{
    return _sysValue;
}

/**************************************************************************************************
 * 获取DIA。
 *************************************************************************************************/
short NIBPParam::getDIA(void)
{
    return _diaValue;
}

/**************************************************************************************************
 * 获取MAP。
 *************************************************************************************************/
short NIBPParam::getMAP(void)
{
    return _mapVaule;
}

/**************************************************************************************************
 * 获取MAP。
 *************************************************************************************************/
short NIBPParam::getPR(void)
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
        //summaryStorageManager.addNIBP(_lastTime, err);
    }
}

/**************************************************************************************************
 * 获取预充气值。
 *************************************************************************************************/
int NIBPParam::getInitPressure(PatientType type)
{
    QString path;
    if (type == PATIENT_TYPE_ADULT)
    {
        path = "NIBP|AdultInitialCuffInflation";
    }
    else if (type == PATIENT_TYPE_PED)
    {
        path = "NIBP|PedInitialCuffInflation";
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        path = "NIBP|NeoInitialCuffInflation";
    }
    int index = 0;
    superRunConfig.getNumValue(path, index);

    QString str;
    if (type == PATIENT_TYPE_ADULT)
    {
        str = NIBPSymbol::convert((NIBPAdultInitialCuff)index);
    }
    else if (type == PATIENT_TYPE_PED)
    {
        str = NIBPSymbol::convert((NIBPPrediatrictInitialCuff)index);
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        str = NIBPSymbol::convert((NIBPNeonatalInitialCuff)index);
    }

    return str.toInt();
}

/**************************************************************************************************
 * 设置预充气值。
 *************************************************************************************************/
void NIBPParam::setInitPressure(int index)
{
    PatientType type = patientManager.getType();
    QString path;
    if (type == PATIENT_TYPE_ADULT)
    {
        path = "NIBP|AdultInitialCuffInflation";
    }
    else if (type == PATIENT_TYPE_PED)
    {
        path = "NIBP|PedInitialCuffInflation";
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        path = "NIBP|NeoInitialCuffInflation";
    }

    superRunConfig.setNumValue(path, index);

    if (_provider != NULL)
    {
        _provider->setInitPressure(nibpParam.getInitPressure(type));
    }
}

/**************************************************************************************************
 * 设置测量模式。
 * STAT模式不存配置列表
 *************************************************************************************************/
void NIBPParam::setMeasurMode(NIBPMode mode)
{
    if (mode == NIBP_MODE_STAT)
    {
        nibpParam.setSTATFirst(true);
        nibpEventTrigger.triggermodel(true);
    }
    else
    {
        superRunConfig.setNumValue("NIBP|MeasureMode", (int)mode);
        nibpEventTrigger.triggermodel(false);  // 模式修改则停止当前的测量。
    }
}

/**************************************************************************************************
 * 获取当前的测量模式。
 *************************************************************************************************/
NIBPMode NIBPParam::getMeasurMode(void)
{
    int mode = NIBP_MODE_MANUAL;
    superRunConfig.getNumValue("NIBP|MeasureMode", mode);
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
    superRunConfig.getNumValue("NIBP|MeasureMode", mode);
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
    nibpMenu.statBtnShow();
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
    superRunConfig.setNumValue("NIBP|AutoInterval", (int)interv);

    // 测量间隔修改，停止当前的测量，只在自动模式生效。
    if (getSuperMeasurMode() == NIBP_MODE_AUTO)
    {
        setSwitchFlagTime(true);
        nibpEventTrigger.triggermodel(false);
    }
}

/**************************************************************************************************
 * 获取自动测量时的时间间隔。
 *************************************************************************************************/
NIBPAutoInterval NIBPParam::getAutoInterval(void)
{
    int interv = NIBP_AUTO_INTERVAL_30;
    superRunConfig.getNumValue("NIBP|AutoInterval", interv);
    return (NIBPAutoInterval)interv;
}

/**************************************************************************************************
 * 获取自动测量时的时间间隔。
 *************************************************************************************************/
int NIBPParam::getAutoIntervalTime(void)
{
    int _timing = 0;
    switch(getAutoInterval())
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
    int on_off = (int)mode;
    systemConfig.setNumValue("PrimaryCfg|NIBP|IntelligentInflate", (int)mode);
    if(on_off == 0)
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
    superConfig.getNumValue("Local|NIBPUnit", unit);
    return (UnitType)unit;
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
    superRunConfig.getNumValue("NIBP|StatFunction", index);
    //STAT功能关闭，按钮为按下触发，无长按功能
    if (index == NIBP_PR_DISPLAY_OFF)
    {
        _toggleMeasureLongFlag = true;
        toggleMeasureShort();
    }
    //STAT功能打开，按钮为弹起触发，有长按，短按功能
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

/**************************************************************************************************
 * 短按NIBP按钮触发测量控制。
 *************************************************************************************************/
void NIBPParam::toggleMeasureShort(void)
{
    nibpEventTrigger.triggerbutton();
}

/**************************************************************************************************
 * 长按NIBP按钮触发测量控制，进入STST
 *************************************************************************************************/
void NIBPParam::toggleMeasureLong(void)
{
    nibpParam.setMeasurMode(NIBP_MODE_STAT);
}

/**************************************************************************************************
 * 停止测量。
 *************************************************************************************************/
void NIBPParam::stopMeasure(void)
{
    nibpEventTrigger.stop();
}

/**************************************************************************************************
 * 解析包。
 *************************************************************************************************/
void NIBPParam::unPacket(unsigned char *packet, int len)
{
    if (_currentState == NULL)
    {
        return;
    }

    _currentState->unPacket(packet, len);
}

/**************************************************************************************************
 * 状态机主运行。
 *************************************************************************************************/
void NIBPParam::machineRun(void)
{
    if (_currentState == NULL)
    {
        return;
    }

    _currentState->run();
    nibpCountdownTime.run();
}

/**************************************************************************************************
 * 切换状态机的当前状态。
 * 参数：
 *      state：即将切换的状态。
 *************************************************************************************************/
void NIBPParam::switchState(NIBPStateType state)
{
    StateMap::iterator it = _states.find(state);
    if (it == _states.end())
    {
        return;
    }

    // 先退出之前的状态，载进入新状态。
    _currentState = it.value();
    _currentState->enter();
}

NIBPStateType NIBPParam::currentState()
{
    if (NULL != _currentState)
    {
        return _currentState->type();
    }
    else
    {
        return NIBP_STATE_NR;
    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPParam::NIBPParam() : Param(PARAM_NIBP)
{
    _provider = NULL;
    _trendWidget = NULL;

    _sysValue = InvData();
    _diaValue = InvData();
    _mapVaule = InvData();
    _prVaule = InvData();
    _lastTime = 0;

    _measureResult = NIBP_MEASURE_RESULT_NONE;
    _SwitchFlagTime = false;
    _SwitchFlagType = false;
    _additionalMeasureFlag = false;
    _autoMeasureFlag = false;
    _statFirst = true;
    _statModelFlag = false;
    _statCloseFlag = false;
    _toggleMeasureLongFlag = false;
    _statOpenTemp = false;
    _isCreateSnapshotFlag = false;
    _isNIBPDisable = false;
    _isManualMeasure = false;
    _text = InvStr();

    // 构造状态对象。
    NIBPState *state;

    state = new NIBPStandbyState();
    _currentState = state;
    _states.insert(state->type(), state);

    state = new NIBPGetResultState();
    _states.insert(state->type(), state);

    state = new NIBPStopState();
    _states.insert(state->type(), state);

    state = new NIBPMeasureState();
    _states.insert(state->type(), state);

    state = new NIBPStartingState();
    _states.insert(state->type(), state);

    state = new NIBPSafeWaitTime();
    _states.insert(state->type(), state);

    state = new NIBPErrorState();
    _states.insert(state->type(), state);

    nibpEventTrigger.construction();
    nibpCountdownTime.construction();

    connect(&patientManager, SIGNAL(signalPatientType(PatientType)),
            this, SLOT(_patientTypeChangeSlot(PatientType)));

    _btnTimer = new QTimer();
    connect(_btnTimer, SIGNAL(timeout()), this, SLOT(_btnTimeOut()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPParam::~NIBPParam()
{
    // 删除状态机。
    StateMap::iterator it = _states.begin();
    for (; it != _states.end(); ++it)
    {
        delete it.value();
    }

    if (NULL != _btnTimer)
    {
        delete _btnTimer;
        _btnTimer = NULL;
    }

    deleteNIBPEventTrigger();
    deleteNIBPCountdownTime();
}
