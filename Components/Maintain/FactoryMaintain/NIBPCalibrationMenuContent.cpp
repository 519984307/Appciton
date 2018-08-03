/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/25
 **/
#include "NIBPCalibrationMenuContent.h"
#include "NIBPRepairMenuWindow.h"
#include "NIBPCalibrateContent.h"
#include "NIBPManometerContent.h"
#include "NIBPPressureControlContent.h"
#include "NIBPZeroPointContent.h"
#include "Button.h"
#include <QLabel>
#include <QHBoxLayout>
#include "LanguageManager.h"
#include "Debug.h"
#include "WindowManager.h"
#include "NIBPRepairMenuWindow.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPCalibrationMenuContent::NIBPCalibrationMenuContent()
    : MenuContent(trs("NIBPCalibrationMenu"),
                  trs("NIBPCalibrationMenuDesc"))

{
    NIBPRepairMenuWindow *w = NIBPRepairMenuWindow::getInstance();
    w->addMenuContent(NIBPCalibrateContent::getInstance());
    w->addMenuContent(NIBPManometerContent::getInstance());
    w->addMenuContent(NIBPZeroPointContent::getInstance());
    w->addMenuContent(NIBPPressureControlContent::getInstance());
}

void NIBPCalibrationMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label = new QLabel(trs("NIBPCalibration"));
    layout->addWidget(label);

    Button *button = new Button(trs("Enter"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnSlot()));
}

void NIBPCalibrationMenuContent::onBtnSlot()
{
    NIBPRepairMenuWindow *w = NIBPRepairMenuWindow::getInstance();
    w->init();
    windowManager.showWindow(w);
}
