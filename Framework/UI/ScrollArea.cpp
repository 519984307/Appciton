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
#include <QScrollBar>
#include <QDebug>


ScrollAreaPrivate::ScrollAreaPrivate(ScrollArea *const q_ptr)
    : q_ptr(q_ptr),
      scrollBar(NULL),
      scroller(NULL),
      scrollBarPolicy(ScrollArea::FloatBarShowForAWhile),
      direction(ScrollArea::ScrollBothDirection)
{
}

void ScrollAreaPrivate::init()
{
    scrollBar = new FloatScrollBar(q_ptr);
    scroller = new QScrollAreaKineticScroller();

    scroller->setWidget(q_ptr);
    scroller->setScrollMetric(QKineticScroller::OvershootMaximumDistance,
                                      qVariantFromValue(QPointF(0, 0.02)));
    scroller->setScrollMetric(QKineticScroller::OvershootSpringConstant, qVariantFromValue(80));
    scroller->setScrollMetric(QKineticScroller::DragStartDistance, qVariantFromValue(0.005));
}

void ScrollAreaPrivate::setupScrollBarPolicy()
{
    QWidget *w = q_ptr->widget();
    if (!w)
    {
        return;
    }
    int widgetHeight = w->height();

    // handle the scroll bar policy
    switch (scrollBarPolicy)
    {
    case ScrollArea::FloatBarNotShow:
    {
        scrollBar->setVisible(false);
        scrollBar->setShowWhenMove(false);
    }
    break;
    case ScrollArea::FloatBarShowForAWhile:
    {
        scrollBar->setVisible(true);
        scrollBar->setAutoHide(true);
        scrollBar->setShowWhenMove(true);
    }
    break;
    case ScrollArea::FloatBarShowForAWhileWhenNeeded:
    {
        if (widgetHeight > q_ptr->viewport()->height())
        {
            scrollBar->setVisible(true);
            scrollBar->setAutoHide(true);
            scrollBar->setShowWhenMove(true);
        }
        else
        {
            scrollBar->setVisible(false);
            scrollBar->setShowWhenMove(false);
        }
    }
    break;
    case ScrollArea::FloatBarShowForever:
    {
        scrollBar->setVisible(true);
        scrollBar->setAutoHide(false);
    }
    break;
    case ScrollArea::FloatBarShowForeverWhenNeeded:
    {
        if (widgetHeight > q_ptr->viewport()->height())
        {
            scrollBar->setVisible(true);
            scrollBar->setAutoHide(false);
        }
        else
        {
            scrollBar->setVisible(false);
            scrollBar->setShowWhenMove(false);
        }
    }
    break;
    default:
        break;
    }
}

ScrollArea::ScrollArea(QWidget *parent)
    : QScrollArea(parent), d_ptr(new ScrollAreaPrivate(this))
{
    d_ptr->init();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameStyle(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
}

ScrollArea::ScrollArea(ScrollAreaPrivate *d, QWidget *parent)
    : QScrollArea(parent), d_ptr(d)
{
    d_ptr->init();
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setWidgetResizable(true);
    setFrameStyle(QFrame::NoFrame);
    setFocusPolicy(Qt::NoFocus);
}

ScrollArea::~ScrollArea()
{
}

void ScrollArea::setFloatbarPolicy(ScrollArea::FloatBarPolicy policy)
{
    if (d_ptr->scrollBarPolicy == policy)
    {
        return;
    }

    d_ptr->scrollBarPolicy = policy;
}

void ScrollArea::setOverShot(bool enable)
{
    if (enable)
    {
        d_ptr->scroller->setVerticalOvershootPolicy(QKineticScroller::OvershootWhenScrollable);
    }
    else
    {
        d_ptr->scroller->setVerticalOvershootPolicy(QKineticScroller::OvershootAlwaysOff);
    }
}

void ScrollArea::updateFloatBar()
{
    if (widget())
    {
        int widgetHeight = widget()->height();

        int h = viewport()->height() * viewport()->height() / widgetHeight;
        if (h >= viewport()->height())
        {
            h = viewport()->height();
        }
        d_ptr->scrollBar->resize(d_ptr->scrollBar->width(), h);
        d_ptr->scrollBar->raise();

        d_ptr->setupScrollBarPolicy();
    }
}

void ScrollArea::setScrollDirection(ScrollArea::ScrollDirection dir)
{
    d_ptr->direction = dir;
}

void ScrollArea::resizeEvent(QResizeEvent *ev)
{
    QScrollArea::resizeEvent(ev);

    d_ptr->scrollBar->move(ev->size().width() - d_ptr->scrollBar->width(),
                           d_ptr->scrollBar->y());
}

void ScrollArea::showEvent(QShowEvent *ev)
{
    updateFloatBar();
    QScrollArea::showEvent(ev);
}

void ScrollArea::scrollContentsBy(int dx, int dy)
{
    if (!(d_ptr->direction & ScrollHorizontal))
    {
        if (horizontalScrollBar()->value() != 0)
        {
            horizontalScrollBar()->setValue(0);
        }
        dx = 0;
    }

    if (!(d_ptr->direction & ScrollVertical))
    {
        if (verticalScrollBar()->value() != 0)
        {
            verticalScrollBar()->setValue(0);
        }
        dy = 0;
    }

    QScrollArea::scrollContentsBy(dx, dy);
    if (widget())
    {
        int widgetHeight = widget()->height();
        int viewportHeight = viewport()->height();
        if (widgetHeight > viewportHeight)
        {
            // update the float bar position
            int y = widget()->pos().y();
            int range = widgetHeight - viewportHeight;
            int barRange = viewportHeight - d_ptr->scrollBar->height();
            int yPos = -barRange * y / range;
            d_ptr->scrollBar->move(d_ptr->scrollBar->x(), yPos);
        }
    }
}

bool ScrollArea::viewportEvent(QEvent *ev)
{
    if (ev->type() == QEvent::Resize)
    {
        if (widget())
        {
            QResizeEvent *re = static_cast<QResizeEvent *>(ev);
            QSize s = re->size();
            int widgetHeight = widget()->height();

            int h = s.height() * s.height() / widgetHeight;
            if (h >= s.height())
            {
                h = s.height();
            }
            d_ptr->scrollBar->resize(d_ptr->scrollBar->width(), h);
            d_ptr->scrollBar->raise();
            d_ptr->setupScrollBarPolicy();

            ensureWidgetVisible(focusWidget());

            if (widgetHeight > s.height())
            {
                // update the float bar position
                int y = widget()->pos().y();
                int range = widgetHeight - s.height();
                int barRange = s.height() - d_ptr->scrollBar->height();
                int yPos = -barRange * y / range;
                d_ptr->scrollBar->move(d_ptr->scrollBar->x(), yPos);
            }
        }
    }

    return QScrollArea::viewportEvent(ev);
}
