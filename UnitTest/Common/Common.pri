QT += xml

# project base dir
PROJECT_BASE = ../../..

SOURCES += $$PROJECT_BASE/Utility/Debug/Debug.cpp \
    $$PROJECT_BASE/Utility/Utility.cpp

HEADERS += $$PROJECT_BASE/Utility/Debug/Debug.h \
    $$PROJECT_BASE/Utility/Utility.h

INCLUDEPATH += $$PROJECT_BASE/Utility \
    $$PROJECT_BASE/Utility/Debug

