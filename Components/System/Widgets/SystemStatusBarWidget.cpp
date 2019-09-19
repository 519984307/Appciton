/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include <QPixmap>
#include <QPainter>
#include "SystemStatusBarWidget.h"
#include "FontManager.h"
#include "PatientManager.h"
#include <QHBoxLayout>
#include <QLabel>
#include "NetworkManager.h"
#include "WindowManager.h"
#include "LayoutManager.h"
#include "WiFiProfileWindow.h"
#include "IConfig.h"
#include "SystemManager.h"
#include "MessageBox.h"
#include "USBManager.h"
#include "LanguageManager.h"

SystemStatusBarWidget *SystemStatusBarWidget::_selfObj = NULL;
#define ICON_WIDTH 32

/**************************************************************************************************
 * 隐藏图标。
 *************************************************************************************************/
void SystemStatusBarWidget::hideIcon(SystemIconLabel iconlabel)
{
    if (iconlabel >= SYSTEM_ICON_LABEL_NR)
    {
        return;
    }

    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(iconlabel)->children().at(1));
    if (l)
    {
        l->setPixmap(_icons[SYSTEM_ICON_NONE]);
        _iconMap.value(iconlabel)->setFocusPolicy(Qt::NoFocus);
    }
}

/**************************************************************************************************
 * 函数说明:
 *         修改图标。
 * 参数说明:
 *         iconlabel:对应图标标签。
 *         status:具体图标。
 *         enableFocus:是否可以获取焦点。
 *************************************************************************************************/
void SystemStatusBarWidget::changeIcon(SystemIconLabel iconlabel, int status, bool enableFocus)
{
    if (iconlabel >= SYSTEM_ICON_LABEL_NR)
    {
        return;
    }

    if (status >= SYSTEM_ICON_NR)
    {
        return;
    }

    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(iconlabel)->children().at(1));
    if (l)
    {
        l->setPixmap(_icons[(SystemStatusIcon)status]);
        IWidget *w = _iconMap.value(iconlabel);
        if (NULL != w)
        {
            if (enableFocus)
            {
                if (w->focusPolicy() == Qt::NoFocus)
                {
                    w->setFocusPolicy(Qt::StrongFocus);
                    w->setEnabled(true);
                    layoutManager.updateTabOrder();
                }
            }
            else
            {
                if (w->focusPolicy() == Qt::StrongFocus)
                {
                    w->setFocusPolicy(Qt::NoFocus);
                    w->setEnabled(false);
                    layoutManager.updateTabOrder();
                }
            }
        }
    }
}

/**************************************************************************************************
 * 函数说明:
 *         获取焦点子控件。
 *************************************************************************************************/
void SystemStatusBarWidget::getSubFocusWidget(QList<QWidget *> &subWidgets) const
{
    subWidgets.clear();

    for (int i = 0; i < SYSTEM_ICON_LABEL_NR; i++)
    {
        IWidget *subWidget = _iconMap.value((SystemIconLabel)i);
        if (NULL != subWidget)
        {
            subWidgets.append(subWidget);
        }
    }
}

/**************************************************************************************************
 * 图标点击事件。
 *************************************************************************************************/
void SystemStatusBarWidget::onIconClicked(int iconLabel)
{
    if (iconLabel == SYSTEM_ICON_LABEL_WIFI && systemManager.isSupport(CONFIG_WIFI))
    {
        WiFiProfileWindow w;
        windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
    }

    if (iconLabel == SYSTEM_ICON_LABEL_USB)
    {
        MessageBox msg(trs("Prompt"), trs("SureSafePopUpUDisk"), true, true);
        connect(&usbManager, SIGNAL(popupUDisk()), &msg, SLOT(reject()));  // 弹出u盘时，主动关闭该消息框
        if (msg.exec() && usbManager.isUSBExist())
        {
            usbManager.stopRawCollectData();
            this->focusNextChild();
        }
        if (usbManager.isUSBExist() == false)  // u盘不存在时，聚焦点主动移动到下一个item
        {
            this->focusNextChild();
        }
    }
}

/**************************************************************************************************
 * 绘画事件。
 *************************************************************************************************/
