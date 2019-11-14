QT += xml

CONFIG += link_prl

# project base dir
PROJECT_BASE = $$PWD/../..

SOURCES += $$PROJECT_BASE/Utility/Debug/Debug.cpp

HEADERS += $$PROJECT_BASE/Utility/Debug/Debug.h

INCLUDEPATH += \
    $$PROJECT_BASE                                          \
    $$PROJECT_BASE/Utility                                  \
    $$PROJECT_BASE/Framewrok                                \
    $$PROJECT_BASE/Utility/Debug

LIBS += -L$$PROJECT_BASE/Framework/lib -lFramework
