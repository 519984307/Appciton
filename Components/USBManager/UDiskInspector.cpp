/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/6
 **/

#include "UDiskInspector.h"
#include <QTimerEvent>
#include <QDir>
#include <QFile>
#include <Debug.h>
#include <QStringList>
#include <QProcess>

#define LINUX_MOUNT_FILE "/proc/mounts"         // system mount file record
#define USB_DEV_PREFIX "/dev/sd"               // usb device name prefix in the system
#define USB_MOUNT_PATH "/media/usbdisk"        // mount point of the usb disk

UDiskInspector::UDiskInspector()
{
    _timerId = startTimer(1000);
}

UDiskInspector::~UDiskInspector()
{
    qdebug("UdiskInspector destruct.");
}

bool UDiskInspector::checkUsbConnectStatus()
{
#if defined(Q_WS_X11)   // 在PC上运行时直接返回有U盘，为了能完整使用U盘功能还需在/media下建立usbdisk文件夹。
    if (QFile::exists(USB_MOUNT_PATH))
    {
        return true;
    }
    return false;
#else

    if (!QFile::exists(USB_MOUNT_PATH))
    {
        if (!QDir().mkpath(USB_MOUNT_PATH))
        {
            qdebug("Failed to create usb mount point.");
            return false;
        }
    }

    QFile file(LINUX_MOUNT_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qdebug("\033[31m open %s error:%s\n \033[m", LINUX_MOUNT_FILE, qPrintable(file.errorString()));
        return false;
    }

    QString fileStr = file.readAll();
    file.close();

    QString mountPath(USB_MOUNT_PATH);
    QString usbName(USB_DEV_PREFIX);
    QStringList usbNameList;

    // U盘异常拔出时，系统文件中的挂载信息不会删除，这里还需检查具体的设备是否存在
    int usbIndex = fileStr.indexOf(usbName);
    int mountPathIndex = fileStr.indexOf(mountPath);
    while (usbIndex > 0 && mountPathIndex > 0)
    {
        QString tmpName = fileStr.right(fileStr.size() - usbIndex);
        int tmpIndex = tmpName.indexOf(' ');
        if (tmpIndex > 0)
        {
            usbNameList << tmpName.left(tmpIndex);
        }

        fileStr = fileStr.right(fileStr.size() - mountPathIndex - mountPath.size());

        usbIndex = fileStr.indexOf(usbName);
        mountPathIndex = fileStr.indexOf(mountPath);
    }

    bool result = false;

    foreach(const QString &usbdev, usbNameList)
    {
        if (QFile::exists(usbdev))
        {
            result = true;
            emit mountUDisk();
            break;
        }
    }

    if (!result)
    {
        if (!usbNameList.isEmpty())
        {
            // the usb storage device might be removed
            // do the umount stuff here
            QString cmd = QString("umount -f %1").arg(USB_MOUNT_PATH);
            if (QProcess::execute(cmd) != 0)
            {
                qdebug("Fail to umount %s", USB_MOUNT_PATH);
            }
        }

        // find a exist usb device and mount it
        QDir devDir("/dev");
        devDir.setFilter(QDir::System);
        devDir.setNameFilters(QStringList() << "sd??");
        usbNameList = devDir.entryList();

        // find a device and mount it
        foreach(const QString &devName, usbNameList)
        {
            QString mountCmd = QString("mount /dev/%1 %2").arg(devName).arg(USB_MOUNT_PATH);
            if (QProcess::execute(mountCmd) == 0)
            {
                qdebug("mount /dev/%s success!", qPrintable(devName));
                result = true;
                emit mountUDisk();
                break;
            }
        }
    }

    return result;

#endif
}

bool UDiskInspector::checkUDiskInsert()
{
    static int usbNum = 0;
    // find a exist usb device and mount it
    QDir devDir("/sys/class/scsi_disk");
    int usbFileNum = devDir.count();
    if (usbFileNum != usbNum)
    {
        usbNum = usbFileNum;
        return true;
    }
    else
    {
        return false;
    }
    return true;
}

void UDiskInspector::timerEvent(QTimerEvent *ev)
{
    if (ev->timerId() == _timerId)
    {
        if (checkUDiskInsert())
        {
            bool status = checkUsbConnectStatus();
            emit  statusUpdate(status);
        }
    }
}
