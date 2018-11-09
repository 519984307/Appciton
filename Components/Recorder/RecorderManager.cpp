/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/7
 **/

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
#include "FontManager.h"
#include "EventStorageManager.h"
#include "TimeManager.h"

class RecorderManagerPrivate
{
public:
    RecorderManagerPrivate()
        : connected(false),
          isAborted(false),
          status(PRINTER_STAT_NORMAL),
          curSpeed(PRINT_SPEED_250),
          processor(NULL),
          procThread(NULL),
          iface(NULL),
          generator(NULL),
          timeSec(PRINT_TIME_CONTINOUS)
    {
    }

    bool connected;
    bool isAborted;
    PrinterStatus status;
    PrintSpeed curSpeed;
    RecordPageProcessor *processor;
    QThread *procThread;
    PrintProviderIFace *iface;
    QPointer<RecordPageGenerator> generator;
    PrintTime timeSec;
};

RecorderManager &RecorderManager::getInstance()
{
    static RecorderManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new RecorderManager;
    }
    return *instance;
}

RecorderManager::~RecorderManager()
{
    if (d_ptr->procThread)
    {
        d_ptr->procThread->quit();
        qDebug() << "recorder thread exit wait";
        d_ptr->procThread->wait();
        qDebug() << "recorder thread exit";
    }
}

PrintSpeed RecorderManager::getPrintSpeed() const
{
    int speed = 0;
    currentConfig.getNumValue("Print|PrintSpeed", speed);
    if (speed >= PRINT_SPEED_NR)
    {
        speed = PRINT_SPEED_250;
    }
    // speed = PRINT_SPEED_500;
//    speed = PRINT_SPEED_250;
    // speed = PRINT_SPEED_125;
    return (PrintSpeed)speed;
}

void RecorderManager::setPrintSpeed(PrintSpeed speed)
{
    if (speed >= PRINT_SPEED_NR || d_ptr->curSpeed == speed)
    {
        return;
    }

    d_ptr->curSpeed = speed;
    currentConfig.setNumValue("Print|PrintSpeed", static_cast<int>(speed));

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
    if (num  > 4)
    {
        return;
    }

    systemConfig.setNumValue("Print|PrintWaveformsNum", num);
}

void RecorderManager::setPrintPrividerIFace(PrintProviderIFace *iface)
{
    if (d_ptr->iface)
    {
        qDebug() << "PrintProviderIFace is already set.";
        return;
    }

    d_ptr->iface = iface;
    d_ptr->iface->getStatusInfo();
    d_ptr->processor = new RecordPageProcessor(iface);
    d_ptr->processor->moveToThread(d_ptr->procThread);
    connect(d_ptr->procThread, SIGNAL(finished()), d_ptr->processor, SLOT(deleteLater()));
    connect(this, SIGNAL(speedChanged(PrintSpeed)), d_ptr->processor, SLOT(updatePrintSpeed(PrintSpeed)));

    PrinterProviderSignalSender *sigSender = iface->signalSender();
    if (sigSender)
    {
        connect(sigSender, SIGNAL(bufferFull(bool)), this, SLOT(providerBufferStatusChanged(bool)));
        connect(sigSender, SIGNAL(connectionChanged(bool)), this, SLOT(providerConnectionChanged(bool)));
        connect(sigSender, SIGNAL(restart()), this, SLOT(providerRestarted()));
        connect(sigSender, SIGNAL(statusChanged(PrinterStatus)), this, SLOT(providerStatusChanged(PrinterStatus)));
        connect(sigSender, SIGNAL(error(unsigned char)), this, SLOT(providerReportError(unsigned char)));
    }

    PrintSpeed speed = getPrintSpeed();
    QMetaObject::invokeMethod(d_ptr->processor, "updatePrintSpeed", Q_ARG(PrintSpeed, speed));
}

PrintProviderIFace *RecorderManager::provider() const
{
    return d_ptr->iface;
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
    if (d_ptr->iface)
    {
        if (d_ptr->generator)
        {
            QMetaObject::invokeMethod(d_ptr->generator, "stop");
        }
        QMetaObject::invokeMethod(d_ptr->processor, "stopProcess");
        d_ptr->isAborted = true;
    }
}

bool RecorderManager::isAbort() const
{
    return d_ptr->isAborted;
}

PrinterStatus RecorderManager::getPrintStatus() const
{
    return d_ptr->status;
}

void RecorderManager::selfTest()
{
    if (!d_ptr->processor)
    {
        return;
    }

    RecordPage *testPage = new RecordPage(10 * RECORDER_PIXEL_PER_MM);
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

    QMetaObject::invokeMethod(d_ptr->processor, "addPage", Q_ARG(RecordPage *, testPage));

    QTimer::singleShot(5000, this, SLOT(testSlot()));
}

