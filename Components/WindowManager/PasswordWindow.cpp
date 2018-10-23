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
#include "PasswordWidget.h"

PasswordWindow::PasswordWindow(const QString &name, const QString &password)
{
    QVBoxLayout *layout = new QVBoxLayout();

    PasswordWidget *w = new PasswordWidget(name, password);
    layout->addWidget(w);
    connect(w, SIGNAL(correctPassword()), this, SLOT(accept()));
    setWindowLayout(layout);
    setFixedSize(480, 480);
}
