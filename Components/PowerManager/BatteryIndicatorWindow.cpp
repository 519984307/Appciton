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
        : temperatureLabel(NULL), voltageLabel(NULL),
          voltageADCLabel(NULL), currentLabel(NULL),
          averageCurrentLabel(NULL), relativeStateOfChargeLabel(NULL),
          absoluteStateOfChargeLabel(NULL), remainingCapacityLabel(NULL),
          runTimeToEmptyLabel(NULL), averageTimeToEmptyLabel(NULL),
          averageTimeToFullLabel(NULL), statusLabel(NULL),
          modeLabel(NULL),
          recordBatteryInfo(false)
    {}
    ~BatteryIndicatorWindowPrivate(){}

    QLabel *temperatureLabel;               // 获取电池温度
    QLabel *voltageLabel;                   // 获取电池电压
    QLabel *voltageADCLabel;                // 获取电池ADC采集电压
    QLabel *currentLabel;                   // 获取电池电流
    QLabel *averageCurrentLabel;            // 获取每分钟平均电流
    QLabel *relativeStateOfChargeLabel;     // 获取相对电荷状态，即相对电池剩余百分比
    QLabel *absoluteStateOfChargeLabel;     // 获取绝对电荷状态，即绝对电池剩余百分比
    QLabel *remainingCapacityLabel;         // 获取剩余电池容量
    QLabel *runTimeToEmptyLabel;            // 获取电池剩余运行时间
    QLabel *averageTimeToEmptyLabel;        // 获取电池平均剩余运行时间
    QLabel *averageTimeToFullLabel;         // 获取电池平均充满时间
    QLabel *statusLabel;                    // 获取电池状态信息
    QLabel *modeLabel;                      // 获取电池模式

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

    d_ptr->temperatureLabel = new QLabel();
    d_ptr->temperatureLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->temperatureLabel->setAlignment(Qt::AlignLeft);
    d_ptr->temperatureLabel->setText("temperature:");

    d_ptr->currentLabel = new QLabel();
    d_ptr->currentLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->currentLabel->setAlignment(Qt::AlignLeft);
    d_ptr->currentLabel->setText("current:");

    QHBoxLayout *layout0 = new QHBoxLayout();
    layout0->setMargin(0);
    layout0->setSpacing(10);
    layout0->addWidget(d_ptr->temperatureLabel);
    layout0->addWidget(d_ptr->currentLabel);

    d_ptr->voltageLabel = new QLabel();
    d_ptr->voltageLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->voltageLabel->setAlignment(Qt::AlignLeft);
    d_ptr->voltageLabel->setText("voltage:");

    d_ptr->voltageADCLabel = new QLabel();
    d_ptr->voltageADCLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->voltageADCLabel->setAlignment(Qt::AlignLeft);
    d_ptr->voltageADCLabel->setText("voltage ADC:");

    QHBoxLayout *layout1 = new QHBoxLayout();
    layout1->setMargin(0);
    layout1->setSpacing(10);
    layout1->addWidget(d_ptr->voltageLabel);
    layout1->addWidget(d_ptr->voltageADCLabel);

    d_ptr->averageCurrentLabel = new QLabel();
    d_ptr->averageCurrentLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->averageCurrentLabel->setAlignment(Qt::AlignLeft);
    d_ptr->averageCurrentLabel->setText("averageCurrent:");

    d_ptr->relativeStateOfChargeLabel = new QLabel();
    d_ptr->relativeStateOfChargeLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->relativeStateOfChargeLabel->setAlignment(Qt::AlignLeft);
    d_ptr->relativeStateOfChargeLabel->setText("relativeStateOfCharge:");

    QHBoxLayout *layout2 = new QHBoxLayout();
    layout2->setMargin(0);
    layout2->setSpacing(10);
    layout2->addWidget(d_ptr->averageCurrentLabel);
    layout2->addWidget(d_ptr->relativeStateOfChargeLabel);

    d_ptr->absoluteStateOfChargeLabel = new QLabel();
    d_ptr->absoluteStateOfChargeLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->absoluteStateOfChargeLabel->setAlignment(Qt::AlignLeft);
    d_ptr->absoluteStateOfChargeLabel->setText("absoluteStateOfCharge:");

    d_ptr->remainingCapacityLabel = new QLabel();
    d_ptr->remainingCapacityLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->remainingCapacityLabel->setAlignment(Qt::AlignLeft);
    d_ptr->remainingCapacityLabel->setText("remainingCapacity:");

    d_ptr->runTimeToEmptyLabel = new QLabel();
    d_ptr->runTimeToEmptyLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->runTimeToEmptyLabel->setAlignment(Qt::AlignLeft);
    d_ptr->runTimeToEmptyLabel->setText("runTimeToEmpty:");

    QHBoxLayout *layout3 = new QHBoxLayout();
    layout3->setMargin(0);
    layout3->setSpacing(10);
    layout3->addWidget(d_ptr->remainingCapacityLabel);
    layout3->addWidget(d_ptr->runTimeToEmptyLabel);

    d_ptr->averageTimeToEmptyLabel = new QLabel();
    d_ptr->averageTimeToEmptyLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->averageTimeToEmptyLabel->setAlignment(Qt::AlignLeft);
    d_ptr->averageTimeToEmptyLabel->setText("averageTimeToEmpty:");

    d_ptr->averageTimeToFullLabel = new QLabel();
    d_ptr->averageTimeToFullLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->averageTimeToFullLabel->setAlignment(Qt::AlignLeft);
    d_ptr->averageTimeToFullLabel->setText("averageTimeToFull:");


    QHBoxLayout *layout4 = new QHBoxLayout();
    layout4->setMargin(0);
    layout4->setSpacing(10);
    layout4->addWidget(d_ptr->averageTimeToEmptyLabel);
    layout4->addWidget(d_ptr->averageTimeToFullLabel);

    d_ptr->statusLabel = new QLabel();
    d_ptr->statusLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->statusLabel->setAlignment(Qt::AlignLeft);
    d_ptr->statusLabel->setText("status:");

    d_ptr->modeLabel = new QLabel();
    d_ptr->modeLabel->setFont(fontManager.textFont(fontSize));
    d_ptr->modeLabel->setAlignment(Qt::AlignLeft);
    d_ptr->modeLabel->setText("mode:");

    QHBoxLayout *layout5 = new QHBoxLayout();
    layout5->setMargin(0);
    layout5->setSpacing(10);
    layout5->addWidget(d_ptr->statusLabel);
    layout5->addWidget(d_ptr->modeLabel);

    QVBoxLayout *layoutValue = new QVBoxLayout();
    layoutValue->setMargin(2);
    layoutValue->setSpacing(2);

    layoutValue->addLayout(layout0);
    layoutValue->addLayout(layout1);
    layoutValue->addLayout(layout2);
    layoutValue->addLayout(layout3);
    layoutValue->addLayout(layout4);
    layoutValue->addLayout(layout5);

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
