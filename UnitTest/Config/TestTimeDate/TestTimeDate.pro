QT       += testlib

QT       -= gui

TARGET = testtimedate
CONFIG += qt console warn_on depend_includepath testcase
CONFIG   -= app_bundle

include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    ../../../Utility/TimeDate/TimeDate.cpp \
    ../../../Utility/Config/IConfig.cpp \
    main.cpp \
    TestTimeDate.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../Utility/TimeDate/TimeDate.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/TimeDate/TimeDefine.h \
    TestTimeDate.h

INCLUDEPATH += \
    ../../../Utility/TimeDate \
    ../../../Utility/Config \
