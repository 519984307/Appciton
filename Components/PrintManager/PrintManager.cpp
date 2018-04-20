#include <QMutex>
#include "Debug.h"
#include "IThread.h"
#include "IConfig.h"
#include "PrintManager.h"
#include "PrintExec.h"
#include "PrintAlarm.h"
#include "WindowManager.h"
#include "TimeManager.h"
#include "SummaryStorageManager.h"
#include "SystemManager.h"
#include "ErrorLog.h"
#include "ErrorLogItem.h"
#include "ECGParam.h"

PrintManager *PrintManager::_selfObj = NULL;

/**************************************************************************************************
 * 创建打印请求，并放到_contentList中。
 *************************************************************************************************/
void PrintManager::_newPrintContent(PrintType type, PrintLayoutID id, void *data, int len, PrintFinishedCallback cb)
{
    unsigned char *ptr = NULL;
    if ((data != NULL) && (len >= 0))
    {
        ptr = new unsigned char[len];
        if (ptr == NULL)
        {
            debug("Allocate memory failed!!!");
            return;
        }

        qMemCopy(ptr, (char*)data, len);
    }

    _mutex->lock();

    _requestAbort = true;
    while (!_contentList.isEmpty())
    {
        PrintContent *content = _contentList.takeFirst();
        content->finished(PrintCancel);
        delete content;
    }
    _contentList.append(new PrintContent(type, id, ptr, len, cb));
    _mutex->unlock();
}

/**************************************************************************************************
 * 获取打印类型。
 *************************************************************************************************/
PrintPriority PrintManager::_getPrintPriorty(PrintType type) const
{
    switch (type)
    {
        case PRINT_TYPE_CONTINUOUS:
            return PRINT_PRIORITY_LEVEL_1;

        case PRINT_TYPE_REVIEW:
            return PRINT_PRIORITY_LEVEL_2;

        case PRINT_TYPE_TRIGGER_CODE_MARKER:
        case PRINT_TYPE_TRIGGER_NIBP:
        case PRINT_TYPE_TRIGGER_ECG_RHYTHM:
        case PRINT_TYPE_TRIGGER_PACER_STARTUP:
            return PRINT_PRIORITY_LEVEL_3;

        case PRINT_TYPE_TRIGGER_DIAG_ECG:
        case PRINT_TYPE_TRIGGER_12L_ECG:
        case PRINT_TYPE_TRIGGER_CHECK_PATIENT:
        case PRINT_TYPE_TRIGGER_FOREGROUND_ECG_ANALYSIS:
        case PRINT_TYPE_TRIGGER_PHY_ALARM:
        case PRINT_TYPE_TRIGGER_SHOCK_DELIVERY:
        case PRINT_TYPE_TRIGGER_30J_SELFTEST:
            return PRINT_PRIORITY_LEVEL_4;

        default:
            return PRINT_PRIORITY_LEVEL_NONE;
    }
}

/**************************************************************************************************
 * 获取打印类型名称。
 *************************************************************************************************/
void PrintManager::_getPrintTypeName(PrintType type, QString &name)
{
    switch (type)
    {
        case PRINT_TYPE_CONTINUOUS:
            name = "Real-Time Print";
            break;

        case PRINT_TYPE_REVIEW:
            name = "Data Review Print";
            break;

        case PRINT_TYPE_TRIGGER_CODE_MARKER:
            name = "Code Marker Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_NIBP:
            name = "NIBP Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_ECG_RHYTHM:
            name = "ECG Rhythm Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_PACER_STARTUP:
            name = "Pacer Startup Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_30J_SELFTEST:
            name = "30J Selftest Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_DIAG_ECG:
            name = "Diagnose ECG Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_12L_ECG:
            name = "12Lead ECG Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_CHECK_PATIENT:
            name = "Check Patient Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_FOREGROUND_ECG_ANALYSIS:
            name = "Foreground ECG Analysis Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_PHY_ALARM:
            name = "Physiology Alarm Trigger Print";
            break;

        case PRINT_TYPE_TRIGGER_SHOCK_DELIVERY:
            name = "Shock Delivery Trigger Print";
            break;

        default:
            name = "Request Print Type is Invalid";
            break;
    }
}

/**************************************************************************************************
 * 获取当前打印机状态。
 *************************************************************************************************/
PrinterStatus PrintManager::getPrintStatus(void)
{
    return _status;
}

