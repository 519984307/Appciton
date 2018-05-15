#include "RecordPageProcessor.h"
#include <QList>
#include <QApplication>
#include <QTimer>
#include "Utility.h"
#include "Debug.h"

#define PAGE_QUEUE_SIZE 32

class RecordPageProcessorPrivate
{
public:
    RecordPageProcessorPrivate(PrintProviderIFace *iface)
        :iface(iface), flush(false), processing(false), pause(false),
          updateSpeed(false), queueIsFull(false), curSpeed(PRINT_SPEED_250)
    {}


    PrintProviderIFace *iface;
    QList<RecordPage *> pages;
    bool flush;
    bool processing;
    bool pause;
    bool updateSpeed;
    bool queueIsFull;
    PrintSpeed curSpeed;
};

RecordPageProcessor::RecordPageProcessor(PrintProviderIFace *iface, QObject *parent)
    :QObject(parent), d_ptr(new RecordPageProcessorPrivate(iface))
{

}

RecordPageProcessor::~RecordPageProcessor()
{
    qDeleteAll(d_ptr->pages);
}

bool RecordPageProcessor::isProcessing() const
{
    return d_ptr->processing;
}

void RecordPageProcessor::updatePrintSpeed(PrintSpeed speed)
{
    if(d_ptr->curSpeed == speed || speed > PRINT_SPEED_NR)
    {
        return;
    }

    d_ptr->curSpeed = speed;
    d_ptr->updateSpeed = true;
}

void RecordPageProcessor::addPage(RecordPage *page)
{
    if(!page)
    {
        return;
    }

#if 1
    QString path("/srv/nfs/tmp/");
    //QString path("/usr/local/nPM/bin/");
    path += page->getID();
    path += ".png";
    page->save(path);
#endif

    d_ptr->pages.append(page);

    if(d_ptr->pages.size() >= PAGE_QUEUE_SIZE)
    {
        d_ptr->queueIsFull = true;
        emit pageQueueFull(d_ptr->queueIsFull);
    }

    if(!d_ptr->processing)
    {
        //process the page in the next event loop
        d_ptr->processing = true;
        QTimer::singleShot(0, this, SLOT(processPages()));
    }
}

void RecordPageProcessor::flushPages()
{
    qDeleteAll(d_ptr->pages);
    d_ptr->pages.clear();


    if(d_ptr->queueIsFull)
    {
        d_ptr->queueIsFull = false;
        emit pageQueueFull(d_ptr->queueIsFull);
    }

    if(d_ptr->processing)
    {
        d_ptr->flush = true;
    }
}

void RecordPageProcessor::pauseProcessing(bool pause)
{
    if(!d_ptr->processing)
    {
        qDebug()<<"Not start yet, not need to pause";
    }

    d_ptr->pause = pause;
}


void RecordPageProcessor::processPages()
{
    RecordPage *page = NULL;

    while(!d_ptr->pages.isEmpty())
    {
        //check whether speed need to update
        if(d_ptr->updateSpeed)
        {
            d_ptr->updateSpeed = false;
            d_ptr->iface->setPrintSpeed(d_ptr->curSpeed);
        }
        /*
         * Note: If the pages are flushed, current page will be the last page
         */
        page = d_ptr->pages.takeFirst();

        // update page queue status
        if (d_ptr->queueIsFull && d_ptr->pages.size() < PAGE_QUEUE_SIZE)
        {
            d_ptr->queueIsFull = false;
            emit pageQueueFull(d_ptr->queueIsFull);
        }

        int dataLen = page->height() / 8;
        unsigned char data[dataLen];
        for(int x = 0; x < page->width(); x ++)
        {
            memset(data, 0, dataLen);
            page->getColumnData(x, data);

            while(d_ptr->pause && !d_ptr->flush)
            {
                Util::waitInEventLoop(15);
            }

            if(!d_ptr->iface->sendBitmapData(data, dataLen))
            {
                //send failed, uart buffer might be full
                qdebug("send bitmap data failed, send again after 20 ms.");
                Util::waitInEventLoop(15);

                //send again
                d_ptr->iface->sendBitmapData(data, dataLen);
            }

            Util::waitInEventLoop(2);       // 2.5ms per column when print speed is 50ms

            if(d_ptr->flush)
            {
                break;
            }
        }

        delete page;
        page = NULL;
    }

    d_ptr->processing = false;
    if(d_ptr->flush)
    {
        d_ptr->iface->flush();
        d_ptr->iface->stop();
    }
    d_ptr->flush = false;

    emit processFinished();
}
