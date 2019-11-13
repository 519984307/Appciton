/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/3
 **/

#include "PopupListItem.h"
#include <QPainter>
#include <ThemeManager.h>
#include <QKeyEvent>
#include <QDebug>
#include "SoundManagerInterface.h"

#define DEFAULT_HEIGHT (themeManager.getAcceptableControlHeight())


class PopupListItemPrivate
{
public:
    PopupListItemPrivate() : hasBeenPressed(false) {}
    bool hasBeenPressed;
};

PopupListItem::PopupListItem(const QString &text, QWidget *parent)
    : QAbstractButton(parent), d_ptr(new PopupListItemPrivate)
{
    setText(text);

    QPalette p = palette();
    themeManager.setupPalette(ThemeManager::ControlPopupListItem, &p);
    setPalette(p);

    setCheckable(true);
}

PopupListItem::~PopupListItem()
{
}

QSize PopupListItem::sizeHint() const
{
    QFontMetrics fm = fontMetrics();
    QSize s;
    s.setWidth(fm.width(text()));
    s.setHeight(DEFAULT_HEIGHT);
    return s;
}

void PopupListItem::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)

    QPainter painter(this);

    const QPalette &pal = palette();
    QColor bgColor;

    if (hasFocus())
    {
        bgColor = pal.color(QPalette::Highlight);
        painter.setPen(pal.color(QPalette::HighlightedText));
    }
    else if (isChecked())
    {
        bgColor = pal.color(QPalette::Active, QPalette::Window);
        painter.setPen(pal.color(QPalette::Active, QPalette::WindowText));
    }
    else if (!isEnabled())
    {
        bgColor = pal.color(QPalette::Disabled, QPalette::Window);
        painter.setPen(pal.color(QPalette::Disabled, QPalette::WindowText));
    }
    else
    {
        bgColor = pal.color(QPalette::Inactive, QPalette::Window);
        painter.setPen(pal.color(QPalette::Inactive, QPalette::WindowText));
    }

    QRect r = contentsRect();

    painter.fillRect(r, bgColor);

    painter.drawText(r, Qt::AlignCenter, text());
}

void PopupListItem::nextCheckState()
{
    // do nothing
}

void PopupListItem::keyPressEvent(QKeyEvent *e)
{
    d_ptr->hasBeenPressed = true;
    switch (e->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        break;
    case Qt::Key_Return:
        break;
    default:
        QAbstractButton::keyPressEvent(e);
        break;
    }
}

void PopupListItem::keyReleaseEvent(QKeyEvent *e)
{
    if (!d_ptr->hasBeenPressed)
    {
        return;
    }
    d_ptr->hasBeenPressed = false;
    switch (e->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        focusPreviousChild();
        break;
    case Qt::Key_Right:
    case Qt::Key_Down:
        focusNextChild();
        break;
    case Qt::Key_Return:
        click();
        break;
    default:
        QAbstractButton::keyPressEvent(e);
        break;
    }
}

void PopupListItem::mousePressEvent(QMouseEvent *e)
{
    QAbstractButton::mousePressEvent(e);
    // 触屏点击播放按键音
    SoundManagerInterface *sound = SoundManagerInterface::getSoundManager();
    if (sound)
    {
        sound->keyPressTone();
    }
}
