/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "ConfigManagerMenuContent.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"
#include "ConfigManagerMenuWindow.h"

class ConfigManagerMenuContentPrivate
{
public:
    ConfigManagerMenuContentPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

ConfigManagerMenuContent::ConfigManagerMenuContent()
    : Window(),
      d_ptr(new ConfigManagerMenuContentPrivate)
{
    layoutExec();
}

ConfigManagerMenuContent::~ConfigManagerMenuContent()
{
    delete d_ptr;
}

void ConfigManagerMenuContent::layoutExec()
{
    setWindowTitle(trs("ConfigManager"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QString password;
    systemConfig.getStrValue("General|ConfigManagerPassword", password);
    d_ptr->passwordWidget = new PasswordWidget(trs("ConfigManagerPassword"), password);
    layout->addWidget(d_ptr->passwordWidget, Qt::AlignCenter);
    connect(d_ptr->passwordWidget, SIGNAL(correctPassword()), this, SLOT(userInputCorrect()));
    setWindowLayout(layout);
    setFixedSize(480, 480);
}

void ConfigManagerMenuContent::userInputCorrect()
{
    ConfigManagerMenuWindow *w = ConfigManagerMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::ShowBehaviorCloseOthers);
}
