#-------------------------------------------------
#
# Project created by QtCreator 2015-06-17T11:48:33
#
#-------------------------------------------------

QT       += core gui xml network svg
DEFINES += QT_NO_EXCEPTIONS

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#build.commands += ($$PWD/Scripts/copyFiles.sh $$PWD)
#QMAKE_EXTRA_TARGETS += build
#PRE_TARGETDEPS += build

#版本及编译时间
#versioninfo.commands += ($$PWD/Scripts/version.sh $$PWD)
#QMAKE_EXTRA_TARGETS += versioninfo
#PRE_TARGETDEPS += versioninfo

#use git version
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

# Build as an application
TARGET = nPM
TEMPLATE = app

QMAKE_LFLAGS = -rdynamic
#QMAKE_CXXFLAGS = -DUSE_NEW_STORAGE

debug {
QMAKE_CXXFLAGS -= -Os
}

# Auto-generated files subdirectories
UI_DIR = ui
MOC_DIR = moc
OBJECTS_DIR = obj
DESTDIR = $$PWD/nPM/bin

# Install path
target.path = /usr/local/nPM/bin
INSTALLS += target

# Wifi Wpa interface setting
DEFINES += CONFIG_CTRL_IFACE
DEFINES += CONFIG_CTRL_IFACE_UNIX

#support screen capture
DEFINES += CONFIG_CAPTURE_SCREEN

#used by ECG algorithm team, default closed
#DEFINES += CONFIG_ECG_TEST

#用于隐藏待机功能
DEFINES += HIDE_STANDBY_FUNCTION
DEFINES += HIDE_PARAM_SWITCH

#用于隐藏ST功能
DEFINES += HIDE_ECG_ST_PVCS_SUBPARAM

#隐藏IBP校零软按键
DEFINES += HIDE_IBP_CALIBRATE_ZERO

#隐藏ECG心率失常功能
DEFINES += HIDE_ECG_ARRHYTHMIA_FUNCTION

#去使能报警栓锁功能
DEFINES += DISABLED_ALARM_LATCH

#取消报警音关闭和报警暂停功能
DEFINES += CLOSE_ALARM_AUDIO_OFF_ALARM_OFF

#关闭报警维护菜单中无用的功能接口
DEFINES += CLOSE_USELESS_ALARM_FUNCTION

#隐藏计算功能
DEFINES += HIDE_CALCULATE_FUNCITON

#隐藏呼吸氧和窗口
DEFINES += HIDE_MONITOR_OXYCRG

#隐藏趋势窗口
DEFINES += HIDE_MONITOR_TREND

#隐藏呼吸氧合回顾功能
DEFINES += HIDE_OXYCRG_REVIEW_FUNCTION

#隐藏有线网络功能
DEFINES += HIDE_WIRED_NETWORK_FUNCTION

#隐藏无线网络功能
DEFINES += HIDE_WIFI_FUNCTION

#隐藏护士呼叫功能
#DEFINES += HIDE_NURSE_CALL_FUNCTION

#隐藏ECG12导功能
DEFINES += HIDE_ECG_12_LEAD_FUNCTION

#隐藏机器配置中的部分选项
DEFINES += HIDE_MACHINE_CONFIG_ITEMS

#失能co2模块维护
DEFINES += DISABLE_CO2_MODULE_MAINTAIN

#使能氧浓度,窒息唤醒模块功能
#DEFINES += ENABLE_O2_APNEASTIMULATION

#隐藏其它维护菜单中的部分选项
DEFINES += HIDE_OTHER_MAINTAIN_ITEMS

#失能工厂维护中测试菜单里的item
DEFINES += DISABLE_FACTORY_MODULE_SELFTEST

#失能ecg模块校准
DEFINES += DISABLE_ECG_MODULE_CALIBRATION

#大字体界面，co2模块替换resp模块
DEFINES += BIG_FONT_LAYOUT_CO2_REPLACE_RESP

#隐藏界面布局功能
DEFINES += HIDE_SCREEN_LAYOUT

#支持ECG监护模式下，关闭陷波滤波
DEFINES += ECG_MONITOR_NOTIFY_FILTER_OFF

# Depending libraries
LIBS += -ldl -lasound -lz

linux-cortexa9hf-vfp-neon-gnueabi-g++ {
    LIBS += -lts -L$$PWD/nPM/lib/
}

linux-arm-g++ {
    LIBS += -lts -L$$PWD/nPM/lib/
}

linux-g++ {
    LIBS += -lX11 -L$$PWD/nPM/libamd64/
}

OTHER_FILES +=                                                                  \
    nPM/etc/Machine.xml                                                         \
    nPM/etc/System.Original.xml                                                 \
    nPM/etc/Supervision.Original.Original.xml                                   \
    nPM/locale/ChineseSimplified.xml                                            \
    nPM/locale/English.xml                                                      \
    nPM/locale/PrintEnglish.xml                                                 \
    nPM/etc/AdultConfig.Original.xml                                            \
    nPM/etc/PedConfig.Original.xml                                              \
    nPM/etc/NeoConfig.Original.xml                                              \
    nPM/etc/usercheck.xml                                                       \
    nPM/etc/PatientInfo.xml                                                     \

SOURCES +=                                                                      \
    SOUP/Json/json_parser.cc                                                    \
    SOUP/Json/json_scanner.cc                                                   \
    SOUP/Json/json_scanner.cpp                                                  \
    SOUP/Json/parser.cpp                                                        \
    SOUP/Json/parserrunnable.cpp                                                \
    SOUP/Json/qobjecthelper.cpp                                                 \
    SOUP/Json/serializer.cpp                                                    \
    SOUP/Json/serializerrunnable.cpp                                            \
    Utility/Utility.cpp                                                         \
    Utility/Config/Config.cpp                                                   \
    Utility/Config/XmlParser.cpp                                                \
    Utility/Config/IConfig.cpp                                                  \
    Utility/DataStorage/CircleFile.cpp                                          \
    Utility/DataStorage/DataStorageDirManager.cpp                               \
    Utility/DataStorage/StorageFile.cpp                                         \
    Utility/DataStorage/StorageManager.cpp                                      \
    Utility/DataStorage/DataStorageDirManagerInterface.cpp                      \
    Utility/ErrorLog/ErrorLogInterface.cpp                                      \
    Utility/ErrorLog/ErrorLog.cpp                                               \
    Utility/ErrorLog/ErrorLogItem.cpp                                           \
    Utility/ErrorLog/ErrorLogViewer.cpp                                         \
    Utility/ErrorLog/ErrorLogViewerWindow.cpp                                   \
    Utility/TimeDate/TimeDate.cpp                                               \
    Utility/Uart/Uart.cpp                                                       \
    Utility/Uart/UartSocket.cpp                                                 \
    Utility/Debug/Debug.cpp                                                     \
    Utility/LanguageManager/LanguageManager.cpp                                 \
    Utility/FontManager/FontManager.cpp                                         \
    Utility/FontManager/FontManagerInterface.cpp                                \
    Utility/ColorManager/ColorManager.cpp                                       \
    Utility/NightModeManager/NightModeManager.cpp                               \
    Utility/Widgets/Base/ShadowEffect.cpp                                       \
    Utility/Widgets/Base/ThemeManager.cpp                                       \
    Utility/Widgets/Base/Button.cpp                                             \
    Utility/Widgets/Base/Frame.cpp                                              \
    Utility/Widgets/Base/FrameItem.cpp                                          \
    Utility/Widgets/Base/ComboBox.cpp                                           \
    Utility/Widgets/Base/PopupList.cpp                                          \
    Utility/Widgets/Base/PopupListItem.cpp                                      \
    Utility/Widgets/Base/PopupNumEditor.cpp                                     \
    Utility/Widgets/Base/TableView.cpp                                          \
    Utility/Widgets/Base/TableHeaderView.cpp                                    \
    Utility/Widgets/Base/TableViewItemDelegate.cpp                              \
    Utility/Widgets/Base/ListView.cpp                                           \
    Utility/Widgets/Base/ListDataModel.cpp                                      \
    Utility/Widgets/Base/ListViewItemDelegate.cpp                               \
    Utility/Widgets/Base/MenuWindow.cpp                                         \
    Utility/Widgets/Base/MenuSidebar.cpp                                        \
    Utility/Widgets/Base/MenuSidebarItem.cpp                                    \
    Utility/Widgets/Base/MenuContent.cpp                                        \
    Utility/Widgets/Base/ScrollArea.cpp                                         \
    Utility/Widgets/Base/FloatScrollBar.cpp                                     \
    Utility/Widgets/Base/KineticScroller/qkineticscroller.cpp                   \
    Utility/Widgets/Base/KineticScroller/qscrollareakineticscroller.cpp         \
    Utility/Widgets/Base/PasswordWidget.cpp                                     \
    Utility/Widgets/Base/SpinBox.cpp                                            \
    Utility/Widgets/Base/DropList.cpp                                           \
    Utility/Widgets/Base/PopupMoveEditor.cpp                                    \
    Utility/Widgets/Base/MoveButton.cpp                                         \
    Utility/Widgets/Base/Dialog.cpp                                             \
    Utility/Widgets/IComboList/ComboList.cpp                                    \
    Utility/Widgets/IComboList/ComboListPopup.cpp                               \
    Utility/Widgets/IComboList/ComboListItem.cpp                                \
    Utility/Widgets/IComboList/IComboList.cpp                                   \
    Utility/Widgets/IComboList/CombolistWidget.cpp                              \
    Utility/Widgets/InputMethod/EnglishInputPanel.cpp                           \
    Utility/Widgets/InputMethod/Key.cpp                                         \
    Utility/Widgets/InputMethod/KeyBoardPanel.cpp                               \
    Utility/Widgets/InputMethod/KeyInputPanel.cpp                               \
    Utility/Widgets/InputMethod/NumberPanel.cpp                                 \
    Utility/Widgets/OxyCRGWidget/OxyCRGTrendWaveWidget.cpp                      \
    Utility/Widgets/OxyCRGWidget/OxyCRGWidget.cpp                               \
    Utility/Widgets/OxyCRGWidget/OxyCRGRESPWaveWidget.cpp                       \
    Utility/Widgets/OxyCRGWidget/OxyCRGCO2WaveWidget.cpp                        \
    Utility/Widgets/OxyCRGWidget/OxyCRGRRHRWaveWidget.cpp                       \
    Utility/Widgets/OxyCRGWidget/OxyCRGSPO2TrendWidget.cpp                      \
    Utility/Widgets/OxyCRGWidget/OxyCRGWidgetLabel.cpp                          \
    Utility/Widgets/OxyCRGWidget/OxyCRGSetupWindow.cpp                          \
    Utility/Widgets/Trendform/TrendCanvasWidget.cpp                             \
    Utility/Widgets/Trendform/ShortTrendItem.cpp                                \
    Utility/Widgets/Trendform/ShortTrendContainer.cpp                           \
    Utility/Widgets/Trendform/ShortTrendWindow.cpp                              \
    Utility/Widgets/TrendWidget/TrendWidget.cpp                                 \
    Utility/Widgets/TrendWidget/TrendWidgetLabel.cpp                            \
    Utility/Widgets/Waveform/FreezeWaveReviewMode.cpp                           \
    Utility/Widgets/Waveform/FreezeTimeIndicator.cpp                            \
    Utility/Widgets/Waveform/WaveCascadeMode.cpp                                \
    Utility/Widgets/Waveform/WaveReviewMode.cpp                                 \
    Utility/Widgets/Waveform/WaveScrollCascadeMode.cpp                          \
    Utility/Widgets/Waveform/WaveWidgetItem.cpp                                 \
    Utility/Widgets/Waveform/WaveNormalMode.cpp                                 \
    Utility/Widgets/Waveform/WaveScanCascadeMode.cpp                            \
    Utility/Widgets/Waveform/WaveScrollMode.cpp                                 \
    Utility/Widgets/Waveform/WaveWidgetLabel.cpp                                \
    Utility/Widgets/Waveform/WaveReviewCascadeMode.cpp                          \
    Utility/Widgets/Waveform/WaveScanMode.cpp                                   \
    Utility/Widgets/Waveform/WaveWidget.cpp                                     \
    Utility/Widgets/MessageBox.cpp                                              \
    Utility/Widgets/ExportDataWidget.cpp                                        \
    Utility/Widgets/FreezeTableWidget.cpp                                       \
    Utility/Widgets/IButton.cpp                                                 \
    Utility/Widgets/IDropList.cpp                                               \
    Utility/Widgets/ILabel.cpp                                                  \
    Utility/Widgets/IListWidget.cpp                                             \
    Utility/Widgets/IPrograssBar.cpp                                            \
    Utility/Widgets/ISpinBox.cpp                                                \
    Utility/Widgets/ITableWidget.cpp                                            \
    Utility/Widgets/LabelButton.cpp                                             \
    Utility/Widgets/MenuGroup.cpp                                               \
    Utility/Widgets/MenuWidget.cpp                                              \
    Utility/Widgets/PMessageBox.cpp                                             \
    Utility/Widgets/PopupWidget.cpp                                             \
    Utility/Widgets/SubMenu.cpp                                                 \
    Utility/Widgets/ImportFileSubWidget.cpp                                     \
    Utility/Widgets/TableItemDelegate.cpp                                       \
    Utility/Widgets/EventWidget/EventWaveWidget.cpp                             \
    Utility/Widgets/EventWidget/EventTrendItemDelegate.cpp                      \
    Utility/Widgets/EventWidget/EventInfoWidget.cpp                             \
    Utility/Widgets/EventWidget/OxyCRGEventWaveWidget.cpp                       \
    Utility/Widgets/EventWidget/EventWindow.cpp                                 \
    Utility/Widgets/EventWidget/EventReviewModel.cpp                            \
    Utility/Widgets/EventWidget/EventWaveSetWindow.cpp                          \
    Utility/Widgets/EventWidget/OxyCRGEventWindow.cpp                           \
    Utility/Widgets/EventWidget/OxyCRGEventSetWindow.cpp                        \
    Utility/ScreenCapture.cpp                                                   \
    Utility/RunLengthEncode.cpp                                                 \
    Utility/ImageQuant.cpp                                                      \
