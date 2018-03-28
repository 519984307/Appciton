TEMPLATE = app

TARGET = TestSystemTick

CONFIG += warn_on

QT += core

LIBS += -ldl

HEADERS += \
    ../../Components/System/SystemTick.h

SOURCES += \
    TestSystemTick.cpp \
    ../../Components/System/SystemTick.cpp \
    ../../Utility/Debug/Debug.cpp

INCLUDEPATH += \
    ../../Utility/Debug \
    ../../Components/System \
