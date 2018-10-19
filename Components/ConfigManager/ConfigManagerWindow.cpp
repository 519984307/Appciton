/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/12
 **/

#include "ConfigManagerWindow.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"
#include "ConfigManagerMenuWindow.h"

class ConfigManagerWindowPrivate
{
public:
    ConfigManagerWindowPrivate() : passwordWidget(NULL)
    {
    }

    PasswordWidget *passwordWidget;
};

ConfigManagerWindow::ConfigManagerWindow()
    : Window(),
      d_ptr(new ConfigManagerWindowPrivate)
{
    layoutExec();
}

ConfigManagerWindow::~ConfigManagerWindow()
{
    delete d_ptr;
}

void ConfigManagerWindow::layoutExec()
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

void ConfigManagerWindow::userInputCorrect()
{
    ConfigManagerMenuWindow *w = ConfigManagerMenuWindow::getInstance();
    windowManager.showWindow(w, WindowManager::ShowBehaviorHideOthers |
                                WindowManager::ShowBehaviorNoAutoClose);

    // 每次打开主界面时，强制聚焦在首个item
    // 需要放在showWindow下面
    w->focusMenuItem();
}