/**************************************************************************************************
 * 是否连接。
 *************************************************************************************************/
bool PrintManager::isConnected()
{
    return _isConected;
}

/**************************************************************************************************
 * 自检。
 *************************************************************************************************/
void PrintManager::selftest()
{
    PrintPage *testPage = new PrintPage(10 * PIXELS_PER_MM);
    testPage->setID("test");
    QPainter painter(testPage);
    int penWidth = 2;

    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setWidth(penWidth);
    pen.setDashPattern(darsh);
    painter.setPen(pen);

    int x = testPage->width() - penWidth;
    painter.drawLine(QPoint(penWidth, 0), QPoint(penWidth, testPage->height() - 1));
    painter.drawLine(QPoint(x, 0), QPoint(x, testPage->height() - 1));
    printExec.addPrintPage(testPage);
}

/**************************************************************************************************
 * 添加打印机provider对象。
 * 参数:
 *      iface: provider对象。
 *************************************************************************************************/
void PrintManager::setPrintProviderIFace(PrintProviderIFace *iface)
{
    _provider = iface;
    printExec.setPrintProviderIFace(iface);
    iface->setPrintSpeed(getPrintSpeed());
    iface->startSelfTest();
    iface->getStatusInfo();
    //    iface->runPaper(20);// 暂时注释掉，免得浪费纸。
}

/**************************************************************************************************
 * 设置服务模式升级数据提供对象。
 *************************************************************************************************/
void PrintManager::setPrintServiceProvider(PrintProviderIFace *iface)
{
    _provider = iface;
}

/**************************************************************************************************
 * 发送协议命令。
 *************************************************************************************************/
void PrintManager::sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    if (NULL != _provider)
    {
        _provider->sendCmdData(cmdId,data,len);
    }
}

void PrintManager::sendUART(unsigned int rate)
{
    if (NULL != _provider)
    {
        _provider->sendUART(rate);
    }
}

/**************************************************************************************************
 * 添加一个打印布局对象
 * 参数:
 *      layout: 打印布局对象指针
 *************************************************************************************************/
void PrintManager::addPrintLayout(PrintLayoutIFace *layout)
{
    if (NULL == layout)
    {
        debug("Invalid print layout!");
        return;
    }

    _layoutMap.insert(layout->getLayoutID(), layout);
}

/**************************************************************************************************
 * 请求打印，添加打印内容
 * 参数:
 *      type: 打印类型；
 *      id: 布局器的ID；
 *      data: 打印的数据，可以为NULL；
 *      len:  数据内容的长度。
 *************************************************************************************************/
bool PrintManager::requestPrint(PrintType type, PrintLayoutID id, void *data, int len, PrintFinishedCallback cb)
{
    // 打印机非正常状态
    if (_status)
    {
        qdebug("Print abnormal condition (status = %d)!\n", _status);
        return false;
    }

#if 1
    if (!_isConected)
    {
        qdebug("Printer Not Connected!\n");
        return false;
    }
#endif

    if (type == PRINT_TYPE_NONE)
    {
        qdebug("Invalid request for printing.\n");
        return false;
    }

    PrintPriority priority = _getPrintPriorty(type);
    PrintPriority reqPriority = _getPrintPriorty(_requestPrintType);// _requestPrintType多线程访问。
    if (reqPriority > priority)
    {
        if (PRINT_TYPE_CONTINUOUS == type)
        {
            abort();
            _requestPrintType = PRINT_TYPE_NONE;
        }
        else
        {
            qdebug("High Print Priority Event is Printing\n");
            ErrorLogItem *item = new ErrorLogItem();
            item->setName(QString("Print Request Discarded"));

            QString curTypeName, requestTypeName;
            _getPrintTypeName(_requestPrintType, curTypeName);
            _getPrintTypeName(type, requestTypeName);
            item->setLog(QString("Current Print Type:%1\r\nRequest Print Type:%2\r\n").
                         arg(curTypeName).arg(requestTypeName));
            item->setSubSystem(ErrorLogItem::SUB_SYS_PRINTER);
            item->setSystemResponse(ErrorLogItem::SYS_RSP_REPORT);
            item->setSystemState(ErrorLogItem::SYS_STAT_RUNTIME);
            errorLog.append(item);
            return false;
        }
    }
    else if (reqPriority == priority)
    {
        if (PRINT_TYPE_CONTINUOUS != type)
        {
            _newPrintContent(type, id, data, len, cb);
            _requestPrintType = type;

            //肆放打印信号
            _printSemaphore.release();
        }
        else
        {
            abort();
            _requestPrintType = PRINT_TYPE_NONE;
        }
    }
    else
    {
        _newPrintContent(type, id, data, len, cb);
        _requestPrintType = type;

        //肆放打印信号
        _printSemaphore.release();
    }

    return true;
}

