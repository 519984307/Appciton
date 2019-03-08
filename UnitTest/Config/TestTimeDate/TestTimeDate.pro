QT       += testlib

QT       -= gui

TARGET = testtimedate
CONFIG += qt console warn_on depend_includepath testcase
CONFIG   -= app_bundle

include("../../Common/Common.pri")
include("../../Common/googletest.pri")

TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp \
    ../../../Utility/TimeDate/TimeDate.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/Config/Config.cpp \
    main.cpp \
    TestTimeDate.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../../Utility/TimeDate/TimeDate.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/Config/Config.h \
    TestTimeDate.h

INCLUDEPATH += \
    ../../../Utility/ErrorLog \
    ../../../Utility/TimeDate \
    ../../../Utility/Config \
