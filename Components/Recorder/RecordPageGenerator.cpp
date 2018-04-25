#include "RecordPageGenerator.h"
#include <QTimerEvent>
#include <QDateTime>

RecordPageGenerator::RecordPageGenerator(QObject *parent)
    :QObject(parent), _requestStop(false), _timerID(-1)
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

RecordPage *RecordPageGenerator::createPage()
{
    RecordPage *p = new RecordPage(100);
    return p;
}

void RecordPageGenerator::timerEvent(QTimerEvent *ev)
{
    if(_timerID == ev->timerId())
    {
        if(_requestStop)
        {
            emit stop();
            killTimer(_timerID);
            _timerID = -1;
            _requestStop = false;
        }

        RecordPage *page = createPage();
        if(page == NULL)
        {
            emit completed();
            killTimer(_timerID);
        }

        emit generatePage(page);
    }
}
