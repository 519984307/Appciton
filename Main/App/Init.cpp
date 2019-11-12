/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/2
 **/


#include "Init.h"
#include "LayoutManager.h"
#include "ShortTrendContainer.h"
#include "NightModeManager.h"
#include "RunningStatusBar.h"
#include "RainbowProvider.h"
#include "ConfigManager.h"
#include "NurseCallManager.h"
#include "TestBatteryTime.h"
#include "Framework/ErrorLog/ErrorLog.h"
#include "Framework/ErrorLog/ErrorLogItem.h"

#include "Framework/Language/LanguageManager.h"
#include "Framework/Language/Translator.h"

/**
 * @brief initLanguage initialize the language manager
 */
static void initLanguage()
{
#define LOCALE_FILE_PATH "/usr/local/nPM/locale/"
     int langNo = 0;
     systemConfig.getNumAttr("General|Language", "CurrentOption", langNo);
     int langNext = 0;
     systemConfig.getNumAttr("General|Language", "NextOption", langNext);

     if (langNo != langNext)
     {
         /* If not identical, we need to switch the new language */
         systemConfig.setNumAttr("General|Language", "CurrentOption", langNext);
         langNo = langNext;
     }

     LanguageManager::LanguageId langId = static_cast<LanguageManager::LanguageId>(langNo);

     LanguageManager *langMgr = LanguageManager::getInstance();

     // 获取语言文件的名称。
     QString language = QString("General|Language|Opt%1").arg(langNo);

     systemConfig.getStrAttr(language, "XmlFileName", language);
     QString path =  LOCALE_FILE_PATH + language + ".xml";

     Translator *translator = new Translator(path);
     if (!translator->isValid())
     {
         qWarning() << Q_FUNC_INFO << "Load langage file" << path << "Failed";
         delete translator;
         return;
     }

     if (!langMgr->registerTranslator(langId, translator))
     {
         qDebug() << Q_FUNC_INFO << "register Language translator failed!";
         delete translator;
     }
}
/**************************************************************************************************
 * 功能： 初始化系统。
 *************************************************************************************************/
static void _initSystem(void)
{
    // 配置初始化。
    machineConfig.construction();

    systemConfig.construction();

    initLanguage();
    // superConfig.construction();

    // superRunConfig.construction();

    // 存储目录管理。
    dataStorageDirManager.getInstance();

    // 新会话，需要恢复主配置文件
    if (timeManager.getPowerOnSession() == POWER_ON_SESSION_NEW)
    {
        // currentConfig.load(curConfigName);
        Config systemDefCfg(systemConfig.getDefaultFileName(systemConfig.getFileName()));
        systemConfig.setNodeValue("PrimaryCfg|PatientInfo", systemDefCfg);

        ErrorLogItem *item = new ErrorLogItem();
        item->setName("Load default config");
        QString str = systemConfig.getCurConfigName();
        str += "\r\n";
        str += systemConfig.getCurConfigName();
        str += "\r\n";
        item->setLog(str);
        item->setSubSystem(ErrorLogItem::SUB_SYS_MAIN_PROCESSOR);
        item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
        item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
        errorLog.append(item);

        QString selectIndexString;
        if (systemConfig.getCurConfigName().contains("Adult"))
        {
            selectIndexString = QString("ConfigManager|Default|%1").arg(PatientSymbol::convert(PATIENT_TYPE_ADULT));
        }
        else if (systemConfig.getCurConfigName().contains("Ped"))
        {
            selectIndexString = QString("ConfigManager|Default|%1").arg(PatientSymbol::convert(PATIENT_TYPE_PED));
        }
        else if (systemConfig.getCurConfigName().contains("Neo"))
        {
            selectIndexString = QString("ConfigManager|Default|%1").arg(PatientSymbol::convert(PATIENT_TYPE_NEO));
        }
        else
        {
            selectIndexString = QString("ConfigManager|Default|%1").arg(PatientSymbol::convert(PATIENT_TYPE_ADULT));
        }
        QString selectTypeString;
        systemConfig.getStrAttr(selectIndexString, "type", selectTypeString);

        if (selectTypeString != "Current")  // 当前文件配置不再加载
        {
            QString selectNameString;
            systemConfig.getStrValue(selectIndexString, selectNameString);
            QString strOld = QString("%1%2").arg(CFG_PATH).arg(selectNameString);
            QString strNew = systemConfig.getCurConfigName();
            QFile::remove(strNew);
            QFile::copy(strOld, strNew);
            currentConfig.reload();
        }
    }

    // 波形缓存。
    waveformCache.getInstance();

    // tick初始化。
    systemTick.construction();

    // 初始化声音。
    soundManager.selfTest();

    // 网络初始化。
//    networkManager.construction();

    // 按键板初始化。
//    systemBoardProvider.construction();

    // 报警灯。
    lightManager.getInstance();
    lightManager.setProvider(&systemBoardProvider);
    systemManager.enableBrightness(systemManager.getBrightness());

    // 自检
    systemBoardProvider.selfTest();

    // initialize the storage manager in the main thread
    EventStorageManager::getInstance();
    TrendDataStorageManager::getInstance();

    // 初始化夜间模式
    runningStatus.getInstance();
    if (nightModeManager.nightMode())
    {
        nightModeManager.setNightMode(true);
    }

    // 初始化护士呼叫
    NurseCallManager::getInstance();
}

