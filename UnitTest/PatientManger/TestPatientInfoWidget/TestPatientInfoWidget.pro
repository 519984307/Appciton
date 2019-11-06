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
include("../../Common/runtest.pri")

SOURCES += main.cpp \
    TestPatientInfoWidget.cpp \
    ../../../Components/PatientManager/PatientInfoWidget.cpp \
    ../../../Components/PatientManager/PatientManager.cpp \
    ../../../Components/WindowManager/IWidget.cpp \
    ../../../Utility/LanguageManager/LanguageManager.cpp \
    ../../../Utility/FontManager/FontManagerInterface.cpp \
    ../../../Utility/DataStorage/DataStorageDirManagerInterface.cpp \
    ../../../Utility/Widgets/Base/Dialog.cpp \
    ../../../Components/WindowManager/WindowManagerInterface.cpp \
    ../../../Components/PatientManager/PatientInfoWindowInterface.cpp \
    ../../../Components/System/SystemManagerInterface.cpp \
    ../../../Components/ConfigManager/ConfigManagerInterface.cpp \
    ../../../Components/Alarm/AlarmIndicatorInterface.cpp \
    ../../../Params/NIBPParam/NIBPParamInterface.cpp \
    ../../../Params/ECGParam/ECGParamInterface.cpp \
    ../../../Params/ECGParam/ECGDupParamInterface.cpp \
    ../../../Utility/Config/IConfig.cpp \
    ../../../Utility/Widgets/Base/Button.cpp \
    ../../../Utility/Widgets/Base/ThemeManager.cpp \
    ../../../Components/SoundManager/SoundManagerInterface.cpp

HEADERS += \
    TestPatientInfoWidget.h \
    ../../../Components/PatientManager/PatientInfoWidget.h \
    ../../../Components/PatientManager/PatientManager.h \
    ../../../Components/PatientManager/PatientInfoWidgetInterface.h \
    ../../../Components/WindowManager/IWidget.h \
    ../../../Utility/LanguageManager/LanguageManager.h \
    ../../../Utility/FontManager/FontManagerInterface.h \
    ../../../Utility/DataStorage/DataStorageDirManagerInterface.h \
    ../../../Utility/Widgets/Base/Dialog.h \
    ../../../Components/WindowManager/WindowManagerInterface.h \
    ../../../Components/PatientManager/PatientInfoWindowInterface.h \
    ../../../Components/System/SystemManagerInterface.h \
    ../../../Components/ConfigManager/ConfigManagerInterface.h \
    ../../../Components/Alarm/AlarmIndicatorInterface.h \
    ../../../Params/NIBPParam/NIBPParamInterface.h \
    ../../../Params/ECGParam/ECGParamInterface.h \
    ../../../Params/ECGParam/ECGDupParamInterface.h \
    ../../../Utility/Config/IConfig.h \
    ../../../Utility/Widgets/Base/Button.h \
    ../../../Utility/Widgets/Base/ThemeManager.h \
    ../../Utility/MockFontManager.h \
    ../../Utility/MockWindowManager.h \
    ../../../Components/SoundManager/SoundManagerInterface.h

INCLUDEPATH += \
    ../../../Components/PatientManager \
    ../../../Components/WindowManager \
    ../../../Components/ParamManager \
    ../../../Utility/LanguageManager \
    ../../../Utility/Config \
    ../../../Components/ConfigManager \
    ../../../Components/Alarm \
    ../../../Components/System \
    ../../../Utility/FontManager \
    ../../../Utility/Widgets/Base \
    ../../Params \
    ../../../Params/ECGParam \
    ../../../Params/O2Param \
    ../../../Params/NIBPParam \
    ../../../Utility/DataStorage \
    ../../Alarm \
    ../../Utility \
    ../../../Components/SoundManager \

RESOURCES += \
    ../../../Utility/Widgets/Base/res/ui.qrc

