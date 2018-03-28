TEMPLATE = app

TARGET = TestECGWaveformStorage

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS +=


SOURCES += \
    TestCO2WaveformStorage.cpp \
    ../../Utility/DataStorage/DataFileCtrl.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Components/PatientData/CO2WaveformStorage.cpp

INCLUDEPATH += \
    ../../Utility/Debug \
    ../../Utility/DataStorage \
    ../../Components/PatientData \
