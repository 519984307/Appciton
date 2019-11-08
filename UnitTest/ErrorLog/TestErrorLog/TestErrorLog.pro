QT       += testlib widgets

CONFIG   += qt console warn_on depend_includepath testcase
CONFIG   -= app_bundle

TEMPLATE = app


include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

SOURCES += \
        TestErrorLog.cpp \ 
    main.cpp \
    ../../../Utility/Config/IConfig.cpp \
    mythread.cpp


HEADERS += \
    TestErrorLog.h \
    ../../../Utility/Config/IConfig.h \
    mythread.h



INCLUDEPATH += \
    ../../../Utility/Config \




DEFINES += SRCDIR=\\\"$$PWD/\\\"
