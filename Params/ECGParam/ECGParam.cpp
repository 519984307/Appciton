/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/8/15
 **/

#include "ECGParam.h"
#include "ECGDupParam.h"
#include "BaseDefine.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "ECGWaveWidget.h"
#include "ECGPVCSTrendWidget.h"
#include "ECGSTTrendWidget.h"
#include "Debug.h"
#include "LayoutManager.h"
#include "ECGProviderIFace.h"
#include "WaveformCache.h"
#include "SystemManager.h"
#include "SoftKeyManager.h"
#include "MonitorSoftkeyAction.h"
#include "SPO2Param.h"
#include "Provider.h"
#include "SystemStatusBarWidget.h"
#include "ECGAlarm.h"
#include "QApplication"
#include "TimeManager.h"
#include <QTimer>
#include "Debug.h"
#include <QTimer>
#include "Framework/ErrorLog/ErrorLogItem.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "OxyCRGRRHRWaveWidget.h"
#include "AlarmSourceManager.h"
#include "RunningStatusBarInterface.h"

#define ECG_TIMER_INTERVAL (100)
#define GET_DIA_DATA_PERIOD (12000)
#define DISABLE_DIA_SOFTKEY_PERIOD (900)  // 1s,定时有差异，使用900ms

/**************************************************************************************************
 * 获取禁用的波形控件。
 *************************************************************************************************/
QStringList ECGParam::_getDisabledWaveforms()
{
    QStringList waveforms;

    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[(ECGLead)i] == NULL)
        {
            return waveforms;
        }
    }

    if (_curLeadMode == ECG_LEAD_MODE_3)
    {
        ECGLead caclLead = getCalcLead();
        for (int i = ECG_LEAD_I; i <= ECG_LEAD_III; ++i)
        {
            if (i != caclLead)
            {
                waveforms.append(_waveWidget[i]->name());
            }
        }

        waveforms.append(_waveWidget[ECG_LEAD_AVR]->name());
        waveforms.append(_waveWidget[ECG_LEAD_AVL]->name());
        waveforms.append(_waveWidget[ECG_LEAD_AVF]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V1]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V2]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V3]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V4]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V5]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V6]->name());
    }
    else if (_curLeadMode == ECG_LEAD_MODE_5)
    {
        waveforms.append(_waveWidget[ECG_LEAD_V2]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V3]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V4]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V5]->name());
        waveforms.append(_waveWidget[ECG_LEAD_V6]->name());
    }
    return waveforms;
}

short ECGParam::getMaxHrValue()
{
    return 300;
}

short ECGParam::getMinHrValue()
{
    return 20;
}

void ECGParam::adjustPrintWave(ECGLead preECGLead, ECGLead curECGLead)
{
    QStringList printNodeList = systemConfig.getChildNodeNameList("Print");
    QStringList printWaveList = printNodeList.filter("SelectWave");

    for (int i = 0; i < printWaveList.count(); i++)
    {
        int waveID = 0;
        systemConfig.getNumValue(QString("Print|%1").arg(printWaveList.at(i)), waveID);

        if (static_cast<WaveformID>(waveID) == leadToWaveID(preECGLead))
        {
            systemConfig.setNumValue(QString("Print|%1").arg(printWaveList.at(i))
                                     , static_cast<int>(leadToWaveID(curECGLead)));
            break;
        }
    }
}

/**************************************************************************************************
 * 初始化参数。
 *************************************************************************************************/
void ECGParam::initParam(void)
{
    soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT,
                           (SoundManager::VolumeLevel)getQRSToneVolume());

    if (_provider == NULL)
    {
        return;
    }
}

/**************************************************************************************************
 * 处理DEMO时的波形数据。
 *************************************************************************************************/
void ECGParam::handDemoWaveform(WaveformID id, short data)
{
    if (NULL != _waveWidget[id])
    {
        _waveWidget[id]->addWaveformData(data);
    }
    waveformCache.addData((WaveformID)id, data);
}

/**************************************************************************************************
 * 产生DEMO数据。
 *************************************************************************************************/
void ECGParam::handDemoTrendData(void)
{
    int hrValue = 60;
    ecgDupParam.updateHR(hrValue);

    soundManager.heartBeatTone();
    ecgDupParam.updateHRBeatIcon();

    if (oxyCRGRrHrTrend)
    {
        oxyCRGRrHrTrend->addHrTrendData(hrValue);
    }
    int pvcs = 0;
    updatePVCS(pvcs);

//    int st;
    for (int i = ECG_ST_I; i < ECG_ST_NR; i++)
    {
        int st = qrand() % 10 - 5;
        updateST((ECGST)i, st);
    }
}

void ECGParam::exitDemo()
{
    ecgParam.updateHR(InvData());
    ecgDupParam.updateHR(InvData());
    updatePVCS(InvData());

    int filter;
    currentConfig.getNumValue("ECG|FilterMode", filter);
    _filterMode = static_cast<ECGFilterMode>(filter);
    emit updateFilterMode();

    int notchFilter;
    currentConfig.getNumValue("ECG|NotchFilter", notchFilter);
    _notchFilter = static_cast<ECGNotchFilter>(notchFilter);
    emit updateNotchFilter();

    for (int i = ECG_ST_I; i < ECG_ST_NR; i++)
    {
        updateST((ECGST)i, InvData());
    }
}


/**************************************************************************************************
 * 功能： 获取子参数值。
 *************************************************************************************************/
short ECGParam::getSubParamValue(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_HR_PR:
        return getHR();

    case SUB_PARAM_ECG_PVCS:
        return getPVCS();

    case SUB_PARAM_ST_I:
        return getST(ECG_ST_I);

    case SUB_PARAM_ST_II:
        return getST(ECG_ST_II);

    case SUB_PARAM_ST_III:
        return getST(ECG_ST_III);

    case SUB_PARAM_ST_aVR:
        return getST(ECG_ST_aVR);

    case SUB_PARAM_ST_aVL:
        return getST(ECG_ST_aVL);

    case SUB_PARAM_ST_aVF:
        return getST(ECG_ST_aVF);

    case SUB_PARAM_ST_V1:
        return getST(ECG_ST_V1);

    case SUB_PARAM_ST_V2:
        return getST(ECG_ST_V2);

    case SUB_PARAM_ST_V3:
        return getST(ECG_ST_V3);

    case SUB_PARAM_ST_V4:
        return getST(ECG_ST_V4);

    case SUB_PARAM_ST_V5:
        return getST(ECG_ST_V5);

    case SUB_PARAM_ST_V6:
        return getST(ECG_ST_V6);

    default:
        return InvData();
    }
}

/**************************************************************************************************
 * 功能： 显示子参数值。
 *************************************************************************************************/
void ECGParam::showSubParamValue()
{
    if (_pvcsTrendWidget != NULL)
    {
        _pvcsTrendWidget->showValue();
    }
    if (_ecgSTTrendWidget != NULL)
    {
        _ecgSTTrendWidget->showValue();
    }
}

/**************************************************************************************************
 * 获取当前的单位。
 *************************************************************************************************/
UnitType ECGParam::getCurrentUnit(SubParamID id)
{
    switch (id)
    {
    case SUB_PARAM_HR_PR:
    case SUB_PARAM_ECG_PVCS:
        return UNIT_BPM;

    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
        return UNIT_MV;

    default:
        return UNIT_BPM;
    }
}

/**************************************************************************************************
 * 设置数据提供对象。
 *************************************************************************************************/
void ECGParam::setProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }

    _provider = provider;

    // 初始化相关参数。
    // 导联模式。
    _provider->getLeadCabelType();

    // 计算导联。
    _provider->setCalcLead(getCalcLead());

    // 设置病人类型
    _provider->setPatientType(getPatientType());

    // raw data
    _provider->enableRawData(getRawDataOnOff());

    // 设置带宽。
