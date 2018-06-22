#include "USBManager.h"
#include <QThread>
#include <Debug.h>
#include <sys/statfs.h>
#include "ErrorLogExporter.h"
#include <QTimer>
#include <QProcess>
#include "IMessageBox.h"
#include <typeinfo>

#define USB_MOUNT_PATH "/media/usbdisk"

USBManager &USBManager::getInstance()
{
    static USBManager *instance = NULL;
    if(instance == NULL)
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
    return _usbExist;
}

bool USBManager::checkStatus() const
{
    return UDiskInspector::checkUsbConnectStatus();
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
    if(_addDataExporter(new ErrorLogExporter()))
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
    if(!_usbExist)
    {
        return false;
    }

    QString cmdStr = QString("mount -o rw,remount %1").arg(USB_MOUNT_PATH);

    qDebug()<<"remount usb to readwrite";

    return QProcess::NormalExit == QProcess::execute(cmdStr);

}

bool USBManager::stopWriteUSBDisk()
{
    if(!_usbExist)
    {
        return false;
    }

    QString cmdStr = QString("mount -o ro,remount %1").arg(USB_MOUNT_PATH);
    qDebug()<<"remount usb to readonly";

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

void USBManager::cancelExport()
{
    _pendingMutex.lock();
    if(_curExporter)
    {
        _curExporter->cancelExport();
    }
    _pendingMutex.unlock();
}

void USBManager::updateConnectStatus(bool status)
{
    _usbExist = status;
}

void USBManager::onExportFinished(DataExporterBase::ExportStatus status)
{
    _pendingMutex.lock();
    delete _curExporter;
    _curExporter = NULL;

    _lastExportStatus = status;

    emit usbExportFileFinish((int)status);

    if(status == DataExporterBase::Success)
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
    if(progress == 100)
    {
        //100 % only emit when receive the exportfinished signal and export successfully
        progress = 99;
    }

    emit exportProcessChanged(progress);
}

USBManager::USBManager()
    :_workerThread(new QThread()),
      _udiskInspector(new UDiskInspector()),
      _lastExportStatus(DataExporterBase::Success),
      _curExporter(NULL)
{

    qRegisterMetaType<DataExporterBase::ExportStatus>();

#ifndef CONFIG_UNIT_TEST
    _udiskInspector->moveToThread(_workerThread);
#endif
    connect(_workerThread, SIGNAL(finished()), _udiskInspector, SLOT(deleteLater()));
    connect(_udiskInspector, SIGNAL(statusUpdate(bool)), this, SLOT(updateConnectStatus(bool)));
    _workerThread->start();
    qdebug("worker thread start.");
    _workerThread->setPriority(QThread::IdlePriority);
}

bool USBManager::_addDataExporter(DataExporterBase *dataExporter)
{
    if (_curExporter)
    {
        IMessageBox messageBox(trs("Warn"), trs("WarnUSBDeviceisbusy"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
        return false;
    }

#ifndef CONFIG_UNIT_TEST
    dataExporter->moveToThread(_workerThread);
#endif
    connect(dataExporter, SIGNAL(exportFinished(DataExporterBase::ExportStatus)), this, SLOT(onExportFinished(DataExporterBase::ExportStatus)));
    connect(dataExporter, SIGNAL(progressChanged(unsigned char)), this, SLOT(onExportProcessUpdate(unsigned char)));
    connect(dataExporter, SIGNAL(exportBegin()), this, SLOT(onExportBegin()));

    QMutexLocker locker(&_pendingMutex);

    _curExporter = dataExporter;
    QTimer::singleShot(0, _curExporter, SLOT(startExport()));

    return true;
}