#################################################################################
    Components/System/SystemTick.cpp                                            \
    Components/System/SystemManager.cpp                                         \
    Components/System/SystemManagerInterface.cpp                                \
    Components/System/SystemAlarm.cpp                                           \
    Components/System/TDA19988Ctrl.cpp                                          \
    Components/System/Widgets/SystemModeBarWidget.cpp                           \
    Components/System/Widgets/SystemSelftestMenu.cpp                            \
    Components/System/Widgets/SystemStatusBarWidget.cpp                         \
    Components/System/Widgets/RunningStatusBar.cpp                              \
    Components/System/Widgets/RunningStatusBarInterface.cpp                     \
    Components/System/Widgets/NormalFunctionMenuContent.cpp                     \
    Components/System/Widgets/PrintSettingMenuContent.cpp                       \
    Components/System/Widgets/MeasureSettingMenuContent.cpp                     \
    Components/System/Widgets/MeasureSettingWindow.cpp                          \
    Components/System/Widgets/DataReviewMenuContent.cpp                         \
    Components/System/Widgets/CalculateWindow.cpp                               \
    Components/System/Widgets/SystemMaintenanceMenuContent.cpp                  \
    Components/System/Widgets/StandbyWindow.cpp                                 \
    Components/System/Widgets/PowerOffWindow.cpp                                \
    Components/System/Widgets/NightModeWindow.cpp                               \
    Components/SoundManager/SoundManager.cpp                                    \
    Components/SoundManager/SoundManagerInterface.cpp                           \
    Components/SoundManager/WavFile.cpp                                         \
    Components/SoundManager/WavPlayer.cpp                                       \
    Components/LightManager/LightManager.cpp                                    \
    Components/LightManager/LightManagerInterface.cpp                           \
    Components/ParamManager/Param.cpp                                           \
    Components/ParamManager/ParamManager.cpp                                    \
    Components/ParamManager/ParamInfo.cpp                                       \
    Components/ParamManager/ParamInfoInterface.cpp                              \
    Components/Alarm/Alarm.cpp                                                  \
    Components/Alarm/AlarmInterface.cpp                                         \
    Components/Alarm/AlarmConfig.cpp                                            \
    Components/Alarm/AlarmIndicator.cpp                                         \
    Components/Alarm/AlarmIndicatorInterface.cpp                                \
    Components/Alarm/AlarmParamIFace.cpp                                        \
    Components/Alarm/AlarmLimitModel.cpp                                        \
    Components/Alarm/AlarmStateMachine/AlarmStateMachine.cpp                    \
    Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp           \
    Components/Alarm/AlarmStateMachine/AlarmState.cpp                           \
    Components/Alarm/AlarmStateMachine/AlarmNormalState.cpp                     \
    Components/Alarm/AlarmStateMachine/AlarmPauseState.cpp                      \
    Components/Alarm/AlarmStateMachine/AlarmOffState.cpp                        \
    Components/Alarm/AlarmStateMachine/AlarmAudioOffState.cpp                   \
    Components/Alarm/AlarmStateMachine/AlarmResetState.cpp                      \
    Components/Alarm/Widgets/AlarmInfoPopListView.cpp                           \
    Components/Alarm/Widgets/AlarmLimitMenuContent.cpp                          \
    Components/Alarm/Widgets/AlarmLimitWindow.cpp                               \
    Components/Alarm/Widgets/AlarmStatusWidget.cpp                              \
    Components/Alarm/Widgets/AlarmTechInfoBarWidget.cpp                         \
    Components/Alarm/Widgets/AlarmPhyInfoBarWidget.cpp                          \
    Components/Alarm/Widgets/AlarmInfoBarWidget.cpp                             \
    Components/Alarm/Widgets/AlarmInfoWindow.cpp                                \
    Components/Alarm/Widgets/AlarmInfoModel.cpp                                 \
    Components/Alarm/NurseCallManager.cpp                                       \
    Components/Alarm/NurseCallManagerInterface.cpp                              \
    Components/Calculation/DoseCalculationManager/DoseCalculationManager.cpp    \
    Components/Calculation/DoseCalculationManager/DoseCalculationWindow.cpp     \
    Components/Calculation/DoseCalculationManager/TitrateTableModel.cpp         \
    Components/Calculation/DoseCalculationManager/TitrateTableWindow.cpp        \
    Components/Calculation/DoseCalculationManager/TitrateTableManager.cpp       \
    Components/Calculation/DoseCalculationManager/TitrateTableSetWindow.cpp     \
    Components/Calculation/HemodynamicManager/HemodynamicWindow.cpp             \
    Components/Calculation/HemodynamicManager/HemodynamicManager.cpp            \
    Components/Calculation/HemodynamicManager/HemodynamicReviewWindow.cpp       \
    Components/Calculation/HemodynamicManager/HemodynamicDataModel.cpp          \
    Components/PatientData/WaveformCache.cpp                                    \
    Components/PatientData/ParamDataStorageManager.cpp                          \
    Components/PatientData/RescueDataListWidget.cpp                             \
    Components/PatientData/RescueDataListNewWidget.cpp                          \
    Components/PatientData/RescueDataExportWidget.cpp                           \
    Components/PatientData/TrendCache.cpp                                       \
    Components/PatientData/TrendCacheInterface.cpp                              \
    Components/PatientData/TrendDataUtil.cpp                                    \
    Components/PatientData/RescueDataDeleteWidget.cpp                           \
    Components/PatientData/RescueDataDeleteWindow.cpp                           \
    Components/PatientData/TrendDataStorageManager.cpp                          \
    Components/PatientData/TrendDataStorageManagerInterface.cpp                 \
    Components/PatientData/EventDataParseContext.cpp                            \
    Components/PatientData/EventStorageItem.cpp                                 \
    Components/PatientData/EventStorageManager.cpp                              \
    Components/PatientData/EventStorageManagerInterface.cpp                     \
    Components/PatientData/TrendWaveWidget.cpp                                  \
    Components/PatientData/TrendSubWaveWidget.cpp                               \
    Components/PatientData/IMoveButton.cpp                                      \
    Components/PatientData/TrendTableWindow.cpp                                 \
    Components/PatientData/TrendTableModel.cpp                                  \
    Components/PatientData/TrendTableSetWindow.cpp                              \
    Components/PatientData/TrendPrintWindow.cpp                                 \
    Components/PatientData/TrendGraphWindow.cpp                                 \
    Components/PatientData/TrendGraphSetWindow.cpp                              \
    Components/PatientData/HistoryDataReviewWindow.cpp                          \
    Components/PatientData/HistoryDataSelWindow.cpp                             \
    Components/PatientData/HistoryDataSelModel.cpp                              \
    Components/PatientData/WaveformCacheInterface.cpp                           \
    Components/TimeManager/DateTimeWidget.cpp                                   \
    Components/TimeManager/ElapseTimeWidget.cpp                                 \
    Components/TimeManager/TimeManager.cpp                                      \
    Components/TimeManager/TimeEditWindow.cpp                                   \
    Components/PatientManager/PatientManager.cpp                                \
    Components/PatientManager/PatientInfoWidget.cpp                             \
    Components/PatientManager/PatientManagementMenuContent.cpp                  \
    Components/PatientManager/PatientInfoWindow.cpp                             \
    Components/PatientManager/PatientInfoWindowInterface.cpp                    \
    Components/PatientManager/DischargePatientWindow.cpp                        \
    Components/PowerManager/BatteryAlarm.cpp                                    \
    Components/PowerManager/BatteryIndicatorWindow.cpp                          \
    Components/PowerManager/BatteryBarWidget.cpp                                \
    Components/PowerManager/BatteryIconWidget.cpp                               \
    Components/PowerManager/PowerManager.cpp                                    \
    Components/PowerManager/TestBatteryTime.cpp                                 \
    Components/WindowManager/IWidget.cpp                                        \
    Components/WindowManager/WindowManager.cpp                                  \
    Components/WindowManager/WindowManagerInterface.cpp                         \
    Components/WindowManager/PromptInfoBarWidget.cpp                            \
    Components/WindowManager/MainMenuWindow.cpp                                 \
    Components/WindowManager/FactoryMaintainMenuWindow.cpp                      \
    Components/WindowManager/ConfigManagerMenuWindow.cpp                        \
    Components/WindowManager/UserMaintainMenuWindow.cpp                         \
    Components/WindowManager/ArrhythmiaMenuWindow.cpp                           \
    Components/WindowManager/ScreenMenuContent.cpp                              \
    Components/WindowManager/TopBarWidget.cpp                                   \
    Components/WindowManager/LayoutManager.cpp                                  \
    Components/WindowManager/ScreenLayoutWindow.cpp                             \
    Components/WindowManager/ScreenLayoutModel.cpp                              \
    Components/WindowManager/ScreenLayoutItemDelegate.cpp                       \
    Components/WindowManager/ScreenLayoutEditor.cpp                             \
    Components/WindowManager/ParaColorWindow.cpp                                \
    Components/WindowManager/BigFontLayoutWindow.cpp                            \
    Components/WindowManager/BigFontLayoutModel.cpp                             \
    Components/WindowManager/PasswordWindow.cpp                                 \
    Components/USBManager/DataExporterBase.cpp                                  \
    Components/USBManager/ErrorLogExporter.cpp                                  \
    Components/USBManager/UDiskInspector.cpp                                    \
    Components/USBManager/USBManager.cpp                                        \
    Components/PrintManager/Alarm/PrintAlarm.cpp                                \
    Components/Recorder/RecorderManager.cpp                                     \
    Components/Recorder/RecordPageGenerator.cpp                                 \
    Components/Recorder/ContinuousPageGenerator.cpp                             \
    Components/Recorder/EventPageGenerator.cpp                                  \
    Components/Recorder/TrendTablePageGenerator.cpp                             \
    Components/Recorder/OxyCRGPageGenerator.cpp                                 \
    Components/Recorder/FreezePageGenerator.cpp                                 \
    Components/Recorder/TrendGraphPageGenerator.cpp                             \
    Components/Recorder/TriggerPageGenerator.cpp                                \
    Components/Recorder/RecordPageProcessor.cpp                                 \
    Components/Recorder/EventListPageGenerator.cpp                              \
    Components/KeyHandle/SoftKeyManager.cpp                                     \
    Components/KeyHandle/SoftKeyWidget.cpp                                      \
    Components/KeyHandle/SoftkeyActionBase.cpp                                  \
    Components/KeyHandle/SoftkeyActions/CalculateSoftkeyAction.cpp              \
    Components/KeyHandle/SoftkeyActions/DelRescueDataSoftkeyAction.cpp          \
    Components/KeyHandle/SoftkeyActions/MonitorSoftkeyAction.cpp                \
    Components/KeyHandle/SoftkeyActions/RescueDataSoftKeyAction.cpp             \
    Components/KeyHandle/KeyActionManager/KeyActionManager.cpp                  \
    Components/KeyHandle/KeyActionManager/NormalModeKeyAction.cpp               \
    Components/MenuManager/MenuManager.cpp                                      \
    Components/MenuManager/SetWidget.cpp                                        \
    Components/MenuManager/SupervisorMenuManager.cpp                            \
    Components/NetworkManager/NetworkManager.cpp                                \
    Components/NetworkManager/WpaCtrl/common/wpa_ctrl.c                         \
    Components/NetworkManager/WpaCtrl/utils/os_unix.c                           \
    Components/NetworkManager/WpaCtrl/WpaCtrl.cpp                               \
    Components/NetworkManager/Widgets/LabeledLabel.cpp                          \
    Components/NetworkManager/Widgets/WiFiProfileEditor.cpp                     \
    Components/NetworkManager/Widgets/WiFiProfileWindow.cpp                     \
    Components/NetworkManager/Widgets/WiFiProfileEditorWindow.cpp               \
    Components/ActivityLog/ActivityLogManager.cpp                               \
    Components/Maintain/Supervisor/Menu/Supervisor12LMenuContent.cpp            \
    Components/Maintain/Supervisor/Menu/SupervisorPrintMenuContent.cpp          \
    Components/Maintain/Service/ErrorLog/ServiceErrorLogMenu.cpp                \
    Components/Maintain/Service/ErrorLog/ErrorLogWindow.cpp                     \
    Components/Maintain/Service/ErrorLog/ErrorLogTableModel.cpp                 \
    Components/Maintain/Service/Menu/ServiceWindowManager.cpp                   \
    Components/Maintain/Service/NIBP/NIBPPressureControl.cpp                    \
    Components/Maintain/Service/NIBP/NIBPCalibrate.cpp                          \
    Components/Maintain/Service/NIBP/NIBPManometer.cpp                          \
    Components/Maintain/Service/NIBP/NIBPRepairMenuManager.cpp                  \
    Components/Maintain/Service/NIBP/NIBPZeroPoint.cpp                          \
    Components/Maintain/Service/Temp/ServiceCPUTemp.cpp                         \
    Components/Maintain/Factory/FactoryConfigManager.cpp                        \
    Components/Maintain/FactoryMaintain/FactoryDataRecordContent.cpp            \
    Components/Maintain/Factory/Menu/FactoryTempMenuContent.cpp                 \
    Components/Maintain/Factory/Menu/FactoryTestMenuContent.cpp                 \
    Components/Maintain/Factory/Menu/FactoryCO2MenuContent.cpp                  \
    Components/Maintain/UserMaintain/UserMaintainManager.cpp                    \
    Components/Maintain/UserMaintain/WifiMaintainMenu.cpp                       \
    Components/Maintain/UserMaintain/UserMaintainGeneralMenuContent.cpp         \
    Components/Maintain/UserMaintain/ModuleMaintainMenuContent.cpp              \
    Components/Maintain/UserMaintain/AlarmMaintainMenuContent.cpp               \
    Components/Maintain/UserMaintain/WifiMaintainMenuContent.cpp                \
    Components/Maintain/UserMaintain/OthersMaintainMenuContent.cpp              \
    Components/Maintain/UserMaintain/WiredNetworkMaintainMenuContent.cpp        \
    Components/Maintain/UserMaintain/ErrorLogEntranceContent.cpp                \
    Components/Maintain/UserMaintain/DemoMenuContent.cpp                        \
    Components/Maintain/UserMaintain/NurseCallSetWindow.cpp                     \
    Components/Maintain/FactoryMaintain/FactorySystemInfoMenuContent.cpp        \
    Components/Maintain/FactoryMaintain/FactoryMaintainManager.cpp              \
    Components/Maintain/FactoryMaintain/SoftwareVersionWindow.cpp               \
    Components/Maintain/FactoryMaintain/MonitorInfoWindow.cpp                   \
    Components/Maintain/FactoryMaintain/ServiceUpdateEntranceContent.cpp        \
    Components/Maintain/FactoryMaintain/MachineConfigModuleContent.cpp          \
    Components/Maintain/FactoryMaintain/FactoryVersionInfo.cpp                  \
    Components/Maintain/FactoryMaintain/NIBPCalibrationMenuContent.cpp          \
    Components/Maintain/FactoryMaintain/NIBPCalibrateContent.cpp                \
    Components/Maintain/FactoryMaintain/NIBPManometerContent.cpp                \
    Components/Maintain/FactoryMaintain/NIBPRepairMenuWindow.cpp                \
    Components/Maintain/FactoryMaintain/NIBPZeroPointContent.cpp                \
    Components/Maintain/FactoryMaintain/NIBPPressureControlContent.cpp          \
    Components/Maintain/FactoryMaintain/FactoryImportExportMenuContent.cpp      \
    Components/Maintain/FactoryMaintain/UpgradeManager.cpp                      \
    Components/Maintain/FactoryMaintain/UpgradeWindow.cpp                       \
    Components/Maintain/FactoryMaintain/O2CalibrationMenuContent.cpp            \
    Components/CodeMarker/Widgets/CodeMarkerWindow.cpp                          \
    Components/ConfigManager/ConfigManager.cpp                                  \
    Components/ConfigManager/ConfigManagerInterface.cpp                         \
    Components/ConfigManager/SelectDefaultConfigMenuContent.cpp                 \
    Components/ConfigManager/ConfigManagerPassWordEditMenuContent.cpp           \
    Components/ConfigManager/UserConfigEditMenuContent.cpp                      \
    Components/ConfigManager/PatientTypeSelectWindow.cpp                        \
    Components/ConfigManager/ConfigExportImportMenuContent.cpp                  \
    Components/ConfigManager/ConfigEditMenuWindow.cpp                           \
    Components/ConfigManager/ConfigEditGeneralMenuContent.cpp                   \
    Components/ConfigManager/ConfigEditTEMPMenuContent.cpp                      \
    Components/ConfigManager/ConfigEditEcgMenuContent.cpp                       \
    Components/ConfigManager/ConfigEditRespMenuContent.cpp                      \
    Components/ConfigManager/ConfigEditNIBPMenuContent.cpp                      \
    Components/ConfigManager/ConfigEditSpO2MenuContent.cpp                      \
    Components/ConfigManager/ConfigEditCO2MenuContent.cpp                       \
    Components/ConfigManager/ConfigEditCOMenuContent.cpp                        \
    Components/ConfigManager/ConfigEditCodeMarkerMenuContent.cpp                \
    Components/ConfigManager/ConfigEditAlarmLimitMenuContent.cpp                \
    Components/ConfigManager/ConfigEditAlarmLimitModel.cpp                      \
    Components/ConfigManager/ConfigEditIBPMenuContent.cpp                       \
    Components/ConfigManager/ConfigEditDisplayMenuContent.cpp                   \
    Components/ConfigManager/LoadConfigMenuContent.cpp                          \
    Components/ConfigManager/UnitSetupMenuContent.cpp                           \
    Components/ConfigManager/SaveCurrentConfigMenuContent.cpp                   \
    Components/ConfigManager/SaveUserConfigWindow.cpp                           \
    Components/Freeze/FreezeManager.cpp                                         \
    Components/Freeze/FreezeWindow.cpp                                          \
    Components/Freeze/FreezeDataModel.cpp                                       \