//    _provider->setBandwidth((ECGBandwidth)getBandwidth());

    // set fitler mode
    _provider->setFilterMode((ECGFilterMode)getFilterMode());

    // 设置起搏检测。
    _provider->enablePacermaker(getPacermaker());

    // 设置工频滤波。
    _provider->setNotchFilter(getNotchFilter());

    // 发送获取版本
    _provider->sendVersion();

    // enable data sync
    _provider->enableDataSyncCtrl(true);

    int p05 = _provider->getP05mV();
    int n05 = _provider->getN05mV();

    // 设置波形控件的采样率。
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setDataRate(_provider->getWaveformSample());
        _waveWidget[i]->set05MV(p05, n05);
        QString tile = _waveWidget[i]->getTitle();
        // 注册波形缓存。
        waveformCache.registerSource((WaveformID)i, _provider->getWaveformSample(), n05, p05,
                                     tile, _provider->getBaseLine());

        if (_waveWidget[i]->isVisible())
        {
            _waveWidget[i]->updateWaveConfig();
        }
    }

    // 申请完波形缓存后重新设置波形速率，因为进入监控页面波形是以250移动
    _provider->setWaveformSample(250);
    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setDataRate(_provider->getWaveformSample());
    }

    _provider->getSelfTestStatus();
//    <Gain>1</Gain>
    // todo：其他设置。
}

/**************************************************************************************************
 * 设置服务模式升级数据提供对象。
 *************************************************************************************************/
void ECGParam::setServiceProvider(ECGProviderIFace *provider)
{
    if (provider == NULL)
    {
        return;
    }
    _provider = provider;
}


/**************************************************************************************************
 * 设置PVCS界面对象。
 *************************************************************************************************/
void ECGParam::setECGPVCSTrendWidget(ECGPVCSTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _pvcsTrendWidget = trendWidget;
}

/**************************************************************************************************
 * 设置ST界面对象。
 *************************************************************************************************/
void ECGParam::setECGSTTrendWidget(ECGSTTrendWidget *trendWidget)
{
    if (trendWidget == NULL)
    {
        return;
    }
    _ecgSTTrendWidget = trendWidget;
}

/**************************************************************************************************
 * 设置波形对象。
 *************************************************************************************************/
void ECGParam::setWaveWidget(ECGWaveWidget *waveWidget, ECGLead whichLead)
{
    if (NULL != waveWidget)
    {
        _waveWidget[whichLead] = waveWidget;
        _waveWidget[whichLead]->setSpeed(getSweepSpeed());
    }
}

void ECGParam::setOxyCRGHrWaveWidget(OxyCRGRRHRWaveWidget *waveWidget)
{
    if (waveWidget == NULL)
    {
        return;
    }
    oxyCRGRrHrTrend = waveWidget;
}

/**************************************************************************************************
 * 更新波形数据。
 * 参数：
 *      waveform: 波形数据，包括所有导联导波形；
 *      leadoff:导联脱落
 *      ipaceMark：内部起搏器标记；
 *      epaceMark：外部起搏标记；
 *      rMark： R波标记。
 *************************************************************************************************/
void ECGParam::updateWaveform(int waveform[], bool *leadoff, bool ipaceMark, bool /*epaceMark*/, bool rMark)
{
    int displaymode = ecgParam.getDisplayMode();
    int rate = _provider->getWaveformSample();
    UserFaceType faceType = layoutManager.getUFaceType();

    _waveDataInvalid = !_waveDataInvalid;

    // 心跳音
    if (rMark
            && _hrValue != InvData()
            && (ecgDupParam.getCurHRSource() == HR_SOURCE_ECG
                || ecgDupParam.getCurHRSource() == HR_SOURCE_AUTO))
    {
        soundManager.heartBeatTone();
        ecgDupParam.updateHRBeatIcon();
    }

    int flag = 0;
    if (ipaceMark)
    {
        flag |= ECG_INTERNAL_FLAG_BIT;
    }

    int tmpFalg = flag;
    int bandwidth = (ecgParam.getDisplayBandWidth() << 4);
    for (int i = ECG_LEAD_I; i <= ECG_LEAD_V6; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }

        flag = tmpFalg;

        // 12导模式下需要内部起搏标记(数据无效时不标记)
        if ((faceType == UFACE_MONITOR_ECG_FULLSCREEN) && ipaceMark && (!leadoff[i]))
        {
            flag |= ECG_INTERNAL_FLAG_BIT;
        }

        // 无效数据位
        if (leadoff[i])
        {
            flag |= INVALID_WAVE_FALG_BIT;
            flag &= ~ECG_INTERNAL_FLAG_BIT;
//            flag &= ~ECG_RWAVE_MARK_FLAG_BIT;
        }
        else
        {
            flag &= ~INVALID_WAVE_FALG_BIT;
        }

        // can't display wave set invalid flag
        switch (ecgParam.getLeadMode())
        {
        case ECG_LEAD_MODE_3:
            if (i > ECG_LEAD_III && i <= ECG_LEAD_V6)
            {
                flag |= INVALID_WAVE_FALG_BIT;
            }
            break;

        case ECG_LEAD_MODE_5:
            if (i > ECG_LEAD_V1 && i <= ECG_LEAD_V6)
            {
                flag |= INVALID_WAVE_FALG_BIT;
            }
            break;

        default:
            break;
        }

#if 1
        // 诊断模式和12L模式的全部导联
        if ((faceType == UFACE_MONITOR_ECG_FULLSCREEN) || (WAVE_SAMPLE_RATE_500 == rate && i != ECG_LEAD_V6 + 1))
        {
            flag &= ~ECG_WAVE_RATE_250_BIT;
        }
        else
        {
            flag |= ECG_WAVE_RATE_250_BIT;
        }
#else
        if (WAVE_SAMPLE_RATE_250 == rate)
        {
            flag |= ECG_WAVE_RATE_250_BIT;
        }
        else
        {
            flag &= ~ECG_WAVE_RATE_250_BIT;
        }
#endif

//        if (flag & (ECG_EXTERNAL_DOT_FLAG_BIT | ECG_EXTERNAL_SOLD_FLAG_BIT))
        if (ECG_EXTERNAL_SOLD_FLAG_BIT)
        {
//            flag &= ~ECG_EXTERNAL_DOT_FLAG_BIT;
            flag &= ~ECG_EXTERNAL_SOLD_FLAG_BIT;
        }
        // 当前带宽
        flag |= bandwidth;

        static int flagUnsaved[ECG_LEAD_NR] = {0};
        // 在普通模式中，在监控模式下所有的波形数据要扔掉一半，以250显示
        if (ECG_DISPLAY_NORMAL == displaymode)
        {
            if (_waveDataInvalid)
            {
                flagUnsaved[i] = flag;
                continue;
            }
        }

        int norfalg = flag | flagUnsaved[i];
//        if (norfalg & (ECG_EXTERNAL_DOT_FLAG_BIT | ECG_EXTERNAL_SOLD_FLAG_BIT))
        if (norfalg & ECG_EXTERNAL_SOLD_FLAG_BIT)
        {
//            norfalg &= ~ECG_EXTERNAL_DOT_FLAG_BIT;
            norfalg &= ~ECG_EXTERNAL_SOLD_FLAG_BIT;
        }

        if (!(i == WAVE_ECG_aVR && faceType == UFACE_MONITOR_ECG_FULLSCREEN &&
                ecgParam.get12LDisplayFormat() == DISPLAY_12LEAD_CABRELA))
        {
            _waveWidget[i]->addWaveformData(waveform[i], norfalg & 0xFFFF);
        }
        else
        {
            _waveWidget[i]->addWaveformData(-waveform[i], norfalg & 0xFFFF);
        }

        waveformCache.addData((WaveformID)i, ((norfalg & 0xFFFF) << 16) | (waveform[i] & 0xFFFF));

        flagUnsaved[i] = 0;
    }
}

/**************************************************************************************************
 * 更新HR数值。
 *************************************************************************************************/
void ECGParam::updateHR(short hr)
{
    ecgDupParam.restartParamUpdateTime();
    if (_hrValue == hr)
    {
        return;
    }

    if (hr < 0)
    {
        hr = InvData();
    }

    if (hr > getMaxHrValue())
    {
        hr = getMaxHrValue();
    }
    else if (hr < getMinHrValue() && hr != InvData())
    {
        hr = 0;
    }

    _hrValue = hr;
    ecgDupParam.updateHR(hr);

    if (oxyCRGRrHrTrend)
    {
        oxyCRGRrHrTrend->addHrTrendData(hr);
    }
}

