QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app


include("../../Common/Common.pri")
include("../../Common/googletest.pri")
include("../../Common/runtest.pri")


SOURCES +=  \
    $$PROJECT_BASE/Utility/Config/IConfig.cpp \
    main.cpp \
    TestXmlParser.cpp

HEADERS += MockErrorLog.h \
    $$PROJECT_BASE/Utility/Config/IConfig.h \
    TestXmlParser.h

INCLUDEPATH += \
    $$PROJECT_BASE/Utility/Config \

RESOURCES += \
    testresources.qrc
