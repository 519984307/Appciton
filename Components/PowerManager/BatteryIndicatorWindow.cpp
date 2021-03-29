/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/23
 **/

#include "BatteryIndicatorWindow.h"
#include <QLabel>
#include "FontManager.h"
#include "Framework/Language/LanguageManager.h"
#include "BatteryIconWidget.h"
#include <QTimer>
#include "IConfig.h"
#include <QGridLayout>

class BatteryIndicatorWindowPrivate
{
public:
    BatteryIndicatorWindowPrivate()
        : timer(NULL)
    {}
    ~BatteryIndicatorWindowPrivate(){}

    QTimer *timer;
};

BatteryIndicatorWindow::BatteryIndicatorWindow()
    : Dialog(),
      d_ptr(new BatteryIndicatorWindowPrivate())
{
    setWindowTitle(trs("BatteryInfo"));
    setFixedSize(300, 300);

    QGridLayout *layout = new QGridLayout;

    // normal battery
    QLabel *label = new QLabel();
    label->setText(trs("BatteryFullCapacity"));
    layout->addWidget(label, 0, 0);
    BatteryIconWidget *batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(BAT_VOLUME_5);
    batteryImag->setStatus(BATTERY_NORMAL);
    layout->addWidget(batteryImag, 0, 1);

    // low battery
    label = new QLabel();
    label->setText(trs("BatteryLowCapacity"));
    layout->addWidget(label, 1, 0);
    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(BAT_VOLUME_1);
    batteryImag->setTimeValue(BAT_REMAIN_TIME_LOW);
    batteryImag->setFillColor(Qt::red);
    batteryImag->setStatus(BATTERY_NORMAL);
    layout->addWidget(batteryImag, 1, 1);

    // Error battery
    label = new QLabel();
    label->setText(trs("BatteryNoPresent"));
    layout->addWidget(label, 2, 0);
    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_NOT_EXISTED);
    layout->addWidget(batteryImag, 2, 1);

//    d_ptr->timer = new QTimer();
//    d_ptr->timer->setInterval(500);
//    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));

    setWindowLayout(layout);
}

BatteryIndicatorWindow::~BatteryIndicatorWindow()
{
    delete d_ptr;
}
