#-------------------------------------------------
#
# Project created by QtCreator 2019-03-22T10:53:09
#
#-------------------------------------------------

TARGET = testnibpmonitorstandbystatetest
QT       += testlib core widgets  gui xml

CONFIG += qt console warn_on depend_includepath testcase
CONFIG   -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
include("../../../Common/googletest.pri")
include("../../../Common/runtest.pri")
include("../../../Common/Common.pri")
DEFINES += SRCDIR=\\\"$$PWD/\\\"
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStandbyState.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.cpp \
    ../../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../../Params/NIBPParam/NIBPTrigger/NIBPCountdownTimeInterface.cpp \
    ../../../../Components/System/SystemManagerInterface.cpp \
    ../../../../Params/NIBPParam/NIBPParamInterface.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPState.cpp \
    ../../../../Utility/Config/IConfig.cpp \
    TestNIBPMonitorStandbyState.cpp

HEADERS += \
    TestNIBPMonitorStandbyState.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStandbyState.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPState.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStateDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStateDefine.h \
    ../../../../Params/NIBPParam/NIBPDefine.h \
    ../../../../Params/NIBPParam/NIBPSymbol.h \
    ../../../../Params/NIBPParam/NIBPProviderIFace.h \
    ../../../../Utility/LanguageManager/LanguageManager.h \
    ../../../../Utility/Config/IConfig.h \
    ../../../../Params/NIBPParam/NIBPTrigger/NIBPCountdownTimeInterface.h \
    ../../../../Components/PatientManager/PatientDefine.h \
    ../../../../Components/System/SystemManagerInterface.h \
    ../../../../Utility/BaseDefine.h \
    ../../../../Params/NIBPParam/NIBPParamInterface.h \
    ../../../../Params/NIBPParam/NIBPEventDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachineDefine.h \
    MockNIBPCountdownTime.h \
    ../MockNIBPParam.h

INCLUDEPATH += \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine \
    ../../../../Params/NIBPParam/NIBPStateMachine  \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine \
    ../../../../Params/NIBPParam \
    ../../../../Utility/LanguageManager \
    ../../../../Utility/Config \
    ../../../../Params/NIBPParam/NIBPTrigger \
    ../../../../Components/PatientManager \
    ../../../../Components/System \
    ../../../../Components/ParamManager \
    ../../../../Utility \
    ..


