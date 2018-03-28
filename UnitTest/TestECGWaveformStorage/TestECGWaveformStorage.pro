TEMPLATE = app

TARGET = TestECGWaveformStorage

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS +=

SOURCES += \
    TestECGWaveformStorage.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Utility/DataStorage/DataFileCtrl.cpp \
    ../../Components/PatientData/ECGWaveformStorage.cpp

INCLUDEPATH += \
    ../../Params \
    ../../Components/PatientData \
    ../../Components/ParamManager\
    ../../Utility/Debug \
    ../../Utility/DataStorage