#################################################################################
    Params/AGParam/AGWidgets/AGTrendWidget.cpp                                  \
    Params/AGParam/AGWidgets/AGWaveRuler.cpp                                    \
    Params/AGParam/AGWidgets/AGWaveWidget.cpp                                   \
    Params/AGParam/AGWidgets/AGMenuContent.cpp                                  \
    Params/AGParam/AGWidgets/CO2SetAGMenu.cpp                                   \
    Params/AGParam/AGWidgets/N2OSetAGMenu.cpp                                   \
    Params/AGParam/AGWidgets/HalSetAGMenu.cpp                                   \
    Params/AGParam/AGParam.cpp                                                  \
    Params/AGParam/AGAlarm.cpp                                                  \
    Params/ECGParam/ECGAlarm.cpp                                                \
    Params/ECGParam/ECGDupAlarm.cpp                                             \
    Params/ECGParam/ECGDupParam.cpp                                             \
    Params/ECGParam/ECGDupParamInterface.cpp                                    \
    Params/ECGParam/ECGParam.cpp                                                \
    Params/ECGParam/ECGParamInterface.cpp                                       \
    Params/ECGParam/ECGWidgets/ECGWaveWidget.cpp                                \
    Params/ECGParam/ECGWidgets/ECGWaveRuler.cpp                                 \
    Params/ECGParam/ECGWidgets/ECGTrendWidget.cpp                               \
    Params/ECGParam/ECGWidgets/ECGPVCSTrendWidget.cpp                           \
    Params/ECGParam/ECGWidgets/ECGSTTrendWidget.cpp                             \
    Params/ECGParam/ECGWidgets/ECGMenuContent.cpp                               \
    Params/ECGParam/ECGWidgets/ArrhythmiaAnalysisMenu.cpp                       \
    Params/ECGParam/ECGWidgets/ArrhythmiaThresholdMenu.cpp                      \
    Params/CO2Param/CO2Param.cpp                                                \
    Params/CO2Param/CO2Alarm.cpp                                                \
    Params/CO2Param/CO2Widgets/CO2WaveWidget.cpp                                \
    Params/CO2Param/CO2Widgets/CO2WaveRuler.cpp                                 \
    Params/CO2Param/CO2Widgets/CO2TrendWidget.cpp                               \
    Params/CO2Param/CO2Widgets/CO2MenuContent.cpp                               \
    Params/IBPParam/IBPParam.cpp                                                \
    Params/IBPParam/IBPWidgets/IBPWaveWidget.cpp                                \
    Params/IBPParam/IBPWidgets/IBPTrendWidget.cpp                               \
    Params/IBPParam/IBPWidgets/IBPWaveRuler.cpp                                 \
    Params/IBPParam/IBPWidgets/IBPMenuContent.cpp                               \
    Params/IBPParam/IBPAlarm.cpp                                                \
    Params/COParam/COAlarm.cpp                                                  \
    Params/COParam/COParam.cpp                                                  \
    Params/COParam/COWidgets/COTrendWidget.cpp                                  \
    Params/COParam/COWidgets/COMenuContent.cpp                                  \
    Params/NIBPParam/NIBPStateMachine/NIBPState.cpp                             \
    Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.cpp                      \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorErrorState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorGetResultState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorMeasureState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorSafeWaitTimeState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStandbyState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStartingState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStopState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStateMachine.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceCalibrateState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceManometerState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServicePressureControlState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStandbyState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStateMachine.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceZeroPointState.cpp \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceErrorState.cpp \
    Params/NIBPParam/NIBPParam.cpp                                              \
    Params/NIBPParam/NIBPParamInterface.cpp                                     \
    Params/NIBPParam/NIBPAlarm.cpp                                              \
    Params/NIBPParam/NIBPWidgets/NIBPTrendWidget.cpp                            \
    Params/NIBPParam/NIBPWidgets/NIBPDataTrendWidget.cpp                        \
    Params/NIBPParam/NIBPWidgets/NIBPMenuContent.cpp                            \
    Params/NIBPParam/NIBPTrigger/NIBPCountdownTime.cpp                          \
    Params/NIBPParam/NIBPTrigger/NIBPCountdownTimeInterface.cpp                 \
    Params/SPO2Param/SPO2Param.cpp                                              \
    Params/SPO2Param/SPO2Alarm.cpp                                              \
    Params/SPO2Param/SPO2Widgets/SPO2WaveWidget.cpp                             \
    Params/SPO2Param/SPO2Widgets/SPO2TrendWidget.cpp                            \
    Params/SPO2Param/SPO2Widgets/SPO2BarWidget.cpp                              \
    Params/SPO2Param/SPO2Widgets/SPO2MenuContent.cpp                            \
    Params/SPO2Param/SPO2Widgets/CCHDWindow.cpp                                 \
    Params/SPO2Param/SPO2Widgets/CCHDDataModel.cpp                              \
    Params/TEMPParam/TEMPParam.cpp                                              \
    Params/TEMPParam/TEMPAlarm.cpp                                              \
    Params/TEMPParam/TEMPWidgets/TEMPTrendWidget.cpp                            \
    Params/TEMPParam/TEMPWidgets/TEMPMenuContent.cpp                            \
    Params/RESPParam/RESPAlarm.cpp                                              \
    Params/RESPParam/RESPDupAlarm.cpp                                           \
    Params/RESPParam/RESPDupParam.cpp                                           \
    Params/RESPParam/RESPParam.cpp                                              \
    Params/RESPParam/RESPWidgets/RESPWaveWidget.cpp                             \
    Params/RESPParam/RESPWidgets/RESPTrendWidget.cpp                            \
    Params/RESPParam/RESPWidgets/RESPMenuContent.cpp                            \
    Params/O2Param/O2Widgets/O2TrendWidget.cpp                                  \
    Params/O2Param/O2Widgets/O2MenuContent.cpp                                  \
    Params/O2Param/O2Widgets/ApneaStimulationMenuContent.cpp                    \
    Params/O2Param/O2Param.cpp                                                  \
    Params/O2Param/O2Alarm.cpp                                                  \
    Params/O2Param/O2ParamInterface.cpp                                         \
