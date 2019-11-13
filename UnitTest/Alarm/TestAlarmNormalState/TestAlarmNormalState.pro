QT -= gui
QT += testlib

CONFIG += c++11 console
CONFIG -= app_bundle

include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
        TestAlarmNormalState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmNormalState.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.cpp \
    ../../../Components/LightManager/LightManagerInterface.cpp

HEADERS += TestAlarmNormalState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmNormalState.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Components/LightManager/LightProviderIFace.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.h \
    ../../../Components/LightManager/LightManagerInterface.h \
    ../MockAlarmIndicator.h \
    ../MockAlarmStateMachine.h \
    ../MockLightManager.h

INCLUDEPATH += \
    ../../../Components/Alarm \
    ../../../Components/LightManager \
    ../../../Components/Alarm/AlarmStateMachine \
    ..

