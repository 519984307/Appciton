#include "RecorderManager.h"
#include "IConfig.h"
#include "Debug.h"
#include "RecordPageProcessor.h"
#include "PrintAlarm.h"
#include <QThread>
#include "RecordPageGenerator.h"
#include <QPainter>
#include "FontManager.h"
#include <QPointer>
#include "ContinuousPageGenerator.h"
#include <QTimer>


class RecorderManagerPrivate
{
public:
    RecorderManagerPrivate()
        :connected(false),
          status(PRINTER_STAT_NORMAL),
          curSpeed(PRINT_SPEED_250),
          processor(NULL),
          iface(NULL),
          generator(NULL)
    {

    }

    bool connected;
    PrinterStatus status;
    PrintSpeed curSpeed;
    RecordPageProcessor *processor;
    QThread *procThread;
    PrintProviderIFace *iface;
    QPointer<RecordPageGenerator> generator;

};

RecorderManager &RecorderManager::getInstance()
{
    static RecorderManager *instance = NULL;
    if(instance == NULL)
    {
        instance = new RecorderManager;
    }
    return *instance;
}

RecorderManager::~RecorderManager()
{

}

PrintSpeed RecorderManager::getPrintSpeed() const
{
    int speed = 0;
    systemConfig.getNumValue("PrimaryCfg|System|PrintSpeed", speed);
    if (speed >= PRINT_SPEED_NR)
    {
        speed = PRINT_SPEED_250;
    }
    return (PrintSpeed)speed;
}

void RecorderManager::setPrintSpeed(PrintSpeed speed)
{
    if (speed >= PRINT_SPEED_NR || d_ptr->curSpeed == speed)
    {
        return;
    }

    systemConfig.setNumValue("PrimaryCfg|System|PrintSpeed", (int)speed);

    emit speedChanged(speed);
}

int RecorderManager::getPrintWaveNum()
{
    int num = 0;
    systemConfig.getNumValue("Print|PrintWaveformsNum", num);

    return num;
}

void RecorderManager::setPrintWaveNum(int num)
{
    // 3 wave at most
    if (num  > 3)
    {
        return;
    }

    systemConfig.setNumValue("Print|PrintWaveformsNum", num);
}

void RecorderManager::setPrintPrividerIFace(PrintProviderIFace *iface)
{
    if(d_ptr->iface)
    {
        qDebug()<<"PrintProviderIFace is already set.";
        return;
    }

    d_ptr->iface = iface;
    d_ptr->processor = new RecordPageProcessor(iface);
    d_ptr->processor->moveToThread(d_ptr->procThread);
    connect(d_ptr->procThread, SIGNAL(finished()), d_ptr->processor, SLOT(deleteLater()));
    connect(this, SIGNAL(speedChanged(PrintSpeed)), d_ptr->processor, SLOT(updatePrintSpeed(PrintSpeed)));

    PrinterProviderSignalSender *sigSender = iface->signalSender();
    if(sigSender)
    {
        connect(sigSender, SIGNAL(bufferFull(bool)), this, SLOT(providerBufferStatusChanged(bool)));
        connect(sigSender, SIGNAL(connectionChanged(bool)), this, SLOT(providerConnectionChanged(bool)));
        connect(sigSender, SIGNAL(restart()), this, SLOT(providerRestarted()));
        connect(sigSender, SIGNAL(statusChanged(PrinterStatus)), this, SLOT(providerStatusChanged(PrinterStatus)));
        connect(sigSender, SIGNAL(error(unsigned char)), this, SLOT(providerReportError(unsigned char)));
    }
}

bool RecorderManager::isConnected() const
{
    return d_ptr->connected;
}

bool RecorderManager::isPrinting() const
{
    return d_ptr->processor->isProcessing();
}

void RecorderManager::abort()
{
    if(d_ptr->iface)
    {
        if(d_ptr->generator)
        {
            QMetaObject::invokeMethod(d_ptr->generator, "stop");
        }
        QMetaObject::invokeMethod(d_ptr->processor, "flushPages");
    }
}

PrinterStatus RecorderManager::getPrintStatus() const
{
    return d_ptr->status;
}