#################################################################################
    Providers/Provider.cpp                                                      \
    Providers/BLMProvider.cpp                                                   \
    Providers/PhaseinProvider/PhaseinProvider.cpp                               \
    Providers/BLMCO2Provider/BLMCO2Provider.cpp                                 \
    Providers/WitleafProvider/WitleafProvider.cpp                               \
    Providers/MasimoProvider/MasimoProvider.cpp                                 \
    Providers/RainbowProvider/RainbowProvider.cpp                               \
    Providers/NellcorProvider/NellcorProvider.cpp                               \
    Providers/E5Provider/E5Provider.cpp                                         \
    Providers/T5Provider/T5Provider.cpp                                         \
    Providers/DemoProvider/DemoProvider.cpp                                     \
    Providers/N5Provider/N5Provider.cpp                                         \
    Providers/PRT48Provider/PRT48Provider.cpp                                   \
    Providers/SuntechProvider/SuntechProvider.cpp                               \
    Providers/SystemBoardProvider/SystemBoardProvider.cpp                       \
    Providers/RawData/RawDataCollector.cpp                                      \
    Providers/S5Provider/S5Provider.cpp                                         \
    Providers/DataDispatcher.cpp                                                \
    Providers/NeonateProvider/NeonateProvider.cpp                               \
#################################################################################
    Main/App/AppMain.cpp                                                        \
    Main/App/Init.cpp                                                           \
    Main/IApplication.cpp                                                       \
    Main/IThread.cpp                                                            \
    Main/MergeConfig.cpp                                                        \
    Main/Starter.cpp                                                            \
    Components/TimeManager/TimeManagerInterface.cpp                             \
    Components/Alarm/AlarmSourceManager.cpp

