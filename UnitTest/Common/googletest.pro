TARGET = googletest
TEMPLATE = lib
CONFIG += staticlib create_prl


GTEST_SRCDIR = $$PWD/googletest/googletest
GMOCK_SRCDIR = $$PWD/googletest/googlemock

INCLUDEPATH = $$PWD \
    $$GTEST_SRCDIR \
    $$GTEST_SRCDIR/include \
    $$GMOCK_SRCDIR \
    $$GMOCK_SRCDIR/include

SOURCES += \
    $$GTEST_SRCDIR/src/gtest-all.cc \
    $$GMOCK_SRCDIR/src/gmock-all.cc

HEADERS += $$PWD/googletest.h

DESTDIR = $$PWD/lib
