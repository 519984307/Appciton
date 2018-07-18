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
    resize(640, 480);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    setWindowLayout(layout);

    d_ptr->sidebar = new MenuSidebar();
    d_ptr->sidebar->setFixedWidth(180);
    layout->addWidget(d_ptr->sidebar);

    d_ptr->stackWidget = new QStackedWidget();
    layout->addWidget(d_ptr->stackWidget, 1);

    connect(d_ptr->sidebar, SIGNAL(selectItemChanged(int)), this, SLOT(onSelectItemChanged(int)));
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

void MenuWindow::onSelectItemChanged(int index)
{
    Q_ASSERT(index < d_ptr->stackWidget->count());

    d_ptr->stackWidget->setCurrentIndex(index);

    MenuContent *content = qobject_cast<MenuContent *>(d_ptr->stackWidget->currentWidget());
    if (content)
    {
        setWindowTitle(content->description());
    }
}
