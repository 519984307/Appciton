#pragma once
#include <QList>
#include <QThread>
#include <QMutex>
#include <QSemaphore>
#include "PrintProviderIFace.h"

class PrintPage;

/**************************************************************************************************
 * 执行打印动作，将PrintPage发往打印机。
 *************************************************************************************************/
class PrintExec
{
public:
    static PrintExec &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new PrintExec();
        }

        return *_selfObj;
    }
    static PrintExec *_selfObj;
    ~PrintExec();

public:
    // 设置Provider。
    void setPrintProviderIFace(PrintProviderIFace *iface);

    // 暂停与继续，当打印机缓存满时pause被调用，打印机缓存空后continued被调用。
    void enablePauseBuffStat(bool isPauseBuffStat);

    // 暂停与继续，
    void enablePause(bool isPause);

    // 中止打印。
    void abort(void);

    // 停止打印
    void stop(void);

    // 是否打印结束。
    bool isPrinting(void);

    // 添加打印页。
    bool addPrintPage(PrintPage *page);

    // 主入口。
    void run(void);

    // update the print speed of the cache page
    // only update the page that already has valid speed setting
    void updateCachePagePrintSpeed(PrintSpeed speed);

private:
    PrintExec();

    // 获取一列PrintPage数据。
    void _getAColumnData(PrintPage *page, int x, unsigned char *data);

    // 清空所有打印页。
    void _clearPrintPages();

private:
    PrintProviderIFace *_provider;
    bool _isPauseBuffStat; //打印机缓存满，暂停打印
    bool _isPause;
    bool _isStop;
    bool _isPrint;
    PrintSpeed _curPrintSpeed;

    QMutex _mutex;
    QSemaphore _pendingSemaphore;
    QList<PrintPage*> _printPageList;
    static const int _maxPageNR = 32;  // 最大缓存的page数。
};
#define printExec (PrintExec::construction())
#define deletePrintExec() (delete PrintExec::_selfObj)
