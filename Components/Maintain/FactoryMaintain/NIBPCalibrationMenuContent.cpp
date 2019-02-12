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
#include "NIBPParam.h"

#define TIME_INTERVAL       100

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPCalibrationMenuContent::NIBPCalibrationMenuContent()
    : MenuContent(trs("NIBPCalibrationMenu"),
                  trs("NIBPCalibrationMenuDesc")),
      _enterBtn(NULL), _timerId(-1)

{
}

void NIBPCalibrationMenuContent::readyShow()
{
    _timerId = startTimer(TIME_INTERVAL);
}

void NIBPCalibrationMenuContent::layoutExec()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label = new QLabel(trs("NIBPCalibration"));
    layout->addWidget(label);

    _enterBtn = new Button(trs("Enter"));
    _enterBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(_enterBtn);
    connect(_enterBtn, SIGNAL(released()), this, SLOT(onBtnSlot()));
}

void NIBPCalibrationMenuContent::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e)
    killTimer(_timerId);
    _timerId = -1;
}

void NIBPCalibrationMenuContent::timerEvent(QTimerEvent *e)
{
    // 100ms访问一次NIBP测量状态并使能按钮
    if (e->timerId() == _timerId)
    {
        if (nibpParam.isMeasuring())
        {
            _enterBtn->setEnabled(false);
        }
        else
        {
            _enterBtn->setEnabled(true);
        }
    }
}

void NIBPCalibrationMenuContent::onBtnSlot()
{
//    NIBPRepairMenuWindow *w = NIBPRepairMenuWindow::getInstance();
//    w->init();
//    windowManager.showWindow(w, WindowManager::ShowBehaviorModal);
    windowManager.showWindow(NIBPRepairMenuWindow::getInstance(),
                             WindowManager::ShowBehaviorHideOthers
                             | WindowManager::ShowBehaviorNoAutoClose);
    NIBPRepairMenuWindow::getInstance()->focusFirstMenuItem();
    NIBPRepairMenuWindow::getInstance()->init();
}
