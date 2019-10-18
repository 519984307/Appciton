/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/11
 **/


#pragma once
#include "IWidget.h"
#include <QMap>
#include <QSignalMapper>
#include <QBasicTimer>

enum SystemIconLabel
{
    SYSTEM_ICON_LABEL_NONE = -1,
    SYSTEM_ICON_LABEL_WIFI,      // wifi图标
//    SYSTEM_ICON_LABEL_PACER,     // 起搏图标
    SYSTEM_ICON_LABEL_USB,       // usb图标
    SYSTEM_ICON_LABEL_NR,
};

// 图标
enum SystemStatusIcon
{
    SYSTEM_ICON_NONE = -1,
    SYSTEM_ICON_PACER_OFF,
    SYSTEM_ICON_PACER_ON,
    SYSTEM_ICON_WIFI_CONNECTED,
    SYSTEM_ICON_WIFI_DISCONNECTED,
    SYSTEM_ICON_WIFI_CLOSED,
    SYSTEM_ICON_USB_CONNECTED,
    SYSTEM_ICON_NR,
};

class QPixmap;
/***************************************************************************************************
 * 系统状态栏，主要显示一些系统提示图标。
 **************************************************************************************************/
class SystemStatusBarWidget : public IWidget
{
    Q_OBJECT
public:
    static SystemStatusBarWidget &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SystemStatusBarWidget();
        }
        return *_selfObj;
    }
    static SystemStatusBarWidget *_selfObj;
    ~SystemStatusBarWidget();

signals:
    void iconClicked(int iconlabel);

protected:
    void showEvent(QShowEvent *e);
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);

public:
    void hideIcon(SystemIconLabel iconlabel);
    void changeIcon(SystemIconLabel iconlabel, int status, bool enableFocus = false);

    virtual void getSubFocusWidget(QList<QWidget*> &/*subWidget*/) const;

private slots:
    void onIconClicked(int iconLabel);
    void onTouchClicked(IWidget* w);

private:
    SystemStatusBarWidget();

    QMap<SystemStatusIcon, QPixmap> _icons;     // 图标。
    QMap<SystemIconLabel, IWidget *> _iconMap;
    QSignalMapper *_signalMapper;
    QBasicTimer _timer;
};
#define sysStatusBar (SystemStatusBarWidget::construction())
