/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/6
 **/

#include "CalculateWindow.h"
#include "Framework/Language/LanguageManager.h"
#include <QBoxLayout>
#include "Button.h"
#include "DoseCalculationWindow.h"
#include "HemodynamicWindow.h"
#include "WindowManager.h"
#include <QVariant>

class CalculateWindowPrivate
{
public:
    CalculateWindowPrivate() {}
    ~CalculateWindowPrivate() {}

    enum MenuItem
    {
        ITEM_BTN_DOSE_CALCULATION,
        ITEM_BTN_BLOOD_CALCULATION,
    };
};

void CalculateWindow::layoutExec()
{
    setWindowTitle(trs("CalculateWindow"));

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 3);
    mainLayout->setColumnStretch(2, 1);
    setFixedWidth(480);
    int row = 0;

    // Dose Calculate button
    Button *btn = new Button;
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setText(trs("DoseCalculation"));
    btn->setProperty("id" , CalculateWindowPrivate::ITEM_BTN_DOSE_CALCULATION);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    mainLayout->addWidget(btn, row, 1);
    row++;

    // Blood Calculate Button
    btn = new Button;
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setText(trs("HemodynamicCalculation"));
    btn->setProperty("id", CalculateWindowPrivate::ITEM_BTN_BLOOD_CALCULATION);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleased()));
    mainLayout->addWidget(btn, row, 1);
    row++;

    mainLayout->setRowStretch(row, 1);
    setWindowLayout(mainLayout);
}

void CalculateWindow::onBtnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == NULL)
    {
        return;
    }

    int id = btn->property("id").toInt();
    switch (id)
    {
    case CalculateWindowPrivate::ITEM_BTN_DOSE_CALCULATION:
    {
        DoseCalculationWindow *calWin = DoseCalculationWindow::getInstance();
        windowManager.showWindow(calWin, WindowManager::ShowBehaviorCloseIfVisiable
                                 | WindowManager::ShowBehaviorCloseOthers);
        break;
    }
    case CalculateWindowPrivate::ITEM_BTN_BLOOD_CALCULATION:
    {
        HemodynamicWindow *hemWin = HemodynamicWindow::getInstance();
        windowManager.showWindow(hemWin, WindowManager::ShowBehaviorCloseIfVisiable
                                 | WindowManager::ShowBehaviorCloseOthers);
        break;
    }
    default:
        break;
    }
}

CalculateWindow::CalculateWindow()
               : Dialog(),
                 d_ptr(new CalculateWindowPrivate)
{
    layoutExec();
}

CalculateWindow *CalculateWindow::getInstance()
{
    static CalculateWindow *instance = NULL;
    if (instance == NULL)
    {
        instance = new CalculateWindow;
    }
    return instance;
}

CalculateWindow::~CalculateWindow()
{
    delete d_ptr;
}
