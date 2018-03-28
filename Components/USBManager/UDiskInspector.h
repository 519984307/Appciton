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

    //function to check the existance of the USB disk
    static bool checkUsbConnectStatus();

signals:
    //status update signal, emit periodly
    void statusUpdate(bool isUSBConnected);

protected:
    void timerEvent(QTimerEvent *ev);

private:
    int _timerId;
};