void RecorderManager::selfTest()
{
    if(!d_ptr->processor)
    {
        return;
    }

    RecordPage *testPage = new RecordPage(10 * RECORDER_PIXEL_PER_MM);
    testPage->setID("test");
    QPainter painter(testPage);
    int penWidth = 5;

    QPen pen(Qt::white);
    pen.setWidth(penWidth);
    painter.setPen(pen);


    int y = testPage->height() /  2;
    painter.drawLine(QPoint(0, y), QPoint(testPage->width(), y));
    painter.setFont(fontManager.textFont(20));
    //painter.drawText(10, 40, "Hello World!!!");
    //painter.drawText(10, 70, "Hello World!!!");
    //painter.drawText(10, 100, "Hello World!!!");
    //painter.drawText(10, 130, "Hello World!!!");
    //painter.drawText(10, 160, "Hello World!!!");
    //painter.drawText(10, 190, "Hello World!!!");
    //painter.drawText(10, 220, "Hello World!!!");
    //painter.drawText(10, 250, "Hello World!!!");
    //painter.drawText(10, 280, "Hello World!!!");
    //painter.drawText(10, 310, "Hello World!!!");
    //painter.drawText(10, 340, "Hello World!!!");
    //painter.drawText(10, 370, "Hello World!!!");

    painter.fillRect(QRect(20, 0, 5, testPage->height()), Qt::white);


    QMetaObject::invokeMethod(d_ptr->processor, "addPage", Q_ARG(RecordPage*, testPage));

    QTimer::singleShot(5000, this, SLOT(testSlot()));
}

bool RecorderManager::addPageGenerator(RecordPageGenerator *generator)
{
    if(d_ptr->generator)
    {
        //not generatory currently
        d_ptr->generator = generator;
        generator->moveToThread(d_ptr->procThread);
    }
    else
    {
        //TODO: check prority
        QMetaObject::invokeMethod(d_ptr->generator.data(), "stop");
        d_ptr->generator = generator;
        generator->moveToThread(d_ptr->procThread);
    }
    connect(generator, SIGNAL(stopped()), this, SLOT(onGeneratorStopped()), Qt::QueuedConnection);
    connect(generator, SIGNAL(generatePage(RecordPage*)), d_ptr->processor, SLOT(addPage(RecordPage*)));
    connect(d_ptr->processor, SIGNAL(pageQueueFull(bool)), generator, SLOT(pageControl(bool)));
    QMetaObject::invokeMethod(generator, "start");
    return true;
}

void RecorderManager::testSlot()
{

    addPageGenerator(new ContinuousPageGenerator());
}

void RecorderManager::providerRestarted()
{
    if(d_ptr->iface)
    {
        d_ptr->iface->startSelfTest();
        d_ptr->iface->getStatusInfo();
    }
}

void RecorderManager::providerConnectionChanged(bool isConnected)
{
    if(d_ptr->connected == isConnected)
    {
        return;
    }

    d_ptr->connected = isConnected;
    if(d_ptr->connected)
    {
        //connected
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
    }
    else
    {
        //disconected
        printOneShotAlarm.clear();
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        QMetaObject::invokeMethod(d_ptr->processor, "flushPages");
    }
}

