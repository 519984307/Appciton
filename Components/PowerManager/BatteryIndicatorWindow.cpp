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
#include "WindowManager.h"
#include "FontManager.h"
#include "LanguageManager.h"
#include "BatteryIconWidget.h"
#include <QTimer>
#include "IConfig.h"

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
    int fontSize = fontManager.getFontSize(4);
    setWindowTitle(trs("BatteryInfo"));
    setFixedSize(400, 200);

    QVBoxLayout *layoutV = new QVBoxLayout();
    layoutV->setMargin(10);
    layoutV->setSpacing(10);

    // first row.
    QHBoxLayout *layoutH = new QHBoxLayout();
    QLabel *iconLabel = new QLabel();
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFont(fontManager.textFont(fontSize));

    layoutH->setMargin(10);
    layoutH->setSpacing(10);
    BatteryIconWidget *batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(BAT_VOLUME_5);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(BAT_VOLUME_1);
    batteryImag->setTimeValue(BAT_REMAIN_TIME_LOW);
    batteryImag->setFillColor(Qt::red);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_NOT_EXISTED);
    layoutH->addWidget(batteryImag);

    layoutV->addLayout(layoutH);

    d_ptr->timer = new QTimer();
    d_ptr->timer->setInterval(500);
    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));

    setWindowLayout(layoutV);
}

BatteryIndicatorWindow::~BatteryIndicatorWindow()
{
    delete d_ptr;
}
