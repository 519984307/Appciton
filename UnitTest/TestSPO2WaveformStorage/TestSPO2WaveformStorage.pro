TEMPLATE = app

TARGET = TestECGWaveformStorage

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS +=

SOURCES += \
    TestSPO2WaveformStorage.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Utility/DataStorage/DataFileCtrl.cpp \
    ../../Components/PatientData/SPO2WaveformStorage.cpp

INCLUDEPATH += \
    ../../Params \
    ../../Components/PatientData \
    ../../Utility \
    ../../Utility/Debug \
    ../../Utility/DataStorage
