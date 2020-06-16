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
#include "ConfigManager.h"
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
#include "ECGParam.h"
#include "IBPParam.h"
#include "PrintSettingMenuContent.h"
#include "CO2Param.h"
#include "AlarmSourceManager.h"

class RecorderManagerPrivate
{
public:
    RecorderManagerPrivate()
        : connected(false),
          isAborted(false),
          isPrinting(false),
          status(PRINTER_STAT_NORMAL),
          curSpeed(PRINT_SPEED_250),
          processor(NULL),
          procThread(NULL),
          iface(NULL),
          generator(NULL),
          timeSec(PRINT_TIME_CONTINOUS),
          curPrintPriority(RecordPageGenerator::PriorityNone),
          updatePrintSpeed(false)
    {
    }

    bool connected;
    bool isAborted;
    bool isPrinting;
    PrinterStatus status;
    PrintSpeed curSpeed;
    RecordPageProcessor *processor;
    QThread *procThread;
    PrintProviderIFace *iface;
    QPointer<RecordPageGenerator> generator;
    PrintTime timeSec;
    RecordPageGenerator::PrintPriority curPrintPriority;  // current printing page priority
    bool updatePrintSpeed;
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
    systemConfig.getNumValue("Print|PrintSpeed", speed);
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
    systemConfig.setNumValue("Print|PrintSpeed", static_cast<int>(speed));

    // 是否正在打印
    if (isPrinting())
    {
        d_ptr->updatePrintSpeed = true;
    }
    else
    {
        QMetaObject::invokeMethod(d_ptr->processor, "setPrintSpeed", Qt::QueuedConnection, Q_ARG(PrintSpeed, speed));
    }
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
    QMetaObject::invokeMethod(d_ptr->processor, "setPrintSpeed", Qt::QueuedConnection, Q_ARG(PrintSpeed, speed));
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
    return d_ptr->isPrinting;
}

void RecorderManager::setPrintStatus(bool sta)
{
    // 打印停止后更新打印速度
    if (d_ptr->updatePrintSpeed && (sta == false))
    {
        d_ptr->updatePrintSpeed = false;
        QMetaObject::invokeMethod(d_ptr->processor, "setPrintSpeed", Qt::QueuedConnection,
                                  Q_ARG(PrintSpeed, d_ptr->curSpeed));
    }
    d_ptr->isPrinting = sta;
}

void RecorderManager::abort()
{
    if (d_ptr->iface)
    {
        if (d_ptr->generator)
        {
            QMetaObject::invokeMethod(d_ptr->generator, "stop", Qt::QueuedConnection);
        }
        QMetaObject::invokeMethod(d_ptr->processor, "stopProcess", Qt::QueuedConnection);
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

    RecordPage *testPage = new RecordPage(20 * RECORDER_PIXEL_PER_MM);
    testPage->setID("test");

    QPainter painter(testPage);
    int penWidth = 2;

    QVector<qreal> darsh;
    darsh << 5 << 5;
    QPen pen(Qt::white);
    pen.setWidth(penWidth);
    pen.setDashPattern(darsh);
    painter.setPen(pen);

    // 打印 20mm空白走纸,前10mm间隔两条虚线
    int x = testPage->width() - penWidth - 10 * RECORDER_PIXEL_PER_MM;
    painter.drawLine(QPoint(penWidth, 0), QPoint(penWidth, testPage->height() - 1));
    painter.drawLine(QPoint(x, 0), QPoint(x, testPage->height() - 1));

    QMetaObject::invokeMethod(d_ptr->processor, "addPage", Qt::QueuedConnection, Q_ARG(RecordPage *, testPage));

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
            QMetaObject::invokeMethod(d_ptr->generator.data(), "stop", Qt::QueuedConnection);
            // stop page processor
            QMetaObject::invokeMethod(d_ptr->processor, "stopProcess", Qt::QueuedConnection);

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
            QMetaObject::invokeMethod(d_ptr->generator.data(), "stop", Qt::QueuedConnection);
            // stop page processor
            QMetaObject::invokeMethod(d_ptr->processor, "stopProcess", Qt::QueuedConnection);
            d_ptr->generator = generator;
            d_ptr->generator->setPrintTime(d_ptr->timeSec);
            generator->moveToThread(d_ptr->procThread);
        }
    }

    d_ptr->curPrintPriority = generator->getPriority();
    connect(generator, SIGNAL(stopped()), this, SLOT(onGeneratorStopped()), Qt::QueuedConnection);
    connect(generator, SIGNAL(generatePage(RecordPage *)), d_ptr->processor, SLOT(addPage(RecordPage *)),
            Qt::QueuedConnection);
    connect(d_ptr->processor, SIGNAL(pageQueueFull(bool)), generator, SLOT(pageControl(bool)), Qt::QueuedConnection);
    if (startImmediately)
    {
        QMetaObject::invokeMethod(generator, "start", Qt::QueuedConnection);
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

void RecorderManager::stopPrint(void)
{
    // stop current generator
    if (d_ptr->generator)
    {
        QMetaObject::invokeMethod(d_ptr->generator.data(), "stop", Qt::QueuedConnection);
    }
    // stop page processor
    QMetaObject::invokeMethod(d_ptr->processor, "stopProcess", Qt::QueuedConnection);

    RecordPage *blankPage = new RecordPage(160);
    QMetaObject::invokeMethod(d_ptr->processor, "addPage", Qt::QueuedConnection, Q_ARG(RecordPage *, blankPage));
}

RecordPageGenerator::PrintPriority RecorderManager::getCurPrintPriority()
{
    return d_ptr->curPrintPriority;
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
    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_PRINT);
    if (alarmSource == NULL)
    {
        return;
    }

    if (d_ptr->connected)
    {
        // connected
        d_ptr->status = PRINTER_STAT_NORMAL;
    }
    else
    {
        // disconected
        alarmSource->clear();
        d_ptr->status = PRINTER_STAT_COMMUNICATION_STOP;
        if (d_ptr->generator)
        {
            // stop the page generator if we has any page generator
            QMetaObject::invokeMethod(d_ptr->generator.data(), "stop", Qt::QueuedConnection);
        }

        QMetaObject::invokeMethod(d_ptr->processor, "stopProcess", Qt::QueuedConnection);
    }
}

