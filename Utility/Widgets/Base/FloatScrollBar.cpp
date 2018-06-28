#include "FloatScrollBar.h"
#include <QBasicTimer>
#include <QPainter>
#include <QTimerEvent>

#define HIDE_TIMEOUT 1500
#define BAR_WIDTH 6

FloatScrollBar::FloatScrollBar(QWidget *parent)
    :QWidget(parent),
      _timer(new QBasicTimer())
{
    setAutoFillBackground(false);
    resize(BAR_WIDTH, 100); //default height 100 px
}

FloatScrollBar::~FloatScrollBar()
{
    delete _timer;
}

void FloatScrollBar::setVisible(bool visiable)
{
    _timer->start(HIDE_TIMEOUT, this);
    QWidget::setVisible(visiable);
}

void FloatScrollBar::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->contentsRect();
    QColor c= palette().color(QPalette::Highlight);
    c.setAlpha(80);
    painter.setBrush(c);
    painter.drawRoundedRect(rect, rect.width() / 2, rect.width() / 2, Qt::AbsoluteSize);
}

void FloatScrollBar::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == _timer->timerId())
    {
        setVisible(false);
        _timer->stop();
    }
}

void FloatScrollBar::showEvent(QShowEvent *ev)
{
    _timer->start(HIDE_TIMEOUT, this);
    QWidget::showEvent(ev);
}

void FloatScrollBar::moveEvent(QMoveEvent *ev)
{
    setVisible(true);
    QWidget::moveEvent(ev);
}

