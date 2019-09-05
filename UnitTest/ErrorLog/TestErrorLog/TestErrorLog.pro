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
    ../../../Utility/ErrorLog/ErrorLog.cpp \
    ../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../SOUP/Json/serializer.cpp \
    ../../../SOUP/Json/parser.cpp \
    ../../../Utility/DataStorage/StorageFile.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp \
    ../../../Utility/Config/Config.cpp \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../SOUP/Json/json_scanner.cpp \
    ../../../SOUP/Json/json_parser.cc \
    mythread.cpp


HEADERS += \
    TestErrorLog.h \
    ../../../Utility/ErrorLog/ErrorLog.h \
    ../../../Utility/LanguageManager/LanguageManager.h \
    ../../../SOUP/Json/serializer.h \
    ../../../SOUP/Json/parser.h \
    ../../../Utility/DataStorage/StorageFile.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../../Utility/Config/Config.h \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../SOUP/Json/json_parser.hh \
    ../../../SOUP/Json/json_scanner.h \
    mythread.h



INCLUDEPATH += \
     ../../../Utility/ErrorLog \
     ../../../SOUP/Json \
    ../../../Utility/DataStorage \
    ../../../Utility/LanguageManager \
    ../../../Utility/Config \
    ../../../Utility/ErrorLog \
    ../../../Utility/Config \




DEFINES += SRCDIR=\\\"$$PWD/\\\"