/***************************************************************************************************
 * get hr
 **************************************************************************************************/
short ECGParam::getHR() const
{
    return _hrValue;
}

/**************************************************************************************************
 * 更新PVCS数值。
 *************************************************************************************************/
void ECGParam::updatePVCS(short pvcs)
{
    _pvcsValue = pvcs;
    if (_pvcsTrendWidget != NULL)
    {
        _pvcsTrendWidget->setPVCSValue(_pvcsValue);
    }
}

/***************************************************************************************************
 * get PVCS
 **************************************************************************************************/
short ECGParam::getPVCS() const
{
    return _pvcsValue;
}

/**************************************************************************************************
 * 更新ST数值。
 *************************************************************************************************/
void ECGParam::updateST(ECGST lead, short st)
{
    _stValue[lead] = st;
    if (_ecgSTTrendWidget != NULL)
    {
        _ecgSTTrendWidget->setSTValue(lead, st);
    }
}

/***************************************************************************************************
 * get ST
 **************************************************************************************************/
short ECGParam::getST(ECGST lead) const
{
    return _stValue[lead];
}

/**************************************************************************************************
 * 更新VFVT数值。
 *************************************************************************************************/
void ECGParam::updateVFVT(bool onoff)
{
    ecgDupParam.updateVFVT(onoff);
}

/**************************************************************************************************
 * 更新导联脱落状态。
 *************************************************************************************************/
void ECGParam::setLeadOff(ECGLead lead, bool status)
{
    if (_leadOff[lead] != status)
    {
        _leadOff[lead] = status;
        updateECGNotifyMesg(lead, false);
    }

    if (!_isEverLeadOn[lead] && !status)
    {
        QString everLeadOnStr = "ECG|EverLeadOn";
        int leadOnoff = 0;
        currentConfig.getNumValue(everLeadOnStr, leadOnoff);
        switch (_curLeadMode)
        {
        case ECG_LEAD_MODE_3:
            if (lead > ECG_LEAD_III)
            {
                return;
            }
            break;

        case ECG_LEAD_MODE_5:
            if (lead > ECG_LEAD_V1)
            {
                return;
            }
            break;

        default:
            if (lead > ECG_LEAD_V6)
            {
                return;
            }
            break;
        }

        _isEverLeadOn[lead] = true;
        leadOnoff  = leadOnoff | (true >> lead);
        currentConfig.setNumValue(everLeadOnStr, leadOnoff);
    }
}

/**************************************************************************************************
 * 设置是否过载
 *************************************************************************************************/
void ECGParam::setOverLoad(bool flag)
{
    static bool overLoadFlag = false;

    if (overLoadFlag != flag)
    {
        AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
        if (alarmSource)
        {
            alarmSource->setOneShotAlarm(ECG_ONESHOT_ALARM_OVERLOAD, flag);
        }
        overLoadFlag = flag;
    }
}

/**************************************************************************************************
 * update ecg notify message
 *************************************************************************************************/
void ECGParam::updateECGNotifyMesg(ECGLead lead, bool isAlarm)
{
    if (lead >= ECG_LEAD_NR)
    {
        return;
    }

    ECGWaveNotify mesg = ECG_WAVE_NOTIFY_NORMAL;

    if (isConnected())
    {
        if (1 == _leadOff[lead] && _isFristConnect)
        {
            mesg = ECG_WAVE_NOTIFY_LEAD_OFF;
        }

        if (isAlarm)
        {
            if (lead == _calcLead)
            {
                mesg = ECG_WAVE_NOTIFY_CHECK_PATIENT;
            }
        }
    }

    if (_waveWidget[lead])
    {
        _waveWidget[lead]->setNotifyMesg(mesg);
    }
}

void ECGParam::setConnected(bool isConnected)
{
    if (_connectedProvider == isConnected)
    {
        return;
    }

    _connectedProvider = isConnected;
}

/**************************************************************************************************
 * 获取导联脱落状态。
 *************************************************************************************************/
char ECGParam::doesLeadOff(int lead)
{
    if (lead >= ECG_LEAD_NR)
    {
        return 2;
    }

    // never receive ecg module lead status
    if (2 == _leadOff[lead])
    {
        return _leadOff[lead];
    }

    return _leadOff[lead] && isConnected();
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool ECGParam::isConnected()
{
    return _connectedProvider;
}

/**************************************************************************************************
 * 进入12导模式。
 *************************************************************************************************/
void ECGParam::enter12Lead(void)
{
    setDisplayMode(ECG_DISPLAY_12_LEAD_FULL);
}

/**************************************************************************************************
 * 退出12导模式。
 *************************************************************************************************/
void ECGParam::exit12Lead(void)
{
    setDisplayMode(ECG_DISPLAY_NORMAL);
}

/**************************************************************************************************
 * 设置OneShot报警。
 *************************************************************************************************/
void ECGParam::setOneShotAlarm(ECGOneShotType t, bool f)
{
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_ECG);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(t, f);
    }
}

/**************************************************************************************************
 * 超限报警通知。
 *************************************************************************************************/
void ECGParam::noticeLimitAlarm(int id, bool isAlarm)
{
    switch (id)
    {
    case SUB_PARAM_ECG_PVCS:
        if (_pvcsTrendWidget != NULL)
        {
            _pvcsTrendWidget->isAlarm(isAlarm);
        }
        break;

    case SUB_PARAM_ST_I:
    case SUB_PARAM_ST_II:
    case SUB_PARAM_ST_III:
    case SUB_PARAM_ST_aVR:
    case SUB_PARAM_ST_aVL:
    case SUB_PARAM_ST_aVF:
    case SUB_PARAM_ST_V1:
    case SUB_PARAM_ST_V2:
    case SUB_PARAM_ST_V3:
    case SUB_PARAM_ST_V4:
    case SUB_PARAM_ST_V5:
    case SUB_PARAM_ST_V6:
        if (_ecgSTTrendWidget != NULL)
        {
            _ecgSTTrendWidget->isAlarm((id - SUB_PARAM_ST_I), isAlarm);
        }
        break;

    default:
        break;
    }
}

/**************************************************************************************************
 * 获取支持的工作模式。
 *************************************************************************************************/
ECGLeadMode ECGParam::getAvailableMode(void)
{
    int value = 0;
    machineConfig.getModuleInitialStatus("ECG12LEADEnable", reinterpret_cast<bool*>(&value));
    if (0 == value)
    {
        return ECG_LEAD_MODE_5;
    }
    else
    {
        return ECG_LEAD_MODE_12;
    }
}

/**************************************************************************************************
 * 获取Topwaveform的名称。
 *************************************************************************************************/
const QString &ECGParam::getCalcLeadWaveformName(void)
{
    ECGLead lead = getCalcLead();
    if (_waveWidget[lead] != NULL)
    {
        return _waveWidget[lead]->name();
    }
    else
    {
        QString str;
        return str = "";
    }
}

/**************************************************************************************************
 * 获取导联对应的波形窗体的名称。
 *************************************************************************************************/
QString ECGParam::getWaveWidgetName(ECGLead lead)
{
    QString name;
    if (_waveWidget[lead] != NULL)
    {
        name = _waveWidget[lead]->name();
    }
    return name;
}

void ECGParam::updateECGStandard(int standard)
{
    systemConfig.setNumValue("Others|ECGStandard", standard);
    if (_ecgStandard != static_cast<ECGLeadNameConvention>(standard))
    {
        _ecgStandard = static_cast<ECGLeadNameConvention>(standard);
        for (int i = 0; i < ECG_LEAD_NR; i++)
        {
            QString name = ECGSymbol::convert((ECGLead)i, _ecgStandard);
            if (_waveWidget[i])
            {
                _waveWidget[i]->updateLeadDisplayName(name);
            }
        }
    }
}

/**************************************************************************************************
 * 获取可得的导联集。
 *************************************************************************************************/