HEADERS +=                                                                      \
    SOUP/Json/FlexLexer.h                                                       \
    SOUP/Json/json_parser.hh                                                    \
    SOUP/Json/json_parser.yy                                                    \
    SOUP/Json/json_scanner.h                                                    \
    SOUP/Json/json_scanner.yy                                                   \
    SOUP/Json/location.hh                                                       \
    SOUP/Json/parser.h                                                          \
    SOUP/Json/parser_p.h                                                        \
    SOUP/Json/parserrunnable.h                                                  \
    SOUP/Json/position.hh                                                       \
    SOUP/Json/qjson_debug.h                                                     \
    SOUP/Json/qjson_export.h                                                    \
    SOUP/Json/qobjecthelper.h                                                   \
    SOUP/Json/serializer.h                                                      \
    SOUP/Json/serializerrunnable.h                                              \
    SOUP/Json/stack.hh                                                          \
    Utility/OrderedMap.h                                                        \
    Utility/Utility.h                                                           \
    Utility/Config/Config.h                                                     \
    Utility/Config/ConfigDefine.h                                               \
    Utility/Config/IConfig.h                                                    \
    Utility/Config/XmlParser.h                                                  \
    Utility/DataStorage/DataStorageDefine.h                                     \
    Utility/DataStorage/IStorageBackend.h                                       \
    Utility/DataStorage/CircleFile.h                                            \
    Utility/DataStorage/StorageFile.h                                           \
    Utility/DataStorage/DataStorageDirManager.h                                 \
    Utility/DataStorage/StorageManager.h                                        \
    Utility/DataStorage/DataStorageDirManagerInterface.h                        \
    Utility/ErrorLog/ErrorLogInterface.h                                        \
    Utility/ErrorLog/ErrorLog.h                                                 \
    Utility/ErrorLog/ErrorLogItem.h                                             \
    Utility/ErrorLog/ErrorLogViewer.h                                           \
    Utility/ErrorLog/ErrorLogViewerWindow.h                                     \
    Utility/TimeDate/TimeDate.h                                                 \
    Utility/TimeDate/TimeDefine.h                                               \
    Utility/TimeDate/TimeSymbol.h                                               \
    Utility/Uart/Uart.h                                                         \
    Utility/Uart/UartSocketDefine.h                                             \
    Utility/Uart/UartSocket.h                                                   \
    Utility/BaseDefine.h                                                        \
    Utility/UnitManager.h                                                       \
    Utility/FloatHandle/FloatHandle.h                                           \
    Utility/Debug/Debug.h                                                       \
    Utility/RingBuff/RingBuff.h                                                 \
    Utility/LanguageManager/LanguageManager.h                                   \
    Utility/FontManager/FontManager.h                                           \
    Utility/FontManager/FontManagerInterface.h                                  \
    Utility/ColorManager/ColorManager.h                                         \
    Utility/NightModeManager/NightModeManager.h                                 \
    Utility/Widgets/Base/ShadowEffect.h                                         \
    Utility/Widgets/Base/ThemeManager.h                                         \
    Utility/Widgets/Base/Button.h                                               \
    Utility/Widgets/Base/Frame.h                                                \
    Utility/Widgets/Base/FrameItem.h                                            \
    Utility/Widgets/Base/ComboBox.h                                             \
    Utility/Widgets/Base/PopupList.h                                            \
    Utility/Widgets/Base/PopupListItem.h                                        \
    Utility/Widgets/Base/ItemEditInfo.h                                         \
    Utility/Widgets/Base/PopupNumEditor.h                                       \
    Utility/Widgets/Base/TableView.h                                            \
    Utility/Widgets/Base/TableHeaderView.h                                      \
    Utility/Widgets/Base/TableViewItemDelegate.h                                \
    Utility/Widgets/Base/TableViewItemDelegate_p.h                              \
    Utility/Widgets/Base/ListView.h                                             \
    Utility/Widgets/Base/ListDataModel.h                                        \
    Utility/Widgets/Base/ListViewItemDelegate.h                                 \
    Utility/Widgets/Base/MenuWindow.h                                           \
    Utility/Widgets/Base/MenuSidebar.h                                          \
    Utility/Widgets/Base/MenuSidebarItem.h                                      \
    Utility/Widgets/Base/MenuContent.h                                          \
    Utility/Widgets/Base/ScrollArea.h                                           \
    Utility/Widgets/Base/FloatScrollBar.h                                       \
    Utility/Widgets/Base/KineticScroller/qkineticscroller.h                     \
    Utility/Widgets/Base/KineticScroller/qkineticscroller_p.h                   \
    Utility/Widgets/Base/KineticScroller/qscrollareakineticscroller.h           \
    Utility/Widgets/Base/PasswordWidget.h                                       \
    Utility/Widgets/Base/SpinBox.h                                              \
    Utility/Widgets/Base/DropList.h                                             \
    Utility/Widgets/Base/PopupMoveEditor.h                                      \
    Utility/Widgets/Base/MoveButton.h                                           \
    Utility/Widgets/Base/Dialog.h                                               \
    Utility/Widgets/IComboList/ComboList.h                                      \
    Utility/Widgets/IComboList/ComboListPopup.h                                 \
    Utility/Widgets/IComboList/ComboListItem.h                                  \
    Utility/Widgets/IComboList/IComboList.h                                     \
    Utility/Widgets/IComboList/CombolistWidget.h                                \
    Utility/Widgets/InputMethod/EnglishInputPanel.h                             \
    Utility/Widgets/InputMethod/Key.h                                           \
    Utility/Widgets/InputMethod/KeyBoardPanel.h                                 \
    Utility/Widgets/InputMethod/KeyInputPanel.h                                 \
    Utility/Widgets/InputMethod/NumberPanel.h                                   \
    Utility/Widgets/OxyCRGWidget/OxyCRGTrendWaveWidget.h                        \
    Utility/Widgets/OxyCRGWidget/OxyCRGTrendWaveWidget_p.h                      \
    Utility/Widgets/OxyCRGWidget/OxyCRGWidget.h                                 \
    Utility/Widgets/OxyCRGWidget/OxyCRGRESPWaveWidget.h                         \
    Utility/Widgets/OxyCRGWidget/OxyCRGRRHRWaveWidget.h                         \
    Utility/Widgets/OxyCRGWidget/OxyCRGSPO2TrendWidget.h                        \
    Utility/Widgets/OxyCRGWidget/OxyCRGCO2WaveWidget.h                          \
    Utility/Widgets/OxyCRGWidget/OxyCRGSetupWindow.h                            \
    Utility/Widgets/OxyCRGWidget/OxyCRGWidgetLabel.h                            \
    Utility/Widgets/OxyCRGWidget/OxyCRGSymbol.h                                 \
    Utility/Widgets/OxyCRGWidget/OxyCRGDefine.h                                 \
    Utility/Widgets/Trendform/TrendCanvasWidget.h                               \
    Utility/Widgets/Trendform/ShortTrendItem.h                                  \
    Utility/Widgets/Trendform/ShortTrendContainer.h                             \
    Utility/Widgets/Trendform/ShortTrendWindow.h                                \
    Utility/Widgets/TrendWidget/TrendWidget.h                                   \
    Utility/Widgets/TrendWidget/TrendWidgetLabel.h                              \
    Utility/Widgets/Waveform/FreezeWaveReviewMode.h                             \
    Utility/Widgets/Waveform/FreezeTimeIndicator.h                              \
    Utility/Widgets/Waveform/WaveCascadeMode.h                                  \
    Utility/Widgets/Waveform/WaveMode.h                                         \
    Utility/Widgets/Waveform/WaveReviewMode.h                                   \
    Utility/Widgets/Waveform/WaveScrollCascadeMode.h                            \
    Utility/Widgets/Waveform/WaveWidgetItem.h                                   \
    Utility/Widgets/Waveform/WaveDataModel.h                                    \
    Utility/Widgets/Waveform/WaveNormalMode.h                                   \
    Utility/Widgets/Waveform/WaveScanCascadeMode.h                              \
    Utility/Widgets/Waveform/WaveScrollMode.h                                   \
    Utility/Widgets/Waveform/WaveWidgetLabel.h                                  \
    Utility/Widgets/Waveform/WaveDefine.h                                       \
    Utility/Widgets/Waveform/WaveReviewCascadeMode.h                            \
    Utility/Widgets/Waveform/WaveScanMode.h                                     \
    Utility/Widgets/Waveform/WaveWidget.h                                       \
    Utility/Widgets/ExportDataWidget.h                                          \
    Utility/Widgets/FreezeTableWidget.h                                         \
    Utility/Widgets/IButton.h                                                   \
    Utility/Widgets/IDropList.h                                                 \
    Utility/Widgets/ILabel.h                                                    \
    Utility/Widgets/IListWidget.h                                               \
    Utility/Widgets/IMessageBox.h                                               \
    Utility/Widgets/MessageBox.h                                                \
    Utility/Widgets/IPrograssBar.h                                              \
    Utility/Widgets/ISpinBox.h                                                  \
    Utility/Widgets/ITableWidget.h                                              \
    Utility/Widgets/LabelButton.h                                               \
    Utility/Widgets/MenuGroup.h                                                 \
    Utility/Widgets/MenuWidget.h                                                \
    Utility/Widgets/PMessageBox.h                                               \
    Utility/Widgets/PopupWidget.h                                               \
    Utility/Widgets/SubMenu.h                                                   \
    Utility/Widgets/TableItemDelegate.h                                         \
    Utility/Widgets/EventWidget/EventWaveWidget.h                               \
    Utility/Widgets/EventWidget/EventTrendItemDelegate.h                        \
    Utility/Widgets/EventWidget/EventInfoWidget.h                               \
    Utility/Widgets/EventWidget/OxyCRGEventWaveWidget.h                         \
    Utility/Widgets/EventWidget/EventWindow.h                                   \
    Utility/Widgets/EventWidget/EventReviewModel.h                              \
    Utility/Widgets/EventWidget/EventWaveSetWindow.h                            \
    Utility/Widgets/EventWidget/OxyCRGEventWindow.h                             \
    Utility/Widgets/EventWidget/OxyCRGEventSetWindow.h                          \
