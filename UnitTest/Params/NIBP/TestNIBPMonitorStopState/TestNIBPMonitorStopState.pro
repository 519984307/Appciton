#-------------------------------------------------
#
# Project created by QtCreator 2019-03-25T17:00:37
#
#-------------------------------------------------
TARGET = testnibpmonitorstopstate
QT       += testlib core widgets  gui xml

CONFIG += qt console warn_on depend_includepath testcase
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

DEFINES += SRCDIR=\\\"$$PWD/\\\"
include("../../../Common/googletest.pri")
include("../../../Common/runtest.pri")
include("../../../Common/Common.pri")
SOURCES += \
    TestNIBPMonitorStopState.cpp \
    main.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStopState.cpp \
    ../../../../Params/NIBPParam/NIBPParamInterface.cpp \
    ../../../../Params/NIBPParam/NIBPTrigger/NIBPCountdownTimeInterface.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPState.cpp \
    ../../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../../Utility/Config/IConfig.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.cpp \
    ../../../../Utility/ErrorLog/ErrorLogItem.cpp \


HEADERS += \
    TestNIBPMonitorStopState.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStopState.h \
    ../../../../Params/NIBPParam/NIBPParamInterface.h \
    ../../../../Params/NIBPParam/NIBPTrigger/NIBPCountdownTimeInterface.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachineDefine.h \
    ../../../../Params/NIBPParam/NIBPDefine.h \
    ../../../../Params/NIBPParam/NIBPEventDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStateDefine.h \
    ../../../../Params/NIBPParam/NIBPProviderIFace.h \
    ../../../../Components/PatientManager/PatientDefine.h \
    ../../../../Utility/BaseDefine.h \
    ../../../../Utility/LanguageManager/LanguageManager.h \
    ../../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../../Utility/Config/IConfig.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPState.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStateDefine.h \
    ../../../../Params/NIBPParam/NIBPSymbol.h \
    ../../../../Utility/ErrorLog/ErrorLogItem.h \
    ../TestNIBPMonitorStandbyState/MockNIBPCountdownTime.h \
    ../MockNIBPParam.h


INCLUDEPATH += \
     ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine \
     ../../../../Params/NIBPParam \
     ../../../../Params/NIBPParam/NIBPTrigger \
    ../../../../Params/NIBPParam/NIBPStateMachine \
    ../../../../Components/PatientManager \
    ../../../../Utility \
     ../../../../Utility/LanguageManager \
    ../../../../Utility/Config \
    ../../../../Utility/ErrorLog \
    ../../../../Params/NIBPParam/NIBPStateMachine  \
    ../TestNIBPMonitorStandbyState \
    ..








