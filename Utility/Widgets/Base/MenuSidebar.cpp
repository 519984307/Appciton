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
#include <QDebug>

#define PREFER_ITEM_HEIGHT (68)
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
    void onItemFocusChanged(bool in, Qt::FocusReason reason);

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
        if (curSelectIndex >= 0)
        {
            // uncheck the previous item
            MenuSidebarItem *lastCheckItem = itemList.at(curSelectIndex);
            lastCheckItem->setChecked(false);
        }

        curSelectIndex = itemList.indexOf(item);
        item->setChecked(true);

        emit q->selectItemChanged(curSelectIndex);
    }
}

void MenuSidebarPrivate::onItemFocusChanged(bool in, Qt::FocusReason reason)
{
    // 当in为true，菜单侧边栏的高度不小于其视窗的高度时，才进入执行
    if (in && widget->height() >= q_ptr->viewport()->height())
    {
        Q_Q(MenuSidebar);
        MenuSidebarItem *item = qobject_cast<MenuSidebarItem *>(q->sender());
        if (item)
        {
            q_ptr->ensureWidgetVisible(item, 0, 0);
        }
        scrollBar->setVisible(true);

        if (reason == Qt::TabFocusReason || reason == Qt::BacktabFocusReason || reason == Qt::MouseFocusReason)
        {
            if (curSelectIndex >= 0)
            {
                // uncheck the previous item
                MenuSidebarItem *lastCheckItem = itemList.at(curSelectIndex);
                lastCheckItem->setChecked(false);
            }

            curSelectIndex = itemList.indexOf(item);
            item->setChecked(true);
            emit q->visiableItemChanged(curSelectIndex);
        }
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
    connect(item, SIGNAL(focusChanged(bool, Qt::FocusReason)), this, SLOT(onItemFocusChanged(bool, Qt::FocusReason)));
}

int MenuSidebar::itemCount() const
{
    Q_D(const MenuSidebar);
    return d->itemList.count();
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
        emit selectItemChanged(index);
    }
    return true;
}

MenuSidebarItem *MenuSidebar::getChecked() const
{
    Q_D(const MenuSidebar);
    if (d->curSelectIndex >= 0)
    {
        return d->itemList.at(d->curSelectIndex);
    }

    return NULL;
}

int MenuSidebar::indexOf(MenuSidebarItem *item) const
{
    Q_D(const MenuSidebar);
    return d->itemList.indexOf(item);
}

MenuSidebarItem *MenuSidebar::itemAt(int index) const
{
    Q_D(const MenuSidebar);
    if (index < 0 || index >= d->itemList.count())
    {
        return NULL;
    }

    return d->itemList.at(index);
}

QSize MenuSidebar::sizeHint() const
{
    return QSize(200, PREFER_ITEM_HEIGHT * DEFAULT_ITEM_NUM);
}

void MenuSidebar::showEvent(QShowEvent *ev)
{
    Q_D(MenuSidebar);

    setWidget(d->widget);

    if (d->curSelectIndex >= 0) {
        emit selectItemChanged(d->curSelectIndex);
    }

    ScrollArea::showEvent(ev);
}

#include "moc_MenuSidebar.cpp"

