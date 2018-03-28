#pragma once
#include "Uart.h"

/* create a fake uart port, data transmit through the TCP socket */
class UartSocket : public Uart
{
    Q_OBJECT
public:
    /* overriden */
    bool initPort(const QString &port, const UartAttrDesc &desc, bool needNotify);
    /* overriden */
    int write(const unsigned char buff[], int len);
    /* overriden */
    int read(unsigned char *buff, int nbytes);
    /* overriden */
    void sync();

    /* overriden */
    void closePort(void);

    UartSocket();
    ~UartSocket();

private:
    bool _nonblock;
};
