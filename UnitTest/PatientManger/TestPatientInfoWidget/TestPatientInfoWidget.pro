QT += testlib svg gui widgets

CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include("../../Common/Common.pri")
include("../../Common/googletest.pri")

SOURCES += main.cpp \
    TestPatientInfoWidget.cpp \
    ../../../Components/PatientManager/PatientInfoWidget.cpp \
    ../../../Components/WindowManager/IWidget.cpp \
    ../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../Utility/Config/XmlParser.cpp \
    ../../../Utility/FontManager/FontManagerInterface.cpp \
    ../../../Utility/Widgets/Base/Dialog.cpp \
    ../../../Utility/ErrorLog/ErrorLogInterface.cpp \
    ../../../Components/WindowManager/WindowManagerInterface.cpp \
    ../../../Components/PatientManager/PatientInfoWindowInterface.cpp \
    ../../../Utility/Config/Config.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/ErrorLog/ErrorLogItem.cpp \
    ../../../Utility/Widgets/Base/Button.cpp \
    ../../../Utility/Widgets/Base/ThemeManager.cpp \

HEADERS += \
    TestPatientInfoWidget.h \
    ../../../Components/PatientManager/PatientInfoWidget.h \
    ../../../Components/PatientManager/PatientInfoWidgetInterface.h \
    ../../../Components/WindowManager/IWidget.h \
    ../../../Utility/LanguageManager/LanguageManager.h \
    ../../../Utility/Config/XmlParser.h \
    ../../../Utility/FontManager/FontManagerInterface.h \
    ../../../Utility/Widgets/Base/Dialog.h \
    ../../../Utility/ErrorLog/ErrorLogInterface.h \
    ../../../Components/WindowManager/WindowManagerInterface.h \
    ../../../Components/PatientManager/PatientInfoWindowInterface.h \
    ../../../Utility/Config/Config.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/ErrorLog/ErrorLogItem.h \
    ../../../Utility/Widgets/Base/Button.h \
    ../../../Utility/Widgets/Base/ThemeManager.h \
    ../../Utility/MockFontManager.h \
    ../../Utility/MockWindowManager.h \

INCLUDEPATH += \
    ../../../Components/PatientManager \
    ../../../Components/WindowManager \
    ../../../Utility/LanguageManager \
    ../../../Utility/Config \
    ../../../Utility/FontManager \
    ../../../Utility/Widgets/Base \
    ../../../Utility/ErrorLog \
    ../../Utility \

RESOURCES += \
    ../../../Utility/Widgets/Base/res/ui.qrc

