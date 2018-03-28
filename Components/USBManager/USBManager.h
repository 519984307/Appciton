#pragma once
#include <QObject>
#include <UDiskInspector.h>
#include <DataExporterBase.h>
#include <QMetaType>
#include <QMutex>

class ECG12LeadFileBuilderBase;
class USBManager : public QObject
{

#ifdef CONFIG_UNIT_TEST
    friend class TestUpgradeParam;
#endif

    Q_OBJECT
public:

    static USBManager &getInstance();

    ~USBManager();

    //check whether a usb disk is connected, not realtime
    bool isUSBExist() const;

    //check the usb connect status, realtime
    bool checkStatus() const;

    //get the free space of the usb disk, in unit of KB
    int getUSBFreeSize() const;

    //export 12 Lead ECG, will take the ownership of th fileBuilder
    //will take the ownership of the file builder
    bool export12LeadData(ECG12LeadFileBuilderBase *fileBuilder);

    //export error Log
    bool exportErrorLog();

    // get the last export status, should call when export stop
    DataExporterBase::ExportStatus getLastExportStatus() const;

    // get the mount point of the usb disk
    QString getUdiskMountPoint() const;

    // call before write data to the udisk, remount the udisk as rw
    bool startWriteUSBDisk();
    // call after write data to the udisk, remount the udisk as ro
    bool stopWriteUSBDisk();

    //USB export finish
    bool isUSBExportFinish();

signals:
    //report the process, 0~100
    void exportProcessChanged(unsigned char process);
    //report the export error
    void exportError();
    void usbExportFileFinish(int status);

public slots:
    //cancel export
    void cancelExport();

private slots:
    //update usb connection status
    void updateConnectStatus(bool status);
    //handle export status
    void onExportFinished(DataExporterBase::ExportStatus status);
    //reset export status
    void onExportBegin();
    //handle export process
    void onExportProcessUpdate(unsigned char progress);

private:
    USBManager();
    bool _addDataExporter(DataExporterBase *dataExporter);
    QThread *_workerThread;
    UDiskInspector *_udiskInspector;
    DataExporterBase::ExportStatus _lastExportStatus;
    DataExporterBase *_curExporter;
    QMutex _pendingMutex;
    bool _usbExist;

};


#define usbManager (USBManager::getInstance())
#define deleteUsbManager() (delete &usbManager)

