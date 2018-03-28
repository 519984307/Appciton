#pragma once
#include <QMap>
#include <QList>
#include <QSemaphore>
#include "PrintTypeDefine.h"
#include "PrintLayoutIFace.h"
#include "PrintProviderIFace.h"
#include "SystemStatusBarWidget.h"
#include "PrinterActionSummary.h"

/**************************************************************************************************
 * 打印管理，打印系统的核心。
 *************************************************************************************************/
class PrintProviderIFace;
class QMutex;
class PrintManager : public QObject
{
    Q_OBJECT

public:
    static PrintManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PrintManager();
        }

        return *_selfObj;
    }
    static PrintManager *_selfObj;
    ~PrintManager();

public:
    PrintSpeed getPrintSpeed(void);
    void setPrintSpeed(PrintSpeed speed);
    void enablePrinterSpeed(PrintSpeed speed);

    int getPrintWaveNum(void);
    void setPrintWaveNum(int num);

    // 当前打印类型
    PrintType getCurPrintType() const {return _requestPrintType;}

public:
    // 添加打印机provider对象。
    void setPrintProviderIFace(PrintProviderIFace *iface);

    // 添加打印机service对象。
    void setPrintServiceProvider(PrintProviderIFace *iface);

    // 发送协议命令
    void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 修改波特率
    void sendUART(unsigned int rate);

    // 添加一个打印布局
    void addPrintLayout(PrintLayoutIFace *layout);

    // 请求打印。
    bool requestPrint(PrintType type, PrintLayoutID id, void *data = NULL, int len = 0, PrintFinishedCallback cb = NULL);

    // 是否正处于打印状态。
    bool isPrinting(void) const;

    // 立即停止所有打印。
    void abort(void);

    // 主运行入口。
    void threadRun(void);

    // 是否在申请停止。
    bool isAbort(void);

    // 获取当前打印机状态。
    PrinterStatus getPrintStatus(void);

    // 是否连接
    bool isConnected();

    // 自检
    void selftest();

public: // provider 调用。
    // 复位重启。
    void providerRestarted(void);

    // 连接断开。
    void providerDisconnected(void);

    // 连接
    void providerConnected(void);

    // 工作状态变化。
    void providerStatusChanged(PrinterStatus status);

    // 缓存状态变化。
    void providerBuffStatChanged(unsigned char stat);

    // 错误报告。
    void providerErrorReport(unsigned char error);

    // 添加打印机激活快照
    void addPrinterActiveSnapshot();

private:
    PrintManager();

    // 创建打印请求，并放到_contentList中。
    void _newPrintContent(PrintType type, PrintLayoutID id, void *data, int len, PrintFinishedCallback cb = NULL);

    // 获取打印类型
    PrintPriority _getPrintPriorty(PrintType type) const;
    void _getPrintTypeName(PrintType type, QString &name);

private:
    typedef QMap<PrintLayoutID, PrintLayoutIFace*> PrintLayoutMap;
    PrintLayoutMap _layoutMap;
    PrintLayoutIFace *_activeLayout;

    PrintType _requestPrintType;      // 请求打印类型。
    bool _requestAbort;               // 请求停止所有的打印。
    bool _requestCancel;              // 停止当前正在打印的。
    bool _isPrinting;                 // 是否正在处于打印状态。
    bool _isConected;                 // 打印机是否连接
    PrinterStatus _status;            // 打印机当前状态。
    PrintProviderIFace *_provider;    // 打印机对象。
    QSemaphore _printSemaphore;       // print semaphore

private:
    PrintContent* _content;
    QList<PrintContent*> _contentList;
    QMutex *_mutex;

};
#define printManager (PrintManager::construction())
#define deletePrintManager() (delete PrintManager::_selfObj)