/**************************************************************************************************
 * 是否正处于打印状态。
 *************************************************************************************************/
bool PrintManager::isPrinting(void) const
{
    return _isPrinting;
}

/**************************************************************************************************
 * 停止打印。
 *************************************************************************************************/
void PrintManager::abort(void)
{
    _mutex->lock();
    _requestAbort = true;

    while (!_contentList.isEmpty())
    {
        PrintContent *content = _contentList.takeFirst();
        content->finished(PrintCancel);
        delete content;
    }
    _mutex->unlock();
}

/**************************************************************************************************
 * 是否在申请停止。
 *************************************************************************************************/
bool PrintManager::isAbort(void)
{
    return _requestAbort;
}

/**************************************************************************************************
 * 打印布局执行线程
 *************************************************************************************************/
void PrintManager::threadRun(void)
{
    if (!_printSemaphore.tryAcquire(1, 1000))
    {
        //没有获取到信号量的话直接返回
        return;
    }

    if (NULL == _content)
    {
        _mutex->lock();
        if (_contentList.isEmpty())
        {
            _isPrinting = false;
            _requestPrintType = PRINT_TYPE_NONE;
            _mutex->unlock();
            return;
        }
        else
        {
            //取一个打印请求
            _requestAbort = false;
            _requestCancel = false;
            _content = _contentList.takeFirst();
        }
        _mutex->unlock();
    }

    // Find print layout
    PrintLayoutIFace *layout = _layoutMap.value(_content->layoutID, NULL);
    if (NULL == layout)
    {
        delete _content;
        _content->finished(PrintCancel);
        _content = NULL;
        return;
    }

    _activeLayout = layout;

    // 标记当前正在打印。
    _isPrinting = true;

    // 开始进行打印，直到打印完成后才返回。
    layout->start(); // 开始布局。
    PrintPage *page = layout->exec(_content);  // 获取第一个页。
    bool needBreak = false;
    while (page != NULL)
    {
        if (_requestAbort || _requestCancel)
        {
            layout->abort();  // 中止布局，释放资源
            break;
        }

        // 交给printExec处理，printExec处理完后会删除page对象。
        while (!printExec.addPrintPage(page))
        {
            if (_requestAbort || _requestCancel)
            {
                layout->abort(); //中止布局，释放资源
                needBreak =true; //break the outer loop
                break;
            }

            IThread::msleep(1);
        }

        if(needBreak)
        {
            break;
        }

        page = layout->exec(_content);
        if (NULL == page)
        {
            break;
        }

        IThread::msleep(1);
    }

    if (page == NULL)
    {
        layout->abort();//布局结束，释放资源
        _content->finished(PrintSuccess);
    }
    else
    {
        _content->finished(PrintAbort);
    }

    // 移除该打印请求。
    delete _content;
    _content = NULL;
    _activeLayout = NULL;

    // 等待打印真正结束。
    while (printExec.isPrinting())
    {
        if (_requestAbort || _requestCancel)
        {
            printExec.stop();
        }

        IThread::msleep(1);
    }

    if(_status == PRINTER_STAT_NORMAL)
    {
        //打印完继续走纸3cm
        PrintPage *blankPage = new PrintPage(30 * PIXELS_PER_MM);
        while (!printExec.addPrintPage(blankPage))
        {
            IThread::msleep(1);
        }
    }

    // 在这里清除该标记能确保打印真正停止了。
    _requestAbort = false;
    _requestCancel = false;

    //clear flag
    _mutex->lock();
    if (_contentList.isEmpty())
    {
        _isPrinting = false;
        _requestPrintType = PRINT_TYPE_NONE;
    }
    _mutex->unlock();
}

/**************************************************************************************************
 * 复位重启。
 *************************************************************************************************/
void PrintManager::providerRestarted(void)
{
    if (NULL != _provider)
    {
        _provider->startSelfTest();
        _provider->getStatusInfo();
    }
}

/**************************************************************************************************
 * 连接断开。
 *************************************************************************************************/
