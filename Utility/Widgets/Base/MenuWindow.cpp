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
#include "MenuSidebar.h"

class MenuWindowPrivate
{
public:
    MenuWindowPrivate()
        : sidebar(NULL),
          scrollArea(NULL)

    {
    }

    MenuSidebar *sidebar;
    ScrollArea *scrollArea;
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
    d_ptr->scrollArea = new ScrollArea();
    layout->addWidget(d_ptr->scrollArea, 1);
}

MenuWindow::~MenuWindow()
{
}

void MenuWindow::addMenuGroup(const QString &title, QWidget *w)
{
    d_ptr->sidebar->addItem(title);
    if (w)
    {
        d_ptr->scrollArea->setWidget(w);
    }
}
