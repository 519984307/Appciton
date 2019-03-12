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

SOURCES += main.cpp \
    TestNightManager.cpp \
    ../../../Utility/NightModeManager/NightModeManager.cpp \
    ../../../Components/SoundManager/SoundManagerInterface.cpp \
    ../../../Components/System/SystemManagerInterface.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Components/WindowManager/IWidget.cpp \
    ../../../Params/NIBPParam/NIBPParamInterface.cpp \
    ../../../Components/ConfigManager/ConfigManagerInterface.cpp \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/Config/Config.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp \
    ../../../Components/System/Widgets/RunningStatusBarInterface.cpp \

HEADERS += \
    TestNightManager.h \
    ../../../Utility/NightModeManager/NightModeManager.h \
    ../../../Components/SoundManager/SoundManagerInterface.h \
    ../../../Components/System/SystemManagerInterface.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Components/WindowManager/IWidget.h \
    ../../../Params/NIBPParam/NIBPParamInterface.h \
    ../../../Components/ConfigManager/ConfigManagerInterface.h \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/Config/Config.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../../Components/PatientManager/PatientDefine.h \
    ../../../Components/System/Widgets/RunningStatusBarInterface.h \
    MockSystemManager.h \
    ../../../Components/System/SystemDefine.h \
    MockSoundManager.h \
    MockNIBPParam.h \
    MockConfigManager.h \

INCLUDEPATH += \
    ../../../Utility/NightModeManager \
    ../../../Components/SoundManager \
    ../../../Components/System \
    ../../../Components/System/Widgets \
    ../../../Utility/Config \
    ../../../Components/WindowManager \
    ../../../Params/NIBPParam \
    ../../../Components/ConfigManager \
    ../../../Utility/ErrorLog \
    ../../../Components/PatientManager