void PrintManager::providerDisconnected(void)
{
    if (_isConected)
    {
        _isConected = false;
        printOneShotAlarm.clear();
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        // 是否处于打印状态。
        if (_isPrinting)
        {
            abort();
        }
    }
}

/**************************************************************************************************
 * 连接。
 *************************************************************************************************/
void PrintManager::providerConnected(void)
{
    if (!_isConected)
    {
        _isConected = true;
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
    }
}

/**************************************************************************************************
 * 工作状态变化。
 *************************************************************************************************/
void PrintManager::providerStatusChanged(PrinterStatus status)
{
    switch (status)
    {
    case PRINTER_STAT_NORMAL:            // 正常
        break;

    case PRINTER_STAT_DOOR_OPENED:       // 仓门未关闭
    case PRINTER_STAT_DOOR_OUT_OF_PAPER: // 仓门未关闭、缺纸
        // 发送"仓门为关闭"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, true);
        // 移除缺纸的报警。
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, false);

        // 是否处于打印状态。
        if (_isPrinting)
        {
            abort();
        }
        break;
    case PRINTER_STAT_DOOR_OPEND_OVER_HEATING: // 仓门未关闭、过热（打印头温度)
    case PRINTER_STAT_DOOR_OPEND_TEMP_OVERRUN: // 仓门未关闭、过热（电机温度）
    case PRINTER_STAT_DOOR_OPENED_OVER_HEATING_TEMP_OVERRUN: // 仓门未关闭、过热(打印头温度、电机温度)
    case PRINTER_STAT_ALL:               // 仓门未关闭、缺纸、过热(打印头过热、电机过热)
        // nPM发出“打印机仓门未关”的中级技术报警和"打印机故障"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, true);
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        // 是否处于打印状态。
        if (_isPrinting)
        {
            abort();
        }
        break;
    case PRINTER_STAT_OUT_OF_PAPER:      // 缺纸 (若仓门未关与缺纸状态同时存在，则仓门状态优先级高)
        // 发送"打印机缺纸"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, true);

        // 是否处于打印状态。
        if (_isPrinting)
        {
            abort();
        } 
        break;
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING: // 缺纸、过热（打印头温度）
    case PRINTER_STAT_OUT_OF_PAPER_TEMP_OVERRUN: // 缺纸、过热
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING_TEMP_OVERRUN: // 缺纸、过热(打印头温度、电机温度)
        // 发送"打印机缺纸"的中级技术报警和"打印机故障"中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, true);

        // 是否处于打印状态。
        if (_isPrinting)
        {
            abort();
        }
        break;
    case PRINTER_STAT_OVER_HEATING: //过热 (打印头温度)
    case PRINTER_STAT_TEMP_OVERRUN: // 过热 (电机温度)
    case PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN: // 过热（打印头温度、电机温度）
        // todo，nPM发出"打印机故障"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        // 是否处于打印状态。
        if (_isPrinting)
        {
            abort();
        }
        break;
    default:
        break;
    }

    // 解除报警
    switch(_status)
    {
    case PRINTER_STAT_NORMAL:            // 正常
        break;
    case PRINTER_STAT_DOOR_OPENED:       // 仓门未关闭
    case PRINTER_STAT_DOOR_OUT_OF_PAPER: // 仓门未关闭、缺纸
        // 仓门关闭
        if (!(status & 0x01))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, false);
        }
        break;
    case PRINTER_STAT_DOOR_OPEND_OVER_HEATING: // 仓门未关闭、过热（打印头温度)
    case PRINTER_STAT_DOOR_OPEND_TEMP_OVERRUN: // 仓门未关闭、过热（电机温度）
    case PRINTER_STAT_DOOR_OPENED_OVER_HEATING_TEMP_OVERRUN: // 仓门未关闭、过热(打印头温度、电机温度)
    case PRINTER_STAT_ALL:               // 仓门未关闭、缺纸、过热(打印头过热、电机过热)
        // 仓门关闭
        if (!(status & 0x01))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, false);
        }

        // 温度正常
        if (!(status & 0x0C))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
        }
        break;
    case PRINTER_STAT_OUT_OF_PAPER:      // 缺纸 (若仓门未关与缺纸状态同时存在，则仓门状态优先级高)
        // 有纸
        if (!(status & 0x02))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, false);
        }
        break;
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING: // 缺纸、过热（打印头温度）
    case PRINTER_STAT_OUT_OF_PAPER_TEMP_OVERRUN: // 缺纸、过热
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING_TEMP_OVERRUN: // 缺纸、过热(打印头温度、电机温度)
        // 有纸
        if (!(status & 0x02))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, false);
        }

        // 温度正常
        if (!(status & 0x0C))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
        }
        break;

    case PRINTER_STAT_OVER_HEATING: //过热 (打印头温度)
    case PRINTER_STAT_TEMP_OVERRUN: // 过热 (电机温度)
    case PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN: // 过热（打印头温度、电机温度）
        // 温度正常
        if (!(status & 0x0C))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
        }
        break;
    default:
        break;
    }
    _status = status;
}