/**************************************************************************************************
 * 功能： 初始化界面相关。
 *************************************************************************************************/
static void _initWindowManager(void)
{
    // Set window size
    int screenWidth = 0;
    int screenHeight = 0;
    machineConfig.getNumValue("ScreenWidth", screenWidth);
    machineConfig.getNumValue("ScreenHeight", screenHeight);
    windowManager.setFixedSize(screenWidth, screenHeight);
}

/**************************************************************************************************
 * 功能： 初始化报警。
 *************************************************************************************************/
static void _initComponents(void)
{
    keyActionManager.Construction();
    keyActionManager.registerKetAction(new NormalModeKeyAction());

    // Softkeys
    softkeyManager.getInstance();
    layoutManager.addLayoutWidget(&softkeyManager);

    // 时间管理功能初始化。
    DateTimeWidget *timeWigdet = new DateTimeWidget();

    layoutManager.addLayoutWidget(timeWigdet);
    timeManager.registerWidgets(timeWigdet, NULL);

    // 基础时间日期管理。
    timeDate.construction();

    // 系统状态栏
    layoutManager.addLayoutWidget(&sysStatusBar);

    // 电源
    BatteryBarWidget *bar = &batteryBarWidget;
    powerManger.construction();
    AlarmOneShotIFace *oneShotAlarmSource = new BatteryOneShotAlarm();
    alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_BATTERY);
    alertor.addOneShotSource(oneShotAlarmSource);
    layoutManager.addLayoutWidget(bar);
    testBatteryTime.getInstance();

    // 病人管理初始化。
    PatientInfoWidget *patientInfoWidget = new PatientInfoWidget();
    layoutManager.addLayoutWidget(patientInfoWidget);
    patientManager.setPatientInfoWidget(patientInfoWidget);
    patientInfoWindow.getInstance();

    // 初始化报警。
    alertor.getInstance();
    alarmIndicator.getInstance();

    AlarmPhyInfoBarWidget *alarmPhyInfo = new AlarmPhyInfoBarWidget("AlarmPhyInfoBarWidget");
    AlarmTechInfoBarWidget *alarmTechInfo = new AlarmTechInfoBarWidget("AlarmTechInfoBarWidget");
    AlarmStatusWidget *alarmPhyMuteBar = new AlarmStatusWidget();
    alarmIndicator.setAlarmPhyWidgets(alarmPhyInfo, alarmPhyMuteBar, patientInfoWidget);
    alarmIndicator.setAlarmTechWidgets(alarmTechInfo);
    layoutManager.addLayoutWidget(alarmPhyInfo);
    layoutManager.addLayoutWidget(alarmTechInfo);
    layoutManager.addLayoutWidget(alarmPhyMuteBar);

    // running status
    runningStatus.setPacerStatus(ecgParam.getPacermaker());
    runningStatus.setNightModeStatus(nightModeManager.nightMode());
#ifdef ENABLE_O2_APNEASTIMULATION
    runningStatus.setShakeStatus(static_cast<ShakeStatus>(o2Param.getApneaAwakeStatus()));
