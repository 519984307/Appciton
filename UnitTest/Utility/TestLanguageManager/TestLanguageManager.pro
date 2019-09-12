QT -= gui
QT += testlib xml core

CONFIG += c++11 console
CONFIG -= app_bundle

include("../../Common/googletest.pri")
include("../../Common/runtest.pri")

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += main.cpp \
    TestLanguageManager.cpp \
    ../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/Debug/Debug.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../Utility/Config/Config.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp

HEADERS += \
    TestLanguageManager.h \
    ../../../Utility/LanguageManager/LanguageManager.h \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/Debug/Debug.h \
    ../../../Components/PatientManager/PatientDefine.h \
    ../../../Utility/BaseDefine.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../Utility/Config/Config.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../../Components/PatientManager/PatientDefine.h

INCLUDEPATH += \
    ../../../Utility \
    ../../../Utility/LanguageManager \
    ../../../Utility/Config \
    ../../../Utility/Debug \
    ../../../Utility/ErrorLog \
    ../../../Components/PatientManager
