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

/***************************************************************************************************
 * Object to check the existance of the USB disk
 **************************************************************************************************/
class UDiskInspector : public QObject
{
    Q_OBJECT
public:
    UDiskInspector();
    ~UDiskInspector();

    // function to check the existance of the USB disk
    bool checkUsbConnectStatus();

    // 检测是否有U盘插入
    bool checkUDiskInsert();

signals:
    // status update signal, emit periodly
    void statusUpdate(bool isUSBConnected);

    // 挂载U盘
    void mountUDisk();

protected:
    void timerEvent(QTimerEvent *ev);

private:
    int _timerId;
};
