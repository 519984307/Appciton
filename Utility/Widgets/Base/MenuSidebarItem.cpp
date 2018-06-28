#include "MenuSidebarItem.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

#define MARKER_WIDTH 6

MenuSidebarItem::MenuSidebarItem(QWidget *parent)
    :QAbstractButton(parent)
{
    setCheckable(true);
}

QSize MenuSidebarItem::sizeHint() const
{
    QFontMetrics fm = fontMetrics();
    int w = fm.width(text()) + MARKER_WIDTH * 2;
    int h = fm.height() * 2;
    return QSize(w, h);
}

void MenuSidebarItem::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPalette pal = palette();
    QColor bgColor;
    QColor textColor;

    if(!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Button);
        textColor = pal.color(QPalette::Disabled, QPalette::ButtonText);
    }
    else if(isDown())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Button);
        textColor = pal.color(QPalette::Active, QPalette::ButtonText);
    }
    else if(hasFocus())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        textColor = pal.color(QPalette::Active, QPalette::HighlightedText);
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Button);
        textColor = pal.color(QPalette::Inactive, QPalette::ButtonText);
    }

    QRect rect = contentsRect();

    QPainter painter(this);
    painter.fillRect(rect, bgColor);

    if(isChecked())
    {
        QRect markerRect =  rect;
        markerRect.setWidth(MARKER_WIDTH);
        painter.fillRect(markerRect, Qt::red);
    }

    rect.adjust(2 * MARKER_WIDTH, 0, 0, 0);
    painter.setPen(textColor);
    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text());
}

void MenuSidebarItem::nextCheckState()
{
    //do nothing
}

void MenuSidebarItem::focusInEvent(QFocusEvent *ev)
{
    QAbstractButton::focusInEvent(ev);
    emit focusChanged(true);
}

void MenuSidebarItem::focusOutEvent(QFocusEvent *ev)
{
    QAbstractButton::focusOutEvent(ev);
    emit focusChanged(false);
}
