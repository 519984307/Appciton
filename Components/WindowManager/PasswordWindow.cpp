/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/23
 **/

#include "PasswordWindow.h"
#include <QVBoxLayout>
#include "Framework/UI/PasswordWidget.h"
#include "IConfig.h"

PasswordWindow::PasswordWindow(const QString &name, const QString &password)
{
    QVBoxLayout *layout = new QVBoxLayout();

    PasswordWidget *w = new PasswordWidget(name, password);

    /* set the supper password */
    QString superPassword;
    systemConfig.getStrValue("General|SuperPassword", superPassword);
    if (!superPassword.isEmpty())
    {
        w->setSuperPassword(superPassword);
    }
    layout->addWidget(w);
    connect(w, SIGNAL(correctPassword()), this, SLOT(accept()));
    setWindowLayout(layout);
    setFixedSize(480, 480);
}
