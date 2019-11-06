#-------------------------------------------------
#
# Project created by QtCreator 2019-02-27T18:49:11
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_testalarmstate
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include("../../Common/Common.pri")
include("../../Common/runtest.pri")

SOURCES += \
        TestAlarmState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.cpp \
    ../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmResetState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmPauseState.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmNormalState.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Components/LightManager/LightManagerInterface.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../Components/Alarm/AlarmStateMachine/AlarmState.h \
    ../../../Utility/LanguageManager/LanguageManager.h \
    TestAlarmState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmStateMachineInterface.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmResetState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmPauseState.h \
    ../../../Components/Alarm/AlarmStateMachine/AlarmNormalState.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Components/LightManager/LightManagerInterface.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Components/PatientManager/PatientDefine.h

INCLUDEPATH += \
    ../../../Components/Alarm/AlarmStateMachine \
    ../../../Components/Alarm \
    ../../../Utility/LanguageManager \
    ../../../Components/LightManager \
    ../../../Utility/Config \
    ../../../Components/PatientManager