void RecorderManager::providerStatusChanged(PrinterStatus status)
{
    bool needStop = true;
    switch (status)
    {
    case PRINTER_STAT_NORMAL:            // 正常
        needStop = false;
        break;

    case PRINTER_STAT_DOOR_OPENED:       // 仓门未关闭
    case PRINTER_STAT_DOOR_OUT_OF_PAPER: // 仓门未关闭、缺纸
        // 发送"仓门为关闭"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, true);
        // 移除缺纸的报警。
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, false);

        break;
    case PRINTER_STAT_DOOR_OPEND_OVER_HEATING: // 仓门未关闭、过热（打印头温度)
    case PRINTER_STAT_DOOR_OPEND_TEMP_OVERRUN: // 仓门未关闭、过热（电机温度）
    case PRINTER_STAT_DOOR_OPENED_OVER_HEATING_TEMP_OVERRUN: // 仓门未关闭、过热(打印头温度、电机温度)
    case PRINTER_STAT_ALL:               // 仓门未关闭、缺纸、过热(打印头过热、电机过热)
        // nPM发出“打印机仓门未关”的中级技术报警和"打印机故障"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, true);
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        break;
    case PRINTER_STAT_OUT_OF_PAPER:      // 缺纸 (若仓门未关与缺纸状态同时存在，则仓门状态优先级高)
        // 发送"打印机缺纸"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, true);

        break;
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING: // 缺纸、过热（打印头温度）
    case PRINTER_STAT_OUT_OF_PAPER_TEMP_OVERRUN: // 缺纸、过热
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING_TEMP_OVERRUN: // 缺纸、过热(打印头温度、电机温度)
        // 发送"打印机缺纸"的中级技术报警和"打印机故障"中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, true);

        break;
    case PRINTER_STAT_OVER_HEATING: //过热 (打印头温度)
    case PRINTER_STAT_TEMP_OVERRUN: // 过热 (电机温度)
    case PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN: // 过热（打印头温度、电机温度）
        // todo，nPM发出"打印机故障"的中级技术报警
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        break;
    default:
        break;
    }

    if(needStop)
    {
        // stop page processing
        QMetaObject::invokeMethod(d_ptr->processor, "flushPages");
    }

    // 解除报警
    switch(d_ptr->status)
    {
    case PRINTER_STAT_NORMAL:            // 正常
        break;
    case PRINTER_STAT_DOOR_OPENED:       // 仓门未关闭
    case PRINTER_STAT_DOOR_OUT_OF_PAPER: // 仓门未关闭、缺纸
        // 仓门关闭
        if (!(status & PRINTER_STAT_DOOR_OPENED))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, false);
        }
        break;
    case PRINTER_STAT_DOOR_OPEND_OVER_HEATING: // 仓门未关闭、过热（打印头温度)
    case PRINTER_STAT_DOOR_OPEND_TEMP_OVERRUN: // 仓门未关闭、过热（电机温度）
    case PRINTER_STAT_DOOR_OPENED_OVER_HEATING_TEMP_OVERRUN: // 仓门未关闭、过热(打印头温度、电机温度)
    case PRINTER_STAT_ALL:               // 仓门未关闭、缺纸、过热(打印头过热、电机过热)
        // 仓门关闭
        if (!(status & PRINTER_STAT_DOOR_OPENED))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_DOOR_OPENED, false);
        }

        // 温度正常
        if (!(status & PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
        }
        break;
    case PRINTER_STAT_OUT_OF_PAPER:      // 缺纸 (若仓门未关与缺纸状态同时存在，则仓门状态优先级高)
        // 有纸
        if (!(status & PRINTER_STAT_OUT_OF_PAPER))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, false);
        }
        break;
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING: // 缺纸、过热（打印头温度）
    case PRINTER_STAT_OUT_OF_PAPER_TEMP_OVERRUN: // 缺纸、过热
    case PRINTER_STAT_OUT_OF_PAPER_OVER_HEATING_TEMP_OVERRUN: // 缺纸、过热(打印头温度、电机温度)
        // 有纸
        if (!(status & PRINTER_STAT_OUT_OF_PAPER))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, false);
        }

        // 温度正常
        if (!(status & PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
        }
        break;

    case PRINTER_STAT_OVER_HEATING: //过热 (打印头温度)
    case PRINTER_STAT_TEMP_OVERRUN: // 过热 (电机温度)
    case PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN: // 过热（打印头温度、电机温度）
        // 温度正常
        if (!(status & PRINTER_STAT_OVER_HEATING_TEMP_OVERRUN))
        {
            printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
        }
        break;
    default:
        break;
    }
    d_ptr->status = status;
}

void RecorderManager::providerBufferStatusChanged(bool full)
{
    QMetaObject::invokeMethod(d_ptr->processor, "pauseProcessing", Q_ARG(bool, full));
}

void RecorderManager::providerReportError(unsigned char err)
{
    qDebug()<<"Provider error: 0x"<<hex<<err;
}

void RecorderManager::onGeneratorStopped()
{
    sender()->deleteLater();
}

RecorderManager::RecorderManager()
    :QObject(), d_ptr(new RecorderManagerPrivate())
{
    qRegisterMetaType<RecordPage*>("RecordPage*");
    qRegisterMetaType<PrintSpeed>("PrintSpeed");


    d_ptr->curSpeed = getPrintSpeed();

    d_ptr->procThread = new QThread(this);

    d_ptr->procThread->start();
}
