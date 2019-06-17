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

void MenuContent::setShowParam(const QVariant &param)
{
    Q_UNUSED(param)
}

void MenuContent::showEvent(QShowEvent *ev)
{
    readyShow();
    QWidget::showEvent(ev);
}

void MenuContent::focusInEvent(QFocusEvent *ev)
{
    QObjectList objects = children();
    QWidget *w = NULL;
    if (ev->reason() == Qt::BacktabFocusReason)
    {
        while (!objects.isEmpty())
        {
            QObject *obj = objects.takeLast();
            if (!obj->isWidgetType())
            {
                continue;
            }

            // 再做一层for循环查找聚焦点
            bool isBreak = false;
            QObjectList objectsListChild = obj->children();
            while (!objectsListChild.isEmpty())
            {
                QObject *objChild = objectsListChild.takeFirst();
                if (!objChild->isWidgetType())
                {
                    continue;
                }

                w = qobject_cast<QWidget *>(objChild);

                if (w->isEnabled() == false)
                {
                    continue;
                }

                if (w->focusPolicy() != Qt::NoFocus)
                {
                    w->setFocus();
                    isBreak = true;
                    break;
                }
            }
            if (isBreak)
            {
                break;
            }

            w = qobject_cast<QWidget *>(obj);

            if (w->isEnabled() == false)
            {
                continue;
            }

            if (w->focusPolicy() != Qt::NoFocus)
            {
                w->setFocus();
                break;
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

            // 再做一层for循环查找聚焦点
            bool isBreak = false;
            QObjectList objectsListChild = obj->children();
            while (!objectsListChild.isEmpty())
            {
                QObject *objChild = objectsListChild.takeFirst();
                if (!objChild->isWidgetType())
                {
                    continue;
                }

                w = qobject_cast<QWidget *>(objChild);

                if (w->isEnabled() == false)
                {
                    continue;
                }

                if (w->focusPolicy() != Qt::NoFocus)
                {
                    w->setFocus(Qt::TabFocusReason);
                    isBreak = true;
                    break;
                }
            }
            if (isBreak)
            {
                break;
            }

            w = qobject_cast<QWidget *>(obj);

            if (w->isEnabled() == false)
            {
                continue;
            }

            if (w->focusPolicy() != Qt::NoFocus)
            {
                w->setFocus(Qt::TabFocusReason);
                break;
            }
        }
        // add a judgment on whether the widget is focused or not
        if (objects.isEmpty() && !w->hasFocus())
        {
            focusNextChild();  // 当菜单中无控件可以聚焦时,焦点聚焦到下个窗口.
        }
    }

    // 强制使得聚焦区域视窗可见
    if (w)
    {
        MenuWindow *mw = this->getMenuWindow();
        if (mw)
        {
            mw->ensureWidgetVisiable(w);
        }
    }
}
