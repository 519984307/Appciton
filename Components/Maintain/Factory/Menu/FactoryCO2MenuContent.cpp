/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by TongZhou Fang fangtongzhou@blmed.cn, 2019/8/2
 **/
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTime>
#include "FactoryCO2MenuContent.h"
#include "FactoryMaintainManager.h"
#include "LanguageManager.h"
#include "Button.h"
#include "CO2Param.h"
#include "FontManager.h"

#define CALIBRATE_INTERVAL_TIME                     (100)
#define TIMEOUT_WAIT_NUMBER                         (5000 / CALIBRATE_INTERVAL_TIME)
enum CO2CalibrateValue
{
    CO2_CALIBRATE_0,
    CO2_CALIBRATE_2_5,
    CO2_CALIBRATE_5,
    CO2_CALIBRATE_13,
    CO2_CALIBRATE_20,
    CO2_CALIBRATE_NR
};

enum CO2CalibrateState
{
    CO2_CALIBRATE_STATE_WAITING,
    CO2_CALIBRATE_STATE_CALIBRATING,
    CO2_CALIBRATE_STATE_SUCCESS,
    CO2_CALIBRATE_STATE_FAIL
};

class FactoryCO2MenuContentPrivate
{
public:
    static QString labelStr[CO2_CALIBRATE_NR];

    FactoryCO2MenuContentPrivate();

    void updateResultLabel(int column);

    QLabel* titleLabel;
    int calibrateValue;
    int calibrateTimerId;
    int timeoutNum;
    Button *lbtn[CO2_CALIBRATE_NR];
    QLabel *label[CO2_CALIBRATE_NR];
    QLabel *calibrateResultLbl[CO2_CALIBRATE_NR];

    CO2CalibrateState calibrateResult[CO2_CALIBRATE_NR];
};

FactoryCO2MenuContentPrivate::FactoryCO2MenuContentPrivate()
    : titleLabel(NULL), calibrateValue(0),
      calibrateTimerId(-1), timeoutNum(0)
{
    for (int i = 0 ; i < CO2_CALIBRATE_NR; i++)
    {
        label[i] = NULL;
        lbtn[i] = NULL;
        calibrateResultLbl[i] = NULL;
        calibrateResult[i] = CO2_CALIBRATE_STATE_WAITING;
    }
}

void FactoryCO2MenuContentPrivate::updateResultLabel(int column)
{
    CO2CalibrateState state = calibrateResult[column];
    QString text;
    switch (state)
    {
    case CO2_CALIBRATE_STATE_WAITING:
        text = trs("WaitingCalibration");
        break;
    case CO2_CALIBRATE_STATE_CALIBRATING:
        text = trs("Calibrating");
        break;
    case CO2_CALIBRATE_STATE_SUCCESS:
        text = trs("CalibrationSuccess");
        break;
    case CO2_CALIBRATE_STATE_FAIL:
        text = trs("CalibrationFail");
        break;
    default:
        break;
    }
    calibrateResultLbl[column]->setText(text);
}
QString FactoryCO2MenuContentPrivate::labelStr[CO2_CALIBRATE_NR] =
{
    "0.0",
    "2.5",
    "5.0",
    "13.0",
    "20.0"
};

FactoryCO2MenuContent::FactoryCO2MenuContent()
    : MenuContent(trs("CO2CalibrateMenu"),
                  trs("CO2CalibrateDesc")),
      d_ptr(new FactoryCO2MenuContentPrivate)
{
    connect(&co2Param, SIGNAL(connectStatusUpdated(bool)), this, SLOT(updateCO2Content(bool)));
}

FactoryCO2MenuContent::~FactoryCO2MenuContent()
{
    delete d_ptr;
}

void FactoryCO2MenuContent::showError(QString str)
{
    d_ptr->titleLabel->setText(str);
}

