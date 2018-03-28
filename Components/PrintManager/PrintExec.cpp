#include <QTimer>
#include <QMutexLocker>
#include "Debug.h"
#include "IThread.h"
#include "PrintPage.h"
#include "PrintProviderIFace.h"
#include "PrintExec.h"
#include "PrintManager.h"

PrintExec *PrintExec::_selfObj = NULL;

/**************************************************************************************************
 * 获取一列PrintPage的数据。
 *************************************************************************************************/
void PrintExec::_getAColumnData(PrintPage *page, int x, unsigned char *data)
{
    int y = page->height() - 1;
    unsigned char tmp = 0;
    unsigned char bit = 0;
    int index = 0;
    int dindex = 0;

    for (; y >= 0; y--)
    {
        bit = page->pixelIndex(x, y);
        tmp |= (bit << index);
        index++;
        if (index >= 8)
        {
            index = 0;
            data[dindex++] = tmp;
            tmp = 0;
        }
    }
}

/**************************************************************************************************
 * 设置Provider。
 *************************************************************************************************/
void PrintExec::setPrintProviderIFace(PrintProviderIFace *iface)
{
    _provider = iface;
}

/**************************************************************************************************
 * 暂停与继续。
 *************************************************************************************************/
void PrintExec::enablePause(bool isPause)
{
    _isPause = isPause;
}

/**************************************************************************************************
 * 暂停与继续，当打印机缓存满时pause被调用，打印机缓存空后continued被调用。
 *************************************************************************************************/
void PrintExec::enablePauseBuffStat(bool isPauseBuffStat)
{
    _isPauseBuffStat = isPauseBuffStat;
}

/**************************************************************************************************
 * 中止打印。
 *************************************************************************************************/
void PrintExec::abort(void)
{
    _isStop = true;
}

/**************************************************************************************************
 * 停止打印。
 *************************************************************************************************/
void PrintExec::stop()
{
    if (_isStop)
    {
        return;
    }

    _isStop = true;

    if (NULL != _provider)
    {
        _provider->stop();
    }
}

/**************************************************************************************************
 * 是否打印结束
 *************************************************************************************************/
bool PrintExec::isPrinting(void)
{
    return _isPrint;
}

/**************************************************************************************************
 * 添加打印页
 *************************************************************************************************/
bool PrintExec::addPrintPage(PrintPage *page)
{
#if defined(Q_WS_X11)
    QString path("/usr/local/nPM/bin/");
    path += page->getID();
    path += ".png";
    page->save(path);
#endif
#if 0
    //QString path("/mnt/nfs/tmp/");
    QString path("/usr/local/nPM/bin/");
    path += page->getID();
    path += ".png";
    page->save(path);
#endif

    if (NULL == page)
    {
        debug("Invalid print page!!!");
        return false;
    }

    if (!_mutex.tryLock())
    {
        return false;
    }

    if (_isStop)
    {
        debug("It is stopping tprint.");
        _mutex.unlock();
        return false;
    }

    if (_printPageList.size() >= _maxPageNR)
    {
        _mutex.unlock();
        return false;
    }
    _printPageList.append(page);
    _mutex.unlock();

    _pendingSemaphore.release();

    return true;
}

/**************************************************************************************************
 * 打印线程函数
 *************************************************************************************************/
void PrintExec::run()
{
   if(!_pendingSemaphore.tryAcquire(1, 1000))  //wait 1s at most
   {
       //no pending print pages
        _isPrint = false;
        _curPrintSpeed = printManager.getPrintSpeed();
        if (_isStop)
        {
            _clearPrintPages();
            _isStop = false;
        }
       return;
   }

    _mutex.lock();
    if (_printPageList.isEmpty())
    {
        _isStop = false;
        _isPrint = false;
        _curPrintSpeed = printManager.getPrintSpeed();
        _pendingSemaphore.acquire(_pendingSemaphore.available());
        _mutex.unlock();
        return;
    }

    PrintPage *page = _printPageList.first();
    _printPageList.removeFirst();
    _mutex.unlock();

#if 0  //don't update print speed if speed changed while printing
    if(page->getPrintSpeed() < PRINT_SPEED_NR && page->getPrintSpeed() != _curPrintSpeed)
    {
        //speed has changed while printing, need to update print speed
        //wait long enough until the cached data print
        _provider->flush();
        IThread::msleep(1500);
        printManager.enablePrinterSpeed(page->getPrintSpeed());
        _curPrintSpeed = page->getPrintSpeed();
    }
#endif

    _isPrint = true;
    int dataLen = page->height() / 8;
    unsigned char data[dataLen];
    for (int x = 0; x < page->width(); x++)
    {
        _getAColumnData(page, x, data);
        while (_isPauseBuffStat || _isPause)  // 处于暂停状态。
        {
            IThread::msleep(15);
            if (_isStop)
            {
                break;
            }
        }

        if (_isStop)
        {
            break;
        }

        if(!_provider->sendBitmapData(data, dataLen))
        {
            //send failed, uart buffer might be full
            qdebug("send bitmap data failed, send again after 20 ms.");
            IThread::msleep(20);
            //send again
            _provider->sendBitmapData(data, dataLen);
        }
        IThread::msleep(2);  // 50mm/s时需要2.5ms发送一列数据到打印机。
    }

    delete page;

    if (_isStop)
    {
        _clearPrintPages();
        _isStop = false;
    }

    //check print complete
    _mutex.lock();
    if (_printPageList.isEmpty())
    {
        _isStop = false;
        _isPrint = false;
        _curPrintSpeed = printManager.getPrintSpeed();
        _pendingSemaphore.acquire(_pendingSemaphore.available());
    }
    _mutex.unlock();
}

/***************************************************************************************************
 * update the print speed of the cache page
 * only update the page that already has valid speed setting
 **************************************************************************************************/
void PrintExec::updateCachePagePrintSpeed(PrintSpeed speed)
{
    _mutex.lock();
    foreach (PrintPage *page, _printPageList) {
        if(page->getPrintSpeed() < PRINT_SPEED_NR)
        {
            page->setPrintSpeed(speed);
        }
    }
    _mutex.unlock();
}

/**************************************************************************************************
 * 清空所有打印页
 *************************************************************************************************/
void PrintExec::_clearPrintPages()
{
    _mutex.lock();
    if (_printPageList.isEmpty())
    {
        _mutex.unlock();
        return;
    }

    foreach (PrintPage *page, _printPageList)
    {
        if (NULL == page)
        {
            continue;
        }

        delete page;
    }

    _printPageList.clear();
    _mutex.unlock();
}

/**************************************************************************************************
 * 构造函数
 *************************************************************************************************/
PrintExec::PrintExec()
{
    _provider = NULL;
    _isPauseBuffStat = false;
    _isPause = false;
    _isStop = false;
    _isPrint = false;
}

/**************************************************************************************************
 * 析构函数
 *************************************************************************************************/
PrintExec::~PrintExec()
{
    _clearPrintPages();
}
