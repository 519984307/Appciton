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

#define BORDER_COLOR QColor(223, 222, 222)
#define BORDER_WIDTH 2
#define HEIGHT_HINT 50
#define WIDTH_HINT  200
#define CORNER_RADIUS 4


class FrameItemPrivate
{
public:
    explicit FrameItemPrivate(FrameItem *const q_ptr, Frame *frame)
        : q_ptr(q_ptr), frame(frame)
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
    : QAbstractButton(parent), d_ptr(new FrameItemPrivate(this, parent))
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
    QColor bgColor;
    QColor textColor;
    QPalette pal = palette();

    if (!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Button);
        textColor = pal.color(QPalette::Disabled, QPalette::ButtonText);
    }
    else if ((isCheckable() && isChecked()) || isDown())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Button);
        textColor = pal.color(QPalette::Active, QPalette::ButtonText);
    }
    else if (hasFocus())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        textColor = pal.color(QPalette::Active, QPalette::HighlightedText);
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Button);
        textColor = pal.color(QPalette::Inactive, QPalette::ButtonText);
    }

    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    QPen pen(BORDER_COLOR, BORDER_WIDTH);
    QRect r = contentsRect();
    int topMargin;
    bool isFirstItem = d_ptr->isFirstItem();
    if (isFirstItem)
    {
        topMargin = 0;
    }
    else
    {
        topMargin = -CORNER_RADIUS;
    }

    int bottomMargin;
    bool isLastItem = d_ptr->isLastItem();
    if (isLastItem)
    {
        bottomMargin = 0;
    }
    else
    {
        bottomMargin = CORNER_RADIUS;
    }

    QRect frame = r.adjusted(BORDER_WIDTH, BORDER_WIDTH + topMargin, -BORDER_WIDTH, bottomMargin - BORDER_WIDTH);
    painter.setPen(pen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(frame, CORNER_RADIUS, CORNER_RADIUS);
    pen.setWidth(BORDER_WIDTH / 2);
    painter.setPen(pen);
    r.adjust(BORDER_WIDTH, 0, -BORDER_WIDTH, 0);
    if (!isFirstItem)
    {
        painter.drawLine(r.topLeft(), r.topRight());
    }

    // if(!isLastItem)
    // {
    //     painter.drawLine(r.bottomLeft(), r.bottomRight());
    // }

    painter.restore();

    painter.setPen(textColor);

    if (!text().isEmpty())
    {
        painter.drawText(contentsRect().adjusted(10, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text());
    }
}
