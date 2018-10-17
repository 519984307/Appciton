/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "FactoryMaintainWindow.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"
#include "FactoryMaintainMenuWindow.h"

class FactoryMaintainWindowPrivate
{
public:
    FactoryMaintainWindowPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

FactoryMaintainWindow::FactoryMaintainWindow()
    : Window(),
      d_ptr(new FactoryMaintainWindowPrivate)
{
    layoutExec();
}

FactoryMaintainWindow::~FactoryMaintainWindow()
{
    delete d_ptr;
}

void FactoryMaintainWindow::layoutExec()
{
    setWindowTitle(trs("FactoryMaintainSystem"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QString password;
    systemConfig.getStrValue("General|FactoryMaintainPassword", password);
    d_ptr->passwordWidget = new PasswordWidget(trs("FactoryMaintainPassword"), password);
    layout->addWidget(d_ptr->passwordWidget, Qt::AlignCenter);
    connect(d_ptr->passwordWidget, SIGNAL(correctPassword()), this, SLOT(userInputCorrect()));
    setWindowLayout(layout);
    setFixedSize(480, 480);
}

void FactoryMaintainWindow::userInputCorrect()
{
    FactoryMaintainMenuWindow *w = FactoryMaintainMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::ShowBehaviorModal |
                                WindowManager::ShowBehaviorCloseOthers |
                                WindowManager::ShowBehaviorNoAutoClose);
}