#################################################################################
    Components/System/SystemTick.h                                              \
    Components/System/SystemManager.h                                           \
    Components/System/SystemManagerInterface.h                                  \
    Components/System/SystemAlarm.h                                             \
    Components/System/TDA19988Ctrl.h                                            \
    Components/System/Widgets/SystemStatusBarWidget.h                           \
    Components/System/Widgets/RunningStatusBar.h                                \
    Components/System/Widgets/RunningStatusBarInterface.h                       \
    Components/System/Widgets/SystemSelftestMenu.h                              \
    Components/System/Widgets/SystemModeBarWidget.h                             \
    Components/System/Widgets/NormalFunctionMenuContent.h                       \
    Components/System/Widgets/PrintSettingMenuContent.h                         \
    Components/System/Widgets/MeasureSettingMenuContent.h                       \
    Components/System/Widgets/MeasureSettingWindow.h                            \
    Components/System/Widgets/DataReviewMenuContent.h                           \
    Components/System/Widgets/CalculateWindow.h                                 \
    Components/System/Widgets/SystemMaintenanceMenuContent.h                    \
    Components/System/Widgets/StandbyWindow.h                                   \
    Components/System/Widgets/PowerOffWindow.h                                  \
    Components/System/Widgets/NightModeWindow.h                                 \
    Components/SoundManager/SoundManager.h                                      \
    Components/SoundManager/SoundManagerInterface.h                             \
    Components/SoundManager/WavFile.h                                           \
    Components/SoundManager/WavPlayer.h                                         \
    Components/LightManager/LightManager.h                                      \
    Components/LightManager/LightManagerInterface.h                             \
    Components/LightManager/LightProviderIFace.h                                \
    Components/ParamManager/Param.h                                             \
    Components/ParamManager/ParamManager.h                                      \
    Components/ParamManager/ParamDefine.h                                       \
    Components/ParamManager/ParamInfo.h                                         \
    Components/ParamManager/ParamInfoInterface.h                                \
    Components/Alarm/Alarm.h                                                    \
    Components/Alarm/AlarmInterface.h                                           \
    Components/Alarm/AlarmConfig.h                                              \
    Components/Alarm/AlarmDefine.h                                              \
    Components/Alarm/AlarmIndicator.h                                           \
    Components/Alarm/AlarmIndicatorInterface.h                                  \
    Components/Alarm/AlarmParamIFace.h                                          \
    Components/Alarm/AlarmLimitModel.h                                          \
    Components/Alarm/AlarmSymbol.h                                              \
    Components/Alarm/AlarmStateMachine/AlarmStateMachine.h                      \
    Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h             \
    Components/Alarm/AlarmStateMachine/AlarmState.h                             \
    Components/Alarm/AlarmStateMachine/AlarmStateDefine.h                       \
    Components/Alarm/AlarmStateMachine/AlarmNormalState.h                       \
    Components/Alarm/AlarmStateMachine/AlarmPauseState.h                        \
    Components/Alarm/AlarmStateMachine/AlarmOffState.h                          \
    Components/Alarm/AlarmStateMachine/AlarmAudioOffState.h                     \
    Components/Alarm/AlarmStateMachine/AlarmResetState.h                        \
    Components/Alarm/Widgets/AlarmInfoPopListView.h                             \
    Components/Alarm/Widgets/AlarmLimitMenuContent.h                            \
    Components/Alarm/Widgets/AlarmLimitWindow.h                                 \
    Components/Alarm/Widgets/AlarmStatusWidget.h                                \
    Components/Alarm/Widgets/AlarmTechInfoBarWidget.h                           \
    Components/Alarm/Widgets/AlarmPhyInfoBarWidget.h                            \
    Components/Alarm/Widgets/AlarmInfoBarWidget.h                               \
    Components/Alarm/Widgets/AlarmInfoWindow.h                                  \
    Components/Alarm/Widgets/AlarmInfoModel.h                                   \
    Components/Alarm/NurseCallManager.h                                         \
    Components/Alarm/NurseCallManagerInterface.h                                \
    Components/Calculation/DoseCalculationManager/DoseCalculationManager.h      \
    Components/Calculation/DoseCalculationManager/DoseCalculationDefine.h       \
    Components/Calculation/DoseCalculationManager/DoseCalculationWindow.h       \
    Components/Calculation/DoseCalculationManager/TitrateTableModel.h           \
    Components/Calculation/DoseCalculationManager/TitrateTableWindow.h          \
    Components/Calculation/DoseCalculationManager/TitrateTableDefine.h          \
    Components/Calculation/DoseCalculationManager/TitrateTableManager.h         \
    Components/Calculation/DoseCalculationManager/TitrateTableSetWindow.h       \
    Components/Calculation/HemodynamicManager/HemodynamicWindow.h               \
    Components/Calculation/HemodynamicManager/HemodynamicDefine.h               \
    Components/Calculation/HemodynamicManager/HemodynamicManager.h              \
    Components/Calculation/HemodynamicManager/HemodynamicReviewWindow.h         \
    Components/Calculation/HemodynamicManager/HemodynamicDataModel.h            \
    Components/PatientData/WaveformCache.h                                      \
    Components/PatientData/ParamDataDefine.h                                    \
    Components/PatientData/ParamDataStorageManager.h                            \
    Components/PatientData/RescueDataListWidget.h                               \
    Components/PatientData/RescueDataListNewWidget.h                            \
    Components/PatientData/RescueDataDefine.h                                   \
    Components/PatientData/RescueDataExportWidget.h                             \
    Components/PatientData/TrendCache.h                                         \
    Components/PatientData/TrendCacheInterface.h                                \
    Components/PatientData/TrendDataSymbol.h                                    \
    Components/PatientData/TrendDataDefine.h                                    \
    Components/PatientData/RescueDataDeleteWidget.h                             \
    Components/PatientData/RescueDataDeleteWindow.h                             \
    Components/PatientData/TrendDataStorageManager.h                            \
    Components/PatientData/TrendDataStorageManagerInterface.h                   \
    Components/PatientData/EventDataDefine.h                                    \
    Components/PatientData/EventDataParseContext.h                              \
    Components/PatientData/EventStorageItem.h                                   \
    Components/PatientData/EventStorageManager.h                                \
    Components/PatientData/EventStorageManagerInterface.h                       \
    Components/PatientData/TrendWaveWidget.h                                    \
    Components/PatientData/TrendSubWaveWidget.h                                 \
    Components/PatientData/IMoveButton.h                                        \
    Components/PatientData/EventDataSymbol.h                                    \
    Components/PatientData/TrendTableWindow.h                                   \
    Components/PatientData/TrendTableModel.h                                    \
    Components/PatientData/TrendTableSetWindow.h                                \
    Components/PatientData/TrendPrintWindow.h                                   \
    Components/PatientData/TrendGraphSetWindow.h                                \
    Components/PatientData/TrendGraphWindow.h                                   \
    Components/PatientData/HistoryDataReviewWindow.h                            \
    Components/PatientData/HistoryDataSelWindow.h                               \
    Components/PatientData/HistoryDataSelModel.h                                \
    Components/PatientData/WaveformCacheInterface.h                             \
    Components/PatientManager/PatientManager.h                                  \
    Components/PatientManager/PatientDefine.h                                   \
    Components/PatientManager/PatientInfoWidget.h                               \
    Components/PatientManager/PatientInfoWidgetInterface.h                      \
    Components/PatientManager/PatientManagementMenuContent.h                    \
    Components/PatientManager/PatientInfoWindow.h                               \
    Components/PatientManager/PatientInfoWindowInterface.h                      \
    Components/PatientManager/DischargePatientWindow.h                          \
    Components/PowerManager/BatteryAlarm.h                                      \
    Components/PowerManager/BatteryDefine.h                                     \
    Components/PowerManager/BatteryIndicatorWindow.h                            \
    Components/PowerManager/BatteryBarWidget.h                                  \
    Components/PowerManager/BatteryIconWidget.h                                 \
    Components/PowerManager/PowerManager.h                                      \
    Components/PowerManager/PowerManagerProviderIFace.h                         \
    Components/PowerManager/TestBatteryTime.h                                   \
    Components/TimeManager/DateTimeWidget.h                                     \
    Components/TimeManager/ElapseTimeWidget.h                                   \
    Components/TimeManager/TimeManager.h                                        \
    Components/TimeManager/TimeEditWindow.h                                     \
    Components/USBManager/DataExporterBase.h                                    \
    Components/USBManager/ErrorLogExporter.h                                    \
    Components/USBManager/UDiskInspector.h                                      \
    Components/USBManager/USBManager.h                                          \
    Components/WindowManager/IWidget.h                                          \
    Components/WindowManager/WindowManager.h                                    \
    Components/WindowManager/WindowManagerInterface.h                           \
    Components/WindowManager/PromptInfoBarWidget.h                              \
    Components/WindowManager/MainMenuWindow.h                                   \
    Components/WindowManager/FactoryMaintainMenuWindow.h                        \
    Components/WindowManager/ConfigManagerMenuWindow.h                          \
    Components/WindowManager/UserMaintainMenuWindow.h                           \
    Components/WindowManager/ArrhythmiaMenuWindow.h                             \
    Components/WindowManager/ScreenMenuContent.h                                \
    Components/WindowManager/TopBarWidget.h                                     \
    Components/WindowManager/LayoutManager.h                                    \
    Components/WindowManager/ScreenLayoutWindow.h                               \
    Components/WindowManager/ScreenLayoutModel.h                                \
    Components/WindowManager/ScreenLayoutItemDelegate.h                         \
    Components/WindowManager/ScreenLayoutDefine.h                               \
    Components/WindowManager/ScreenLayoutEditor.h                               \
    Components/WindowManager/ParaColorWindow.h                                  \
    Components/WindowManager/BigFontLayoutWindow.h                              \
    Components/WindowManager/BigFontLayoutModel.h                               \
    Components/WindowManager/PasswordWindow.h                                   \
    Components/PrintManager/PrintTypeDefine.h                                   \
    Components/PrintManager/PrintDefine.h                                       \
    Components/PrintManager/PrintProviderIFace.h                                \
    Components/PrintManager/Alarm/PrintAlarm.h                                  \
    Components/Recorder/RecorderManager.h                                       \
    Components/Recorder/RecordPage.h                                            \
    Components/Recorder/RecordPageGenerator.h                                   \
    Components/Recorder/ContinuousPageGenerator.h                               \
    Components/Recorder/EventPageGenerator.h                                    \
    Components/Recorder/TrendTablePageGenerator.h                               \
    Components/Recorder/OxyCRGPageGenerator.h                                   \
    Components/Recorder/FreezePageGenerator.h                                   \
    Components/Recorder/TrendGraphPageGenerator.h                               \
    Components/Recorder/TriggerPageGenerator.h                                  \
    Components/Recorder/RecordPageProcessor.h                                   \
    Components/Recorder/EventListPageGenerator.h                                \
    Components/KeyHandle/SoftKeyManager.h                                       \
    Components/KeyHandle/SoftKeyWidget.h                                        \
    Components/KeyHandle/SoftkeyActionBase.h                                    \
    Components/KeyHandle/SoftkeyActions/CalculateSoftkeyAction.h                \
    Components/KeyHandle/SoftkeyActions/DelRescueDataSoftkeyAction.h            \
    Components/KeyHandle/SoftkeyActions/MonitorSoftkeyAction.h                  \
    Components/KeyHandle/SoftkeyActions/RescueDataSoftKeyAction.h               \
    Components/KeyHandle/KeyActionManager/KeyActionManager.h                    \
    Components/KeyHandle/KeyActionManager/KeyActionIFace.h                      \
    Components/KeyHandle/KeyActionManager/NormalModeKeyAction.h                 \
    Components/MenuManager/MenuManager.h                                        \
    Components/MenuManager/SetWidget.h                                          \
    Components/MenuManager/SupervisorMenuManager.h                              \
    Components/NetworkManager/NetworkDefine.h                                   \
    Components/NetworkManager/NetworkSymble.h                                   \
    Components/NetworkManager/NetworkManager.h                                  \
    Components/NetworkManager/WpaCtrl/WpaCtrl.h                                 \
    Components/NetworkManager/Widgets/LabeledLabel.h                            \
    Components/NetworkManager/Widgets/WiFiProfileEditor.h                       \
    Components/NetworkManager/Widgets/WiFiProfileWindow.h                       \
    Components/NetworkManager/Widgets/WiFiProfileEditorWindow.h                 \
    Components/ActivityLog/ActivityLogManager.h                                 \
    Components/Maintain/Supervisor/Menu/Supervisor12LMenuContent.h              \
    Components/Maintain/Supervisor/Menu/SupervisorPrintMenuContent.h            \
    Components/Maintain/Service/ErrorLog/ServiceErrorLogMenu.h                  \
    Components/Maintain/Service/ErrorLog/ErrorLogWindow.h                       \
    Components/Maintain/Service/ErrorLog/ErrorLogTableModel.h                   \
    Components/Maintain/Service/Menu/ServiceWindowManager.h                     \
    Components/Maintain/Service/NIBP/NIBPPressureControl.h                      \
    Components/Maintain/Service/NIBP/NIBPCalibrate.h                            \
    Components/Maintain/Service/NIBP/NIBPManometer.h                            \
    Components/Maintain/Service/NIBP/NIBPRepairMenuManager.h                    \
    Components/Maintain/Service/NIBP/NIBPZeroPoint.h                            \
    Components/Maintain/Service/Temp/ServiceCPUTemp.h                           \
    Components/Maintain/Factory/FactoryConfigManager.h                          \
    Components/Maintain/Factory/Menu/FactoryTempMenuContent.h                   \
    Components/Maintain/Factory/Menu/FactoryTestMenuContent.h                   \
    Components/Maintain/Factory/Menu/FactoryCO2MenuContent.h                    \
    Components/Maintain/FactoryMaintain/FactoryDataRecordContent.h              \
    Components/Maintain/FactoryMaintain/NIBPRepairMenuWindow.h                  \
    Components/Maintain/UserMaintain/UserMaintainManager.h                      \
    Components/Maintain/UserMaintain/AlarmMaintainSymbol.h                      \
    Components/Maintain/UserMaintain/AlarmMaintainDefine.h                      \
    Components/Maintain/UserMaintain/WifiMaintainMenu.h                         \
    Components/Maintain/UserMaintain/UserMaintainGeneralMenuContent.h           \
    Components/Maintain/UserMaintain/ModuleMaintainMenuContent.h                \
    Components/Maintain/UserMaintain/AlarmMaintainMenuContent.h                 \
    Components/Maintain/UserMaintain/WifiMaintainMenuContent.h                  \
    Components/Maintain/UserMaintain/OthersMaintainMenuContent.h                \
    Components/Maintain/UserMaintain/WiredNetworkMaintainMenuContent.h          \
    Components/Maintain/UserMaintain/ErrorLogEntranceContent.h                  \
    Components/Maintain/UserMaintain/DemoMenuContent.h                          \
    Components/Maintain/UserMaintain/NurseCallSetWindow.h                       \
    Components/Maintain/FactoryMaintain/FactorySystemInfoMenuContent.h          \
    Components/Maintain/FactoryMaintain/FactoryMaintainManager.h                \
    Components/Maintain/FactoryMaintain/SoftwareVersionWindow.h                 \
    Components/Maintain/FactoryMaintain/MonitorInfoWindow.h                     \
    Components/Maintain/FactoryMaintain/ServiceUpdateEntranceContent.h          \
    Components/Maintain/FactoryMaintain/MachineConfigModuleContent.h            \
    Components/Maintain/FactoryMaintain/FactoryVersionInfo.h                    \
    Components/Maintain/FactoryMaintain/NIBPCalibrationMenuContent.h            \
    Components/Maintain/FactoryMaintain/NIBPCalibrateContent.h                  \
    Components/Maintain/FactoryMaintain/NIBPManometerContent.h                  \
    Components/Maintain/FactoryMaintain/NIBPZeroPointContent.h                  \
    Components/Maintain/FactoryMaintain/NIBPPressureControlContent.h            \
    Components/Maintain/FactoryMaintain/FactoryImportExportMenuContent.h        \
    Components/Maintain/FactoryMaintain/UpgradeManager.h                        \
    Components/Maintain/FactoryMaintain/UpgradeWindow.h                         \
    Components/Maintain/FactoryMaintain/O2CalibrationMenuContent.h              \
    Components/CodeMarker/Widgets/CodeMarkerList.h                              \
    Components/CodeMarker/Widgets/CodeMarkerWindow.h                            \
    Components/ConfigManager/ConfigManager.h                                    \
    Components/ConfigManager/ConfigManagerInterface.h                           \
    Components/ConfigManager/SelectDefaultConfigMenuContent.h                   \
    Components/ConfigManager/ConfigManagerPassWordEditMenuContent.h             \
    Components/ConfigManager/UserConfigEditMenuContent.h                        \
    Components/ConfigManager/PatientTypeSelectWindow.h                          \
    Components/ConfigManager/ConfigExportImportMenuContent.h                    \
    Components/ConfigManager/ConfigEditMenuWindow.h                             \
    Components/ConfigManager/ConfigEditGeneralMenuContent.h                     \
    Components/ConfigManager/ConfigEditTEMPMenuContent.h                        \
    Components/ConfigManager/ConfigEditEcgMenuContent.h                         \
    Components/ConfigManager/ConfigEditRespMenuContent.h                        \
    Components/ConfigManager/ConfigEditNIBPMenuContent.h                        \
    Components/ConfigManager/ConfigEditSpO2MenuContent.h                        \
    Components/ConfigManager/ConfigEditCO2MenuContent.h                         \
    Components/ConfigManager/ConfigEditCOMenuContent.h                          \
    Components/ConfigManager/ConfigEditAlarmLimitMenuContent.h                  \
    Components/ConfigManager/ConfigEditAlarmLimitModel.h                        \
    Components/ConfigManager/ConfigEditIBPMenuContent.h                         \
    Components/ConfigManager/ConfigEditDisplayMenuContent.h                     \
    Components/ConfigManager/ConfigEditCodeMarkerMenuContent.h                  \
    Components/ConfigManager/LoadConfigMenuContent.h                            \
    Components/ConfigManager/UnitSetupMenuContent.h                             \
    Components/ConfigManager/SaveCurrentConfigMenuContent.h                     \
    Components/ConfigManager/SaveUserConfigWindow.h                             \
    Components/Freeze/FreezeManager.h                                           \
    Components/Freeze/FreezeWindow.h                                            \
    Components/Freeze/FreezeDataModel.h                                         \
