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
        ErrorLog/ErrorLogItem.cpp                                                   \
        ErrorLog/ErrorLogInterface.cpp                                              \
        ErrorLog/ErrorLog.cpp                                                       \
        Language/LanguageManager.cpp                                                \
        Language/Translator.cpp                                                     \
        TimeDate/TimeDate.cpp                                                       \
        UI/Button.cpp                                                               \
        UI/ComboBox.cpp                                                             \
        UI/Dialog.cpp                                                               \
        UI/DropList.cpp                                                             \
        UI/FloatScrollBar.cpp                                                       \
        UI/Frame.cpp                                                                \
        UI/FrameItem.cpp                                                            \
        UI/ListDataModel.cpp                                                        \
        UI/ListView.cpp                                                             \
        UI/ListViewItemDelegate.cpp                                                 \
        UI/MenuContent.cpp                                                          \
        UI/MenuSidebar.cpp                                                          \
        UI/MenuSidebarItem.cpp                                                      \
        UI/MenuWindow.cpp                                                           \
        UI/MoveButton.cpp                                                           \
        UI/PasswordWidget.cpp                                                       \
        UI/PopupList.cpp                                                            \
        UI/PopupListItem.cpp                                                        \
        UI/PopupMoveEditor.cpp                                                      \
        UI/PopupNumEditor.cpp                                                       \
        UI/ScrollArea.cpp                                                           \
        UI/ShadowEffect.cpp                                                         \
        UI/SpinBox.cpp                                                              \
        UI/TableHeaderView.cpp                                                      \
        UI/TableView.cpp                                                            \
        UI/TableViewItemDelegate.cpp                                                \
        UI/ThemeManager.cpp                                                         \
        UI/KineticScroller/qkineticscroller.cpp                                     \
        UI/KineticScroller/qscrollareakineticscroller.cpp                           \
        Windows/WindowManagerInterface.cpp                                          \
        Font/FontManagerInterface.cpp                                               \
        Sound/SoundManagerInterface.cpp                                             \


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
        ErrorLog/ErrorLogItem.h                                                     \
        ErrorLog/ErrorLogInterface.h                                                \
        ErrorLog/ErrorLog.h                                                         \
        Language/LanguageManager.h                                                  \
        Language/Translator.h                                                       \
        TimeDate/TimeDefine.h                                                       \
        TimeDate/TimeSymbol.h                                                       \
        TimeDate/TimeDate.h                                                         \
        UI/Button.h                                                                 \
        UI/ComboBox.h                                                               \
        UI/Dialog.h                                                                 \
        UI/DropList.h                                                               \
        UI/FloatScrollBar.h                                                         \
        UI/Frame.h                                                                  \
        UI/FrameItem.h                                                              \
        UI/ItemEditInfo.h                                                           \
        UI/ListDataModel.h                                                          \
        UI/ListView.h                                                               \
        UI/ListViewItemDelegate.h                                                   \
        UI/MenuContent.h                                                            \
        UI/MenuSidebar.h                                                            \
        UI/MenuSidebarItem.h                                                        \
        UI/MenuWindow.h                                                             \
        UI/MoveButton.h                                                             \
        UI/PasswordWidget.h                                                         \
        UI/PopupList.h                                                              \
        UI/PopupListItem.h                                                          \
        UI/PopupMoveEditor.h                                                        \
        UI/PopupNumEditor.h                                                         \
        UI/ScrollArea.h                                                             \
        UI/ShadowEffect.h                                                           \
        UI/SpinBox.h                                                                \
        UI/TableHeaderView.h                                                        \
        UI/TableView.h                                                              \
        UI/TableViewItemDelegate.h                                                  \
        UI/TableViewItemDelegate_p.h                                                \
        UI/ThemeManager.h                                                           \
        UI/KineticScroller/qkineticscroller.h                                       \
        UI/KineticScroller/qkineticscroller_p.h                                     \
        UI/KineticScroller/qscrollareakineticscroller.h                             \
        Windows/WindowManagerInterface.h                                            \
        Font/FontManagerInterface.h                                                 \
        Sound/SoundManagerInterface.h                                               \
        Alarm/AlarmTypeDefine.h                                                     \

DEPENDPATH += \
        Utility                                                                     \
        Config                                                                      \
        Uart                                                                        \
        Storage                                                                     \
        SOUP/Json                                                                   \
        Language                                                                    \
        TimeDate                                                                    \
        UI                                                                          \
        UI/KineticScroller                                                          \
        Windows                                                                     \
        Font                                                                        \
        Sound                                                                       \

INCLUDEPATH += \
        Utility                                                                     \
        Config                                                                      \
        Uart                                                                        \
        Storage                                                                     \
        SOUP/Json                                                                   \
        Language                                                                    \
        TimeDate                                                                    \
        UI                                                                          \
        UI/KineticScroller                                                          \
        Windows                                                                     \
        Font                                                                        \
        Sound                                                                       \

RESOURCES += UI/res/ui.qrc

unix {
    target.path = /usr/lib
    INSTALLS += target
}

UI_DIR = ui
MOC_DIR = moc
OBJECTS_DIR = obj
DESTDIR = $$PWD/lib

QMAKE_CLEAN += -r $$DESTDIR
