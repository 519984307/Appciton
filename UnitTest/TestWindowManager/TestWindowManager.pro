TEMPLATE = app

TARGET = TestWindowManager

CONFIG += warn_on

QT += core gui xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -ldl

HEADERS += \
    ../../Components/WindowManager/IWidget.h \
    ../../Components/WindowManager/WindowManager.h \
    ../../Components/Alarm//TechAlarmInfoWidget.h \
    ../../Components/Alarm/AlarmMuteIndicator.h \
    ../../Components/Alarm//PhysAlarmInfoWidget.h \
    ../../Components/Softkey/SoftKeyManager.h \
    ../../Components/Softkey/SoftKeyWidget.h \
    ../../Components/System/SystemStateWidget.h \
    ../../Utility/Debug/Debug.h \
    ../../Utility/Config/Config.h \
    ../../Utility/Config/XmlParser.h \
    ../../Components/TimeManager/DateTimeWidget.h \
    ../../Components/TimeManager/ElapsedTimeWidget.h \
    ../../Components/PatientManager/PatientInfoWidget.h \

SOURCES += \
    TestWindowManager.cpp \
    ../../Components/WindowManager/IWidget.cpp \
    ../../Components/WindowManager/WindowManager.cpp \
    ../../Components/Alarm//TechAlarmInfoWidget.cpp \
    ../../Components/Alarm/AlarmMuteIndicator.cpp \
    ../../Components/Alarm//PhysAlarmInfoWidget.cpp \
    ../../Components/Softkey/SoftKeyManager.cpp \
    ../../Components/Softkey/SoftKeyWidget.cpp \
    ../../Components/System/SystemStateWidget.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Utility/Config/Config.cpp \
    ../../Utility/Config/IConfig.cpp \
    ../../Utility/Config/XmlParser.cpp \
    ../../Utility/FontManager/FontManager.cpp \
    ../../Utility/LanguageManager/LanguageManager.cpp \
    ../../Components/TimeManager/DateTimeWidget.cpp \
    ../../Components/TimeManager/ElapsedTimeWidget.cpp \
    ../../Components/PatientManager/PatientManager.cpp \
    ../../Components/PatientManager/PatientInfoWidget.cpp \


INCLUDEPATH += \
    ../../Utility \
    ../../Utility/Debug \
    ../../Utility/Config \
    ../../Utility/FontManager \
    ../../Utility/LanguageManager \
    ../../Components/WindowManager \
    ../../Components/WindowManager/WinLayout \
    ../../Components/Alarm \
    ../../Components/TimeManager \
    ../../Components/PatientManager \
    ../../Components/Softkey \
    ../../Components/System \
    ../../Components/MenuManager \
    ../../Utility/Widgets \
