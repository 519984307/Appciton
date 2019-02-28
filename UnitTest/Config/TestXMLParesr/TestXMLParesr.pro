QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app


include("../../Common/Common.pri")
include("../../Common/googletest.pri")


SOURCES +=  \
    $$PROJECT_BASE/Utility/Config/XmlParser.cpp \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogItem.cpp \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogInterface.cpp \
    main.cpp \
    TestXmlParser.cpp

HEADERS += MockErrorLog.h \
    $$PROJECT_BASE/Utility/Config/XmlParser.h \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogItem.h   \
    $$PROJECT_BASE/Utility/ErrorLog/ErrorLogInterface.h \
    TestXmlParser.h

INCLUDEPATH += \
    $$PROJECT_BASE/Utility/Config \
    $$PROJECT_BASE/Utility/ErrorLog

RESOURCES += \
    testresources.qrc
