#include "UDiskInspector.h"
#include <QTimerEvent>
#include <QFile>
#include <Debug.h>
#include <QStringList>

#define LINUX_MOUNT_FILE "/proc/mounts"         //system mount file record
#define USB_DEV_PREFIX "/dev/sd"               //usb device name prefix in the system
#define USB_MOUNT_PATH "/media/usbdisk"        //mount point of the usb disk

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
//#if defined(Q_WS_X11)// 在PC上运行时直接返回有U盘，为了能完整使用U盘功能还需在/media下建立usbdisk文件夹。
//    return true;
//#else
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
    int tmpIndex = 0;
    while (usbIndex > 0 && mountPathIndex > 0)
    {
        QString tmpName = fileStr.right(fileStr.size() - usbIndex);
        tmpIndex = tmpName.indexOf(' ');
        if (tmpIndex > 0)
        {
            usbNameList << tmpName.left(tmpIndex);
        }

        fileStr = fileStr.right(fileStr.size() - mountPathIndex - mountPath.size());

        usbIndex = fileStr.indexOf(usbName);
        mountPathIndex = fileStr.indexOf(mountPath);
    }

    bool result = false;

    while (!usbNameList.isEmpty())
    {
        if (QFile::exists(usbNameList.takeLast()))
        {
            result = true;
            break;
        }
    }

    return result;

//#endif
}

void UDiskInspector::timerEvent(QTimerEvent *ev)
{
    if(ev->timerId() == _timerId)
    {
        bool status = checkUsbConnectStatus();
        emit  statusUpdate(status);
    }
}
