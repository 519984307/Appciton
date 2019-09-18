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

SOURCES += main.cpp \
    TestPatientManager.cpp \
    ../../../Components/PatientManager/PatientManager.cpp \
    ../../../Components/ConfigManager/ConfigManagerInterface.cpp \
    ../../../Components/WindowManager/IWidget.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../Params/ECGParam/ECGParamInterface.cpp \
    ../../../Params/ECGParam/ECGDupParamInterface.cpp \
    ../../../Utility/DataStorage/DataStorageDirManagerInterface.cpp \
    ../../../Params/NIBPParam/NIBPParamInterface.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Params/O2Param/O2ParamInterface.cpp \
    ../../../Components/System/SystemManagerInterface.cpp \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../Utility/Config/Config.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp \
    ../../../Components/System/Widgets/RunningStatusBarInterface.cpp

HEADERS += \
    TestPatientManager.h \
    ../../../Components/PatientManager/PatientManager.h \
    ../../../Utility/BaseDefine.h \
    ../../../Components/ParamManager/ParamDefine.h \
    ../../../Utility/LanguageManager/LanguageManager.h \
    ../../../Components/ConfigManager/ConfigManagerInterface.h \
    ../../../Components/WindowManager/IWidget.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Params/ECGParam/ECGDupParamInterface.h \
    ../../../Params/ECGParam/ECGParamInterface.h \
    ../../../Utility/DataStorage/DataStorageDirManagerInterface.h \
    ../../../Params/NIBPParam/NIBPParamInterface.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Params/O2Param/O2ParamInterface.h \
    ../../../Components/System/SystemManagerInterface.h \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../Utility/Config/Config.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../Utility/MockConfigManager.h \
    ../../Params/MockECGParam.h \
    ../../Utility/MockSystemManager.h \
    ../../Params/NIBP/MockNIBPParam.h \
    ../../Storage/TestDataStorageDirManager/MockDataStorageDirManager.h \
    ../../Alarm/MockAlarmIndicator.h \
    ../../../Components/System/Widgets/RunningStatusBarInterface.h

INCLUDEPATH += \
    ../../../Components/PatientManager \
    ../../../Utility \
    ../../../Components/ParamManager \
    ../../../Components/ConfigManager \
    ../../../Components/WindowManager \
    ../../../Utility/Config \
    ../../../Params/ECGParam \
    ../../../Params/O2Param \
    ../../../Utility/DataStorage \
    ../../../Params/NIBPParam \
    ../../../Components/Alarm \
    ../../../Components/System \
    ../../../Utility/Config \
    ../../../Utility/ErrorLog \
    ../../../Utility/LanguageManager \
    ../../Utility \
    ../../Params \
    ../../Params/NIBP \
    ../../Storage/TestDataStorageDirManager \
    ../../Alarm \
    ../../../Components/System/Widgets

RESOURCES += \
    res.qrc

