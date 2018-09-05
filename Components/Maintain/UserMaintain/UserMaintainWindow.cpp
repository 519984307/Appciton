/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "UserMaintainWindow.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"
#include "UserMaintainMenuWindow.h"

class UserMaintainWindowPrivate
{
public:
    UserMaintainWindowPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

UserMaintainWindow::UserMaintainWindow()
    : Window(),
      d_ptr(new UserMaintainWindowPrivate)
{
    layoutExec();
}

UserMaintainWindow::~UserMaintainWindow()
{
    delete d_ptr;
}

void UserMaintainWindow::layoutExec()
{
    setWindowTitle(trs("UserMaintainSystem"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QString password;
    systemConfig.getStrValue("General|UserMaintainPassword", password);
    d_ptr->passwordWidget = new PasswordWidget(trs("UserMaintainPassword"), password);
    layout->addWidget(d_ptr->passwordWidget, Qt::AlignCenter);
    connect(d_ptr->passwordWidget, SIGNAL(correctPassword()), this, SLOT(userInputCorrect()));
    setWindowLayout(layout);
    setFixedSize(480, 480);
}

void UserMaintainWindow::userInputCorrect()
{
    UserMaintainMenuWindow *w = UserMaintainMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorCloseOthers);
}
