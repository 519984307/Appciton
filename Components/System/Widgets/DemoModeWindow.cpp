/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/4
 **/

#include "DemoModeWindow.h"
#include "LanguageManager.h"
#include "IConfig.h"
#include "PasswordWidget.h"
#include <QVBoxLayout>
#include "WindowManager.h"

class DemoModeWindowPrivate
{
public:
    DemoModeWindowPrivate() : passwordWidget(NULL),
                              isInputCorrect(false)
    {
    }

    PasswordWidget *passwordWidget;
    bool isInputCorrect;
};

DemoModeWindow::DemoModeWindow()
    : Window(),
      d_ptr(new DemoModeWindowPrivate)
{
    layoutExec();
}

DemoModeWindow::~DemoModeWindow()
{
    delete d_ptr;
}

void DemoModeWindow::layoutExec()
{
    setWindowTitle(trs("DemoModeMenu"));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QString password;
    systemConfig.getStrValue("General|DemoModePassword", password);
    d_ptr->passwordWidget = new PasswordWidget(trs("DemoModePassword"), password);
    layout->addWidget(d_ptr->passwordWidget, Qt::AlignCenter);
    connect(d_ptr->passwordWidget, SIGNAL(correctPassword()), this, SLOT(userInputCorrect()));
    setWindowLayout(layout);
    setFixedSize(480, 480);
}

bool DemoModeWindow::isUserInputCorrect() const
{
    bool isCorrect;
    isCorrect = d_ptr->isInputCorrect;
    d_ptr->isInputCorrect = false;

    return isCorrect;
}


void DemoModeWindow::userInputCorrect()
{
    d_ptr->isInputCorrect = true;
    done(0);
}
