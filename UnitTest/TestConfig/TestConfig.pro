TEMPLATE = app

TARGET = TestConfig

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS += \
    ../../Utility/Config/Config.h \
    ../../Utility/Config/ConfigDefine.h \
    ../../Utility/Config/KConfig.h \
    ../../Utility/Config/XmlParser.h

SOURCES += \
    TestConfig.cpp \
    ../../Utility/Config/Config.cpp \
    ../../Utility/Config/IConfig.cpp \
    ../../Utility/Config/XmlParser.cpp \
    ../../Utility/Debug/Debug.cpp

INCLUDEPATH += \
    ../../Utility/Debug \
    ../../Utility/Config \
