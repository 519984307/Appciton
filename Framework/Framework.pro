#-------------------------------------------------
#
# Project created by QtCreator 2019-10-31T18:19:29
#
#-------------------------------------------------

QT       += network svg xml

TARGET = Framework
TEMPLATE = lib
CONFIG += staticlib create_prl

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Utility/Utility.cpp                                                         \
        Utility/ImageQuant.cpp                                                      \
        Utility/RunLengthEncode.cpp                                                 \
        Config/XmlParser.cpp                                                        \
        Config/Config.cpp                                                           \
        Uart/Uart.cpp                                                               \
        Uart/UartSocket.cpp                                                         \
        Storage/StorageFile.cpp                                                     \
        Storage/StorageManager.cpp                                                  \
        SOUP/Json/json_parser.cc                                                    \
        SOUP/Json/json_scanner.cc                                                   \
        SOUP/Json/json_scanner.cpp                                                  \
        SOUP/Json/parser.cpp                                                        \
        SOUP/Json/parserrunnable.cpp                                                \
        SOUP/Json/qobjecthelper.cpp                                                 \
        SOUP/Json/serializer.cpp                                                    \
        SOUP/Json/serializerrunnable.cpp                                            \


HEADERS += \
        Utility/Utility.h                                                           \
        Utility/Unit.h                                                              \
        Utility/crc8.h                                                              \
        Utility/RingBuff.h                                                          \
        Utility/OrderedMap.h                                                        \
        Config/XmlParser.h                                                          \
        Config/Config.h                                                             \
        Uart/Uart.h                                                                 \
        Uart/UartSocket.h                                                           \
        Storage/IStorageBackend.h                                                   \
        Storage/StorageFile.h                                                       \
        Storage/StorageManager.h                                                    \
        SOUP/Json/qjson_export.h                                                    \
        SOUP/Json/parser.h                                                          \
        SOUP/Json/serializer.h                                                      \

DEPENDPATH += \
        Utility                                                                     \
        Config                                                                      \
        Uart                                                                        \
        Storage                                                                     \
        SOUP/Json                                                                    \

INCLUDEPATH += \
        Utility                                                                      \
        Config                                                                       \
        Uart                                                                         \
        Storage                                                                      \
        SOUP/Json                                                                    \


unix {
    target.path = /usr/lib
    INSTALLS += target
}

UI_DIR = ui
MOC_DIR = moc
OBJECTS_DIR = obj
DESTDIR = $$PWD/lib

QMAKE_CLEAN += -r $$DESTDIR
