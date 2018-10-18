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

#include "OxyCRGRRHRWaveWidget.h"
#include "OxyCRGTrendWaveWidget.h"
#include "OxyCRGSPO2TrendWidget.h"
#include "OxyCRGCO2WaveWidget.h"
#include "OxyCRGRESPWaveWidget.h"


#include "IConfig.h"
#include "SoftKeyManager.h"
#include "KeyActionManager.h"
#include "NormalModeKeyAction.h"
#include "WindowManager.h"
#include "MenuManager.h"
#include "SupervisorMenuManager.h"
#include "KeyBoardPanel.h"
#include "NumberPanel.h"
#include "SetWidget.h"
#include "NellcorProvider.h"

#include "DataStorageDirManager.h"
#include "SoundManager.h"
#include "NetworkManager.h"
#include "SystemTick.h"
#include "UserMaintainManager.h"
#include "FactoryMaintainManager.h"
#include "WifiMaintainMenu.h"
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
#include "AlarmStateMachine.h"

#include "ParamManager.h"
#include "DemoProvider.h"
#include "N5Provider.h"
#include "SuntechProvider.h"
#include "S5Provider.h"
#include "MasimoProvider.h"
#include "T5Provider.h"
#include "BLMCO2Provider.h"
#include "NIBPParam.h"
#include "NIBPAlarm.h"
#include "NIBPTrendWidget.h"
#include "NIBPDataTrendWidget.h"

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

#include "E5Provider.h"
#include "RawDataCollection.h"
#include "RawDataCollectionTxt.h"

#include "USBManager.h"

#include "ActivityLogManager.h"

#include "PMessageBox.h"

#include "OxyCRGWidget.h"
#include "OxyCRGHRWidget.h"
#include "OxyCRGRESPWidget.h"
#include "OxyCRGRRWidget.h"
#include "OxyCRGSPO2Widget.h"

#include "TopBarWidget.h"
#include "NightStatusBar.h"

// 构造所有的对象。
extern void newObjects(void);

// 删除对象。
extern void deleteObjects(void);
