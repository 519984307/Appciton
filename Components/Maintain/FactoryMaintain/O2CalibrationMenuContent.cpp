/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/11/14
 **/

#include "O2CalibrationMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include "Button.h"
#include <QLabel>
#include <QGridLayout>
#include "O2Param.h"
#include <QTimerEvent>

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)

enum O2ConcentrationCalibrte
{
    O2_PERCENT_21,
    O2_PERCENT_100
};

class O2CalibrationMenuContentPrivate
{
public:
    O2CalibrationMenuContentPrivate()
        : calibTimerID(-1), timeoutNum(0),
          calibratePercent(O2_PERCENT_21)
    {}

    QList<Button *> percentBtns;
    QList<QLabel *> percentLab;

    int calibTimerID;
    int timeoutNum;
    O2ConcentrationCalibrte calibratePercent;
};

O2CalibrationMenuContent::O2CalibrationMenuContent()
    : MenuContent(trs("O2Calibrate"),
                  trs("O2CalibrateDesc")),
      d_ptr(new O2CalibrationMenuContentPrivate())
{
}

O2CalibrationMenuContent::~O2CalibrationMenuContent()
{
    delete d_ptr;
}

void O2CalibrationMenuContent::readyShow()
{
    d_ptr->percentLab.at(O2_PERCENT_21)->setText(trs("WaitingCalibration"));
    d_ptr->percentLab.at(O2_PERCENT_100)->setText(trs("WaitingCalibration"));
}

void O2CalibrationMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    Button *button;
    QLabel *label;

    button = new Button(trs("Percent21Calibration"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(percent21Released()));
    layout->addWidget(button, 0, 0);
    d_ptr->percentBtns.append(button);

    label = new QLabel();
    layout->addWidget(label, 0, 1, Qt::AlignCenter);
    d_ptr->percentLab.append(label);

    button = new Button(trs("Percent100Calibration"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(percent100Released()));
    layout->addWidget(button, 1, 0);
    d_ptr->percentBtns.append(button);

    label = new QLabel();
    layout->addWidget(label, 1, 1, Qt::AlignCenter);
    d_ptr->percentLab.append(label);

    layout->setRowStretch(2, 1);
}

void O2CalibrationMenuContent::timerEvent(QTimerEvent *ev)
{
    if (d_ptr->calibTimerID == ev->timerId())
    {
        bool reply = o2Param.getCalibrationReply();
        if (reply || d_ptr->timeoutNum == TIMEOUT_WAIT_NUMBER)
        {
            if (reply && o2Param.getCalibrationResult())
            {
                d_ptr->percentLab.at(d_ptr->calibratePercent)->setText(trs("CalibrationSuccess"));
            }
            else
            {
                d_ptr->percentLab.at(d_ptr->calibratePercent)->setText(trs("CalibrationFail"));
            }
            killTimer(d_ptr->calibTimerID);
            d_ptr->calibTimerID = -1;
            d_ptr->timeoutNum = 0;
        }
        else
        {
            d_ptr->timeoutNum++;
        }
    }
}

void O2CalibrationMenuContent::percent21Released(void)
{
    if (o2Param.isServiceProviderOk())
    {
        o2Param.sendCalibration(1);
        d_ptr->calibTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->calibratePercent = O2_PERCENT_21;
    }
}

void O2CalibrationMenuContent::percent100Released(void)
{
    if (o2Param.isServiceProviderOk())
    {
        o2Param.sendCalibration(2);
        d_ptr->calibTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
        d_ptr->calibratePercent = O2_PERCENT_100;
    }
}