bool RecorderManager::addPageGenerator(RecordPageGenerator *generator)
{
    bool startImmediately = false;
    if (!d_ptr->generator && !d_ptr->status)
    {
        // no generator currently
        d_ptr->generator = generator;
        generator->moveToThread(d_ptr->procThread);
        startImmediately = true;
    }
    else
    {

        if (generator->getPriority() == RecordPageGenerator::PriorityContinuous)
        {
            // don't add the generator
            generator->deleteLater();

            // stop current generator
            QMetaObject::invokeMethod(d_ptr->generator.data(), "stop");
            // stop page processor
            QMetaObject::invokeMethod(d_ptr->processor, "stopProcess");

            return false;
        }
        else if (generator->getPriority() <= d_ptr->generator->getPriority())
        {
            // the priority is lower or equal to current generator
            // don't add the generator
            generator->deleteLater();
            return false;
        }
        else
        {
            // stop current generator
            QMetaObject::invokeMethod(d_ptr->generator.data(), "stop");
            // stop page processor
            QMetaObject::invokeMethod(d_ptr->processor, "stopProcess");
            d_ptr->generator = generator;
            d_ptr->generator->setPrintTime(d_ptr->timeSec);
            generator->moveToThread(d_ptr->procThread);
        }
    }

    connect(generator, SIGNAL(stopped()), this, SLOT(onGeneratorStopped()), Qt::QueuedConnection);
    connect(generator, SIGNAL(generatePage(RecordPage *)), d_ptr->processor, SLOT(addPage(RecordPage *)),
            Qt::QueuedConnection);
    connect(d_ptr->processor, SIGNAL(pageQueueFull(bool)), generator, SLOT(pageControl(bool)), Qt::QueuedConnection);
    if (startImmediately)
    {
        QMetaObject::invokeMethod(generator, "start");
    }

    if (generator->getPriority() == RecordPageGenerator::PriorityContinuous)
    {
        // 触发实时打印
        unsigned currentTime = timeManager.getCurTime();
        eventStorageManager.triggerRealtimePrintEvent(currentTime);
    }
    return true;
}

void RecorderManager::setPrintTime(PrintTime timeSec)
{
    if (timeSec == PRINT_TIME_NR)
    {
        return;
    }
    d_ptr->timeSec = timeSec;
    systemConfig.setNumValue("Print|PrintTime", static_cast<int>(d_ptr->timeSec));
}

PrintTime RecorderManager::getPrintTime() const
{
    return d_ptr->timeSec;
}

void RecorderManager::testSlot()
{
    // addPageGenerator(new ContinuousPageGenerator());
    // addPageGenerator(new RecordPageGenerator());
}

void RecorderManager::providerRestarted()
{
    if (d_ptr->iface)
    {
        d_ptr->iface->startSelfTest();
        d_ptr->iface->getStatusInfo();
    }
}

void RecorderManager::providerConnectionChanged(bool isConnected)
{
    if (d_ptr->connected == isConnected)
    {
        return;
    }

    d_ptr->connected = isConnected;
    if (d_ptr->connected)
    {
        // connected
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, false);
    }
    else
    {
        // disconected
        printOneShotAlarm.clear();
        printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, true);

        QMetaObject::invokeMethod(d_ptr->processor, "stopProcess");
    }
}

void RecorderManager::providerStatusChanged(PrinterStatus status)
{
    bool isOutOfPaper = status & 0x01;
    bool isOverHeating = (status >> 1) & 0x01;
    bool isPrinterFault = (status >> 2) & 0x01;

    printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, isOutOfPaper);
    printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_OVER_HEATING, isOverHeating);
    printOneShotAlarm.setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, isPrinterFault);

    if (status)
    {
        // stop current generator
        QMetaObject::invokeMethod(d_ptr->generator.data(), "stop");
        // stop page processor
        QMetaObject::invokeMethod(d_ptr->processor, "stopProcess");
    }
    d_ptr->status = status;
}

void RecorderManager::providerBufferStatusChanged(bool full)
{
    QMetaObject::invokeMethod(d_ptr->processor, "pauseProcessing", Q_ARG(bool, full));
}

void RecorderManager::providerReportError(unsigned char err)
{
    qDebug() << "Provider error: 0x" << hex << err;
}

void RecorderManager::onGeneratorStopped()
{
    sender()->deleteLater();
    if (d_ptr->generator && d_ptr->generator.data() != sender())
    {
        QMetaObject::invokeMethod(d_ptr->generator.data(), "start");
    }
    d_ptr->isAborted = false;
}

RecorderManager::RecorderManager()
    : QObject(), d_ptr(new RecorderManagerPrivate())
{
    qRegisterMetaType<RecordPage *>("RecordPage*");
    qRegisterMetaType<PrintSpeed>("PrintSpeed");

    // 获取打印时间
    int index = 0;
    systemConfig.getNumValue("Print|PrintTime", index);
    d_ptr->timeSec = static_cast<PrintTime>(index);

    d_ptr->curSpeed = getPrintSpeed();

    d_ptr->procThread = new QThread(this);
    d_ptr->procThread->setObjectName("Record");

    d_ptr->procThread->start();
}
