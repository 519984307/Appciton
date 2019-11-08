QT -= gui
QT += testlib

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

DEFINES += HIDE_OXYCRG_REVIEW_FUNCTION

SOURCES += main.cpp \
    TestAlarm.cpp \
    ../../../Components/Alarm/Alarm.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Components/PatientData/TrendCacheInterface.cpp \
    ../../../Components/PatientData/EventStorageManagerInterface.cpp \
    ../../../Components/PatientData/TrendDataStorageManagerInterface.cpp \
    ../../../Params/ECGParam/ECGParamInterface.cpp \
    ../../../Components/Alarm/AlarmInterface.cpp \
    ../../../Components/ParamManager/ParamInfoInterface.cpp \
    ../../../Components/Alarm/AlarmParamIFace.cpp

HEADERS += \
    TestAlarm.h \
    ../../../Components/Alarm/Alarm.h \
    ../../../Components/ParamManager/ParamDefine.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Components/PatientData/TrendCacheInterface.h \
    ../../../Params/AGParam/AGSymbol.h \
    ../../../Params/CO2Param/CO2Symbol.h \
    ../../../Params/COParam/COSymbol.h \
    ../../../Params/ECGParam/ECGSymbol.h \
    ../../../Params/IBPParam/IBPSymbol.h \
    ../../../Params/NIBPParam/NIBPSymbol.h \
    ../../../Params/RESPParam/RESPSymbol.h \
    ../../../Params/TEMPParam/TEMPSymbol.h \
    ../../../Params/SPO2Param/SPO2Symbol.h \
    ../../../Components/PatientData/EventStorageManagerInterface.h \
    ../../../Components/PatientData/TrendDataStorageManagerInterface.h \
    ../../../Params/ECGParam/ECGParamInterface.h \
    ../../../Components/Alarm/AlarmInterface.h \
    ../../../Components/ParamManager/ParamInfoInterface.h \
    ../../../Components/Alarm/AlarmParamIFace.h \
    ../MockAlarmStateMachine.h \
    ../../Params/MockParamInfo.h \
    ../../Storage/MockTrendDataStorageManager.h \
    ../MockAlarmIndicator.h \
    ../../Trend/MockTrendCache.h \
    ../../Event/MockEventStorageManager.h \
    ../../../Params/O2Param/O2Symbol.h

INCLUDEPATH += \
    ../../../Components/Alarm \
    ../../../Components/ParamManager \
    ../../../Components/Alarm \
    ../../../Components/Alarm/AlarmStateMachine \
    ../../../Utility/Config \
    ../../../Components/PatientData \
    ../../Params \
    ../../../Params/AGParam \
    ../../../Params/CO2Param \
    ../../../Params/ECGParam \
    ../../../Params/SPO2Param \
    ../../../Params/RESPParam \
    ../../../Params/IBPParam \
    ../../../Params/COParam \
    ../../../Params/TEMPParam \
    ../../../Params/NIBPParam \
    ../../../Params/O2Param \
    .. \
    ../../Storage \
    ../../Trend \
    ../../Event