QList<ECGLead> ECGParam::getAvailableLeads()
{
    QList<ECGLead> leads;
    leads.append(ECG_LEAD_I);
    leads.append(ECG_LEAD_II);
    leads.append(ECG_LEAD_III);

    if (_curLeadMode >= ECG_LEAD_MODE_5)
    {
        leads.append(ECG_LEAD_AVR);
        leads.append(ECG_LEAD_AVL);
        leads.append(ECG_LEAD_AVF);
        leads.append(ECG_LEAD_V1);
    }
    if (_curLeadMode >= ECG_LEAD_MODE_12)
    {
        leads.append(ECG_LEAD_V2);
        leads.append(ECG_LEAD_V3);
        leads.append(ECG_LEAD_V4);
        leads.append(ECG_LEAD_V5);
        leads.append(ECG_LEAD_V6);
    }
    return leads;
}


/**************************************************************************************************
 * wave ID->Lead ID。
 *************************************************************************************************/
ECGLead ECGParam::waveIDToLeadID(WaveformID id)
{
    switch (id)
    {
    case WAVE_ECG_I:
        return ECG_LEAD_I;
    case WAVE_ECG_II:
        return ECG_LEAD_II;
    case WAVE_ECG_III:
        return ECG_LEAD_III;
    case WAVE_ECG_aVR:
        return ECG_LEAD_AVR;
    case WAVE_ECG_aVL:
        return ECG_LEAD_AVL;
    case WAVE_ECG_aVF:
        return ECG_LEAD_AVF;
    case WAVE_ECG_V1:
        return ECG_LEAD_V1;
    case WAVE_ECG_V2:
        return ECG_LEAD_V2;
    case WAVE_ECG_V3:
        return ECG_LEAD_V3;
    case WAVE_ECG_V4:
        return ECG_LEAD_V4;
    case WAVE_ECG_V5:
        return ECG_LEAD_V5;
    case WAVE_ECG_V6:
        return ECG_LEAD_V6;
    default:
        return ECG_LEAD_NR;
    }
}

/**************************************************************************************************
 * lead ID->wave ID。
 *************************************************************************************************/
WaveformID ECGParam::leadToWaveID(ECGLead lead)
{
    switch (lead)
    {
    case ECG_LEAD_I:
        return WAVE_ECG_I;
    case ECG_LEAD_II:
        return WAVE_ECG_II;
    case ECG_LEAD_III:
        return WAVE_ECG_III;
    case ECG_LEAD_AVR:
        return WAVE_ECG_aVR;
    case ECG_LEAD_AVL:
        return WAVE_ECG_aVL;
    case ECG_LEAD_AVF:
        return WAVE_ECG_aVF;
    case ECG_LEAD_V1:
        return WAVE_ECG_V1;
    case ECG_LEAD_V2:
        return WAVE_ECG_V2;
    case ECG_LEAD_V3:
        return WAVE_ECG_V3;
    case ECG_LEAD_V4:
        return WAVE_ECG_V4;
    case ECG_LEAD_V5:
        return WAVE_ECG_V5;
    case ECG_LEAD_V6:
        return WAVE_ECG_V6;
    default:
        return WAVE_ECG_II;
    }
}

/**************************************************************************************************
 * 模块复位。
 *************************************************************************************************/
void ECGParam::reset(void)
{
    if (_provider == NULL)
    {
        return;
    }

    _provider->setLeadSystem(_curLeadMode);
    _provider->getLeadCabelType();

    // 计算导联。
    _provider->setCalcLead(getCalcLead());

    // 设置病人类型
    _provider->setPatientType(getPatientType());

    // 设置带宽。
//    _provider->setBandwidth((ECGBandwidth)getBandwidth());

    // 设置起搏检测。
    _provider->enablePacermaker(getPacermaker());

    // 设置工频滤波。
    _provider->setNotchFilter(getNotchFilter());
}

/***************************************************************************************************
 * handle ecg selftest result
 **************************************************************************************************/
void ECGParam::handleSelfTestResult(unsigned selfTestResult)
{
    QString errStr("");
    bool flag = true;
    if (selfTestResult & MAJOR_SAMPLE_MODULE_INIT_FAILED)
    {
        errStr += "Major Collection Module Init Failed.\r\n";
        flag = false;
    }

    if (selfTestResult & MINOR_SAMPLE_MODULE_INIT_FAILED)
    {
        errStr += "Minor Collection Module Init Failed.\r\n";
        flag = false;
    }

    if (selfTestResult & PD_BLANK_TEST_FAILED)
    {
        errStr += "PD Blank Test Failed.\r\n";
        flag = false;
    }

    if (selfTestResult & PACE_SYNC_TEST_FAILED)
    {
        errStr += "Pace Sync Test Failed.\r\n";
        flag = false;
    }
    systemManager.setPoweronTestResult(E5_MODULE_SELFTEST_RESULT,
                                       flag ? SELFTEST_SUCCESS : SELFTEST_FAILED);

    if (!errStr.isEmpty())
    {
        ErrorLogItem *item = new CriticalFaultLogItem();
        item->setName("E5 Self Test Failed");
        item->setLog(errStr);
        item->setSubSystem(ErrorLogItem::SUB_SYS_E5);
        item->setSystemState(ErrorLogItem::SYS_STAT_SELFTEST);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);
    }
}

/**************************************************************************************************
 * enable VF calc。
 *************************************************************************************************/
void ECGParam::enableVFCalc(bool enable)
{
    if (NULL != _provider)
    {
        _provider->enableVFCalcCtrl(enable);
    }
}

/**************************************************************************************************
 * 获取可得的波形控件集。
 *************************************************************************************************/
void ECGParam::getAvailableWaveforms(QStringList *waveforms,
                                     QStringList *waveformShowName, int)
{
    waveforms->clear();
    waveformShowName->clear();

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[(ECGLead)i] == NULL)
        {
            return;
        }
    }

    waveforms->append(_waveWidget[ECG_LEAD_I]->name());
    waveforms->append(_waveWidget[ECG_LEAD_II]->name());
    waveforms->append(_waveWidget[ECG_LEAD_III]->name());
    waveformShowName->append(_waveWidget[ECG_LEAD_I]->getTitle());
    waveformShowName->append(_waveWidget[ECG_LEAD_II]->getTitle());
    waveformShowName->append(_waveWidget[ECG_LEAD_III]->getTitle());

    if (_curLeadMode >= ECG_LEAD_MODE_5)
    {
        waveforms->append(_waveWidget[ECG_LEAD_AVR]->name());
        waveforms->append(_waveWidget[ECG_LEAD_AVL]->name());
        waveforms->append(_waveWidget[ECG_LEAD_AVF]->name());
        waveforms->append(_waveWidget[ECG_LEAD_V1]->name());

        waveformShowName->append(_waveWidget[ECG_LEAD_AVR]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_AVL]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_AVF]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_V1]->getTitle());
    }
    if (_curLeadMode >= ECG_LEAD_MODE_12)
    {
        waveforms->append(_waveWidget[ECG_LEAD_V2]->name());
        waveforms->append(_waveWidget[ECG_LEAD_V3]->name());
        waveforms->append(_waveWidget[ECG_LEAD_V4]->name());
        waveforms->append(_waveWidget[ECG_LEAD_V5]->name());
        waveforms->append(_waveWidget[ECG_LEAD_V6]->name());

        waveformShowName->append(_waveWidget[ECG_LEAD_V2]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_V3]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_V4]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_V5]->getTitle());
        waveformShowName->append(_waveWidget[ECG_LEAD_V6]->getTitle());
    }
}

/**************************************************************************************************
 * 获取可得的趋势控件集。
 *************************************************************************************************/
QStringList ECGParam::getTrendWindowNames()
{
    QStringList trend;
    if (_pvcsTrendWidget != NULL)
    {
        trend.append(_pvcsTrendWidget->name());
    }
    if (_ecgSTTrendWidget != NULL)
    {
        trend.append(_ecgSTTrendWidget->name());
    }
    return trend;
}

/**************************************************************************************************
 * 获取导联模式。
 *************************************************************************************************/