void RecorderManager::providerStatusChanged(PrinterStatus status)
{
    bool isOutOfPaper = status & 0x01;
    bool isOverHeating = (status >> 1) & 0x01;
    bool isPrinterFault = (status >> 2) & 0x01;
    bool isPrinting = (status >> 3) & 0x01;

    AlarmOneShotIFace *alarmSource = alarmSourceManager.getOneShotAlarmSource(ONESHOT_ALARMSOURCE_PRINT);
    if (alarmSource)
    {
        alarmSource->setOneShotAlarm(PRINT_ONESHOT_ALARM_OUT_OF_PAPER, isOutOfPaper);
        alarmSource->setOneShotAlarm(PRINT_ONESHOT_ALARM_OVER_HEATING, isOverHeating);
        alarmSource->setOneShotAlarm(PRINT_ONESHOT_ALARM_FAULT, isPrinterFault);
    }

    setPrintStatus(isPrinting);

    if (isOutOfPaper || isOverHeating || isPrinterFault)
    {
        // stop current generator
        QMetaObject::invokeMethod(d_ptr->generator.data(), "stop", Qt::QueuedConnection);
        // stop page processor
        QMetaObject::invokeMethod(d_ptr->processor, "stopProcess", Qt::QueuedConnection);
    }

    if (!isPrinting)
    {
        d_ptr->curPrintPriority = RecordPageGenerator::PriorityNone;
    }

    d_ptr->status = status;
}

void RecorderManager::providerBufferStatusChanged(bool full)
{
    QMetaObject::invokeMethod(d_ptr->processor, "pauseProcessing", Qt::QueuedConnection, Q_ARG(bool, full));
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
        QMetaObject::invokeMethod(d_ptr->generator.data(), "start", Qt::QueuedConnection);
    }
    d_ptr->isAborted = false;
}

void RecorderManager::printWavesInit()
{
    QList<int> waveIDs;
    // ecg
    int index = 0;
    currentConfig.getNumValue("ECG|Ecg1Wave", index);
    ECGLead ecgLead = static_cast<ECGLead>(index);
    WaveformID waveID = ecgParam.leadToWaveID(ecgLead);
    waveIDs.append(waveID);

    ECGLeadMode leadMode = ecgParam.getLeadMode();
    if (leadMode == ECG_LEAD_MODE_5
            || leadMode == ECG_LEAD_MODE_12)
    {
        int index = 0;
        currentConfig.getNumValue("ECG|Ecg2Wave", index);
        ECGLead ecgLead = static_cast<ECGLead>(index);
        WaveformID waveID = ecgParam.leadToWaveID(ecgLead);
        waveIDs.append(waveID);
    }

    if (systemManager.isSupport(CONFIG_RESP))
    {
        // resp
        waveIDs.append(WAVE_RESP);
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        // spo2
        waveIDs.append(WAVE_SPO2);
    }

    if (systemManager.isSupport(CONFIG_IBP) && ibpParam.isConnected())
    {
        // ibp
        IBPLabel ibpTitle = ibpParam.getEntitle(IBP_CHN_1);
        waveID = ibpParam.getWaveformID(ibpTitle);
        waveIDs.append(waveID);

        ibpTitle = ibpParam.getEntitle(IBP_CHN_2);
        waveID = ibpParam.getWaveformID(ibpTitle);
        waveIDs.append(waveID);
    }

    if (co2Param.isConnected())
    {
        // co2
        waveIDs.append(WAVE_CO2);
    }

    int offCount = 0;
    QSet<int> waveCboIds;
    int savedWaveIds[PRINT_WAVE_NUM] = {0};

    // 搜集所有选择的波形id
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        systemConfig.getNumValue(path, savedWaveIds[i]);

        if (savedWaveIds[i] < WAVE_NR)
        {
            waveCboIds.insert(savedWaveIds[i]);
        }

        if (savedWaveIds[i] == WAVE_NONE)
        {
            offCount++;
        }
    }

    // 如果出现重复选择项，重新按照当前显示波形序列更新打印波形id
    if (offCount < PRINT_WAVE_NUM - 1)
    {
        if ((waveCboIds.size()) < PRINT_WAVE_NUM)
        {
            int idCount = waveIDs.count();
            for (int i = 0; i < PRINT_WAVE_NUM; i++)
            {
                QString path;
                path = QString("Print|SelectWave%1").arg(i + 1);
                int waveId = WAVE_NONE;
                if (i < idCount)
                {
                    waveId = waveIDs.at(i);
                }
                systemConfig.setNumValue(path, waveId);
                // 更新打印的波形ids
                savedWaveIds[i] = waveId;
            }
        }
    }

    // 重新更新配置文件的中的波形id
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        // 选择空波形的索引
        if (savedWaveIds[i] == WAVE_NONE)
        {
            continue;
        }

        int cboIndex = waveIDs.indexOf(savedWaveIds[i]);

        // 范围之外的波形选择更新为WAVE_NONE
        if (cboIndex == -1)
        {
            QString path;
            path = QString("Print|SelectWave%1").arg(i + 1);
            systemConfig.setNumValue(path, static_cast<int>(WAVE_NONE));
            continue;
        }
    }
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
