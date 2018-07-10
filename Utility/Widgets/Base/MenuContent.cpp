/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/5
 **/

#include "MenuContent.h"


class MenuContentPrivate
{
public:
    MenuContentPrivate(const QString &name, const QString &desc)
        : name(name), description(desc)
    {
    }

    QString name;
    QString description;
};

MenuContent::MenuContent(const QString &name, const QString &description, QWidget *parent)
    : QWidget(parent), d_ptr(new MenuContentPrivate(name, description))
{
}

MenuContent::~MenuContent()
{
    if (d_ptr)
    {
        delete d_ptr;
    }
}

const QString &MenuContent::name() const
{
    return d_ptr->name;
}

const QString &MenuContent::description() const
{
    return d_ptr->description;
}

void MenuContent::showEvent(QShowEvent *ev)
{
    readyShow();
    QWidget::showEvent(ev);
}
