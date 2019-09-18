/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/

#include "MonitorInfoWindow.h"
#include <QMap>
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include "IConfig.h"
#include "KeyInputPanel.h"
#include "Button.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QVBoxLayout>
#include "TimeManager.h"
#include <QTimer>
#include "PowerManager.h"
#include "UnitManager.h"
#include <QNetworkInterface>

class MonitorInfoWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_LAB_CMLV_WORKTIME = 0,
        ITEM_LAB_BAT_CAPACITY,
        ITEM_LAB_MACHINE_TYPE,
        ITEM_LAB_MAC_ADDR,
        ITEM_LAB_SCR_RESOLASIZE,
        ITEM_BTN_ELEC_SERIALNUM,
    };
    MonitorInfoWindowPrivate()
            : timer(NULL)
    {
    }
    /**
     * @brief loadOptions  加载显示参数
     */
    void loadOptions();
    QMap <MenuItem, QLabel *> labs;
    QTimer *timer;
};

MonitorInfoWindow::~MonitorInfoWindow()
{
    delete d_ptr;
}

MonitorInfoWindow::MonitorInfoWindow()
    : Dialog(),
      d_ptr(new MonitorInfoWindowPrivate)
{
    layoutExec();
    readyShow();
}

void MonitorInfoWindowPrivate::loadOptions()
{
    QDesktopWidget *w = QApplication::desktop();
    labs[ITEM_LAB_SCR_RESOLASIZE]->setText(QString("%1*%2").arg(w->width()).arg(w->height()));

    QString tmpStr;
    labs[ITEM_LAB_BAT_CAPACITY]->setText(powerManger.getBatteryCapacity());

    tmpStr.clear();
    systemConfig.getStrValue("MonitorInfo|MachineType", tmpStr);
    labs[ITEM_LAB_MACHINE_TYPE]->setText(tmpStr);

    tmpStr.clear();
    machineConfig.getStrValue("SerialNumber", tmpStr);
    labs[ITEM_BTN_ELEC_SERIALNUM]->setText(tmpStr);

    tmpStr.clear();
    foreach(QNetworkInterface workInterface, QNetworkInterface::allInterfaces())
    {
        if (workInterface.name().contains("eth"))  // filter the localhost(the ip address is 127.0.0.1) by name
        {
            tmpStr = workInterface.hardwareAddress();
            break;
        }
    }
    if (tmpStr.isEmpty())
    {
        tmpStr = "-";  // indicate that the MAC address is invalid.
    }
    labs[ITEM_LAB_MAC_ADDR]->setText(tmpStr);
}

void MonitorInfoWindow::readyShow()
{
    d_ptr->loadOptions();
    onTimeOutExec();
}

void MonitorInfoWindow::onTimeOutExec()
{
    QString showtime = getRunTime();
    d_ptr->labs[MonitorInfoWindowPrivate::ITEM_LAB_CMLV_WORKTIME]->setText(showtime);
    d_ptr->labs[MonitorInfoWindowPrivate::ITEM_LAB_BAT_CAPACITY]->setText(powerManger.getBatteryCapacity());
}

void MonitorInfoWindow::layoutExec()
{
    setWindowTitle(trs("MonitorInfoMenu"));

    QGridLayout *layout = new QGridLayout;
    layout->setVerticalSpacing(20);
    setFixedSize(480, 420);

    QLabel *labelLeft;
    QLabel *labelRight;

    labelLeft = new QLabel(trs("RunningTime"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_CMLV_WORKTIME, labelRight);

    labelLeft = new QLabel(trs("BatteryCapacity"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_BAT_CAPACITY, labelRight);

    labelLeft = new QLabel(trs("ProductModel"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_MACHINE_TYPE, labelRight);

    labelLeft = new QLabel(trs("MACAddress"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_MAC_ADDR, labelRight);

    labelLeft = new QLabel(trs("ScreenResolution"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_LAB_SCR_RESOLASIZE, labelRight);

    labelLeft = new QLabel(trs("ElectronicSerialNumber"));
    layout->addWidget(labelLeft, d_ptr->labs.count(), 0);
    labelRight = new QLabel("");
    labelRight->setAlignment(Qt::AlignCenter|Qt::AlignRight);
    layout->addWidget(labelRight, d_ptr->labs.count(), 1);
    d_ptr->labs.insert(MonitorInfoWindowPrivate
                       ::ITEM_BTN_ELEC_SERIALNUM, labelRight);

    layout->setRowStretch((layout->rowCount() + 1), 1);
    setWindowLayout(layout);
}

QString MonitorInfoWindow::getRunTime()
{
    QString t;
    unsigned diffTime = timeManager.getRunTime();
    int hour = diffTime / 3600;
    int min = (diffTime - hour * 3600) / 60;
    int sec = diffTime % 60;
    t = QString("%1:%2:%3").
            arg(QString::number(hour, 'g', 2)).
            arg(QString::number(min, 'g', 2)).
            arg(QString::number(sec, 'g', 2));
    return t;
}

void MonitorInfoWindow::showEvent(QShowEvent *e)
{
    Dialog::showEvent(e);
    if (d_ptr->timer)
    {
        delete d_ptr->timer;
        d_ptr->timer = NULL;
    }
    d_ptr->timer = new QTimer;
    d_ptr->timer->setSingleShot(false);
    connect(d_ptr->timer, SIGNAL(timeout()), SLOT(onTimeOutExec()));
    d_ptr->timer->start(1000);
}

void MonitorInfoWindow::hideEvent(QHideEvent *e)
{
    Dialog::hideEvent(e);
    if (d_ptr->timer)
    {
        delete d_ptr->timer;
        d_ptr->timer = NULL;
    }
}
