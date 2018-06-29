/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/


#include "MenuSidebar.h"
#include <QList>
#include "MenuSidebarItem.h"
#include <QBoxLayout>
#include <QResizeEvent>
#include <QSpacerItem>
#include <qscrollareakineticscroller.h>
#include "FloatScrollBar.h"
#include <QDebug>

#define PREFER_ITEM_HEIGHT 40
#define DEFAULT_ITEM_NUM 8

class MenuSidebarPrivate
{
public:
    explicit MenuSidebarPrivate(MenuSidebar *const q_ptr)
        : q_ptr(q_ptr),
          widget(NULL),
          scroller(NULL),
          scrollBar(NULL),
          itemLayout(NULL),
          curSelectIndex(-1)
    {}

    void updateItems(int itemNum);
    MenuSidebar *const q_ptr;
    QWidget *widget;
    QScrollAreaKineticScroller *scroller;
    FloatScrollBar *scrollBar;
    QStringList itemTextList;
    QList<MenuSidebarItem *> itemList;
    QVBoxLayout *itemLayout;
    int curSelectIndex;
};

void MenuSidebarPrivate::updateItems(int itemNum)
{
    if (itemList.count() < itemNum)
    {
        while (itemList.count() < itemNum)
        {
            MenuSidebarItem *item = new MenuSidebarItem();
            itemLayout->insertWidget(itemList.count(), item);
            itemList.append(item);
            QObject::connect(item, SIGNAL(clicked(bool)), q_ptr, SLOT(onItemClicked()));
        }
    }
    else
    {
        while (itemList.count() > itemNum)
        {
            MenuSidebarItem *item = itemList.takeLast();
            itemLayout->removeWidget(item);
            item->setVisible(false);
            item->deleteLater();
        }
    }
}

MenuSidebar::MenuSidebar(QWidget *parent)
    : QScrollArea(parent),
      d_ptr(new MenuSidebarPrivate(this))
{
    setAutoFillBackground(true);

    d_ptr->widget = new QWidget();

    d_ptr->itemLayout = new QVBoxLayout(d_ptr->widget);
    d_ptr->itemLayout->setSpacing(0);
    d_ptr->itemLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->itemLayout->addStretch(1);

    d_ptr->scroller = new QScrollAreaKineticScroller();
    d_ptr->scroller->setWidget(this);
    // d_ptr->scroller->setScrollMetric(QKineticScroller::OvershootMaximumDistance,
    //                                  qVariantFromValue(QPointF(0, 0.02)));
    d_ptr->scroller->setScrollMetric(QKineticScroller::OvershootSpringConstant, qVariantFromValue(80));

    d_ptr->scrollBar = new FloatScrollBar(this);

    QPalette pal = d_ptr->widget->palette();
    QColor bgColor = pal.color(QPalette::Button);
    bgColor = bgColor.darker(120);
    pal.setColor(QPalette::Window, bgColor);
    pal.setColor(QPalette::Button, bgColor);
    d_ptr->widget->setPalette(pal);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameStyle(QFrame::NoFrame);
}

MenuSidebar::~MenuSidebar()
{
}

void MenuSidebar::addItem(const QString &text)
{
    if (d_ptr->itemTextList.contains(text))
    {
        return;
    }

    d_ptr->itemTextList.append(text);

    MenuSidebarItem *item = new MenuSidebarItem();
    item->setText(text);
    item->setFixedHeight(PREFER_ITEM_HEIGHT);
    d_ptr->itemLayout->insertWidget(d_ptr->itemList.count(), item);
    d_ptr->itemList.append(item);
    connect(item, SIGNAL(clicked(bool)), this, SLOT(onItemClicked()));
    connect(item, SIGNAL(focusChanged(bool)), this, SLOT(onItemFocusChanged(bool)));
}

QSize MenuSidebar::sizeHint() const
{
    return QSize(180, PREFER_ITEM_HEIGHT * DEFAULT_ITEM_NUM);
}

void MenuSidebar::resizeEvent(QResizeEvent *ev)
{
    QScrollArea::resizeEvent(ev);

    d_ptr->scrollBar->move(ev->size().width() - d_ptr->scrollBar->width(),
                           d_ptr->scrollBar->y());
}

void MenuSidebar::showEvent(QShowEvent *ev)
{
    setWidget(d_ptr->widget);

    int h = viewport()->height() * viewport()->height() / d_ptr->widget->height();
    if (h >= viewport()->height())
    {
        h = viewport()->height();
    }
    d_ptr->scrollBar->resize(d_ptr->scrollBar->width(), h);
    d_ptr->scrollBar->raise();

    QScrollArea::showEvent(ev);
}

void MenuSidebar::scrollContentsBy(int dx, int dy)
{
    QScrollArea::scrollContentsBy(dx, dy);
    int y = d_ptr->scrollBar->y() - dy;
    if (y < 0)
    {
        y = 0;
    }
    else if (y > viewport()->height() - d_ptr->scrollBar->height())
    {
        y = viewport()->height() - d_ptr->scrollBar->height();
    }
    d_ptr->scrollBar->move(d_ptr->scrollBar->x(), y);
    d_ptr->scrollBar->setVisible(true);
}

void MenuSidebar::onItemClicked()
{
    MenuSidebarItem *item = qobject_cast<MenuSidebarItem *>(sender());
    if (item)
    {
        if (item->isChecked())
        {
            // current item, do nothing
            return;
        }

        if (d_ptr->curSelectIndex >= 0)
        {
            // uncheck the previous item
            MenuSidebarItem *lastCheckItem = d_ptr->itemList.at(d_ptr->curSelectIndex);
            lastCheckItem->setChecked(false);
        }

        d_ptr->curSelectIndex = d_ptr->itemList.indexOf(item);
        item->setChecked(true);

        emit selectItemChanged(item->text());
    }
}

void MenuSidebar::onItemFocusChanged(bool in)
{
    if (in)
    {
        d_ptr->scrollBar->setVisible(true);
    }
}

