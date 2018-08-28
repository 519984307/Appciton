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
        : timer(NULL),
          recordBatteryInfo(false)
    {}
    ~BatteryIndicatorWindowPrivate(){}

    QTimer *timer;
    bool recordBatteryInfo;
};

BatteryIndicatorWindow::BatteryIndicatorWindow()
    : Window(),
      d_ptr(new BatteryIndicatorWindowPrivate())
{
    int fontSize = fontManager.getFontSize(3);
    setWindowTitle(trs("BatteryInfo"));
    setFixedWidth(windowManager.getPopMenuWidth());

    QVBoxLayout *layoutV = new QVBoxLayout();
    layoutV->setMargin(5);
    layoutV->setSpacing(10);

    // first row.
    QLabel *iconLabel = new QLabel();
    iconLabel->setAlignment(Qt::AlignHCenter);
    iconLabel->setFont(fontManager.textFont(fontSize));
    iconLabel->setWordWrap(true);
    iconLabel->setText(trs("BatteryCapacityIcons"));

    layoutV->addWidget(iconLabel);

    // second row.
    QHBoxLayout *layoutH = new QHBoxLayout();
    layoutH->setMargin(0);
    layoutH->setSpacing(10);

    BatteryIconWidget *batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(5);
    batteryImag->setTimeValue(-2);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(4);
    batteryImag->setTimeValue(6);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(3);
    batteryImag->setTimeValue(4);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(2);
    batteryImag->setTimeValue(2);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(1);
    batteryImag->setTimeValue(1);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    batteryImag = new BatteryIconWidget();
    batteryImag->setVolume(1);
    batteryImag->setTimeValue(-1);
    batteryImag->setFillColor(Qt::yellow);
    batteryImag->setStatus(BATTERY_NORMAL);
    layoutH->addWidget(batteryImag, 1);

    layoutV->addLayout(layoutH);
    layoutV->addStretch(1);

    // third row.
    layoutH = new QHBoxLayout();
    layoutH->setMargin(0);
    layoutH->setSpacing(0);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize));
    iconLabel->setText(trs("BatteryDisconnected"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_NOT_EXISTED);

    QVBoxLayout *disconnectV = new QVBoxLayout();
    disconnectV->setSpacing(10);
    disconnectV->setMargin(0);
    disconnectV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    disconnectV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize));
    iconLabel->setText(trs("BatteryCommFault"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_COM_FAULT);

    QVBoxLayout *comfaultV = new QVBoxLayout();
    comfaultV->setSpacing(10);
    comfaultV->setMargin(0);
    comfaultV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    comfaultV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    layoutH->addStretch();
    layoutH->addLayout(disconnectV);
    layoutH->addStretch();
    layoutH->addLayout(comfaultV);
    layoutH->addStretch();

    layoutV->addLayout(layoutH);
    layoutV->addStretch(1);

    // fourth row.
    layoutH = new QHBoxLayout();
    layoutH->setMargin(0);
    layoutH->setSpacing(0);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize));
    iconLabel->setText(trs("BatteryError"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_ERROR);

    QVBoxLayout *errorV = new QVBoxLayout();
    errorV->setSpacing(10);
    errorV->setMargin(0);
    errorV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    errorV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    iconLabel = new QLabel();
    iconLabel->setFont(fontManager.textFont(fontSize));
    iconLabel->setText(trs("BatteryCalibrationRequired"));
    iconLabel->setFixedWidth(300);
    iconLabel->setAlignment(Qt::AlignHCenter);

    batteryImag = new BatteryIconWidget();
    batteryImag->setStatus(BATTERY_CALIBRATION_REQUIRED);

    QVBoxLayout *calibrateV = new QVBoxLayout();
    calibrateV->setSpacing(10);
    calibrateV->setMargin(0);
    calibrateV->addWidget(iconLabel, 1, Qt::AlignHCenter);
    calibrateV->addWidget(batteryImag, 1, Qt::AlignHCenter);

    layoutH->addStretch();
    layoutH->addLayout(errorV);
    layoutH->addStretch();
    layoutH->addLayout(calibrateV);
    layoutH->addStretch();

    layoutV->addLayout(layoutH);
    layoutV->addStretch(1);


    QVBoxLayout *layoutValue = new QVBoxLayout();
    layoutValue->setMargin(2);
    layoutValue->setSpacing(2);

    d_ptr->recordBatteryInfo = false;
    machineConfig.getNumValue("Record|Battery", d_ptr->recordBatteryInfo);
    if (d_ptr->recordBatteryInfo)
    {
        layoutV->addLayout(layoutValue);
    }

    layoutV->addStretch();

    d_ptr->timer = new QTimer();
    d_ptr->timer->setInterval(500);
    connect(d_ptr->timer, SIGNAL(timeout()), this, SLOT(_timeOutSlot()));

    setWindowLayout(layoutV);
}

BatteryIndicatorWindow::~BatteryIndicatorWindow()
{
    delete d_ptr;
}
