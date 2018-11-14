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
#include "LanguageManager.h"
#include "Button.h"
#include <QLabel>
#include <QGridLayout>
#include "O2Param.h"
#include <QTimerEvent>

#define CALIBRATION_INTERVAL_TIME              (100)
#define TIMEOUT_WAIT_NUMBER                    (5000 / CALIBRATION_INTERVAL_TIME)


class O2CalibrationMenuContentPrivate
{
public:
    O2CalibrationMenuContentPrivate()
        :percent21Btn(NULL), percent100Btn(NULL),
          percent21Lbe(NULL), percent100Lbe(NULL),
          calibTimerID(-1), timeoutNum(0)
    {}

    Button *percent21Btn;
    Button *percent100Btn;
    QLabel *percent21Lbe;
    QLabel *percent100Lbe;

    int calibTimerID;
    int timeoutNum;
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
    d_ptr->percent21Lbe->setText(trs("WaitingCalibration"));
    d_ptr->percent100Lbe->setText(trs("WaitingCalibration"));
}

void O2CalibrationMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    Button *button;
    QLabel *label;

    button = new Button(trs("Percent21Calibration"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(percent21Released(int)));
    layout->addWidget(button, 0, 0);
    d_ptr->percent21Btn = button;

    label = new QLabel();
    layout->addWidget(label, 0, 1, Qt::AlignCenter);
    d_ptr->percent21Lbe = label;

    button = new Button(trs("Percent100Calibration"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(percent100Released(int)));
    layout->addWidget(button, 1, 0);
    d_ptr->percent21Btn = button;

    label = new QLabel();
    layout->addWidget(label, 1, 1, Qt::AlignCenter);
    d_ptr->percent100Lbe = label;

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
                d_ptr->percent21Lbe->setText(trs("CalibrationSuccess"));
            }
        }
    }
}

void O2CalibrationMenuContent::percent21Released(void)
{
    if (o2Param.isServiceProviderOk())
    {
        o2Param.sendCalibration(0);
        d_ptr->calibTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    }
}

void O2CalibrationMenuContent::percent100Released(void)
{
    if (o2Param.isServiceProviderOk())
    {
        o2Param.sendCalibration(1);
        d_ptr->calibTimerID = startTimer(CALIBRATION_INTERVAL_TIME);
    }
}
