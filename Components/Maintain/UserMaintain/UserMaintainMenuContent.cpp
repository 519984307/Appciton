/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "UserMaintainMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "SoundManager.h"
#include "SystemManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"
#include "UserMaintainMenuWindow.h"

class UserMaintainMenuContentPrivate
{
public:
    UserMaintainMenuContentPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

UserMaintainMenuContent::UserMaintainMenuContent()
    : MenuContent(trs("UserMaintainSystem"), trs("UserMaintainSystem")),
      d_ptr(new UserMaintainMenuContentPrivate)
{
}

UserMaintainMenuContent::~UserMaintainMenuContent()
{
    delete d_ptr;
}

void UserMaintainMenuContent::readyShow()
{
}

void UserMaintainMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QString password;
    systemConfig.getStrValue("General|UserMaintainPassword", password);
    d_ptr->passwordWidget = new PasswordWidget(trs("UserMaintainPassword"), password);
    layout->addWidget(d_ptr->passwordWidget, Qt::AlignCenter);
    connect(d_ptr->passwordWidget, SIGNAL(correctPassword()), this, SLOT(userInputCorrect()));
}

void UserMaintainMenuContent::userInputCorrect()
{
    UserMaintainMenuWindow *w = UserMaintainMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::WINDOW_TYPE_NONMODAL);
}