void ECGParam::setLeadMode(ECGLeadMode newMode)
{
    if (newMode == _curLeadMode)
    {
        return;
    }

    ECGLeadMode oldMode = _curLeadMode;

    // 将新模式保存到配置文件中。
    currentConfig.setNumValue("ECG|LeadMode", static_cast<int>(newMode));
    _curLeadMode = newMode;
    if (NULL != _provider)
    {
        _provider->setLeadSystem(newMode);
    }

    // 切换到正确的计算导联。
    ECGLead calcLead = getCalcLead();
    ECGLead newCaclLead = calcLead;
    if (oldMode == ECG_LEAD_MODE_5)  // 5导切3导或12导。
    {
        if (newMode == ECG_LEAD_MODE_3)
        {
            if (calcLead > ECG_LEAD_III)
            {
                newCaclLead = ECG_LEAD_II;
            }
        }
        else if (newMode == ECG_LEAD_MODE_12)
        {
        }
    }
    else if (oldMode == ECG_LEAD_MODE_12)  // 12导切5到或3导。
    {
        if (newMode == ECG_LEAD_MODE_5)    // 切换5导。
        {
            if (calcLead > ECG_LEAD_V1)
            {
                newCaclLead = ECG_LEAD_II;
            }
        }
        else if (newMode == ECG_LEAD_MODE_3)  // 切换3导。
        {
            if (calcLead > ECG_LEAD_III)
            {
                newCaclLead = ECG_LEAD_II;
            }
        }
    }

    int needUpdateLayout = 0;
    if (calcLead != newCaclLead)
    {
        setCalcLead(newCaclLead);
        if (layoutManager.getUFaceType() != UFACE_MONITOR_ECG_FULLSCREEN)
        {
            needUpdateLayout = 1;
        }
    }

    // update visiable or invisiable waves
    QStringList disabledWaveforms = _getDisabledWaveforms();  // 不会被显示的波形集合。
    foreach(const QString &n, disabledWaveforms) {
        needUpdateLayout += layoutManager.setWidgetLayoutable(n, false);
    }
    QStringList visiableWaveforms;
    QStringList visiableWaveformsTitle;
    getAvailableWaveforms(&visiableWaveforms, &visiableWaveformsTitle, 0);
    foreach(const QString &n, visiableWaveforms)
    {
        needUpdateLayout += layoutManager.setWidgetLayoutable(n, true);
    }

    if (needUpdateLayout)
    {
        layoutManager.updateLayout();
    }

    if (layoutManager.getUFaceType() != UFACE_MONITOR_ECG_FULLSCREEN)
    {
        if (newMode == ECG_LEAD_MODE_3)
        {
            QStringList waveName = layoutManager.getDisplayedWaveforms();
            for (int i = 0; i < waveName.size(); ++i)
            {
                setLeadMode3DisplayLead(waveName.at(i));
            }
        }
    }
}

/**************************************************************************************************
 * 获取导联模式。
 *************************************************************************************************/
ECGLeadMode ECGParam::getLeadMode(void) const
{
    return _curLeadMode;
}

/**************************************************************************************************
 * 设置显示模式。
 *************************************************************************************************/
void ECGParam::setDisplayMode(ECGDisplayMode mode, bool refresh)
{
//    int band = 0;
    int filter = 0;
    ECGPaceMode ecgPaceMaker = getPacermaker();
    ECGPaceMode ecg12LPaceMaker = get12LPacermaker();

    if (mode == ECG_DISPLAY_NORMAL)
    {
//        band = _chestFreqBand;
        filter = ECG_NOTCH_50_AND_60HZ;
    }
    else
    {
        // 进入12L模式前，如果是在诊断时间范围內则先关闭诊断功能
//        band = _12LeadFreqBand;

        currentConfig.getNumValue("ECG|NotchFilter", filter);
    }

    if (NULL != _provider)
    {
//        _provider->setBandwidth((ECGBandwidth)band);
        _provider->setNotchFilter((ECGNotchFilter)filter);
    }

    currentConfig.setNumValue("ECG|DisplayMode", static_cast<int>(mode));
    _displayMode = mode;

    // 布局界面。
//    UserFaceType type = UFACE_MONITOR_STANDARD;
    if (mode == ECG_DISPLAY_NORMAL)
    {
        // 设置波形采样率
        _provider->setWaveformSample(WAVE_SAMPLE_RATE_250);

        // 退出12L界面前先还原起搏标记
        setPacermaker(ecgPaceMaker);
    }
    else if (mode == ECG_DISPLAY_12_LEAD_FULL)
    {
//        type = UFACE_MONITOR_12LEAD;

        // 设置波形采样率
        _provider->setWaveformSample(WAVE_SAMPLE_RATE_500);

        // 进入12L界面前先设置起搏标记
        set12LPacermaker(ecg12LPaceMaker);
    }

    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        // 更新波形速率
        _waveWidget[i]->setDataRate(_provider->getWaveformSample());
    }

    if (!refresh)
    {
        return;
    }

    // TODO(fang): check whether need to refresh layout
}

/**************************************************************************************************
 * 获取显示模式。
 *************************************************************************************************/
ECGDisplayMode ECGParam::getDisplayMode(void)
{
    return _displayMode;
}

/**************************************************************************************************
 * 根据输入的波形名称设置计算导联。
 *************************************************************************************************/
void ECGParam::setCalcLead(const QString &leadWaveform)
{
    for (int i = 0; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        if (_waveWidget[i]->name() == leadWaveform)
        {
            setCalcLead(static_cast<ECGLead>(i));
            break;
        }
    }
}

/**************************************************************************************************
 * 设置计算导联。
 *************************************************************************************************/
void ECGParam::setCalcLead(ECGLead lead)
{
    QList<ECGLead> leads;
    leads.clear();
    leads.append(ECG_LEAD_I);
    leads.append(ECG_LEAD_II);
    leads.append(ECG_LEAD_III);

    if (_curLeadMode >= ECG_LEAD_MODE_5)
    {
        leads.append(ECG_LEAD_AVR);
        leads.append(ECG_LEAD_AVL);
        leads.append(ECG_LEAD_AVF);
        leads.append(ECG_LEAD_V1);
    }
    if (_curLeadMode >= ECG_LEAD_MODE_12)
    {
        leads.append(ECG_LEAD_V2);
        leads.append(ECG_LEAD_V3);
        leads.append(ECG_LEAD_V4);
        leads.append(ECG_LEAD_V5);
        leads.append(ECG_LEAD_V6);
    }

    ECGLead preCalcLead = getCalcLead();

    if (-1 == leads.indexOf(lead))
    {
        lead = ECG_LEAD_II;
    }

    if (preCalcLead == lead)
    {
        return;
    }

    // 将新的计算导联保存道配置文件。
    currentConfig.setNumValue("ECG|CalcLead", static_cast<int>(lead));
    _calcLead = lead;
    if (NULL != _provider)
    {
        _provider->setCalcLead(lead);
    }

    // 将新的ECG1和ECG2对应的波形保存进配置文件
    int preECG1Lead = 0;
    int preECG2Lead = 0;
    currentConfig.getNumValue("ECG|Ecg1Wave", preECG1Lead);
    currentConfig.getNumValue("ECG|Ecg2Wave", preECG2Lead);

    if (layoutManager.getUFaceType() == UFACE_MONITOR_STANDARD)
    {
        // 标准界面且ECG模式大于3导时，处理ECG2波形与ECG1波形重复
        if (static_cast<int>(lead) == preECG2Lead)
        {
            // 计算导联与ECG2波形重复时，将ECG2波形设置为前ECG1波形
            currentConfig.setNumValue("ECG|Ecg2Wave", preECG1Lead);
            adjustPrintWave(static_cast<ECGLead>(preECG2Lead), static_cast<ECGLead>(preECG1Lead));
        }
    }

    adjustPrintWave(static_cast<ECGLead>(preECG1Lead), lead);
    currentConfig.setNumValue("ECG|Ecg1Wave", static_cast<int>(lead));

    emit calcLeadChanged();
}

/**************************************************************************************************
 * 3导联模式，设置显示导联。
 *************************************************************************************************/
