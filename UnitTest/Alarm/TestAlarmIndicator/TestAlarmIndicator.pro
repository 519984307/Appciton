QT += testlib svg

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
    TestAlarmIndicator.cpp \
    ../../../Components/Alarm/AlarmIndicator.cpp \
    ../../../Components/SoundManager/SoundManagerInterface.cpp \
    ../../../Components/Alarm/Widgets/AlarmStatusWidget.cpp \
    ../../../Components/WindowManager/IWidget.cpp \
    ../../../Utility/Widgets/Base/Dialog.cpp \
    ../../../Components/LightManager/LightManagerInterface.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp \
    ../../../Utility/FontManager/FontManagerInterface.cpp \
    ../../../Components/Alarm/AlarmInterface.cpp \
    ../../../Components/Alarm/Widgets/AlarmInfoBarWidget.cpp \
    ../../../Utility/Widgets/Base/Button.cpp \
    ../../../Utility/Widgets/Base/ThemeManager.cpp \
    ../../../Components/WindowManager/WindowManagerInterface.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Components/Alarm/AlarmParamIFace.cpp \
    ../../../Components/Alarm/NurseCallManagerInterface.cpp

HEADERS += \
    TestAlarmIndicator.h \
    ../../../Components/Alarm/AlarmIndicator.h \
    ../../../Components/Alarm/AlarmDefine.h \
    ../../../Components/SoundManager/SoundManagerInterface.h \
    ../../../Components/Alarm/Widgets/AlarmStatusWidget.h \
    ../../../Components/PatientManager/PatientInfoWidgetInterface.h \
    ../../../Components/WindowManager/IWidget.h \
    ../../../Utility/Widgets/Base/Dialog.h \
    ../../../Components/LightManager/LightManagerInterface.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateDefine.h \
    ../../../Components/ParamManager/ParamDefine.h \
    ../../../Utility/FontManager/FontManagerInterface.h \
    ../../../Components/Alarm/AlarmInterface.h \
    ../../../Components/Alarm/Widgets/AlarmInfoBarWidget.h \
    ../../../Utility/Widgets/Base/Button.h \
    ../../../Utility/Widgets/Base/ThemeManager.h \
    ../../../Components/WindowManager/WindowManagerInterface.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Components/Alarm/AlarmParamIFace.h \
    ../MockAlarm.h \
    ../MockAlarmInfoBarWidget.h \
    ../../Utility/MockSoundManager.h \
    ../MockLightManager.h \
    ../MockAlarmStateMachine.h \
    ../../../Components/Alarm/NurseCallManagerInterface.h

INCLUDEPATH += \
    ../../../Components/Alarm \
    ../../../Components/SoundManager \
    ../../../Components/Alarm/Widgets \
    ../../../Components/WindowManager \
    ../../../Utility/Widgets/Base \
    ../../../Components/LightManager \
    ../../../Components/PatientManager \
    ../../../Utility/Config \
    ../../../Components/Alarm/AlarmStateMachine \
    ../../../Components/ParamManager \
    ../../../Utility/FontManager \
    ../../../Components/Alarm/Widgets \
    ../../../Utility/Widgets/Base \
    .. \
    ../../Utility
