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
#include "FloatScrollBar.h"
#include <QResizeEvent>

ScrollAreaPrivate::ScrollAreaPrivate(ScrollArea *const q_ptr)
    : q_ptr(q_ptr),
      scrollBar(NULL),
      scroller(NULL)
{
}

void ScrollAreaPrivate::init()
{
    scrollBar = new FloatScrollBar(q_ptr);
    scroller = new QScrollAreaKineticScroller();

    scroller->setWidget(q_ptr);
    // scroller->setScrollMetric(QKineticScroller::OvershootMaximumDistance,
    //                                  qVariantFromValue(QPointF(0, 0.02)));
    scroller->setScrollMetric(QKineticScroller::OvershootSpringConstant, qVariantFromValue(80));
}

ScrollArea::ScrollArea(QWidget *parent)
    : QScrollArea(parent), d_ptr(new ScrollAreaPrivate(this))
{
    d_ptr->init();
}

ScrollArea::ScrollArea(ScrollAreaPrivate *d, QWidget *parent)
    : QScrollArea(parent), d_ptr(d)
{
    d_ptr->init();
}

ScrollArea::~ScrollArea()
{
}

void ScrollArea::resizeEvent(QResizeEvent *ev)
{
    QScrollArea::resizeEvent(ev);

    d_ptr->scrollBar->move(ev->size().width() - d_ptr->scrollBar->width(),
                           d_ptr->scrollBar->y());
}

void ScrollArea::showEvent(QShowEvent *ev)
{
    int widgetHeight = 1;
    if (widget())
    {
        widgetHeight = widget()->height();
    }

    int h = viewport()->height() * viewport()->height() / widgetHeight;
    if (h >= viewport()->height())
    {
        h = viewport()->height();
    }
    d_ptr->scrollBar->resize(d_ptr->scrollBar->width(), h);
    d_ptr->scrollBar->raise();

    QScrollArea::showEvent(ev);
}

void ScrollArea::scrollContentsBy(int dx, int dy)
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
}

