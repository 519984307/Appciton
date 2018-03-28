TEMPLATE = app

TARGET = TestDataFileCtrl

CONFIG += warn_on

QT += core

LIBS += -ldl

HEADERS += \
    ../../Utility/DataStorage/DataFileCtrl.h

SOURCES += \
    TestDataFileCtrl.cpp \
    ../../Utility/DataStorage/DataFileCtrl.cpp \
    ../../Utility/Debug/Debug.cpp

INCLUDEPATH += \
    ../../Utility/Debug \
    ../../Utility/DataStorage \
