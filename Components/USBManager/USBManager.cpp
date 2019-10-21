/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/6
 **/

#include "USBManager.h"
#include <QThread>
#include <Debug.h>
#include <sys/statfs.h>
#include "ErrorLogExporter.h"
#include <QTimer>
#include <QProcess>
#include "IMessageBox.h"
#include <typeinfo>
#include "RawDataCollector.h"
#include "MessageBox.h"

#define USB_MOUNT_PATH "/media/usbdisk"

USBManager &USBManager::getInstance()
{
    static USBManager *instance = NULL;
    if (instance == NULL)
    {
        instance = new USBManager();
    }
    return *instance;
}

USBManager::~USBManager()
{
    _workerThread->quit();
    _workerThread->wait();
    qdebug("worker thread exit.");
}

bool USBManager::isUSBExist() const
{
    return _usbExist && _isMount;
}

bool USBManager::checkStatus() const
{
    return isUSBExist();
}

int USBManager::getUSBFreeSize() const
{
    if (!_usbExist)
    {
        return -1;
    }

    struct statfs usbStatfs;
    memset(&usbStatfs, 0, sizeof(struct statfs));
    if (0 != statfs(USB_MOUNT_PATH, &usbStatfs))
    {
        qdebug("\033[31m statfs file error:%s\n \033[m", USB_MOUNT_PATH);
        return -1;
    }

    return (usbStatfs.f_bfree * usbStatfs.f_bsize) >> 10;
}


bool USBManager::exportErrorLog()
{
    if (_addDataExporter(new ErrorLogExporter()))
    {
        return true;
    }

    return false;
}

DataExporterBase::ExportStatus USBManager::getLastExportStatus() const
{
    return _lastExportStatus;
}

QString USBManager::getUdiskMountPoint() const
{
    return USB_MOUNT_PATH;
}

bool USBManager::startWriteUSBDisk()
{
    if (!_usbExist)
    {
        return false;
    }

    QString cmdStr = QString("mount -o rw,remount %1").arg(USB_MOUNT_PATH);

    qDebug() << "remount usb to readwrite";

    return QProcess::NormalExit == QProcess::execute(cmdStr);
}

bool USBManager::stopWriteUSBDisk()
{
    if (!_usbExist)
    {
        return false;
    }

    QString cmdStr = QString("mount -o ro,remount %1").arg(USB_MOUNT_PATH);
    qDebug() << "remount usb to readonly";

    return QProcess::NormalExit == QProcess::execute(cmdStr);
}

bool USBManager::isUSBExportFinish()
{
    if (NULL == _curExporter)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void USBManager::stopRawCollectData()
{
    // 卸载U盘之前停止数据收集定时器
    QMetaObject::invokeMethod(&rawDataCollector, "stopCollectData", Qt::QueuedConnection);
}

void USBManager::umountUDisk()
{
    QString cmd = QString("umount -f %1").arg(USB_MOUNT_PATH);
    if (QProcess::execute(cmd) != 0)
    {
        qdebug("Fail to umount %s", USB_MOUNT_PATH);
        _isMount = true;
        MessageBox warnMsg(trs("Warn"), trs("OperationFailedPleaseAgain"), false);
        warnMsg.exec();
    }
    else
    {
        _isMount = false;
    }
}

void USBManager::forceUmountDisk()
{
    _isMount = false;
}

void USBManager::cancelExport()
{
    _pendingMutex.lock();
    if (_curExporter)
    {
        _curExporter->cancelExport();
    }
    _pendingMutex.unlock();
}

void USBManager::updateConnectStatus(bool status)
{
    _usbExist = status;
    if (_usbExist == false)  // u盘不存在时发出u盘弹出信号
    {
        emit popupUDisk();
    }
}

void USBManager::onExportFinished(DataExporterBase::ExportStatus status)
{
    _pendingMutex.lock();
    delete _curExporter;
    _curExporter = NULL;

    _lastExportStatus = status;

    emit usbExportFileFinish(static_cast<int>(status));

    if (status == DataExporterBase::Success)
    {
        emit exportProcessChanged(100);
    }
    else
    {
        emit exportError();
    }

    _pendingMutex.unlock();
}

void USBManager::onExportBegin()
{
    _lastExportStatus = DataExporterBase::Success;
}

void USBManager::onExportProcessUpdate(unsigned char progress)
{
    if (progress == 100)
    {
        // 100 % only emit when receive the exportfinished signal and export successfully
        progress = 99;
    }

    emit exportProcessChanged(progress);
}

void USBManager::mountUDiskSuccess()
{
    _isMount = true;
    // 启动数据收集定时器
    QMetaObject::invokeMethod(&rawDataCollector, "startCollectData", Qt::QueuedConnection);
}

USBManager::USBManager()
    : _workerThread(new QThread()),
      _udiskInspector(new UDiskInspector()),
      _lastExportStatus(DataExporterBase::Success),
      _curExporter(NULL),
      _isMount(false),
      _procThread(NULL)
{
    qRegisterMetaType<DataExporterBase::ExportStatus>();

    _workerThread->setObjectName("USBManager");
    _udiskInspector->moveToThread(_workerThread);
    connect(_workerThread, SIGNAL(finished()), _udiskInspector, SLOT(deleteLater()));
    connect(_udiskInspector, SIGNAL(statusUpdate(bool)), this, SLOT(updateConnectStatus(bool)));
    connect(_udiskInspector, SIGNAL(mountUDisk()), this, SLOT(mountUDiskSuccess()));
    _workerThread->start();
    qdebug("worker thread start.");
    _workerThread->setPriority(QThread::IdlePriority);

    _procThread = new QThread(this);
    _procThread->setObjectName("RawDataCollect");
    rawDataCollector.moveToThread(_procThread);
    _procThread->start();
}

bool USBManager::_addDataExporter(DataExporterBase *dataExporter)
{
    if (_curExporter)
    {
        IMessageBox messageBox(trs("Warn"), trs("WarnUSBDeviceisbusy"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
        return false;
    }

    dataExporter->moveToThread(_workerThread);
    connect(dataExporter, SIGNAL(exportFinished(DataExporterBase::ExportStatus)), this,
            SLOT(onExportFinished(DataExporterBase::ExportStatus)));
    connect(dataExporter, SIGNAL(progressChanged(unsigned char)), this, SLOT(onExportProcessUpdate(unsigned char)));
    connect(dataExporter, SIGNAL(exportBegin()), this, SLOT(onExportBegin()));

    QMutexLocker locker(&_pendingMutex);

    _curExporter = dataExporter;
    QTimer::singleShot(0, _curExporter, SLOT(startExport()));

    return true;
}