void ECGParam::setLeadMode3DisplayLead(ECGLead lead)
{
    if (lead > ECG_LEAD_III)
    {
        return;
    }

    if (NULL != _provider)
    {
        _provider->setCalcLead(lead);
    }
}

/**************************************************************************************************
 * 3导联模式，设置显示导联。
 *************************************************************************************************/
void ECGParam::setLeadMode3DisplayLead(const QString &leadWaveform)
{
    if (_curLeadMode != ECG_LEAD_MODE_3)
    {
        return;
    }

    QStringList waveNameList;
    waveNameList << _waveWidget[ECG_LEAD_I]->name()
                 << _waveWidget[ECG_LEAD_II]->name()
                 << _waveWidget[ECG_LEAD_III]->name();
    int index = waveNameList.indexOf(leadWaveform);
    if (-1 != index)
    {
        setLeadMode3DisplayLead((ECGLead)index);
    }
}

/**************************************************************************************************
 * 获取计算导联。
 *************************************************************************************************/
ECGLead ECGParam::getCalcLead(void)
{
    return _calcLead;
}

/**************************************************************************************************
 * 轮转设置计算导联。
 *************************************************************************************************/
void ECGParam::autoSetCalcLead(void)
{
    // 获取支持的导联。
    QList<ECGLead> leads = getAvailableLeads();

    if (leads.isEmpty())
    {
        return;
    }

    // 当前的计算导联。
    ECGLead calcLead = getCalcLead();

    int index = 0;
    for (index = 0; index < leads.size(); index++)
    {
        if (leads[index] == calcLead)
        {
            break;
        }
    }

    if (index >= leads.size())  // 没找到则置为0。
    {
        return;
    }
    else
    {
        index++;
        index = (index >= leads.size()) ? 0 : index;
    }

    if (leads[index] == calcLead)
    {
        return;
    }

    setCalcLead(leads[index]);
    if (NULL != _waveWidget[calcLead] && NULL != _waveWidget[leads[index]])
    {
        layoutManager.updateLayout();
    }
}

/**************************************************************************************************
 * 设置病人类型。
 *************************************************************************************************/
void ECGParam::setPatientType(unsigned char type)
{
    if (NULL != _provider)
    {
        _provider->setPatientType(type);
    }
}

/**************************************************************************************************
 * 获取病人类型。
 *************************************************************************************************/
unsigned char ECGParam::getPatientType(void)
{
    int type = 3;
    systemConfig.getNumValue("General|PatientType", type);
    return (unsigned char)type;
}
/**************************************************************************************************
 * 设置带宽。
 *************************************************************************************************/
void ECGParam::setBandwidth(int band)
{
    if (_curLeadMode != ECG_LEAD_MODE_12)
    {
        currentConfig.setNumValue("ECG|BandWidth", band);
        _chestFreqBand = static_cast<ECGBandwidth>(band);
    }
    else
    {
        if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
        {
            currentConfig.setNumValue("ECG12L|ECG12LeadBandwidth", band);
            band += ECG_BANDWIDTH_0525_40HZ;
            _12LeadFreqBand = static_cast<ECGBandwidth>(band);
        }
        else
        {
            currentConfig.setNumValue("ECG|BandWidth", band);
            _chestFreqBand = static_cast<ECGBandwidth>(band);
        }
    }

    //    if (NULL != _provider)
    //    {
    //        _provider->setBandwidth(static_cast<ECGBandwidth>(band));
    //    }
}

/***************************************************************************************************
 * get the bandwidth of the calc lead
 **************************************************************************************************/
ECGBandwidth ECGParam::getCalcLeadBandWidth()
{
    return _chestFreqBand;
}

/**************************************************************************************************
 * 获取带宽。
 *************************************************************************************************/
ECGBandwidth ECGParam::getBandwidth(void)
{
    int band = 0;

    if (ecgParam.getDisplayMode() == ECG_DISPLAY_12_LEAD_FULL)
    {
        band = _12LeadFreqBand;
    }
    else
    {
        band = _chestFreqBand;
    }

    return static_cast<ECGBandwidth>(band);
}

/**************************************************************************************************
 * 获取显示带宽。
 *************************************************************************************************/
ECGBandwidth ECGParam::getDisplayBandWidth(void)
{
    int band = 0;
    band = _chestFreqBand;
    if (_displayMode == ECG_DISPLAY_12_LEAD_FULL)
    {
        band = _12LeadFreqBand;
    }

    return static_cast<ECGBandwidth>(band);
}

/**************************************************************************************************
 * set filter mode
 *************************************************************************************************/
void ECGParam::setFilterMode(int mode)
{
    currentConfig.setNumValue("ECG|FilterMode", static_cast<int>(mode));
    if (mode == _filterMode)
    {
        return;
    }
    _filterMode = static_cast<ECGFilterMode>(mode);
    if (NULL != _provider)
    {
        _provider->setFilterMode(_filterMode);
    }
    emit updateFilterMode();
}

/**************************************************************************************************
 * get filter mode
 *************************************************************************************************/
ECGFilterMode ECGParam::getFilterMode() const
{
    return _filterMode;
}

void ECGParam::setSelfLearn(bool onOff)
{
    if (NULL != _provider)
    {
        _provider->setSelfLearn(onOff);
    }
}

void ECGParam::setARRThreshold(ECGAlg::ARRPara parameter, short value)
{
    if (NULL != _provider)
    {
        _provider->setARRThreshold(parameter, value);
    }
}

/**************************************************************************************************
 * 设置起搏标记。
 *************************************************************************************************/
void ECGParam::setPacermaker(ECGPaceMode onoff)
{
    currentConfig.setNumValue("ECG|PacerMaker", static_cast<int>(onoff));

    if (NULL != _provider)
    {
        _provider->enablePacermaker(onoff);
    }

    RunningStatusBarInterface *runningStatus = RunningStatusBarInterface::getRunningStatusBar();
    if (runningStatus)
    {
        runningStatus->setPacerStatus(static_cast<bool>(onoff));
    }
    return;
}

/**************************************************************************************************
 * 获取起搏标记。
 *************************************************************************************************/
ECGPaceMode ECGParam::getPacermaker(void)
{
    int onoff = 0;
    currentConfig.getNumValue("ECG|PacerMaker", onoff);
    return static_cast<ECGPaceMode>(onoff);
}

void ECGParam::updatePacermaker()
{
    int index = 0;
    currentConfig.getNumValue("ECG|PacerMaker", index);
    if (NULL != _provider)
    {
        _provider->enablePacermaker(static_cast<ECGPaceMode>(index));
    }

    RunningStatusBarInterface *runningStatus = RunningStatusBarInterface::getRunningStatusBar();
    if (runningStatus)
    {
        runningStatus->setPacerStatus(static_cast<bool>(index));
    }
}

/**************************************************************************************************
 * 设置12L起搏标记。
 *************************************************************************************************/
void ECGParam::set12LPacermaker(ECGPaceMode onoff)
{
    currentConfig.setNumValue("ECG|ECG12LeadPacerMaker", static_cast<int>(onoff));
    _12LeadPacerMarker = onoff;

    if (ECG_DISPLAY_12_LEAD_FULL == ecgParam.getDisplayMode())
    {
        if (NULL != _provider)
        {
            _provider->enablePacermaker(onoff);
        }
    }

    return;
}

/**************************************************************************************************
 * 获取12L起搏标记。
 *************************************************************************************************/
ECGPaceMode ECGParam::get12LPacermaker(void)
{
    return _12LeadPacerMarker;
}

/**************************************************************************************************
 * 设置波形速度。
 *************************************************************************************************/
void ECGParam::setSweepSpeed(ECGSweepSpeed speed)
{
    currentConfig.setNumValue("ECG|SweepSpeed", static_cast<int>(speed));
    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setSpeed(speed);
    }

    layoutManager.resetWave();
}

/**************************************************************************************************
 * 获取波形速度。
 *************************************************************************************************/
ECGSweepSpeed ECGParam::getSweepSpeed(void)
{
    int speed = ECG_SWEEP_SPEED_250;
    currentConfig.getNumValue("ECG|SweepSpeed", speed);
    return static_cast<ECGSweepSpeed>(speed);
}