#endif
#ifdef Q_WS_QWS
    if (systemManager.isSupport(CONFIG_TOUCH))
    {
        runningStatus.setTouchStatus(systemManager.isTouchScreenOn());
    }
#endif
    layoutManager.addLayoutWidget(&runningStatus);

    //报警状态
    alarmStateMachine.getInstance();
    alarmStateMachine.start();

    // U盘管理
    usbManager.getInstance();
    // U盘数据储存
    rawDataCollector.getInstance();
}
/**************************************************************************************************
 * 功能： 初始化参数和提供者对象。
 *************************************************************************************************/
static void _initProviderParam(void)
{
    paramInfo.getInstance();

    paramManager.addProvider(&systemBoardProvider);

    // 创建Provider.
    DemoProvider *demo = new DemoProvider();
    paramManager.addProvider(demo);
    // TE3Provider *te3 = new TE3Provider();
    // paramManager.addProvider(*te3);

    E5Provider *te3 = new E5Provider();
    paramManager.addProvider(te3);

    DataDispatcher::addDataDispatcher(new DataDispatcher("DataDispatcher"));

    // ECG部分。
    paramManager.addParam(&ecgDupParam.getInstance());

    AlarmLimitIFace *limitAlarmSource = new ECGDupLimitAlarm();
    alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_ECGDUP);
    alertor.addLimtSource(limitAlarmSource);

    paramManager.addParam(&ecgParam.getInstance());

    limitAlarmSource = new ECGLimitAlarm();
    alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_ECG);
    alertor.addLimtSource(limitAlarmSource);
    AlarmOneShotIFace *oneShotAlarmSource = new ECGOneShotAlarm();
    alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_ECG);
    alertor.addOneShotSource(oneShotAlarmSource);

    ECGTrendWidget *ecgTrendWidget = new ECGTrendWidget();
    ecgDupParam.setTrendWidget(ecgTrendWidget);
    layoutManager.addLayoutWidget(ecgTrendWidget, LAYOUT_NODE_PARAM_ECG);
#ifndef HIDE_ECG_ST_PVCS_SUBPARAM
    ECGPVCSTrendWidget *ecgPVCSTrendWidget = new ECGPVCSTrendWidget();
    ecgParam.setECGPVCSTrendWidget(ecgPVCSTrendWidget);
    layoutManager.addLayoutWidget(ecgPVCSTrendWidget, LAYOUT_NODE_PARAM_PVCS);
    ECGSTTrendWidget *ecgSTTrendWidget = new ECGSTTrendWidget();
    ecgParam.setECGSTTrendWidget(ecgSTTrendWidget);
    layoutManager.addLayoutWidget(ecgSTTrendWidget, LAYOUT_NODE_PARAM_ST);
