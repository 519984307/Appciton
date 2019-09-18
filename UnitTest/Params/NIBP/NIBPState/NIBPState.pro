#-------------------------------------------------
#
# Project created by QtCreator 2019-03-19T18:21:29
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_nibpstatetest
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


SOURCES += \
    TestNIBPState.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPState.cpp \
    main.cpp \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.cpp \
    ../../../../Params/NIBPParam/NIBPParamInterface.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

include("../../../Common/googletest.pri")
include("../../../Common/runtest.pri")

HEADERS += \
    TestNIBPState.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPState.h \
    ../../../../Params/NIBPParam/NIBPEventDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachineDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine/NIBPMonitorStateDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine/NIBPServiceStateDefine.h \
    ../../../../Utility/Debug/Debug.h \
    ../../../../Params/NIBPParam/NIBPProviderIFace.h \
    ../../../../Components/PatientManager/PatientDefine.h \
    ../../../../Utility/BaseDefine.h \
    ../../../../Params/NIBPParam/NIBPDefine.h \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPStateMachine.h \
    ../../../../Params/NIBPParam/NIBPParamInterface.h \
    MockNIBPStateMachine.h

INCLUDEPATH += \
    ../../../../Params/NIBPParam/NIBPStateMachine   \
    ../../../../Params/NIBPParam    \
    ../../../../Params/NIBPParam/NIBPStateMachine   \
     ../../../../Params/NIBPParam/NIBPStateMachine/NIBPMonitorStateMachine  \
    ../../../../Params/NIBPParam/NIBPStateMachine/NIBPServiceStateMachine   \
    ../../../../Utility/Debug   \
    ../../../../Components/PatientManager   \
    ../../../../Utility \


