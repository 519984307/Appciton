TEMPLATE = app

TARGET = TestNIBPDataStorage

CONFIG += warn_on

QT += core

LIBS += -ldl

HEADERS +=

SOURCES += \
    TestNIBPDataStorage.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Utility/DataStorage/DataFileCtrl.cpp \
    ../../Components/PatientData/NIBPDataStorage.cpp

INCLUDEPATH += \
    ../../Params \
    ../../Components/PatientData \
    ../../Utility/ \
    ../../Utility/Debug \
    ../../Utility/DataStorage
