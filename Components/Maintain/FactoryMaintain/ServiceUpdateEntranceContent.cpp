/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/24
 **/

#include "ServiceUpdateEntranceContent.h"
#include <QHBoxLayout>
#include <QLabel>
#include "Button.h"
#include "ServiceUpgradeWindow.h"
#include "LanguageManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceUpdateEntranceContent ::ServiceUpdateEntranceContent()
    : MenuContent(trs("ServiceUpdateEntrance"),
                  trs("ServiceUpdateEntranceDesc"))
{
}


void ServiceUpdateEntranceContent ::readyShow()
{
}

void ServiceUpdateEntranceContent ::layoutExec()
{
    if (layout())
    {
        return;
    }

    QHBoxLayout *layout  = new QHBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label = new QLabel(trs("ServiceUpdateEntrance"));
    layout->addWidget(label);
    Button *button = new Button(trs("Enter"));
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));
}

void ServiceUpdateEntranceContent ::onBtnReleased()
{
    ServiceUpgradeWindow *w = ServiceUpgradeWindow::getInstance();
    w->exec();
}
