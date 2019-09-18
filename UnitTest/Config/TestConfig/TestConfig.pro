QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app
include("../../Common/runtest.pri")

SOURCES +=  main.cpp \
    TestConfig.cpp

HEADERS += \
    TestConfig.h
