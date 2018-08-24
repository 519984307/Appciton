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
#include "MenuWindow.h"
#include <QFocusEvent>

class MenuContentPrivate
{
public:
    MenuContentPrivate(const QString &name, const QString &desc)
        : name(name), description(desc)
    {
    }

    QString name;
    QString description;
    QVariant showParam;
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

void MenuContent::doLayout()
{
    if (layout())
    {
        // alread layout
        return;
    }

    layoutExec();
}

MenuWindow *MenuContent::getMenuWindow()
{
    QWidget *p = parentWidget();
    while ((p != NULL))
    {
        MenuWindow *w = qobject_cast<MenuWindow *>(p);
        if (w)
        {
            return w;
        }
        else
        {
            p = p->parentWidget();
        }
    }

    return NULL;
}

QVariant MenuContent::takeShowParam()
{
    QVariant v = d_ptr->showParam;
    d_ptr->showParam = QVariant();
    return v;
}

void MenuContent::setShowParam(const QVariant &param)
{
    d_ptr->showParam = param;
}

void MenuContent::showEvent(QShowEvent *ev)
{
    readyShow();
    QWidget::showEvent(ev);
}

void MenuContent::focusInEvent(QFocusEvent *ev)
{
    QObjectList objects = children();
    if (ev->reason() == Qt::BacktabFocusReason)
    {
        while (!objects.isEmpty())
        {
            QObject *obj = objects.takeLast();
            if (!obj->isWidgetType())
            {
                continue;
            }
            QWidget *w = qobject_cast<QWidget *>(obj);
            if (w->focusPolicy() != Qt::NoFocus)
            {
                w->setFocus();
                return;
            }
        }
    }
    else
    {
        while (!objects.isEmpty())
        {
            QObject *obj = objects.takeFirst();
            if (!obj->isWidgetType())
            {
                continue;
            }
            QWidget *w = qobject_cast<QWidget *>(obj);
            if (w->focusPolicy() != Qt::NoFocus)
            {
                w->setFocus(Qt::TabFocusReason);
                return;
            }
        }
    }
}