#endif

    ECGWaveWidget *ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_I, "ECGIWaveWidget",
            ECGSymbol::convert(ECG_LEAD_I, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_I);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_II, "ECGIIWaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_II, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_II);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_III, "ECGIIIWaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_III, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_III);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_aVR, "ECGaVRWaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_AVR, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_AVR);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_aVL, "ECGaVLWaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_AVL, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_AVL);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_aVF, "ECGaVFWaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_AVF, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_AVF);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V1, "ECGV1WaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_V1, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V1);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V2, "ECGV2WaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_V2, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V2);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V3, "ECGV3WaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_V3, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V3);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V4, "ECGV4WaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_V4, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V4);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V5, "ECGV5WaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_V5, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V5);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    ecgWaveWidget = new ECGWaveWidget(WAVE_ECG_V6, "ECGV6WaveWidget",
                                      ECGSymbol::convert(ECG_LEAD_V6, ecgParam.getLeadConvention()));
    ecgParam.setWaveWidget(ecgWaveWidget, ECG_LEAD_V6);
    layoutManager.addLayoutWidget(ecgWaveWidget);

    OxyCRGWidget *oxyCRGWidget = new OxyCRGWidget();
    layoutManager.addLayoutWidget(oxyCRGWidget);

    OxyCRGRRHRWaveWidget *hrOxyCRGWidget = new OxyCRGRRHRWaveWidget("OxyCRGHRWidget");
    ecgParam.setOxyCRGHrWaveWidget(hrOxyCRGWidget);
    layoutManager.addLayoutWidget(hrOxyCRGWidget);
    oxyCRGWidget->setOxyCRGRrHrWidget(hrOxyCRGWidget);

    // RESP部分。
    if (systemManager.isSupport(CONFIG_RESP) || systemManager.isSupport(CONFIG_CO2))
    {
        paramManager.addParam(&respDupParam.construction());
        limitAlarmSource = new RESPDupLimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_RESPDUP);
        alertor.addLimtSource(limitAlarmSource);
        if (systemManager.isSupport(CONFIG_RESP))
        {
            paramManager.addParam(&respParam.construction());

            limitAlarmSource = new RESPLimitAlarm();
            alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_RESP);
            alertor.addLimtSource(limitAlarmSource);
            oneShotAlarmSource = new RESPOneShotAlarm();
            alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_RESP);
            alertor.addOneShotSource(oneShotAlarmSource);

            RESPWaveWidget *respWaveWidget = new RESPWaveWidget("RESPWaveWidget",
                    trs(paramInfo.getParamName(PARAM_RESP)));
            RESPTrendWidget *respTrendWidget = new RESPTrendWidget();
            respDupParam.setTrendWidget(respTrendWidget);
            respParam.setWaveWidget(respWaveWidget);
            layoutManager.addLayoutWidget(respWaveWidget, LAYOUT_NODE_WAVE_RESP);
            layoutManager.addLayoutWidget(respTrendWidget, LAYOUT_NODE_PARAM_RESP);

            OxyCRGRESPWaveWidget *respOxyCRGWidget = new OxyCRGRESPWaveWidget("OxyCRGRESPWidget");
            respParam.setOxyCRGWaveRESPWidget(respOxyCRGWidget);
            layoutManager.addLayoutWidget(respOxyCRGWidget);
            oxyCRGWidget->setOxyCRGRespWidget(respOxyCRGWidget);
            respParam.setOxyCRGRrHrTrend(hrOxyCRGWidget);
        }
    }

    // SPO2部分。
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        QString str;
        machineConfig.getStrValue("SPO2", str);
        if (str == "MASIMO_SPO2")
        {
            paramManager.addProvider(new MasimoSetProvider());
            spo2Param.setModuleType(MODULE_MASIMO_SPO2);
        }
        else if (str == "BLM_S5")
        {
            paramManager.addProvider(new S5Provider());
            spo2Param.setModuleType(MODULE_BLM_S5);
        }
        else if (str == "RAINBOW_SPO2")
        {
            paramManager.addProvider(new RainbowProvider());
            spo2Param.setModuleType(MODULE_RAINBOW_SPO2);
        }
        paramManager.addParam(&spo2Param.construction());

        limitAlarmSource = new SPO2LimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_SPO2);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new SPO2OneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_SPO2);
        alertor.addOneShotSource(oneShotAlarmSource);

        SPO2WaveWidget *spo2WaveWidget = new SPO2WaveWidget("SPO2WaveWidget", trs("PLETH"));
        SPO2TrendWidget *spo2TrendWidget = new SPO2TrendWidget();
        spo2Param.setTrendWidget(spo2TrendWidget);
        spo2Param.setWaveWidget(spo2WaveWidget);
        layoutManager.addLayoutWidget(spo2WaveWidget, LAYOUT_NODE_WAVE_SPO2);
        layoutManager.addLayoutWidget(spo2TrendWidget, LAYOUT_NODE_PARAM_SPO2);

        OxyCRGSPO2TrendWidget *spo2OxyCRGWidget = new OxyCRGSPO2TrendWidget("OxyCRGSPO2Widget");
        spo2Param.setOxyCRGSPO2Trend(spo2OxyCRGWidget);
        layoutManager.addLayoutWidget(spo2OxyCRGWidget);
        oxyCRGWidget->setOxyCRGSPO2Trend(spo2OxyCRGWidget);
    }

    // NIBP部分。
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        QString str = "SUNTECH_NIBP";
        machineConfig.getStrValue("NIBP", str);
        if (str == "SUNTECH_NIBP")
        {
            paramManager.addProvider(new SuntechProvider());
        }
        else
        {
            paramManager.addProvider(new N5Provider());
        }

        paramManager.addParam(&nibpParam.getInstance());

        limitAlarmSource = new NIBPLimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_NIBP);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new NIBPOneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_NIBP);
        alertor.addOneShotSource(oneShotAlarmSource);

        NIBPTrendWidget *nibpTrenWidget = new NIBPTrendWidget();
        nibpParam.setNIBPTrendWidget(nibpTrenWidget);
        layoutManager.addLayoutWidget(nibpTrenWidget, LAYOUT_NODE_PARAM_NIBP);
        NIBPDataTrendWidget *nibpDataTrenWidget = new NIBPDataTrendWidget();
        nibpParam.setNIBPDataTrendWidget(nibpDataTrenWidget);
        layoutManager.addLayoutWidget(nibpDataTrenWidget, LAYOUT_NODE_PARAM_NIBPLIST);
    }

    // CO2部分。
    if (systemManager.isSupport(CONFIG_CO2))
    {
        QString str;
        machineConfig.getStrValue("CO2", str);
        paramManager.addProvider(new BLMCO2Provider(str));

        paramManager.addParam(&co2Param.construction());

        limitAlarmSource = new CO2LimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_CO2);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new CO2OneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_CO2);
        alertor.addOneShotSource(oneShotAlarmSource);

        CO2WaveWidget *co2WaveWidget = new CO2WaveWidget("CO2WaveWidget",
                trs(paramInfo.getParamName(PARAM_CO2)));
        CO2TrendWidget *co2TrendWidget = new CO2TrendWidget();
        co2Param.setTrendWidget(co2TrendWidget);
        co2Param.setWaveWidget(co2WaveWidget);
        layoutManager.addLayoutWidget(co2WaveWidget, LAYOUT_NODE_WAVE_CO2);
        layoutManager.addLayoutWidget(co2TrendWidget, LAYOUT_NODE_PARAM_CO2);

        OxyCRGCO2WaveWidget *co2OxyCRGWidget = new OxyCRGCO2WaveWidget("OxyCRGCO2Widget");
        co2Param.setOxyCRGCO2Widget(co2OxyCRGWidget);
        layoutManager.addLayoutWidget(co2OxyCRGWidget);
        oxyCRGWidget->setOxyCRGCO2Widget(co2OxyCRGWidget);
    }

    // IBP test
    if (systemManager.isSupport(CONFIG_IBP))
    {
        paramManager.addProvider(new WitleafProvider());
        paramManager.addParam(&ibpParam.construction());

        limitAlarmSource = new IBPLimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_IBP);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new IBPOneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_IBP);
        alertor.addOneShotSource(oneShotAlarmSource);

        IBPTrendWidget *ibp1TrendWidget = new IBPTrendWidget("IBP1TrendWidget",
                IBP_PRESSURE_ART);
        ibpParam.setIBPTrendWidget(ibp1TrendWidget, IBP_INPUT_1);
        layoutManager.addLayoutWidget(ibp1TrendWidget, LAYOUT_NODE_PARAM_IBP1);

        IBPTrendWidget *ibp2TrendWidget = new IBPTrendWidget("IBP2TrendWidget",
                IBP_PRESSURE_PA);
        ibpParam.setIBPTrendWidget(ibp2TrendWidget, IBP_INPUT_2);
        layoutManager.addLayoutWidget(ibp2TrendWidget, LAYOUT_NODE_PARAM_IBP2);

        IBPWaveWidget *ibp1WaveWidget = new IBPWaveWidget(WAVE_ART, "IBP1WaveWidget",
                IBP_PRESSURE_ART);
        ibpParam.setWaveWidget(ibp1WaveWidget, IBP_INPUT_1);
        layoutManager.addLayoutWidget(ibp1WaveWidget, LAYOUT_NODE_WAVE_IBP1);

        IBPWaveWidget *ibp2WaveWidget = new IBPWaveWidget(WAVE_PA, "IBP2WaveWidget",
                IBP_PRESSURE_PA);
        ibpParam.setWaveWidget(ibp2WaveWidget, IBP_INPUT_2);
        layoutManager.addLayoutWidget(ibp2WaveWidget, LAYOUT_NODE_WAVE_IBP2);
    }

    // CO
    if (systemManager.isSupport(CONFIG_CO))
    {
        paramManager.addParam(&coParam.construction());

        limitAlarmSource = new COLimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_CO);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new COOneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_CO);
        alertor.addOneShotSource(oneShotAlarmSource);

        COTrendWidget *coTrendWidget = new COTrendWidget("COTrendWidget");
        coParam.setCOTrendWidget(coTrendWidget);
        layoutManager.addLayoutWidget(coTrendWidget, LAYOUT_NODE_PARAM_CO);
    }

    // AG
    if (systemManager.isSupport(CONFIG_AG))
    {
        paramManager.addProvider(new PhaseinProvider());
        paramManager.addParam(&agParam.construction());

        limitAlarmSource = new AGLimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_AG);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new AGOneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_AG);
        alertor.addOneShotSource(oneShotAlarmSource);
    }

    AGTrendWidget *trendWidgetN2O = new AGTrendWidget("N2OTrendWidget", AG_TYPE_N2O);
    agParam.setTrendWidget(trendWidgetN2O, AG_TYPE_N2O);
    layoutManager.addLayoutWidget(trendWidgetN2O, LAYOUT_NODE_PARAM_N2O);

    AGTrendWidget *trendWidgetAA1 = new AGTrendWidget("AA1TrendWidget", AG_TYPE_AA1);
    agParam.setTrendWidget(trendWidgetAA1, AG_TYPE_AA1);
    layoutManager.addLayoutWidget(trendWidgetAA1, LAYOUT_NODE_PARAM_AA1);

    AGTrendWidget *trendWidgetAA2 = new AGTrendWidget("AA2TrendWidget", AG_TYPE_AA2);
    agParam.setTrendWidget(trendWidgetAA2, AG_TYPE_AA2);
    layoutManager.addLayoutWidget(trendWidgetAA2, LAYOUT_NODE_PARAM_AA2);

    AGTrendWidget *trendWidgetO2 = new AGTrendWidget("O2TrendWidget", AG_TYPE_O2);
    agParam.setTrendWidget(trendWidgetO2, AG_TYPE_O2);
    layoutManager.addLayoutWidget(trendWidgetO2, LAYOUT_NODE_PARAM_O2);

    AGWaveWidget *waveWidgetN2O = new AGWaveWidget(WAVE_N2O, "N2OWaveWidget", AG_TYPE_N2O);
    agParam.setWaveWidget(waveWidgetN2O, AG_TYPE_N2O);
    layoutManager.addLayoutWidget(waveWidgetN2O, LAYOUT_NODE_WAVE_N2O);

    AGWaveWidget *waveWidgetAA1 = new AGWaveWidget(WAVE_AA1, "AA1WaveWidget", AG_TYPE_AA1);
    agParam.setWaveWidget(waveWidgetAA1, AG_TYPE_AA1);
    layoutManager.addLayoutWidget(waveWidgetAA1, LAYOUT_NODE_WAVE_AA1);

    AGWaveWidget *waveWidgetAA2 = new AGWaveWidget(WAVE_AA2, "AA2WaveWidget", AG_TYPE_AA2);
    agParam.setWaveWidget(waveWidgetAA2, AG_TYPE_AA2);
    layoutManager.addLayoutWidget(waveWidgetAA2, LAYOUT_NODE_WAVE_AA2);

    AGWaveWidget *waveWidgetO2 = new AGWaveWidget(WAVE_O2, "O2WaveWidget", AG_TYPE_O2);
    agParam.setWaveWidget(waveWidgetO2, AG_TYPE_O2);
    layoutManager.addLayoutWidget(waveWidgetO2, LAYOUT_NODE_WAVE_O2);

    // TEMP部分。
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        QString str;
        machineConfig.getStrValue("TEMP", str);
        if (str == "BLM_T5")
        {
            paramManager.addProvider(new T5Provider());
        }

        paramManager.addParam(&tempParam.construction());

        limitAlarmSource = new TEMPLimitAlarm();
        alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_TEMP);
        alertor.addLimtSource(limitAlarmSource);
        oneShotAlarmSource = new TEMPOneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_TEMP);
        alertor.addOneShotSource(oneShotAlarmSource);

        TEMPTrendWidget *tempTrendWidget = new TEMPTrendWidget();
        tempParam.setTrendWidget(tempTrendWidget);
        layoutManager.addLayoutWidget(tempTrendWidget, LAYOUT_NODE_PARAM_TEMP);
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    if (systemManager.isSupport(CONFIG_O2))
    {
         paramManager.addProvider(*new NeonateProvider());
         paramManager.addParam(o2Param.getInstance());

         limitAlarmSource = new O2LimitAlarm();
         alarmSourceManager.registerLimitAlarmSource(limitAlarmSource, LIMIT_ALARMSOURCE_O2);
         alertor.addLimtSource(limitAlarmSource);
         oneShotAlarmSource = new O2OneShotAlarm();
         alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_O2);
         alertor.addOneShotSource(oneShotAlarmSource);
         O2TrendWidget *o2TrendWidget  = new O2TrendWidget();
         o2Param.setTrendWidget(o2TrendWidget);
         layoutManager.addLayoutWidget(o2TrendWidget, LAYOUT_NODE_PARAM_OXYGEN);
    }
