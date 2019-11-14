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

#取消报警音关闭和报警暂停功能
DEFINES += CLOSE_ALARM_AUDIO_OFF_ALARM_OFF

SOURCES += main.cpp \
    TestAlarmStateMachine.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachine.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmNormalState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmPauseState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmResetState.cpp \
    ../../../Components/LightManager/LightManagerInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp

HEADERS += \
    TestAlarmStateMachine.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachine.h \
    ../../../Components/Alarm/AlarmDefine.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmNormalState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmPauseState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmResetState.h \
    ../../../Components/LightManager/LightManagerInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../MockAlarmIndicator.h \
    ../MockLightManager.h \
    MockAlarmState.h

INCLUDEPATH += \
    ../../../Components/Alarm/AlarmStateMachine \
    ../../../Components/Alarm \
    ../../../Utility/Config \
    ../../../Components/LightManager \
    ..

RESOURCES += \
    res.qrc
