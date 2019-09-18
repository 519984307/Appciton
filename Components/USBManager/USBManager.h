/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/6
 **/

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

    // check whether a usb disk is connected, not realtime
    bool isUSBExist() const;

    // check the usb connect status, realtime
    bool checkStatus() const;

    // get the free space of the usb disk, in unit of KB
    int getUSBFreeSize() const;


    // export error Log
    bool exportErrorLog();

    // get the last export status, should call when export stop
    DataExporterBase::ExportStatus getLastExportStatus() const;

    // get the mount point of the usb disk
    QString getUdiskMountPoint() const;

    // call before write data to the udisk, remount the udisk as rw
    bool startWriteUSBDisk();
    // call after write data to the udisk, remount the udisk as ro
    bool stopWriteUSBDisk();

    // USB export finish
    bool isUSBExportFinish();

    // 停止数据收集
    void stopRawCollectData();

    // 卸载U盘
    void umountUDisk();

    void forceUmountDisk();

signals:
    // report the process, 0~100
    void exportProcessChanged(unsigned char process);
    // report the export error
    void exportError();
    void usbExportFileFinish(int status);

    void startCollectData(void);
    void stopCollectData(void);

    /**
     * @brief popupUDisk - u盘弹出信号
     */
    void popupUDisk();

public slots:
    // cancel export
    void cancelExport();

private slots:
    // update usb connection status
    void updateConnectStatus(bool status);
    // handle export status
    void onExportFinished(DataExporterBase::ExportStatus status);
    // reset export status
    void onExportBegin();
    // handle export process
    void onExportProcessUpdate(unsigned char progress);

    // 挂载U盘成功
    void mountUDiskSuccess(void);

private:
    USBManager();
    bool _addDataExporter(DataExporterBase *dataExporter);
    QThread *_workerThread;
    UDiskInspector *_udiskInspector;
    DataExporterBase::ExportStatus _lastExportStatus;
    DataExporterBase *_curExporter;
    QMutex _pendingMutex;
    bool _usbExist;
    bool _isMount;
    QThread *_procThread;
};

#define usbManager (USBManager::getInstance())
#define deleteUsbManager() (delete &usbManager)
