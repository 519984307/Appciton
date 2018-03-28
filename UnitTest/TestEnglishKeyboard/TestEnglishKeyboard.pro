TEMPLATE = app

TARGET = TestEnglishKeyboard

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS += \
    TestEnglishKeyboard.h \
    ../../Utility/Widgets/InputMethod/Key.h \
    ../../Utility/Widgets/InputMethod/EnglishPanel.h \
    ../../Utility/Widgets/PopupWidget.h \
    ../../Utility/Config/Config.h \
    ../../Utility/Config/IConfig.h \
    ../../Utility/Config/XmlParser.h \

SOURCES += \
    TestEnglishKeyboard.cpp \
    ../../Utility/FontManager/FontManager.cpp \
    ../../Utility/LanguageManager/LanguageManager.cpp \
    ../../Utility/Widgets/InputMethod/Key.cpp \
    ../../Utility/Widgets/InputMethod/EnglishPanel.cpp \
    ../../Utility/Widgets/PopupWidget.cpp \
    ../../Utility/Config/Config.cpp \
    ../../Utility/Config/IConfig.cpp \
    ../../Utility/Config/XmlParser.cpp \

INCLUDEPATH += \
    ../../Utility \
    ../../Utility/Config \
    ../../Utility/Debug \
    ../../Utility/Widgets \
    ../../Utility/Widgets/InputMethod \
    ../../Utility/FontManager \
    ../../Utility/LanguageManager \
