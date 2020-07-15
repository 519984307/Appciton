/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/5/13
 **/

#include "IBPCalibrationMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/SpinBox.h"
#include "IBPParam.h"
#include <QGridLayout>
#include <QTimerEvent>
#include <QLabel>

#define QUERY_CALIBRATE_INTERVAL    100   // ms
#define MAX_CALIBRATION_DURATION    5000    // ms

class IBPCalibrationMenuContentPrivate
{
public:
    IBPCalibrationMenuContentPrivate()
        : chnCbo(NULL),
          pointSpb(NULL),
          infoLbl(NULL),
          calBtn(NULL),
          calChn(IBP_CHN_1),
          timeElapsed(0),
          timerID(-1)
    {}
    ComboBox *chnCbo;   /* channel combox */
    SpinBox *pointSpb;  /* calibrate point spinbox */
    QLabel *infoLbl;    /* calibrate info label */
    Button *calBtn;     /* calirabte button */
    IBPChannel calChn;  /* calibrate channel */
    int timeElapsed;    /* time elapsed since calibrate start */
    int timerID;
};

IBPCalibrationMenuContent::IBPCalibrationMenuContent()
    :MenuContent(trs("IBPCalibrate"), trs("IBPCalibrateDesc")),
      pimpl(new IBPCalibrationMenuContentPrivate())
{
}

IBPCalibrationMenuContent::~IBPCalibrationMenuContent()
{
}

void IBPCalibrationMenuContent::readyShow()
{
    if (ibpParam.getMoudleType() == IBP_MODULE_SMART_IBP)
    {
        /* fix calibrate point */
        pimpl->pointSpb->setEnabled(false);
    }
    if (pimpl->calBtn)
    {
        pimpl->calBtn->setEnabled(ibpParam.isConnected());
    }
}

void IBPCalibrationMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    Button *button;
    ComboBox *combo;
    SpinBox *spinbox;

    label = new QLabel(trs("CalibrateChannel"));
    layout->addWidget(label, 0, 0);

    combo = new ComboBox();
    QStringList items;
    for (int i = 0; i < IBP_CHN_NR; i++)
    {
        items.append(QString("IBP %1").arg(i + 1));
    }
    combo->addItems(items);
    layout->addWidget(combo, 0, 1);
    pimpl->chnCbo = combo;

    label = new QLabel(trs("CalibratePoint"));
    layout->addWidget(label, 1, 0);

    spinbox = new SpinBox();
    spinbox->setRange(100, 240);
    spinbox->setValue(200);
    spinbox->setStep(1);
    layout->addWidget(spinbox, 1, 1);
    pimpl->pointSpb = spinbox;

    button = new Button(trs("Start"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, 2, 1);
    connect(button, SIGNAL(released()), this, SLOT(startCalibrate()));
    pimpl->calBtn = button;

    QWidget *placeHolder = new QWidget();
    placeHolder->setFixedHeight(40);
    layout->addWidget(placeHolder, 3, 0, 1, 2);

    label = new QLabel();
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label, 4, 0, 1, 2);
    pimpl->infoLbl = label;
}

void IBPCalibrationMenuContent::timerEvent(QTimerEvent *ev)
{
    MenuContent::timerEvent(ev);
    if (ev->timerId() == pimpl->timerID)
    {
        pimpl->timeElapsed += QUERY_CALIBRATE_INTERVAL;
        if (pimpl->timeElapsed >= MAX_CALIBRATION_DURATION)
        {
            pimpl->timeElapsed = 0;
            killTimer(pimpl->timerID);
            pimpl->timerID = -1;
            pimpl->infoLbl->setText(QString("IBP %1 %2").arg(pimpl->calChn + 1).arg(trs("CalibrateFailed")));
            pimpl->calBtn->setEnabled(true);
        }
        else
        {
            if (ibpParam.hasIBPCalibReply(pimpl->calChn))
            {
                bool result = ibpParam.getLaseCalibResult(pimpl->calChn);
                pimpl->infoLbl->setText(QString("IBP %1 %2").arg(pimpl->calChn + 1)
                                        .arg(result ? trs("CalibrateSuccessfully")
                                                    : trs("CalibrateFailed")));
                pimpl->timeElapsed = 0;
                killTimer(pimpl->timerID);
                pimpl->timerID = -1;
                pimpl->calBtn->setEnabled(true);
            }
        }
    }
}

void IBPCalibrationMenuContent::startCalibrate()
{
    pimpl->calChn = static_cast<IBPChannel>(pimpl->chnCbo->currentIndex());

    // IBP channel need zero, prompt needs to be zeroed first.
    if (ibpParam.channelNeedZero(pimpl->calChn))
    {
        pimpl->infoLbl->setText(trs("IBPZeroBeforeCalib"));
        return;
    }

    pimpl->infoLbl->setText(trs("IBPCalibrating"));

    ibpParam.setCalibration(pimpl->calChn, pimpl->pointSpb->getValue());

    pimpl->calBtn->setEnabled(false);
    pimpl->timerID = startTimer(QUERY_CALIBRATE_INTERVAL);
}

