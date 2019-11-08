/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/10
 **/

#include "AlarmLimitMenuContent.h"
#include "Framework/Language/LanguageManager.h"
#include <Button.h>
#include <QGridLayout>
#include <QVariant>
#include "AlarmLimitWindow.h"
#include "WindowManager.h"
#include "SystemManager.h"
#include <QDebug>

#define TABLE_ROW_NUM 7

class AlarmLimitMenuContentPrivate
{
public:
    AlarmLimitMenuContentPrivate()
        : alarmLimitBtn(NULL)
        , otherBtn(NULL)
    {
    }

    Button *alarmLimitBtn;
    Button *otherBtn;
};


AlarmLimitMenuContent::AlarmLimitMenuContent()
    : MenuContent(trs("AlarmLimitMenu"), trs("AlarmLimitMenuDesc")),
      d_ptr(new AlarmLimitMenuContentPrivate())
{
}

AlarmLimitMenuContent::~AlarmLimitMenuContent()
{
    delete d_ptr;
}

void AlarmLimitMenuContent::layoutExec()
{
    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    glayout->setColumnStretch(0, 1);
    glayout->setColumnStretch(1, 1);

    Button *btn;

    btn = new Button(QString("%1 >>").arg(trs("AlarmLimit")));
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onbtnClick()));
    glayout->addWidget(btn, 0, 1);
    d_ptr->alarmLimitBtn = btn;

//    btn = new Button(trs("Other"));
//    btn->setButtonStyle(Button::ButtonTextOnly);
//    connect(btn, SIGNAL(released()), this, SLOT(onbtnClick()));
//    glayout->addWidget(btn, 1, 1);
//    d_ptr->otherBtn = btn;

    glayout->setRowStretch(2, 1);
}

void AlarmLimitMenuContent::onbtnClick()
{
    Button *btn = qobject_cast<Button*>(sender());

    if (btn == d_ptr->alarmLimitBtn)
    {
        AlarmLimitWindow w;
        windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
    }
}
