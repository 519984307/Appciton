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

include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

SOURCES += main.cpp \
    TestAlarmResetState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmResetState.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp \
    ../../../Components/LightManager/LightManagerInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.cpp

HEADERS += \
    TestAlarmResetState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmResetState.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h \
    ../../../Components/LightManager/LightManagerInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.h \
    ../MockAlarmIndicator.h \
    ../MockAlarmStateMachine.h \
    ../MockLightManager.h

INCLUDEPATH += \
    ../../../Components/Alarm/AlarmStateMachine \
    ../../../Components/Alarm \
    ../../../Components/LightManager \
    ..
