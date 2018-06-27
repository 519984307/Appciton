#include "FloatScrollBar.h"
#include <QScrollArea>
#include <QResizeEvent>
#include <QBasicTimer>
#include <QPainter>
#include <QResizeEvent>
#include <QMoveEvent>
#include <QChildEvent>
#include <QScrollBar>
#include <QDebug>

#define HIDE_TIMEOUT 1500
#define BAR_WIDTH 6

class FloatScrollBarPrivate
{
public:
    FloatScrollBarPrivate()
        :area(NULL),
          widghtHeight(0),
          areaHeight(0)
    {}

    int getBarHeight();
    QScrollArea *area;
    QBasicTimer *timer;
    int widghtHeight;
    int areaHeight;
};

int FloatScrollBarPrivate::getBarHeight()
{
    int height = areaHeight;
    if(widghtHeight > height)
    {
        height = areaHeight /  widghtHeight * areaHeight;
    }
    return height;
}

FloatScrollBar::FloatScrollBar()
    :QWidget(),
      d_ptr(new FloatScrollBarPrivate)
{
    setAutoFillBackground(false);
    setVisible(false);
}

FloatScrollBar::~FloatScrollBar()
{

}

bool FloatScrollBar::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == d_ptr->area)
    {
        switch (ev->type()) {
        case QEvent::Show:
            this->setVisible(true);
            d_ptr->timer->start(HIDE_TIMEOUT, this);
            break;
        case QEvent::Resize:
        {
            QResizeEvent *resizeEv = static_cast<QResizeEvent *>(ev);
            d_ptr->areaHeight = resizeEv->size().height();
            resize(BAR_WIDTH, d_ptr->getBarHeight());
            move(d_ptr->area->width() - BAR_WIDTH, y());
        }
            break;

        case QEvent::ChildAdded:
        {
            QChildEvent *ce = static_cast<QChildEvent *>(ev);
            if(ce->child()->isWidgetType())
            {
                QWidget *w  = static_cast<QWidget *>(ce->child());
                if(w == d_ptr->area->widget())
                {
                    d_ptr->widghtHeight = w->height();
                    resize(BAR_WIDTH, d_ptr->getBarHeight());
                }
            }
        }
            break;
        default:
            break;
        }
    }
    return false;
}

void FloatScrollBar::setWidget(QScrollArea *widget)
{
    if(d_ptr->area)
    {
        d_ptr->area->removeEventFilter(this);
    }

    d_ptr->area = widget;
    setParent(d_ptr->area);
    if(d_ptr->area)
    {
        d_ptr->area->installEventFilter(this);
        move(d_ptr->area->width() - BAR_WIDTH, y());
        d_ptr->areaHeight = widget->height();
        if(widget->widget())
        {
            d_ptr->widghtHeight = widget->widget()->height();
            resize(BAR_WIDTH, d_ptr->getBarHeight());
        }
    }
}

void FloatScrollBar::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev);

    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->contentsRect();
    painter.setBrush(palette().highlight());
    painter.drawRoundedRect(rect, rect.width() / 2, rect.width() / 2, Qt::AbsoluteSize);
}

void FloatScrollBar::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == d_ptr->timer->timerId())
    {
        setVisible(false);
        d_ptr->timer->stop();
    }
}

void FloatScrollBar::showEvent(QShowEvent *ev)
{
    d_ptr->timer->start(HIDE_TIMEOUT, this);
}

