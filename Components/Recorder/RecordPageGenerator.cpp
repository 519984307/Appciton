#include "RecordPageGenerator.h"
#include <QTimerEvent>
#include <QDateTime>
#include <qmath.h>
#include <QPainterPath>
#include <QPainter>

#define DEFAULT_PAGE_WIDTH 200
RecordPageGenerator::RecordPageGenerator(QObject *parent)
    :QObject(parent), _requestStop(false), _generate(true), _timerID(-1)
{

}

RecordPageGenerator::~RecordPageGenerator()
{
}

int RecordPageGenerator::type() const
{
    return Type;
}

void RecordPageGenerator::start(int interval)
{
    _timerID = startTimer(interval);
}

void RecordPageGenerator::stop()
{
    _requestStop = true;
}

void RecordPageGenerator::pageControl(bool pause)
{
    _generate = !pause;
}

RecordPage *RecordPageGenerator::createPage()
{
    RecordPage *p = new RecordPage(DEFAULT_PAGE_WIDTH);
#if 1
    //draw a sin wave
    qreal yOffset = p->height() / 2;
    qreal xOffset = 2 * 3.1415926 / DEFAULT_PAGE_WIDTH;
    QPainter painter(p);
    painter.setPen(Qt::white);
    static QPointF lastP(0, yOffset);
    QPointF p2;

    for(int i = 0; i< p->width(); i++)
    {
        p2.setX(i);
        p2.setY(yOffset - qSin(i * xOffset) * (p->height() / 2 - 2));
        painter.drawLine(lastP, p2);
        lastP = p2;
    }

    lastP.setX(0);
    lastP.setY(lastP.y() - 1);


#endif
    return p;
}

void RecordPageGenerator::timerEvent(QTimerEvent *ev)
{
    if(_timerID == ev->timerId())
    {
        if(_requestStop)
        {
            killTimer(_timerID);
            _timerID = -1;
            _requestStop = false;
            emit stop();
            return;
        }

        if(!_generate)
        {
            return;
        }

        RecordPage *page = createPage();
        if(page == NULL)
        {
            killTimer(_timerID);
            emit stop();
            return;
        }

        emit generatePage(page);
    }
}
