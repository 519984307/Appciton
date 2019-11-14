TEMPLATE = subdirs

Framework.subdir = $$PWD/../Framework
Googletest.file = Common/googletest.pro

SUBDIRS += \
    Googletest \
    Framework \
    Config \
    Providers \
    Params \
    Storage \
    Widgets \
    Utility \
    Recorder \
    Alarm \
    USB \
    Sound \
    ErrorLog \
    Upgrade \
    Freeze \
    PatientManger \
    Trend \
    Event \
    ScreenLayout

CONFIG += ordered