/**************************************************************************************************
 * 缓存状态变化。
 *************************************************************************************************/
void PrintManager::providerBuffStatChanged(unsigned char status)
{
    if (status == 1)
    {
        printExec.enablePauseBuffStat(true);
    }
    else if (status == 0)
    {
        printExec.enablePauseBuffStat(false);
    }
}

/**************************************************************************************************
 * 错误报告。
 *************************************************************************************************/
void PrintManager::providerErrorReport(unsigned char error)
{
    error = error;
}

/**************************************************************************************************
 * 添加打印机激活快照。
 *************************************************************************************************/
void PrintManager::addPrinterActiveSnapshot()
{
    //summaryStorageManager.addPrinterAction(timeManager.getCurTime());
}

/**************************************************************************************************
 * 获取速度。
 *************************************************************************************************/
PrintSpeed PrintManager::getPrintSpeed(void)
{
    int speed = 0;
    systemConfig.getNumValue("PrimaryCfg|System|PrintSpeed", speed);
    if (speed >= PRINT_SPEED_NR)
    {
        speed = PRINT_SPEED_250;
    }

    return (PrintSpeed)speed;
}

/**************************************************************************************************
 * 设置速度。
 *************************************************************************************************/
void PrintManager::setPrintSpeed(PrintSpeed speed)
{
    if (speed >= PRINT_SPEED_NR)
    {
        return;
    }

    systemConfig.setNumValue("PrimaryCfg|System|PrintSpeed", (int)speed);
    if(printExec.isPrinting())
    {
        printExec.updateCachePagePrintSpeed(speed);
    }
}

/**************************************************************************************************
 * 生效打印速度。
 *************************************************************************************************/
void PrintManager::enablePrinterSpeed(PrintSpeed speed)
{
    if (speed >= PRINT_SPEED_NR)
    {
        return;
    }

    if (NULL != _provider)
    {
        _provider->setPrintSpeed(speed);
    }
}

/**************************************************************************************************
 * 设置打印波形数。
 *************************************************************************************************/
void PrintManager::setPrintWaveNum(int num)
{
    if (num  > 4)
    {
        return;
    }

    systemConfig.setNumValue("Print|PrintWaveformsNum", num);
}

/**************************************************************************************************
 * 获取打印波形数。
 *************************************************************************************************/
int PrintManager::getPrintWaveNum(void)
{
    int num = 0;
    systemConfig.getNumValue("Print|PrintWaveformsNum", num);

    return num;
}

/**************************************************************************************************
 * 构造函数。
 *************************************************************************************************/
PrintManager::PrintManager()
{
    _requestPrintType = PRINT_TYPE_NONE;
    _requestAbort = false;
    _requestCancel = false;
    _isPrinting = false;
    _isConected = false;
    _provider = NULL;
    _status = PRINTER_STAT_NORMAL;
    _content = NULL;
    _contentList.clear();
    _activeLayout = NULL;

    _mutex = new QMutex();
}

/**************************************************************************************************
 * 析构函数。
 *************************************************************************************************/
PrintManager::~PrintManager()
{
    // 清空打印内容
    if (NULL != _content)
    {
        _content->finished(PrintCancel);
        delete _content;
        _content = NULL;
    }

    while (!_contentList.isEmpty())
    {
        PrintContent *content = _contentList.takeFirst();
        content->finished(PrintCancel);
        delete content;
    }

    // 释放打印布局对象内存
    foreach (PrintLayoutIFace *layout, _layoutMap.values())
    {
        if (NULL == layout)
        {
            continue;
        }

        delete layout;
    }

    if (NULL != _mutex)
    {
        delete _mutex;
        _mutex = NULL;
    }

    _layoutMap.clear();
}
