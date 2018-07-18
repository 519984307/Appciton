/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/


#include "ScrollArea_p.h"
#include "MenuSidebar.h"
#include <QList>
#include "MenuSidebarItem.h"
#include <QBoxLayout>
#include <QResizeEvent>
#include <ThemeManager.h>

#define PREFER_ITEM_HEIGHT (themeManger.getAcceptableControlHeight())
#define DEFAULT_ITEM_NUM 8

class MenuSidebarPrivate : public ScrollAreaPrivate
{
public:
    Q_DECLARE_PUBLIC(MenuSidebar)
    explicit MenuSidebarPrivate(MenuSidebar *const q_ptr)
        : ScrollAreaPrivate(q_ptr),
          widget(NULL),
          itemLayout(NULL),
          curSelectIndex(-1)
    {
    }

    void onItemClicked();
    void onItemFocusChanged(bool in);

    QWidget *widget;
    QStringList itemTextList;
    QList<MenuSidebarItem *> itemList;
    QVBoxLayout *itemLayout;
    int curSelectIndex;
};

void MenuSidebarPrivate::onItemClicked()
{
    Q_Q(MenuSidebar);
    MenuSidebarItem *item = qobject_cast<MenuSidebarItem *>(q->sender());
    if (item)
    {
        if (item->isChecked())
        {
            // current item, do nothing
            return;
        }

        if (curSelectIndex >= 0)
        {
            // uncheck the previous item
            MenuSidebarItem *lastCheckItem = itemList.at(curSelectIndex);
            lastCheckItem->setChecked(false);
        }

        curSelectIndex = itemList.indexOf(item);
        item->setChecked(true);

        emit q->selectItemChanged(item->text());
        emit q->selectItemChanged(curSelectIndex);
    }
}

void MenuSidebarPrivate::onItemFocusChanged(bool in)
{
    if (in && widget->height() > q_ptr->viewport()->height())
    {
        Q_Q(MenuSidebar);
        MenuSidebarItem *item = qobject_cast<MenuSidebarItem *>(q->sender());
        if (item)
        {
            q_ptr->ensureWidgetVisible(item);
        }
        scrollBar->setVisible(true);
    }
}

MenuSidebar::MenuSidebar(QWidget *parent)
    : ScrollArea(new MenuSidebarPrivate(this), parent)
{
    Q_D(MenuSidebar);

    setAutoFillBackground(true);

    d->widget = new QWidget();

    d->itemLayout = new QVBoxLayout(d->widget);
    d->itemLayout->setSpacing(0);
    d->itemLayout->setContentsMargins(0, 0, 0, 0);
    d->itemLayout->addStretch(1);

    QPalette pal = d->widget->palette();
    QColor bgColor = pal.color(QPalette::Button);
    bgColor = bgColor.darker(120);
    pal.setColor(QPalette::Window, bgColor);
    pal.setColor(QPalette::Button, bgColor);
    d->widget->setPalette(pal);

    setFloatbarPolicy(FloatBarShowForAWhileWhenNeeded);

    d->scroller->setHorizontalOvershootPolicy(QKineticScroller::OvershootAlwaysOff);
    setScrollDirection(ScrollArea::ScrollVertical);
}

MenuSidebar::~MenuSidebar()
{
}

void MenuSidebar::addItem(const QString &text)
{
    Q_D(MenuSidebar);

    if (d->itemTextList.contains(text))
    {
        return;
    }

    d->itemTextList.append(text);

    MenuSidebarItem *item = new MenuSidebarItem();
    item->setText(text);
    item->setFixedHeight(PREFER_ITEM_HEIGHT);
    d->itemLayout->insertWidget(d->itemList.count(), item);
    d->itemList.append(item);
    connect(item, SIGNAL(clicked(bool)), this, SLOT(onItemClicked()));
    connect(item, SIGNAL(focusChanged(bool)), this, SLOT(onItemFocusChanged(bool)));
}

bool MenuSidebar::setChecked(const QString &text)
{
    Q_D(MenuSidebar);
    int index = 0;
    foreach(MenuSidebarItem *item, d->itemList)
    {
        if (item->text() == text)
        {
            break;
        }
        ++index;
    }

    if (index >= d->itemList.count())
    {
        // not found
        return false;
    }

    if (index != d->curSelectIndex)
    {
        if (d->curSelectIndex >= 0)
        {
            // uncheck the previous item
            MenuSidebarItem *lastCheckItem = d->itemList.at(d->curSelectIndex);
            lastCheckItem->setChecked(false);
        }

        d->itemList.at(index)->setChecked(true);
        d->curSelectIndex = index;
        emit selectItemChanged(d->itemList.at(index)->text());
        emit selectItemChanged(index);
    }
    return true;
}

QSize MenuSidebar::sizeHint() const
{
    return QSize(180, PREFER_ITEM_HEIGHT * DEFAULT_ITEM_NUM);
}

void MenuSidebar::showEvent(QShowEvent *ev)
{
    Q_D(MenuSidebar);

    setWidget(d->widget);

    ScrollArea::showEvent(ev);
}

#include "moc_MenuSidebar.cpp"