void FactoryCO2MenuContent::readyShow()
{
    if (!co2Param.isConnected())
    {
        showError(trs("CO2CommunicationStop"));
    }
    else
    {
        d_ptr->titleLabel->setText(trs("CO2Calibrate"));
    }
    for (int i = 0; i < CO2_CALIBRATE_NR; i++)
    {
        d_ptr->calibrateResult[i] = CO2_CALIBRATE_STATE_WAITING;
        d_ptr->updateResultLabel(i);
    }
}

void FactoryCO2MenuContent::layoutExec()
{
    QVBoxLayout* vLayout = new QVBoxLayout(this);
    QGridLayout* gLayout = new QGridLayout();
    gLayout->setMargin(2);

    QLabel *label;
    Button *button;

    vLayout->addStretch();
    label = new QLabel();
    label->setFont(fontManager.textFont(20));
    label->setText(trs("CO2Calibrte"));
    vLayout->addWidget(label, 0, Qt::AlignCenter);
    d_ptr->titleLabel = label;

    vLayout->addStretch();
    for (int i = 0; i < CO2_CALIBRATE_NR; i++)
    {
        label = new QLabel(QString("%1: %2%3")
                           .arg(trs("CO2CalibratePotency"))
                           .arg(d_ptr->labelStr[i])
                           .arg(trs("percent")));
        gLayout->addWidget(label, i + 1, 0);
        d_ptr->label[i] = label;
        button = new Button(trs("ServiceCalibrate"));
        button->setButtonStyle(Button::ButtonTextOnly);
        button->setProperty("Item", qVariantFromValue(i));
        button->setFixedWidth(150);
        connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
        gLayout->addWidget(button, i + 1, 1);
        d_ptr->lbtn[i] = button;

        label = new QLabel();
        gLayout->addWidget(label, i +1, 2, Qt::AlignHCenter);
        d_ptr->calibrateResultLbl[i] = label;
    }
    gLayout->setRowStretch(CO2_CALIBRATE_NR, 1);
    vLayout->addLayout(gLayout);
    vLayout->addStretch();
}

void FactoryCO2MenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibrateTimerId == ev->timerId())
    {
        bool reply = co2Param.getCalibrateReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && co2Param.getCalibrateResult())
            {
                d_ptr->calibrateResult[d_ptr->calibrateValue] = CO2_CALIBRATE_STATE_SUCCESS;
            }
            else
            {
                d_ptr->calibrateResult[d_ptr->calibrateValue] = CO2_CALIBRATE_STATE_FAIL;
            }
            killTimer(d_ptr->calibrateTimerId);
            d_ptr->calibrateTimerId = -1;
            d_ptr->timeoutNum = 0;
            d_ptr->updateResultLabel(d_ptr->calibrateValue);
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

void FactoryCO2MenuContent::onBtnReleased()
{
    for (int i = 0; i < CO2_CALIBRATE_NR; i++)
    {
        if (d_ptr->calibrateResult[i] == CO2_CALIBRATE_STATE_CALIBRATING)
        {
            d_ptr->calibrateResult[i] = CO2_CALIBRATE_STATE_WAITING;
            d_ptr->updateResultLabel(i);
        }
    }
    Button *button = qobject_cast<Button *>(sender());
    int column = button->property("Item").toInt();
    d_ptr->calibrateValue = column;
    d_ptr->calibrateResult[d_ptr->calibrateValue] = CO2_CALIBRATE_STATE_CALIBRATING;
    d_ptr->updateResultLabel(d_ptr->calibrateValue);
    co2Param.sendCalibrateData(d_ptr->calibrateValue);
    d_ptr->calibrateTimerId = startTimer(CALIBRATE_INTERVAL_TIME);
    d_ptr->timeoutNum = 0;
}

void FactoryCO2MenuContent::updateCO2Content(bool status)
{
    if (status)
    {
        d_ptr->titleLabel->setText(trs("CO2Calibrate"));
    }
    else
    {
        showError(trs("CO2CommunicationStop"));
    }
}

