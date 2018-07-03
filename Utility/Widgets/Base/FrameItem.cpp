/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/2
 **/

#include "FrameItem.h"
#include "Frame.h"
#include <QPainter>
#include "FontManager.h"

#define BORDER_COLOR QColor(223, 222, 222)
#define BORDER_WIDTH 2
#define HEIGHT_HINT 50
#define WIDTH_HINT  200
#define CORNER_RADIUS 4


class FrameItemPrivate
{
public:
    explicit FrameItemPrivate(FrameItem *const q_ptr)
        : q_ptr(q_ptr), frame(NULL)
    {
    }

    bool isLastItem() const
    {
        return frame ? (frame->itemCount() - 1) == frame->indexOf(q_ptr)
               : true;
    }

    bool isFirstItem() const
    {
        return frame ? (frame->indexOf(q_ptr) == 0)
               : true;
    }

    FrameItem *const q_ptr;
    Frame *frame;
};

FrameItem::FrameItem(const QString &text, Frame *parent)
    : QAbstractButton(parent), d_ptr(new FrameItemPrivate(this))
{
    setText(text);
}

FrameItem::~FrameItem()
{
}

QSize FrameItem::sizeHint() const
{
    return QSize(WIDTH_HINT, HEIGHT_HINT);
}

void FrameItem::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);

    QPainter painter(this);

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(BORDER_COLOR, BORDER_WIDTH);
    QRect r = rect();
    int topMargin;
    if (d_ptr->isFirstItem())
    {
        topMargin = -CORNER_RADIUS;
    }
    else
    {
        topMargin = CORNER_RADIUS;
    }

    int bottomMargin;
    if (d_ptr->isLastItem())
    {
        bottomMargin = CORNER_RADIUS;
    }
    else
    {
        bottomMargin = -CORNER_RADIUS;
    }

    r.adjust(0, topMargin, 0, bottomMargin);
    painter.setPen(pen);
    painter.setBrush(Qt::white);
    painter.drawRoundedRect(r, CORNER_RADIUS, CORNER_RADIUS);
    painter.restore();

    if (!text().isEmpty())
    {
        painter.drawText(rect().adjusted(10, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text());
    }
}
