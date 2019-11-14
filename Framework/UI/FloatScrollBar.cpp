/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/6/29
 **/

#include "FloatScrollBar.h"
#include <QBasicTimer>
#include <QPainter>
#include <QTimerEvent>

#define HIDE_TIMEOUT 1500
#define BAR_WIDTH 6

class FloatScrollBarPrivate
{
public:
    FloatScrollBarPrivate()
        : timer(QBasicTimer()),
          autoHide(true),
          showWhenMove(true)
    {}

    QBasicTimer timer;
    bool autoHide;
    bool showWhenMove;
};

FloatScrollBar::FloatScrollBar(QWidget *parent)
    : QWidget(parent),
      d_ptr(new FloatScrollBarPrivate())
{
    setAutoFillBackground(false);
    resize(BAR_WIDTH, 100);  // default height 100 px
}

FloatScrollBar::~FloatScrollBar()
{
    delete d_ptr;
}

void FloatScrollBar::setVisible(bool visiable)
{
    if (visiable && d_ptr->autoHide)
    {
        d_ptr->timer.start(HIDE_TIMEOUT, this);
    }
    QWidget::setVisible(visiable);
}

void FloatScrollBar::setAutoHide(bool autoHide)
{
    d_ptr->autoHide = autoHide;
}

void FloatScrollBar::setShowWhenMove(bool show)
{
    d_ptr->showWhenMove = show;
}

void FloatScrollBar::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->contentsRect();
    QColor c = palette().color(QPalette::Highlight);
    c.setAlpha(250);
    painter.setBrush(c);
    painter.drawRoundedRect(rect, rect.width() / 2, rect.width() / 2, Qt::AbsoluteSize);
}

void FloatScrollBar::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == d_ptr->timer.timerId())
    {
        if (d_ptr->autoHide)
        {
            setVisible(false);
        }
        d_ptr->timer.stop();
    }
}

void FloatScrollBar::showEvent(QShowEvent *ev)
{
    if (d_ptr->autoHide)
    {
        d_ptr->timer.start(HIDE_TIMEOUT, this);
    }
    QWidget::showEvent(ev);
}

void FloatScrollBar::moveEvent(QMoveEvent *ev)
{
    if (d_ptr->showWhenMove)
    {
        setVisible(true);
    }
    QWidget::moveEvent(ev);
}

