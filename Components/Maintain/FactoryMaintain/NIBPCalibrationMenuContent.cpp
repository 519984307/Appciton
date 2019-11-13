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
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QHBoxLayout>
#include "Debug.h"
#include "WindowManager.h"
#include "NIBPRepairMenuWindow.h"
#include "NIBPParam.h"
#include "SystemManager.h"
#include <QTimerEvent>

#define TIME_INTERVAL       100

class NIBPCalibrationMenuContentPrivate
{
public:
    NIBPCalibrationMenuContentPrivate() : enterBtn(NULL), timerId(-1)
    {}
    Button *enterBtn;
    int timerId;
};

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPCalibrationMenuContent::NIBPCalibrationMenuContent()
    : MenuContent(trs("NIBPMaintainMenu"),
                  trs("NIBPMaintainMenuDesc")),
      d_ptr(new NIBPCalibrationMenuContentPrivate)
{
}

NIBPCalibrationMenuContent::~NIBPCalibrationMenuContent()
{
    delete d_ptr;
}

void NIBPCalibrationMenuContent::showEvent(QShowEvent* e)
{
    Q_UNUSED(e);
    if (nibpParam.isConnectedModule() && !nibpParam.isErrorDisable())
    {
        d_ptr->timerId = startTimer(TIME_INTERVAL);
        d_ptr->enterBtn->setEnabled(true);
    }
    else
    {
        d_ptr->enterBtn->setEnabled(false);
    }
}

void NIBPCalibrationMenuContent::layoutExec()
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label = new QLabel(trs("NIBPMaintain"));
    layout->addWidget(label);

    d_ptr->enterBtn = new Button(trs("Enter"));
    d_ptr->enterBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->enterBtn);
    connect(d_ptr->enterBtn, SIGNAL(released()), this, SLOT(onBtnSlot()));
}

void NIBPCalibrationMenuContent::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e)
    killTimer(d_ptr->timerId);
    d_ptr->timerId = -1;
}

void NIBPCalibrationMenuContent::timerEvent(QTimerEvent *e)
{
    // 100ms访问一次NIBP测量状态并使能按钮
    if (e->timerId() == d_ptr->timerId)
    {
        if (nibpParam.isMeasuring())
        {
            d_ptr->enterBtn->setEnabled(false);
        }
        else
        {
            d_ptr->enterBtn->setEnabled(true);
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
