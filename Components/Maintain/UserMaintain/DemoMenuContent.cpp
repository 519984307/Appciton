/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/8/27
 **/

#include "DemoMenuContent.h"
#include "SystemManager.h"
#include <QLabel>
#include <QGridLayout>
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"

DemoMenuContent::DemoMenuContent()
    : MenuContent(trs("DemoMode"), trs("DemoConfigDesc"))
{
}

void DemoMenuContent::layoutExec()
{
    QGridLayout *layout =  new QGridLayout(this);
    QLabel *label = new QLabel(trs("DemoMode"));
    layout->addWidget(label, 0, 0);
    Button *btn = new Button(trs("Start"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, 0, 1);
    layout->setRowStretch(1, 1);

    connect(btn, SIGNAL(clicked(bool)), this, SLOT(onButtonClicked()));
}

void DemoMenuContent::onButtonClicked()
{
    Button *btn = qobject_cast<Button *>(sender());
    if (!btn)
    {
        return;
    }

    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        btn->setText(trs("Start"));
        systemManager.setWorkMode(WORK_MODE_NORMAL);
    }
    else
    {
        btn->setText(trs("Exit"));
        systemManager.setWorkMode(WORK_MODE_DEMO);
    }
}
