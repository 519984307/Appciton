/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/6
 **/


#include "MenuSidebarItem.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include "ThemeManager.h"
#include "SoundManager.h"

#define MARKER_WIDTH 6

MenuSidebarItem::MenuSidebarItem(QWidget *parent)
    : QAbstractButton(parent)
{
    setCheckable(true);
    QPalette pal = palette();
    themeManger.setupPalette(ThemeManager::ControlMenuSideBarItem, pal);
    setPalette(pal);
}

QSize MenuSidebarItem::sizeHint() const
{
    QFontMetrics fm = fontMetrics();
    int w = fm.width(text()) + MARKER_WIDTH * 2;
    int h = fm.height() * 3;
    if (h < themeManger.getAcceptableControlHeight())
    {
        h = themeManger.getAcceptableControlHeight();
    }
    return QSize(w, h);
}

void MenuSidebarItem::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)
    QPalette pal = palette();
    QColor bgColor;
    QColor textColor;

    if (!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Button);
        textColor = pal.color(QPalette::Disabled, QPalette::ButtonText);
    }
    else if (isDown())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Button);
        textColor = pal.color(QPalette::Active, QPalette::ButtonText);
    }
    else if (hasFocus())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Highlight);
        textColor = pal.color(QPalette::Active, QPalette::HighlightedText);
    }
    else if (isChecked())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Window);
        textColor = pal.color(QPalette::Active, QPalette::ButtonText);
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Button);
        textColor = pal.color(QPalette::Inactive, QPalette::ButtonText);
    }

    QRect rect = contentsRect();

    QPainter painter(this);
    painter.fillRect(rect, bgColor);

    rect.adjust(2 * MARKER_WIDTH, 0, 0, 0);
    painter.setPen(textColor);
    painter.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text());
}

void MenuSidebarItem::nextCheckState()
{
    // do nothing
}

void MenuSidebarItem::focusInEvent(QFocusEvent *ev)
{
    QAbstractButton::focusInEvent(ev);
    emit focusChanged(true, ev->reason());
}

void MenuSidebarItem::focusOutEvent(QFocusEvent *ev)
{
    QAbstractButton::focusOutEvent(ev);
    emit focusChanged(false, ev->reason());
}

void MenuSidebarItem::keyPressEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
    case Qt::Key_Right:
    case Qt::Key_Down:
    case Qt::Key_Enter:
    case Qt::Key_Return:
        break;
    default:
        QAbstractButton::keyPressEvent(ev);
        break;
    }
}

void MenuSidebarItem::keyReleaseEvent(QKeyEvent *ev)
{
    switch (ev->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        focusPreviousChild();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        focusNextChild();
        break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
        click();
        break;
    default:
        QAbstractButton::keyReleaseEvent(ev);
        break;
    }
}

void MenuSidebarItem::mousePressEvent(QMouseEvent *e)
{
    QAbstractButton::mousePressEvent(e);
    // 触屏点击播放按键音
    soundManager.keyPressTone();
}
