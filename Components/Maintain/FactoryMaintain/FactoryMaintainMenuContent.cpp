/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "FactoryMaintainMenuContent.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"
#include "FactoryMaintainMenuWindow.h"

class FactoryMaintainMenuContentPrivate
{
public:
    FactoryMaintainMenuContentPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

FactoryMaintainMenuContent::FactoryMaintainMenuContent()
    : Window(),
      d_ptr(new FactoryMaintainMenuContentPrivate)
{
    layoutExec();
}

FactoryMaintainMenuContent::~FactoryMaintainMenuContent()
{
    delete d_ptr;
}

void FactoryMaintainMenuContent::layoutExec()
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

void FactoryMaintainMenuContent::userInputCorrect()
{
    FactoryMaintainMenuWindow *w = FactoryMaintainMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorCloseOthers);
}