int ECGParam::getWaveDataRate() const
{
    if (!_provider)
    {
        return 0;
    }

    return _provider->getWaveformSample();
}

void ECGParam::setGain(ECGGain gain)
{
    currentConfig.setNumValue("ECG|FullScreenGain", static_cast<int>(gain));
    for (int i = 0; i < ECG_LEAD_NR; i++)
    {
        if (_waveWidget[i] == NULL)
        {
            continue;
        }
        _waveWidget[i]->setGain(gain);
    }
}

/**************************************************************************************************
 * 设置增益。
 *************************************************************************************************/
void ECGParam::setGain(ECGGain gain, int waveID)
{
    setGain(gain, waveIDToLeadID((WaveformID)waveID), true);
}

/**************************************************************************************************
 * 设置增益。
 *************************************************************************************************/
void ECGParam::setGain(ECGGain gain, ECGLead lead, bool isAutoGain)
{
    if (lead > ECG_LEAD_V6)
    {
        return;
    }

    if (_waveWidget[lead] == NULL)
    {
        return;
    }

    // 判断是否自动增益设置调用
    if (!isAutoGain)
    {
        QString wavename = _waveWidget[lead]->name();
        currentConfig.setNumValue("ECG|Gain|" + wavename, static_cast<int>(gain));
    }
    else
    {
        _autoGain[lead] = gain;
    }
    _waveWidget[lead]->setGain(gain, isAutoGain);
}

/**************************************************************************************************
 * 设置12导界面下波形的增益。
 *************************************************************************************************/
void ECGParam::set12LGain(ECGGain gain, ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return;
    }

    if (_waveWidget[lead] == NULL)
    {
        return;
    }

    _waveWidget[lead]->set12LGain(gain);
    _waveWidget[lead]->setGain(gain);
}

/**************************************************************************************************
 * 设置12导界面下波形的增益。
 *************************************************************************************************/
ECGGain ECGParam::get12LGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return ECG_GAIN_X10;
    }

    if (_waveWidget[lead] == NULL)
    {
        return ECG_GAIN_X10;
    }

    return _waveWidget[lead]->get12LGain();
}

/**************************************************************************************************
 * 获取增益。
 *************************************************************************************************/
ECGGain ECGParam::getGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return ECG_GAIN_X10;
    }

    if (_waveWidget[lead] == NULL)
    {
        return ECG_GAIN_X10;
    }

    int gain = ECG_GAIN_X10;
    if (layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN)
    {
        currentConfig.getNumValue("ECG|FullScreenGain", gain);
    }
    else
    {
        QString waveName = _waveWidget[lead]->name();
        currentConfig.getNumValue("ECG|Gain|" + waveName, gain);
    }
    return static_cast<ECGGain>(gain);
}

ECGGain ECGParam::getECGAutoGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return ECG_GAIN_X10;
    }

    if (_waveWidget[lead] == NULL)
    {
        return ECG_GAIN_X10;
    }
    return _autoGain[lead];
}

/**************************************************************************************************
 * 设置自动增益。
 *************************************************************************************************/
void ECGParam::setAutoGain(ECGLead lead, int flag)
{
    if (lead > ECG_LEAD_V6)
    {
        return;
    }

    if (_waveWidget[lead] == NULL)
    {
        return;
    }

    QString waveName = _waveWidget[lead]->name();

    currentConfig.setNumValue("ECG|AutoGain|" + waveName, flag);
}

/**************************************************************************************************
 * 获取自动增益。
 *************************************************************************************************/
bool ECGParam::getAutoGain(ECGLead lead)
{
    if (lead > ECG_LEAD_V6)
    {
        return false;
    }

    if (_waveWidget[lead] == NULL)
    {
        return false;
    }

    QString waveName = _waveWidget[lead]->name();

    int gain = 0;
    currentConfig.getNumValue("ECG|AutoGain|" + waveName, gain);
    return gain;
}

/**************************************************************************************************
 * 非监护模式下修改自动增益。
 *************************************************************************************************/
void ECGParam::changeAutoGain(void)
{
    // 获取支持的导联。
    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        if (getAutoGain((ECGLead)i))
        {
            if (NULL != _waveWidget[(ECGLead)i])
            {
                setAutoGain((ECGLead)i, false);
                _waveWidget[(ECGLead)i]->setAutoGain(false);
                _waveWidget[(ECGLead)i]->setGain(ecgParam.getGain((ECGLead)i));
            }
        }
    }
}

/**************************************************************************************************
 * 获取最大的增益值。
 *************************************************************************************************/
int ECGParam::getMaxGain(void)
{
    return ECG_GAIN_NR;
}

/**************************************************************************************************
 * 设置QRS音量。
 *************************************************************************************************/
void ECGParam::setQRSToneVolume(SoundManager::VolumeLevel vol)
{
    currentConfig.setNumValue("ECG|QRSVolume", static_cast<int>(vol));
    soundManager.setVolume(SoundManager::SOUND_TYPE_PULSE, vol);
    soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT, vol);
}

/**************************************************************************************************
 * 获取QRS音量。
 *************************************************************************************************/
int ECGParam::getQRSToneVolume(void)
{
    SoundManager::VolumeLevel vol = soundManager.getVolume(SoundManager::SOUND_TYPE_HEARTBEAT);
    return static_cast<int>(vol);
}

/**************************************************************************************************
 * 设置/获取工频滤波。
 *************************************************************************************************/
void ECGParam::setNotchFilter(ECGNotchFilter filter)
{
    currentConfig.setNumValue("ECG|NotchFilter", static_cast<int>(filter));
    if (filter == _notchFilter)
    {
        return;
    }
    _notchFilter = static_cast<ECGNotchFilter>(filter);
    if (NULL != _provider)
    {
        _provider->setNotchFilter(_notchFilter);
    }
    emit updateNotchFilter();
}

void ECGParam::updateEditNotchFilter()
{
    int filter = 0;
    currentConfig.getNumValue("ECG|NotchFilter", filter);
    if (filter == _notchFilter)
    {
        return;
    }
    _notchFilter = static_cast<ECGNotchFilter>(filter);
    if (NULL != _provider)
    {
        _provider->setNotchFilter(_notchFilter);
    }
    emit updateNotchFilter();
}
/**************************************************************************************************
 * 设置/获取工频滤波。
 *************************************************************************************************/
ECGNotchFilter ECGParam::getNotchFilter()
{
    return _notchFilter;
}

/***************************************************************************************************
 * get the notch filter of the calc lead :
 **************************************************************************************************/
ECGNotchFilter ECGParam::getCalcLeadNotchFilter()
{
    return ECG_NOTCH_50_AND_60HZ;
}

/**************************************************************************************************
 * 获取导联标签标准。
 *************************************************************************************************/
ECGLeadNameConvention ECGParam::getLeadConvention(void) const
{
    int standard = 0;
    systemConfig.getNumValue("Others|ECGStandard", standard);
    if (standard >= ECG_CONVENTION_NR)
    {
        standard = 0;
    }

    return static_cast<ECGLeadNameConvention>(standard);
}

/**************************************************************************************************
 * 获取12导联显示格式。
 *************************************************************************************************/
Display12LeadFormat ECGParam::get12LDisplayFormat(void) const
{
    return _12LeadDispFormat;
}

/***************************************************************************************************
 * 对导联线类型做相应处理
 **************************************************************************************************/
void ECGParam::handleECGLeadCabelType(unsigned char cabelType)
{
    ECGLeadMode leadMode = getECGModeByECGCabelType(cabelType);

    if ((ECG_LEAD_MODE_NR <= leadMode) || (ECG_LEAD_MODE_3 > leadMode))
    {
        // 如果是新规划，并且是开机第一次判断，则如果导联现不能识别就设置为5导，否则保持上一次不变
        if (_isPowerOnNewSession && (POWER_ON_SESSION_NEW == timeManager.getPowerOnSession()))
        {
            leadMode = ECG_LEAD_MODE_5;
        }
        else
        {
            return;
        }
    }

    if (ECG_LEAD_MODE_12 == leadMode)
    {
        int ecg12LEnable = 0;
        machineConfig.getModuleInitialStatus("ECG12LEADEnable", reinterpret_cast<bool*>(&ecg12LEnable));
        if (0 == ecg12LEnable)
        {
            leadMode = ECG_LEAD_MODE_5;
        }
    }

    _isPowerOnNewSession = false;
    setLeadMode(leadMode);
    return;
}

