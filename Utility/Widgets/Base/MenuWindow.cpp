/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/


#include "MenuWindow.h"
#include "FontManager.h"
#include <QBoxLayout>
#include <QStackedWidget>
#include "MenuSidebar.h"
#include <QDebug>

class MenuWindowPrivate
{
public:
    MenuWindowPrivate()
        : sidebar(NULL),
          stackWidget(NULL)
    {
    }

    MenuSidebar *sidebar;
    QStackedWidget *stackWidget;
};

MenuWindow::MenuWindow()
    : Window(), d_ptr(new MenuWindowPrivate())
{
    setWindowTitle("Menu Window");
    resize(800, 580);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setWindowLayout(layout);

    d_ptr->sidebar = new MenuSidebar();
    d_ptr->sidebar->setFixedWidth(200);
    layout->addWidget(d_ptr->sidebar);

    d_ptr->stackWidget = new QStackedWidget();
    layout->addWidget(d_ptr->stackWidget, 1);

    connect(d_ptr->sidebar, SIGNAL(itemClicked(int)), this, SLOT(onSelectItemChanged(int)));
}

MenuWindow::~MenuWindow()
{
}

void MenuWindow::addMenuContent(MenuContent *menu)
{
    d_ptr->sidebar->addItem(menu->name());
    menu->layoutExec();

    ScrollArea *scrollArea = new ScrollArea;
    scrollArea->setWidget(menu);
    scrollArea->setFloatbarPolicy(ScrollArea::FloatBarShowForeverWhenNeeded);
    d_ptr->stackWidget->addWidget(scrollArea);

    if (d_ptr->stackWidget->count() == 1)
    {
        // set the first Item
        d_ptr->sidebar->setChecked(menu->name());
    }
}

void MenuWindow::popup(MenuContent *menu , int x, int y)
{
//    move(x , y);
    show();
    d_ptr->sidebar->popupWidget(menu->name());
}

bool MenuWindow::focusNextPrevChild(bool next)
{
    /*
     *  when the focus switch back to the menusidebar, we need to make menusidebar
     *  focus on the checked item.
     */

    QWidget *cur = focusWidget();

    if (d_ptr->sidebar->isAncestorOf(cur))
    {
        return Window::focusNextPrevChild(next);
    }

    if (next)
    {
        QWidget *w = cur->nextInFocusChain();
        // find previous focus widget in the focus chain
        while (w && cur != w)
        {
            if (w->isEnabled()
                    && (w->focusPolicy() & Qt::TabFocus)
                    && w->isVisibleTo(this)
                    && w->isEnabled())
            {
                break;
            }
            w = w->nextInFocusChain();
        }

        if (w && d_ptr->sidebar->isAncestorOf(w))
        {
            // the next focus widget is inside the menusidebar
            QWidget *f = d_ptr->sidebar->getChecked();
            if (f)
            {
                f->setFocus();
                return true;
            }
        }
    }
    else
    {
        QWidget *w = focusWidget()->previousInFocusChain();

        // find previous focus widget in the focus chain
        while (w && cur != w)
        {
            if (w->isEnabled()
                    && (w->focusPolicy() & Qt::TabFocus)
                    && w->isVisibleTo(this)
                    && w->isEnabled())
            {
                break;
            }
            w = w->previousInFocusChain();
        }

        if (w && d_ptr->sidebar->isAncestorOf(w))
        {
            // the previous focus widget is inside the menusidebar
            QWidget *f = d_ptr->sidebar->getChecked();
            if (f)
            {
                f->setFocus();
                return true;
            }
        }
    }

    return Window::focusNextPrevChild(next);
}

void MenuWindow::onSelectItemChanged(int index)
{
    Q_ASSERT(index < d_ptr->stackWidget->count());

    d_ptr->stackWidget->setCurrentIndex(index);

    ScrollArea *area = qobject_cast<ScrollArea *>(d_ptr->stackWidget->currentWidget());
    if (area)
    {
        d_ptr->stackWidget->setFocusProxy(area);
        MenuContent *content = qobject_cast<MenuContent *>(area->widget());
        if (content)
        {
            setWindowTitle(content->description());
            content->setFocus();
        }
    }
}