#endif

    // short trend container
    ShortTrendContainer *trendContainer = new ShortTrendContainer;
    layoutManager.addLayoutWidget(trendContainer);

    // 关联设备和参数对象。
    paramManager.connectParamProvider(WORK_MODE_NORMAL);

    // system alarm
    oneShotAlarmSource = new SystemAlarm();
    alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_SYSTEM);
    alertor.addOneShotSource(oneShotAlarmSource);
    int status = 0;
    systemConfig.getNumValue("Alarms|AlarmAudio", status);
    oneShotAlarmSource->setOneShotAlarm(SYSTEM_ONE_SHOT_ALARM_AUDIO_OFF, !status);
}

/**************************************************************************************************
 * 功能： 初始化打印。
 *************************************************************************************************/
static void _initPrint(void)
{
    // 初始化打印。
    // printExec.construction();
    // printManager.construction();
    // alertor.addOneShotSource(printOneShotAlarm.construction());
    // PRT72Provider *prtProvider = new PRT72Provider();
    // printManager.setPrintProviderIFace(prtProvider);
    // printManager.addPrintLayout(new PrintContinuousLayout());
    // printManager.addPrintLayout(new PrintTrendDataLayout());
    // printManager.addPrintLayout(new PrintTriggerSummaryLayout());
    // printManager.addPrintLayout(new PrintViewSummaryLayout());
    // printManager.addPrintLayout(new PrintECG12LeadLayout());
    // printManager.addPrintLayout(new PrintOtherTriggerLayout());
    // printManager.addPrintLayout(new PrintSupervisorCfgLayout());
    // printManager.selftest();
    // paramManager.addProvider(*prtProvider);

    // print alarm
    int index = 0;
    machineConfig.getModuleInitialStatus("PrinterEnable", reinterpret_cast<bool*>(&index));
    if (index)
    {
        AlarmOneShotIFace *oneShotAlarmSource = new PrintOneShotAlarm();
        alarmSourceManager.registerOneShotAlarmSource(oneShotAlarmSource, ONESHOT_ALARMSOURCE_PRINT);
        alertor.addOneShotSource(oneShotAlarmSource);
        PRT48Provider *prtProvider = new PRT48Provider();
        recorderManager.setPrintPrividerIFace(prtProvider);
        recorderManager.selfTest();
        recorderManager.printWavesInit();
        paramManager.addProvider(prtProvider);
    }

    paramManager.getVersion();
}

/**************************************************************************************************
 * 功能： 初始化菜单。
 *************************************************************************************************/
static void _initMenu(void)
{
    //其它弹出菜单初始化
    patientManager.getInstance();
}

/**************************************************************************************************
 * 功能： 初始化所有。
 *************************************************************************************************/
void newObjects(void)
{
    _initSystem();
    _initWindowManager();
    _initComponents();
    _initProviderParam();
    _initPrint();
    _initMenu();
}

/**************************************************************************************************
 * 功能： 退出运行。
 *************************************************************************************************/
void deleteObjects(void)
{
    // 优先析构打印管理实例
    deleteRecorderManager();

    //    deleteWaveWidgetSelectMenu();
    // deletePatientMenu();
    deleteParamManager();
    deleteTimeDate();
    deleteMachineConfig();
    deleteDataStorageDirManager();
    deleteSystemConfig();
    // deleteSuperConfig();
    deleteSuperRunConfig();
    deleteSystemTick();
    deleteKeyActionManager();

//    deleteNetworkManager();
    deleteUsbManager();

    deleteColorManager();
    deleteErrorCatch();
}