#################################################################################
    Params/AGParam/AGWidgets/AGTrendWidget.h                                    \
    Params/AGParam/AGWidgets/AGWaveRuler.h                                      \
    Params/AGParam/AGWidgets/AGWaveWidget.h                                     \
    Params/AGParam/AGWidgets/AGMenuContent.h                                    \
    Params/AGParam/AGWidgets/CO2SetAGMenu.h                                     \
    Params/AGParam/AGWidgets/N2OSetAGMenu.h                                     \
    Params/AGParam/AGWidgets/HalSetAGMenu.h                                     \
    Params/AGParam/AGDefine.h                                                   \
    Params/AGParam/AGParam.h                                                    \
    Params/AGParam/AGProviderIFace.h                                            \
    Params/AGParam/AGSymbol.h                                                   \
    Params/AGParam/AGAlarm.h                                                    \
    Params/ECGParam/ECGAlarm.h                                                  \
    Params/ECGParam/ECGDefine.h                                                 \
    Params/ECGParam/ECGDupAlarm.h                                               \
    Params/ECGParam/ECGDupParam.h                                               \
    Params/ECGParam/ECGDupParamInterface.h                                      \
    Params/ECGParam/ECGParam.h                                                  \
    Params/ECGParam/ECGParamInterface.h                                         \
    Params/ECGParam/ECGProviderIFace.h                                          \
    Params/ECGParam/ECGSymbol.h                                                 \
    Params/ECGParam/ECGWidgets/ECGWaveWidget.h                                  \
    Params/ECGParam/ECGWidgets/ECGWaveRuler.h                                   \
    Params/ECGParam/ECGWidgets/ECGTrendWidget.h                                 \
    Params/ECGParam/ECGWidgets/ECGPVCSTrendWidget.h                             \
    Params/ECGParam/ECGWidgets/ECGSTTrendWidget.h                               \
    Params/ECGParam/ECGWidgets/ECGMenuContent.h                                 \
    Params/ECGParam/ECGWidgets/ArrhythmiaAnalysisMenu.h                         \
    Params/ECGParam/ECGWidgets/ArrhythmiaThresholdMenu.h                        \
    Params/CO2Param/CO2Param.h                                                  \
    Params/CO2Param/CO2Alarm.h                                                  \
    Params/CO2Param/CO2Define.h                                                 \
    Params/CO2Param/CO2Symbol.h                                                 \
    Params/CO2Param/CO2ProviderIFace.h                                          \
    Params/CO2Param/CO2Widgets/CO2WaveWidget.h                                  \
    Params/CO2Param/CO2Widgets/CO2WaveRuler.h                                   \
    Params/CO2Param/CO2Widgets/CO2TrendWidget.h                                 \
    Params/CO2Param/CO2Widgets/CO2MenuContent.h                                 \
    Params/IBPParam/IBPProviderIFace.h                                          \
    Params/IBPParam/IBPDefine.h                                                 \
    Params/IBPParam/IBPParam.h                                                  \
    Params/IBPParam/IBPWidgets/IBPWaveWidget.h                                  \
    Params/IBPParam/IBPWidgets/IBPTrendWidget.h                                 \
    Params/IBPParam/IBPSymbol.h                                                 \
    Params/IBPParam/IBPWidgets/IBPWaveRuler.h                                   \
    Params/IBPParam/IBPWidgets/IBPMenuContent.h                                 \
    Params/IBPParam/IBPAlarm.h                                                  \
    Params/COParam/COAlarm.h                                                    \
    Params/COParam/COSymbol.h                                                   \
    Params/COParam/COProviderIFace.h                                            \
    Params/COParam/CODefine.h                                                   \
    Params/COParam/COParam.h                                                    \
    Params/COParam/COWidgets/COTrendWidget.h                                    \
    Params/COParam/COWidgets/COMenuContent.h                                    \
    Params/NIBPParam/NIBPStateMachine/NIBPState.h                               \
    Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.h                        \
    Params/NIBPParam/NIBPStateMachine/NIBPStateMachineDefine.h                  \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorErrorState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorGetResultState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorMeasureState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorSafeWaitTimeState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStandbyState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStartingState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStopState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStateDefine.h \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStateMachine.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceCalibrateState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceManometerState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServicePressureControlState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStandbyState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStateDefine.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStateMachine.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceZeroPointState.h \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceErrorState.h \
    Params/NIBPParam/NIBPParam.h                                                \
    Params/NIBPParam/NIBPParamInterface.h                                       \
    Params/NIBPParam/NIBPAlarm.h                                                \
    Params/NIBPParam/NIBPDefine.h                                               \
    Params/NIBPParam/NIBPEventDefine.h                                          \
    Params/NIBPParam/NIBPSymbol.h                                               \
    Params/NIBPParam/NIBPProviderIFace.h                                        \
    Params/NIBPParam/NIBPWidgets/NIBPTrendWidget.h                              \
    Params/NIBPParam/NIBPWidgets/NIBPDataTrendWidget.h                          \
    Params/NIBPParam/NIBPWidgets/NIBPMenuContent.h                              \
    Params/NIBPParam/NIBPTrigger/NIBPCountdownTime.h                            \
    Params/NIBPParam/NIBPTrigger/NIBPCountdownTimeInterface.h                   \
    Params/SPO2Param/SPO2Param.h                                                \
    Params/SPO2Param/SPO2Alarm.h                                                \
    Params/SPO2Param/SPO2Define.h                                               \
    Params/SPO2Param/SPO2Symbol.h                                               \
    Params/SPO2Param/SPO2ProviderIFace.h                                        \
    Params/SPO2Param/SPO2Widgets/SPO2WaveWidget.h                               \
    Params/SPO2Param/SPO2Widgets/SPO2TrendWidget.h                              \
    Params/SPO2Param/SPO2Widgets/SPO2BarWidget.h                                \
    Params/SPO2Param/SPO2Widgets/SPO2MenuContent.h                              \
    Params/SPO2Param/SPO2Widgets/CCHDWindow.h                                   \
    Params/SPO2Param/SPO2Widgets/CCHDDataModel.h                                \
    Params/TEMPParam/TEMPDefine.h                                               \
    Params/TEMPParam/TEMPSymbol.h                                               \
    Params/TEMPParam/TEMPParam.h                                                \
    Params/TEMPParam/TEMPAlarm.h                                                \
    Params/TEMPParam/TEMPWidgets/TEMPTrendWidget.h                              \
    Params/TEMPParam/TEMPWidgets/TEMPMenuContent.h                              \
    Params/TEMPParam/TEMPProviderIFace.h                                        \
    Params/RESPParam/RESPAlarm.h                                                \
    Params/RESPParam/RESPDefine.h                                               \
    Params/RESPParam/RESPDupAlarm.h                                             \
    Params/RESPParam/RESPDupParam.h                                             \
    Params/RESPParam/RESPParam.h                                                \
    Params/RESPParam/RESPProviderIFace.h                                        \
    Params/RESPParam/RESPSymbol.h                                               \
    Params/RESPParam/RESPWidgets/RESPWaveWidget.h                               \
    Params/RESPParam/RESPWidgets/RESPTrendWidget.h                              \
    Params/RESPParam/RESPWidgets/RESPMenuContent.h                              \
    Params/PAParam/PASymbol.h                                                   \
    Params/O2Param/O2Widgets/O2TrendWidget.h                                    \
    Params/O2Param/O2Widgets/O2MenuContent.h                                    \
    Params/O2Param/O2Widgets/ApneaStimulationMenuContent.h                      \
    Params/O2Param/O2Param.h                                                    \
    Params/O2Param/O2ProviderIFace.h                                            \
    Params/O2Param/O2Alarm.h                                                    \
    Params/O2Param/O2Define.h                                                   \
    Params/O2Param/O2Symbol.h                                                   \
    Params/O2Param/O2ParamInterface.h                                           \
#################################################################################
    Providers/Provider.h                                                        \
    Providers/BLMProvider.h                                                     \
    Providers/PhaseinProvider/PhaseinProvider.h                                 \
    Providers/BLMCO2Provider/BLMCO2Provider.h                                   \
    Providers/WitleafProvider/WitleafProvider.h                                 \
    Providers/MasimoProvider/MasimoProvider.h                                   \
    Providers/RainbowProvider/RainbowProvider.h                                 \
    Providers/NellcorProvider/NellcorProvider.h                                 \
    Providers/E5Provider/E5Provider.h                                           \
    Providers/T5Provider/T5Provider.h                                           \
    Providers/DemoProvider/DemoProvider.h                                       \
    Providers/N5Provider/N5Provider.h                                           \
    Providers/PRT48Provider/PRT48Provider.h                                     \
    Providers/SuntechProvider/SuntechProvider.h                                 \
    Providers/SystemBoardProvider/SystemBoardProvider.h                         \
    Providers/SystemBoardProvider/SystemBoardProviderDefine.h                   \
    Providers/RawData/RawDataCollector.h                                        \
    Providers/S5Provider/S5Provider.h                                           \
    Providers/DataDispatcher.h                                                  \
    Providers/NeonateProvider/NeonateProvider.h                                 \
#################################################################################
    Main/App/Init.h                                                             \
    Main/IApplication.h                                                         \
    Main/IThread.h                                                              \
    Main/MergeConfig.h                                                          \
    Main/Starter.h                                                              \
    Utility/Widgets/ImportFileSubWidget.h \
    Components/TimeManager/TimeManagerInterface.h \
    Components/Alarm/AlarmSourceManager.h \

