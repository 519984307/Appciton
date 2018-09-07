/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/9/6
 **/

#include "CalculateMenuContent.h"
#include "LanguageManager.h"
#include <QBoxLayout>
#include "Button.h"
#include "DoseCalculationWindow.h"
#include "HemodynamicWindow.h"
#include "WindowManager.h"

class CalculateMenuContentPrivate
{
public:
    CalculateMenuContentPrivate() {}
    ~CalculateMenuContentPrivate() {}

    enum MenuItem
    {
        ITEM_BTN_DOSE_CALCULATION,
        ITEM_BTN_BLOOD_CALCULATION,
        ITEM_NR
    };
    QMap<int, Button *> btnMap;
};

void CalculateMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }
    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(20);

    // Dose Calculate button
    Button *btn = new Button;
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setText(trs("DoseCalculation"));
    btn->setProperty("id" , CalculateMenuContentPrivate::ITEM_BTN_DOSE_CALCULATION);
    connect(btn, SIGNAL(released()), this, SLOT(_btnReleased()));
    d_ptr->btnMap.insert(CalculateMenuContentPrivate::ITEM_BTN_DOSE_CALCULATION,
                         btn);
    int btnCount = d_ptr->btnMap.count() - 1;
    mainLayout->addWidget(btn, btnCount / 2 , btnCount % 2);

    // Blood Calculate Button
    btn = new Button;
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setText(trs("HemodynamicCalculation"));
    btn->setProperty("id", CalculateMenuContentPrivate::ITEM_BTN_BLOOD_CALCULATION);
    connect(btn, SIGNAL(released()), this, SLOT(_btnReleased()));
    d_ptr->btnMap.insert(CalculateMenuContentPrivate::ITEM_BTN_BLOOD_CALCULATION,
                         btn);
    btnCount = d_ptr->btnMap.count() - 1;
    mainLayout->addWidget(btn, btnCount / 2, btnCount % 2);
}

void CalculateMenuContent::_btnReleased()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (btn == NULL)
    {
        return;
    }

    int id = btn->property("id").toInt();
    switch (id)
    {
    case CalculateMenuContentPrivate::ITEM_BTN_DOSE_CALCULATION:
    {
        DoseCalculationWindow *calWin = DoseCalculationWindow::getInstance();
        windowManager.showWindow(calWin, WindowManager::ShowBehaviorCloseIfVisiable
                                 | WindowManager::ShowBehaviorCloseOthers);
        break;
    }
    case CalculateMenuContentPrivate::ITEM_BTN_BLOOD_CALCULATION:
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

CalculateMenuContent::CalculateMenuContent()
                        : MenuContent(trs("CalculateMenu"), trs("CalculateMenuDesc")),
                            d_ptr(new CalculateMenuContentPrivate)
{
}

CalculateMenuContent::~CalculateMenuContent()
{
    delete d_ptr;
}
