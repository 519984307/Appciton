/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QProcess>
#include "LanguageManager.h"
#include "FactoryTempMenuContent.h"
#include "Button.h"
#include "ComboBox.h"
#include "TEMPParam.h"
#include <QTimer>
#include "FactoryMaintainManager.h"
#include <QStackedWidget>
#include <QGroupBox>
#include "FontManager.h"

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

enum TempCalibrateValue
{
    TEMP_CALIBRATE_0,
    TEMP_CALIBRATE_5,
    TEMP_CALIBRATE_10,
    TEMP_CALIBRATE_15,
    TEMP_CALIBRATE_20,
    TEMP_CALIBRATE_25,
    TEMP_CALIBRATE_30,
    TEMP_CALIBRATE_35,
    TEMP_CALIBRATE_40,
    TEMP_CALIBRATE_45,
    TEMP_CALIBRATE_50,
    TEMP_CALIBRATE_NR,
};

enum TempCalibrateChannel
{
    TEMP_CALIBRATE_CHANNEL_1,
    TEMP_CALIBRATE_CHANNEL_2,
};

class FactoryTempMenuContentPrivate
{
public:
    static QString btnStr[TEMP_CALIBRATE_NR];
    static QString labelStr[TEMP_CALIBRATE_NR];

    FactoryTempMenuContentPrivate();

    QLabel *tempChannel;
    QLabel *tempValue;

    QLabel *tempError;
    QStackedWidget *stackedwidget;
    ComboBox *channel;

    Button *lbtn[TEMP_CALIBRATE_NR];
    QLabel *calibrateResult[TEMP_CALIBRATE_NR];

    int calibrationTimerId;
    int timeoutNum;

    int calibrateChannel;
    int calibrateValue;
};

FactoryTempMenuContentPrivate::FactoryTempMenuContentPrivate()
    : tempChannel(NULL),
      tempValue(NULL),
      tempError(NULL),
      stackedwidget(NULL),
      channel(NULL),
      calibrationTimerId(-1),
      timeoutNum(0),
      calibrateChannel(0),
      calibrateValue(0)
{
    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        lbtn[i] = NULL;
        calibrateResult[i] = NULL;
    }
}

QString FactoryTempMenuContentPrivate::btnStr[11] =
{
    "0",
    "5",
    "10",
    "15",
    "20",
    "25",
    "30",
    "35",
    "40",
    "45",
    "50"
};

QString FactoryTempMenuContentPrivate::labelStr[11] =
{
    "7409.3",
    "5742.9",
    "4491.1",
    "3541.3",
    "2813.9",
    "2252",
    "1814.5",
    "1471.1",
    "1199.8",
    "984",
    "811.3"
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryTempMenuContent::FactoryTempMenuContent()
    : MenuContent(trs("TEMPCalibrateMenu"),
                  trs("TEMPCalibrateMenuesc")),
      d_ptr(new FactoryTempMenuContentPrivate)
{
}

void FactoryTempMenuContent::layoutExec()
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(10);
    layout->setSpacing(10);
    this->setFocusPolicy(Qt::NoFocus);
    QHBoxLayout *hl;
    QLabel *label;
    Button *button;
    ComboBox *combo;

    hl = new QHBoxLayout;
    label = new QLabel;
    label->setFont(fontManager.textFont(20));
    hl->addWidget(label);
    d_ptr->tempChannel = label;

    label = new QLabel(trs("---"));
    label->setFont(fontManager.textFont(20));
    hl->addWidget(label);
    d_ptr->tempValue = label;

    QStackedWidget *stackWidget = new QStackedWidget;
    QGroupBox *groupBox = new QGroupBox;
    groupBox->setStyleSheet("border:none");
    groupBox->setLayout(hl);
    stackWidget->addWidget(groupBox);

    label = new QLabel();
    stackWidget->addWidget(label);
    d_ptr->stackedwidget = stackWidget;
    d_ptr->tempError = label;

    vLayout->addWidget(stackWidget, 0, Qt::AlignCenter);

    label = new QLabel(trs("TEMPChannel"));
    layout->addWidget(label, 1, 0);

    combo = new ComboBox;
    combo->addItem("Temp1");
    combo->addItem(trs("Temp2"));
    combo->setFixedWidth(150);
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onChannelReleased(int)));
    d_ptr->channel = combo;
    layout->addWidget(combo, 1, 1);

    for (int i = 0; i < TEMP_CALIBRATE_NR; i++)
    {
        label = new QLabel(QString("%1%2%3%4")
                           .arg(trs("TEMPCalibrate"))
                           .arg(d_ptr->btnStr[i])
                           .arg(trs("is"))
                           .arg(d_ptr->labelStr[i]));
        layout->addWidget(label, 2 + i, 0);
        button = new Button(QString("%1%2")
                                .arg(trs("TEMPCalibrate"))
                                .arg(d_ptr->btnStr[i]));
        button->setButtonStyle(Button::ButtonTextOnly);
        button->setFixedWidth(150);
        button->setProperty("Item", qVariantFromValue(i));
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        layout->addWidget(button, 2 + i, 1);
        d_ptr->lbtn[i] = button;

        label = new QLabel;
        d_ptr->calibrateResult[i] = label;
        layout->addWidget(label, 2 + i, 2, Qt::AlignCenter);
    }

    layout->setRowStretch(12, 1);
    vLayout->addLayout(layout);
}

void FactoryTempMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibrationTimerId == ev->timerId())
    {
        bool reply = tempParam.getCalibrationReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && tempParam.getCalibrationResult())
            {
                d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("CalibrationSuccess"));
            }
            else
            {
                d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("CalibrationFail"));
            }
            killTimer(d_ptr->calibrationTimerId);
            d_ptr->calibrationTimerId = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

void FactoryTempMenuContent::hideEvent(QHideEvent *e)
{
    QWidget::hideEvent(e);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactoryTempMenuContent::readyShow()
{
    d_ptr->channel->setCurrentIndex(0);
    d_ptr->calibrateChannel = 0;
    d_ptr->tempChannel->setText(trs("TEMP1"));

    for (int i = 0; i < TEMP_CALIBRATE_NR; ++i)
    {
        d_ptr->calibrateResult[i]->setText(trs("WaitingCalibration"));
    }

    if (tempParam.getErrorDisable())
    {
        showError(trs("TEMPModuleDisable"));
    }

    if (!tempParam.isServiceProviderOk())
    {
        showError(trs("TEMPModuleDisConnect"));
        return;
    }

    if (tempParam.getDisconnected())
    {
        showError(trs("TEMPCommunicationStop"));
    }
}

/**************************************************************************************************
 * 功能:错误信息显示
 *************************************************************************************************/
void FactoryTempMenuContent::showError(QString str)
{
    d_ptr->tempError->setText(str);
    d_ptr->stackedwidget->setCurrentIndex(1);
}

/**************************************************************************************************
 * 功能:更改体温通道
 *************************************************************************************************/
void FactoryTempMenuContent::onChannelReleased(int channel)
{
    d_ptr->calibrateChannel = channel;
    if (channel == TEMP_CALIBRATE_CHANNEL_1)
    {
        d_ptr->tempChannel->setText(trs("TEMP1"));
    }
    else
    {
        d_ptr->tempChannel->setText(trs("TEMP2"));
    }
    for (int i = 0; i < TEMP_CALIBRATE_NR; ++i)
    {
        d_ptr->calibrateResult[i]->setPixmap(QPixmap::fromImage(QImage()));
    }
}

/**************************************************************************************************
 * 功能:按钮释放
 *************************************************************************************************/
void FactoryTempMenuContent::onBtnReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    int value = button->property("Item").toInt();
    d_ptr->calibrateValue = value;
    d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("Calibrating"));
    if (!tempParam.isServiceProviderOk())
    {
        return;
    }
    tempParam.sendCalibrateData(d_ptr->calibrateChannel, d_ptr->calibrateValue);
    d_ptr->calibrationTimerId = startTimer(CALIBRATION_INTERVAL_TIME);
}

/**************************************************************************************************
 * 功能:超时
 *************************************************************************************************/
void FactoryTempMenuContent::timeOut()
{
    d_ptr->calibrateResult[d_ptr->calibrateValue]->setText(trs("CalibrationFail"));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryTempMenuContent::~FactoryTempMenuContent()
{
    delete d_ptr;
}