#头文件修改后，重新编译依赖的源文件
DEPENDPATH +=                                                                   \
    SOUP/Json                                                                   \
    Main                                                                        \
    Main/App                                                                    \
    Main/Factory                                                                \
    Main/Service                                                                \
    Main/Supervisor                                                             \
    Utility                                                                     \
    Utility/FloatHandle                                                         \
    Utility/RingBuff                                                            \
    Utility/Debug                                                               \
    Utility/TimeDate                                                            \
    Utility/Uart                                                                \
    Utility/Config                                                              \
    Utility/DataStorage                                                         \
    Utility/ColorManager                                                        \
    Utility/FontManager                                                         \
    Utility/LanguageManager                                                     \
    Utility/Widgets                                                             \
    Utility/Widgets/Base                                                        \
    Utility/Widgets/Base/KineticScroller                                        \
    Utility/Widgets/IComboList                                                  \
    Utility/Widgets/Waveform                                                    \
    Utility/Widgets/InputMethod                                                 \
    Utility/Widgets/OxyCRGWidget                                                \
    Utility/Widgets/Trendform                                                   \
    Utility/Widgets/TrendWidget                                                 \
    Utility/Widgets/EventWidget                                                 \
    Utility/ErrorLog                                                            \
    Components/System                                                           \
    Components/System/Widgets                                                   \
    Components/SoundManager                                                     \
    Components/LightManager                                                     \
    Components/Alarm                                                            \
    Components/Alarm/AlarmStateMachine                                          \
    Components/Alarm/Widgets                                                    \
    Components/Alarm/FullDisclosure                                             \
    Components/PatientData                                                      \
    Components/TimeManager                                                      \
    Components/PatientManager                                                   \
    Components/ParamManager                                                     \
    Components/USBManager                                                       \
    Components/WindowManager                                                    \
    Components/KeyHandle                                                        \
    Components/KeyHandle/SoftkeyActions                                         \
    Components/KeyHandle/KeyActionManager                                       \
    Components/Print/ContinuousPrint                                            \
    Components/PrintManager                                                     \
    Components/PrintManager/PrintPages                                          \
    Components/PrintManager/Alarm                                               \
    Components/Recorder                                                         \
    Components/PowerManager                                                     \
    Components/PowerManager/Battery                                             \
    Components/MenuManager                                                      \
    Components/NetworkManager                                                   \
    Components/NetworkManager/Widgets                                           \
    Components/NetworkManager/WpaCtrl                                           \
    Components/NetworkManager/WpaCtrl/common                                    \
    Components/NetworkManager/WpaCtrl/utils                                     \
    Components/USBManager                                                       \
    Components/ActivityLog                                                      \
    Components/Maintain/Supervisor                                              \
    Components/Maintain/Supervisor/Menu                                         \
    Components/Maintain/Service                                                 \
    Components/Maintain/Service/Menu                                            \
    Components/Maintain/Service/ErrorLog                                        \
    Components/Maintain/Service/NIBP                                            \
    Components/Maintain/Service/Temp                                            \
    Components/Maintain/Service/Version                                         \
    Components/Maintain/Factory                                                 \
    Components/Maintain/Factory/Menu                                            \
    Components/Maintain/UserMaintain                                            \
    Components/Maintain/FactoryMaintain                                         \
    Components/CodeMarker/Widgets                                               \
    Components/Calculation/DoseCalculationManager                               \
    Components/Calculation/HemodynamicManager                                   \
    Components/ConfigManager                                                    \
    Components/Freeze                                                           \
    Providers                                                                   \
    Providers/PhaseinProvider                                                   \
    Providers/BLMCO2Provider                                                    \
    Providers/WitleafProvider                                                   \
    Providers/MasimoProvider                                                    \
    Providers/RainbowProvider                                                   \
    Providers/NellcorProvider                                                   \
    Providers/SuntechProvider                                                   \
    Providers/BLMTEMPProvider                                                   \
    Providers/T5Provider                                                        \
    Providers/N5Provider                                                        \
    Providers/E5Provider                                                        \
    Providers/DemoProvider                                                      \
    Providers/PRT48Provider                                                     \
    Providers/PRT72Provider                                                     \
    Providers/SystemBoardProvider                                               \
    Providers/RawData                                                           \
    Providers/S5Provider                                                        \
    Providers/NeonateProvider                                                   \
    Params                                                                      \
    Params/AGParam                                                              \
    Params/AGParam/AGWidgets                                                    \
    Params/ECGParam                                                             \
    Params/ECGParam/ECGWidgets                                                  \
    Params/CO2Param                                                             \
    Params/CO2Param/CO2Widgets                                                  \
    Params/IBPParam                                                             \
    Params/IBPParam/IBPWidgets                                                  \
    Params/COParam                                                              \
    Params/COParam/COWidgets                                                    \
    Params/NIBPParam                                                            \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine                   \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine                   \
    Params/NIBPParam/NIBPStateMachine                                           \
    Params/NIBPParam/NIBPWidgets                                                \
    Params/NIBPParam/NIBPTrigger                                                \
    Params/SPO2Param                                                            \
    Params/SPO2Param/SPO2Widgets                                                \
    Params/TEMPParam                                                            \
    Params/TEMPParam/TEMPWidgets                                                \
    Params/RESPParam                                                            \
    Params/RESPParam/RESPWidgets                                                \
    Params/O2Param                                                              \
    Params/O2Param/O2Widgets                                                    \


INCLUDEPATH +=                                                                  \
    SOUP/Json                                                                   \
    SOUP/libharu/include                                                        \
    SOUP/libssh2/include                                                        \
    Main                                                                        \
    Main/App                                                                    \
    Main/Factory                                                                \
    Main/Service                                                                \
    Main/Supervisor                                                             \
    Utility                                                                     \
    Utility/FloatHandle                                                         \
    Utility/RingBuff                                                            \
    Utility/Debug                                                               \
    Utility/TimeDate                                                            \
    Utility/Uart                                                                \
    Utility/Config                                                              \
    Utility/DataStorage                                                         \
    Utility/ColorManager                                                        \
    Utility/NightModeManager                                                    \
    Utility/FontManager                                                         \
    Utility/LanguageManager                                                     \
    Utility/Widgets                                                             \
    Utility/Widgets/Base                                                        \
    Utility/Widgets/Base/KineticScroller                                        \
    Utility/Widgets/IComboList                                                  \
    Utility/Widgets/Waveform                                                    \
    Utility/Widgets/InputMethod                                                 \
    Utility/Widgets/OxyCRGWidget                                                \
    Utility/Widgets/TrendWidget                                                 \
    Utility/Widgets/EventWidget                                                 \
    Utility/Widgets/Trendform                                                   \
    Utility/ErrorLog                                                            \
    Components/System                                                           \
    Components/System/Widgets                                                   \
    Components/SoundManager                                                     \
    Components/LightManager                                                     \
    Components/Alarm                                                            \
    Components/Alarm/AlarmStateMachine                                          \
    Components/Alarm/Widgets                                                    \
    Components/Alarm/FullDisclosure                                             \
    Components/PatientData                                                      \
    Components/TimeManager                                                      \
    Components/PatientManager                                                   \
    Components/ParamManager                                                     \
    Components/USBManager                                                       \
    Components/WindowManager                                                    \
    Components/KeyHandle                                                        \
    Components/KeyHandle/SoftkeyActions                                         \
    Components/KeyHandle/KeyActionManager                                       \
    Components/Print/ContinuousPrint                                            \
    Components/PrintManager                                                     \
    Components/PrintManager/PrintPages                                          \
    Components/PrintManager/Alarm                                               \
    Components/Recorder                                                         \
    Components/PowerManager                                                     \
    Components/PowerManager/Battery                                             \
    Components/MenuManager                                                      \
    Components/NetworkManager                                                   \
    Components/NetworkManager/Widgets                                           \
    Components/NetworkManager/WpaCtrl                                           \
    Components/NetworkManager/WpaCtrl/common                                    \
    Components/NetworkManager/WpaCtrl/utils                                     \
    Components/USBManager                                                       \
    Components/ActivityLog                                                      \
    Components/Maintain/Supervisor                                              \
    Components/Maintain/Supervisor/Menu                                         \
    Components/Maintain/Service                                                 \
    Components/Maintain/Service/Menu                                            \
    Components/Maintain/Service/ErrorLog                                        \
    Components/Maintain/Service/NIBP                                            \
    Components/Maintain/Service/Temp                                            \
    Components/Maintain/Service/Version                                         \
    Components/Maintain/Factory                                                 \
    Components/Maintain/Factory/Menu                                            \
    Components/Maintain/UserMaintain                                            \
    Components/Maintain/FactoryMaintain                                         \
    Components/CodeMarker/Widgets                                               \
    Components/Calculation/DoseCalculationManager                               \
    Components/Calculation/HemodynamicManager                                   \
    Components/ConfigManager                                                    \
    Components/Freeze                                                           \
    Providers                                                                   \
    Providers/PhaseinProvider                                                   \
    Providers/BLMCO2Provider                                                    \
    Providers/WitleafProvider                                                   \
    Providers/MasimoProvider                                                    \
    Providers/RainbowProvider                                                   \
    Providers/NellcorProvider                                                   \
    Providers/SuntechProvider                                                   \
    Providers/BLMTEMPProvider                                                   \
    Providers/T5Provider                                                        \
    Providers/N5Provider                                                       \
    Providers/E5Provider                                                        \
    Providers/DemoProvider                                                      \
    Providers/PRT48Provider                                                     \
    Providers/PRT72Provider                                                     \
    Providers/SystemBoardProvider                                               \
    Providers/RawData                                                           \
    Providers/S5Provider                                                        \
    Providers/NeonateProvider                                                   \
    Params                                                                      \
    Params/AGParam                                                              \
    Params/AGParam/AGWidgets                                                    \
    Params/ECGParam                                                             \
    Params/ECGParam/ECGWidgets                                                  \
    Params/CO2Param                                                             \
    Params/CO2Param/CO2Widgets                                                  \
    Params/IBPParam                                                             \
    Params/IBPParam/IBPWidgets                                                  \
    Params/COParam                                                              \
    Params/COParam/COWidgets                                                    \
    Params/NIBPParam                                                            \
    Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine                   \
    Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine                   \
    Params/NIBPParam/NIBPStateMachine                                           \
    Params/NIBPParam/NIBPWidgets                                                \
    Params/NIBPParam/NIBPTrigger                                                \
    Params/SPO2Param                                                            \
    Params/SPO2Param/SPO2Widgets                                                \
    Params/TEMPParam                                                            \
    Params/TEMPParam/TEMPWidgets                                                \
    Params/RESPParam                                                            \
    Params/RESPParam/RESPWidgets                                                \
    Params/O2Param                                                              \
    Params/O2Param/O2Widgets                                                    \

linux-arm-g++ {
    SOURCES +=  Utility/Widgets/TSCalibrationWindow.cpp

    HEADERS +=  Utility/Widgets/TSCalibrationWindow.h
}

RESOURCES += \
    Utility/Widgets/Base/res/ui.qrc
