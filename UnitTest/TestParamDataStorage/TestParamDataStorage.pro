TEMPLATE = app

TARGET = TestParamDataStorage

CONFIG += warn_on

QT += core xml

LIBS += -ldl

HEADERS +=

SOURCES += \
    TestParamDataStorage.cpp \
    ../../Utility/Debug/Debug.cpp \
    ../../Utility/DataStorage/DataFileCtrl.cpp \
    ../../Components/PatientData/ParamDataStorage.cpp

INCLUDEPATH += \
    ../../Params \
    ../../Components/PatientData \
    ../../Utility \
    ../../Utility/Debug \
    ../../Utility/DataStorage
