/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/2
 **/


#pragma once

#include "IThread.h"
#include "ErrorLog.h"

#include "IConfig.h"
#include "SoftKeyManager.h"
#include "KeyActionManager.h"
#include "NormalModeKeyAction.h"
#include "WindowManager.h"
#include "WaveWidgetSelectMenu.h"
#include "MenuManager.h"
#include "PublicMenuManager.h"
#include "SelectStarterLogo.h"
#include "SupervisorMenuManager.h"
#include "KeyBoardPanel.h"
#include "NumberPanel.h"
#include "SetWidget.h"
#include "ConfigEditUnitSetupMenu.h"
#include "NellcorProvider.h"

#include "DataStorageDirManager.h"
#include "SoundManager.h"
#include "NetworkManager.h"
#include "ServiceErrorLogEntrance.h"
#include "SystemTick.h"
#include "SupervisorEntrance.h"
#include "FactoryImportExportMenu.h"
#include "FactoryDataRecord.h"
#include "FactoryTempMenu.h"
#include "FactoryTestMenu.h"
#include "FactorySystemInfoMenu.h"
#include "ModuleMaintainMenu.h"
#include "SoftWareVersion.h"
#include "NIBPCalibrationMenu.h"
#include "UserMaintainManager.h"
#include "ServiceUpdateEntrance.h"
#include "FactoryMaintainManager.h"
#include "MachineConfigModule.h"
#include "UserMaintainGeneralSet.h"
#include "AlarmMaintainMenu.h"
#include "WifiMaintainMenu.h"
#include "WiredWorkMaintainMenu.h"
#include "OthersMaintainMenu.h"
#include "MonitorInfo.h"
#include "SystemStatusBarWidget.h"
#include "SystemModeBarWidget.h"
#include "SystemManager.h"
#include "SystemBoardProvider.h"
#include "SystemAlarm.h"
#include "LightManager.h"
#include "BatteryMessage.h"
#include "BatteryAlarm.h"
#include "PowerManager.h"
#include "PromptInfoBarWidget.h"

#include "PatientManager.h"
#include "PatientInfoWidget.h"
#include "PatientBarWidget.h"
#include "PatientStatusBarWidget.h"

#include "TimeDate.h"
#include "TimeManager.h"
#include "DateTimeWidget.h"
#include "ElapseTimeWidget.h"

#include "WaveformCache.h"
#include "TrendCache.h"
#include "ParamDataStorageManager.h"
#include "TrendDataStorageManager.h"
#include "TrendRescueRangePrint.h"
#include "RescueDataExportWidget.h"
#include "RescueDataDeleteWidget.h"


#include "EventStorageManager.h"

#include "PRT48Provider.h"
#include "PrintAlarm.h"

#include "RecorderManager.h"

#include "Alarm.h"
#include "AlarmPhyInfoBarWidget.h"
#include "AlarmTechInfoBarWidget.h"
#include "AlarmMuteBarWidget.h"
#include "AlarmLimitMenu.h"
#include "AlarmStateMachine.h"

#include "ParamManager.h"
#include "DemoProvider.h"
#include "TN3Provider.h"
#include "SuntechProvider.h"
#include "BLMNIBPProvider.h"
#include "TS3Provider.h"
#include "S5Provider.h"
#include "BLMSPO2Provider.h"
#include "MasimoProvider.h"
#include "TT3Provider.h"
#include "T5Provider.h"
#include "BLMCO2Provider.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPTrendWidget.h"
#include "NIBPDataTrendWidget.h"
#include "WaveTrendWidget.h"

#include "CO2Param.h"
#include "CO2Alarm.h"
#include "CO2WaveWidget.h"
#include "CO2TrendWidget.h"
#include "CO2BRTrendWidget.h"

#include "WitleafProvider.h"
#include "IBPAlarm.h"
#include "IBPParam.h"
#include "IBPWaveWidget.h"
#include "IBPTrendWidget.h"
#include "IBPMenu.h"

#include "COParam.h"
#include "COAlarm.h"
#include "COTrendWidget.h"
#include "COParam.h"

#include "PhaseinProvider.h"
#include "AGAlarm.h"
#include "AGParam.h"
#include "AGWaveWidget.h"
#include "AGTrendWidget.h"

#include "SPO2Param.h"
#include "SPO2Alarm.h"
#include "SPO2WaveWidget.h"
#include "SPO2TrendWidget.h"
#include "ShortTrendManager.h"

#include "TEMPParam.h"
#include "TEMPAlarm.h"
#include "TEMPTrendWidget.h"

#include "DataDispatcher.h"

#include "RESPDupParam.h"
#include "RESPDupAlarm.h"
#include "RESPParam.h"
#include "RESPAlarm.h"
#include "RESPWaveWidget.h"
#include "RESPTrendWidget.h"

#include "ECGDupAlarm.h"
#include "ECGDupParam.h"
#include "ECGParam.h"
#include "ECGAlarm.h"
#include "ECGWaveWidget.h"
#include "ECGTrendWidget.h"
#include "ECGPVCSTrendWidget.h"
#include "ECGSTTrendWidget.h"
#include "ECGMenu.h"

#include "E5Provider.h"
#include "RawDataCollection.h"
#include "RawDataCollectionTxt.h"

#include "SupervisorConfigManager.h"
#include "SupervisorGeneralSetMenu.h"
#include "SupervisorPortMenu.h"
#include "SupervisorNIBPMenu.h"
#include "SupervisorPrintMenu.h"
#include "Supervisor12LMenu.h"
#include "SupervisorTimeMenu.h"
#include "SupervisorCodeMarker.h"
#include "SupervisorLocalMenu.h"
#include "SupervisorAlarmLimitMenu.h"
#include "SupervisorAlarmMenu.h"
#include "SupervisorWifiMenu.h"
#include "SupervisorMailMenu.h"

#include "USBManager.h"

#include "ActivityLogManager.h"

#include "CodeMarkerWidget.h"


#include "PMessageBox.h"

#include "OxyCRGWidget.h"
#include "OxyCRGHRWidget.h"
#include "OxyCRGRESPWidget.h"
#include "OxyCRGRRWidget.h"
#include "OxyCRGSPO2Widget.h"

#include "TopBarWidget.h"

// 构造所有的对象。
extern void newObjects(void);

// 删除对象。
extern void deleteObjects(void);
