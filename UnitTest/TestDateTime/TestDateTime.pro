TEMPLATE = app

TARGET = TestDateTime

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS += \
    ../../Utility/DateTime/DateTime.h

SOURCES += \
    TestDateTime.cpp \
    ../../Utility/DateTime/DateTime.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Utility/Config/Config.cpp \
    ../../Utility/Config/IConfig.cpp \
    ../../Utility/Config/XmlParser.cpp

INCLUDEPATH += \
    ../../Utility/Debug \
    ../../Utility/Config \
    ../../Utility/DateTime \