/***************************************************************************************************
 * 通过导联线类型获取导联模式
 **************************************************************************************************/
ECGLeadMode ECGParam::getECGModeByECGCabelType(unsigned char cabelType)
{
    ECGLeadMode leadMode = ECG_LEAD_MODE_3;

    switch (cabelType)
    {
    case (0x00):
        leadMode = ECG_LEAD_MODE_NR;
        break;

    case (0x01):
        leadMode = ECG_LEAD_MODE_3;
        break;

    case (0x02):
        leadMode = ECG_LEAD_MODE_5;
        break;

    case (0x03):
        leadMode = ECG_LEAD_MODE_12;
        break;

    default:
        leadMode = ECG_LEAD_MODE_NR;
        break;
    }

    return leadMode;
}

/***************************************************************************************************
 * is ever lead on
 **************************************************************************************************/
bool ECGParam::getEverLeadOn(ECGLead lead)
{
    if (lead <= ECG_LEAD_AVF)
    {
        for (int i = 0; i <= lead; ++i)
        {
            if (_isEverLeadOn[lead])
            {
                return true;
            }
        }

        return false;
    }

    return _isEverLeadOn[lead];
}

/***************************************************************************************************
 * set VF signal
 **************************************************************************************************/
void ECGParam::setCheckPatient(bool flag)
{
    if (_isCheckPatient == flag)
    {
        return;
    }

    _isCheckPatient = flag;
}

void ECGParam::clearOxyCRGWaveNum()
{
    _updateNum = 0;
}

void ECGParam::updateWaveWidgetStatus()
{
    QList<int> waveIdList = layoutManager.getDisplayedWaveformIDs();
    if (!waveIdList.count())
    {
        return;
    }

    for (int i = 0; i < ECG_LEAD_NR; i++)
    {
        if (!_waveWidget[i])
        {
            continue;
        }
        if (!_waveWidget[i]->isVisible())
        {
            continue;
        }
        bool isVisible;
        if (waveIdList.at(0) != _waveWidget[i]->getID())
        {
            isVisible = false;
        }
        else
        {
            isVisible = true;
        }
        _waveWidget[i]->setWaveInfoVisible(isVisible);
    }
}

ECGLeadNameConvention ECGParam::getLeadNameConvention() const
{
    return _ecgStandard;
}

void ECGParam::setFristConnect()
{
    _isFristConnect = true;
}

bool ECGParam::getFristConnect()
{
    return _isFristConnect;
}

void ECGParam::setRawDataOnOff(bool sta)
{
    if (_provider)
    {
        _provider->enableRawData(sta);
    }
}

bool ECGParam::getRawDataOnOff()
{
    int value = false;
    machineConfig.getNumValue("Record|ECG", value);
    return value;
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ECGParam::ECGParam() : Param(PARAM_ECG),
    _updateNum(0), _connectedProvider(false), _isFristConnect(false)
{
    // 初始化成员。
    _provider = NULL;
    _pvcsTrendWidget = NULL;
    _ecgSTTrendWidget = NULL;
    _waveDataInvalid = true;
    _isDisableDiaSoftKey = false;
    _isCheckPatient = false;
    _hrValue = InvData();
    memset(_leadOff, 2, sizeof(_leadOff));

    _monitorSoftkey = static_cast<MonitorSoftkeyAction *>(softkeyManager.getAction(SOFTKEY_ACTION_STANDARD));

//    // 关闭/使能12导的快捷软按键。
//    if (_monitorSoftkey != NULL)
//    {
//        int ecg12LEnable = 0;
//        machineConfig.getNumValue("ECG12LEADEnable", ecg12LEnable);
//        if (ecg12LEnable)
//        {
//            _monitorSoftkey->enable12Lead(true);
//        }
//        else
//        {
//            _monitorSoftkey->enable12Lead(false);
//        }
//    }

    for (int i = ECG_LEAD_I; i < ECG_LEAD_NR; i++)
    {
        _waveWidget[i] = NULL;
        _autoGain[i] = ECG_GAIN_X10;
    }

    int lead = ECG_LEAD_II;
    currentConfig.getNumValue("ECG|CalcLead", lead);
    _calcLead = (ECGLead)lead;

    int mode = ECG_LEAD_MODE_5;
    currentConfig.getNumValue("ECG|LeadMode", mode);
    _curLeadMode = (ECGLeadMode)mode;

    mode = ECG_DISPLAY_NORMAL;
    currentConfig.getNumValue("ECG|DisplayMode", mode);
    _displayMode = (ECGDisplayMode)mode;

    mode = ECG_PACE_OFF;
    currentConfig.getNumValue("ECG|ECG12LeadPacerMaker", mode);
    _12LeadPacerMarker = (ECGPaceMode) mode;

    mode = ECG_BANDWIDTH_067_20HZ;
    currentConfig.getNumValue("ECG|BandWidth", mode);
    _chestFreqBand = (ECGBandwidth) mode;

    mode = ECG_FILTERMODE_MONITOR;
    currentConfig.getNumValue("ECG|FilterMode", mode);
    _filterMode = (ECGFilterMode) mode;

    mode = ECG_NOTCH_OFF;
    currentConfig.getNumValue("ECG|NotchFilter", mode);
    _notchFilter = (ECGNotchFilter) mode;

    mode = 0;
    currentConfig.getNumValue("ECG12L|ECG12LeadBandwidth", mode);
    mode += ECG_BANDWIDTH_0525_40HZ;
    _12LeadFreqBand = (ECGBandwidth) mode;

    mode = DISPLAY_12LEAD_STAND;
    currentConfig.getNumValue("ECG12L|DisplayFormat", mode);
    _12LeadDispFormat = (Display12LeadFormat)mode;

    int standard = 0;
    systemConfig.getNumValue("Others|ECGStandard", standard);
    _ecgStandard = static_cast<ECGLeadNameConvention>(standard);

    for (int i = 0; i < ECG_LEAD_NR; ++i)
    {
        QString everLeadOnStr = "ECG|EverLeadOn";
        int leadOnOff;
        currentConfig.getNumValue(everLeadOnStr, leadOnOff);
        _isEverLeadOn[i] = (leadOnOff << i) & 0x01;
    }

//    _lastCabelType = 0x00;
    _isPowerOnNewSession = true;

    // 绑定当前工作模式改变信号和滤波模式槽函数
    connect(&systemManager, SIGNAL(workModeChanged(WorkMode)), this, SLOT(onWorkModeChanged(WorkMode)));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ECGParam &ECGParam::getInstance()
{
    static ECGParam *instance = NULL;
    if (instance == NULL)
    {
        instance = new ECGParam();
        ECGParamInterface *old = registerECGParam(instance);
        if (old)
        {
            delete old;
        }
    }
    return *instance;
}

ECGParam::~ECGParam()
{
    _timer.stop();
}

void ECGParam::onWorkModeChanged(WorkMode mode)
{
    if (mode != WORK_MODE_DEMO)
    {
        return;
    }

    _filterMode = ECG_FILTERMODE_DIAGNOSTIC;
    emit updateFilterMode();
    _notchFilter = ECG_NOTCH_OFF;
    emit updateNotchFilter();
}

void ECGParam::onPaletteChanged(ParamID id)
{
    if (id != PARAM_ECG)
    {
        return;
    }
    QPalette psrc = colorManager.getPalette(paramInfo.getParamName(PARAM_ECG));
    for (int i = ECG_ST_I; i < ECG_ST_NR; i++)
    {
        _waveWidget[i]->updatePalette(psrc);
    }
#ifndef HIDE_ECG_ST_PVCS_SUBPARAM
    _ecgSTTrendWidget->updatePalette(psrc);
    _pvcsTrendWidget->updatePalette(psrc);
#endif
}
/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void ECGParam::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId, data, len);
    }
}