void SystemStatusBarWidget::paintEvent(QPaintEvent *e)
{
    IWidget::paintEvent(e);
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(20, 20, 20));
}

/***************************************************************************************************
 * timerEvent : use to update wifi icon
 **************************************************************************************************/
void SystemStatusBarWidget::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == _timer.timerId())
    {
        int index = 0;
        machineConfig.getModuleInitialStatus("WIFIEnable", reinterpret_cast<bool*>(&index));
        if (index)
        {
            // 工厂维护中打开wifi模块
            if (networkManager.isWifiTurnOn())
            {
                // 用户维护中打开wifi
                changeIcon(SYSTEM_ICON_LABEL_WIFI, SYSTEM_ICON_WIFI_CONNECTED, true);
            }
            else
            {
                changeIcon(SYSTEM_ICON_LABEL_WIFI, SYSTEM_ICON_WIFI_DISCONNECTED, true);
            }
        }
        else
        {
            changeIcon(SYSTEM_ICON_LABEL_WIFI, SYSTEM_ICON_NONE, false);
        }
    }
}

/**************************************************************************************************
 * 显示事件。
 *************************************************************************************************/
void SystemStatusBarWidget::showEvent(QShowEvent *e)
{
    IWidget::showEvent(e);
    // pacer icon
//    QLabel *l = qobject_cast<QLabel *> (_iconMap.value(SYSTEM_ICON_LABEL_PACER)->children().at(1));
//    if (l)
//    {
//        if (patientManager.getPacermaker())
//        {
//            l->setPixmap(_icons[SYSTEM_ICON_PACER_ON]);
//        }
//        else
//        {
//            l->setPixmap(_icons[SYSTEM_ICON_PACER_OFF]);
//        }
//    }
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SystemStatusBarWidget::SystemStatusBarWidget() : IWidget("SystemStatusBarWidget"),
    _signalMapper(new QSignalMapper(this))
{
    _icons[SYSTEM_ICON_NONE] = QPixmap();
    _icons[SYSTEM_ICON_USB_CONNECTED] = QPixmap("/usr/local/nPM/icons/usb.png");
    _icons[SYSTEM_ICON_WIFI_CONNECTED] = QPixmap("/usr/local/nPM/icons/wifi.png");
    _icons[SYSTEM_ICON_WIFI_DISCONNECTED] = QPixmap("/usr/local/nPM/icons/wifi_disconnected.png");
    _icons[SYSTEM_ICON_WIFI_CLOSED] = QPixmap("/usr/local/nPM/icons/wifi_closed.png");
    _icons[SYSTEM_ICON_PACER_OFF] = QPixmap("/usr/local/nPM/icons/PaceMarkerOff.png");
    _icons[SYSTEM_ICON_PACER_ON] = QPixmap("");  // "/usr/local/nPM/icons/Pacemarkeron.png");打开时不需要图标。

    setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    for (int i = 0; i < SYSTEM_ICON_LABEL_NR; i++)
    {
        IWidget *w = new IWidget(QString("icon%1").arg(i + 1));
        w->setAttribute(Qt::WA_NoSystemBackground);
        if (i == SYSTEM_ICON_LABEL_USB)
        {
            w->setFocusPolicy(Qt::NoFocus);
        }
        w->setFixedWidth(ICON_WIDTH);
        connect(w, SIGNAL(released()), _signalMapper, SLOT(map()));
        _signalMapper->setMapping(w, i);
        QHBoxLayout *layout = new QHBoxLayout(w);
        layout->setContentsMargins(0, 0, 0, 0);
        QLabel *l = new QLabel();
        l->setPixmap(_icons[SYSTEM_ICON_NONE]);
        layout->addWidget(l, 0, Qt::AlignCenter);
        mainLayout->addWidget(w);
        _iconMap.insert((SystemIconLabel)i, w);
    }
    mainLayout->addStretch();
    connect(_signalMapper, SIGNAL(mapped(int)), this, SIGNAL(iconClicked(int)));
    connect(this, SIGNAL(iconClicked(int)), this, SLOT(onIconClicked(int)));

    _timer.start(1000, this);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SystemStatusBarWidget::~SystemStatusBarWidget()
{
    _timer.stop();
}
