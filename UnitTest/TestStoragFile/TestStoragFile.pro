QT += core
QT -= gui

CONFIG += warn_on


TARGET = TestStoragFile
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    ../../Utility/DataStorage/StorageFile.cpp \
    TeststorageFile.cpp

HEADERS += \
    ../../Utility/DataStorage/StorageFile.h

INCLUDEPATH += ../../Utility/DataStorage \
            ../../Utility/Debug
