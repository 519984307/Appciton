#include "UartSocket.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <QSocketNotifier>
#include "Debug.h"

#define DEVICE_IP "192.168.10.2"
#define PORT_START 8000
#define TTYMXC_START_INDEX 0
#define TTYS_START_INDEX 5

/***************************************************************************************************
 * get the tcp port from the port string, port string should in format of "/dev/ttymxc#" or "/dev/ttyS#"
 **************************************************************************************************/
static int getTcpPortNum(const QString &port)
{
    int index;
    if(port.contains("ttymxc"))
    {
        index = port.right(1).toInt();
        return PORT_START + TTYMXC_START_INDEX + index;
    }
    else if(port.contains("ttyS"))
    {
        index = port.right(1).toInt();
        return PORT_START + index + TTYS_START_INDEX;
    }

    return -1;
}

/***************************************************************************************************
 * create tcp connection here
 **************************************************************************************************/
bool UartSocket::initPort(const QString &port, const UartAttrDesc &desc, bool needNotify)
{
    int portnum = getTcpPortNum(port);
    if(portnum < 0)
    {
        return false;
    }

    _port = port;

    _nonblock = desc.nonBlocking;

    struct sockaddr_in remoteAddr;
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr=inet_addr(DEVICE_IP);
    remoteAddr.sin_port=htons((uint16_t)portnum);

    _fd = socket(PF_INET, SOCK_STREAM,0);

    if(_fd < 0)
    {
        perror("UartSocket");
        debug("uartSocket:%s, %s:%d connect fail!", qPrintable(port), DEVICE_IP, portnum);
        return false;
    }

    if(::connect(_fd, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr)) < 0)
    {
        perror("UartSocket");
        debug("uartSocket:%s, %s:%d connect fail!", qPrintable(port), DEVICE_IP, portnum);
        close(_fd);
        _fd = -1;
        return false;
    }

    int enable = 1;
    setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));

    if(needNotify)
    {
        _notifier = new QSocketNotifier(_fd, QSocketNotifier::Read);
        connect(_notifier, SIGNAL(activated(int)), this, SIGNAL(activated(int)));
        _notifier->setEnabled(true);
    }
    debug("UartSocket:%s, %s:%d connected!", qPrintable(port), DEVICE_IP, portnum);
    return true;
}

#define FAIL_RETRY_LIMIT 100
int UartSocket::write(const unsigned char buff[], int len)
{
    if ((_fd < 0) || (buff == NULL))
    {
        return 0;
    }

    int ret = 0;
    int remain = len;
    int retry = 0;
    const unsigned char *current = buff;

    // 确保数据能够全部写往端口。
    while (remain > 0)
    {
        ret = TEMP_FAILURE_RETRY(::send(_fd, current, remain, _nonblock ? MSG_DONTWAIT : 0));
        if (ret == -1)
        {
            if (errno == EAGAIN)
            {
                if(++retry <= FAIL_RETRY_LIMIT)
                    continue;
            }

            debug("%s(%d), %s", qPrintable(_port), _fd, strerror(errno));
            return (len - remain);
        }
::fsync(_fd);
        current += ret;
        remain -= ret;
        retry = 0;
    }
    return len;
}

int UartSocket::read(unsigned char *buff, int nbytes)
{
    if (_fd < 0 || buff == NULL)
    {
        return 0;
    }
    return ::recv(_fd, buff, nbytes, _nonblock ? MSG_DONTWAIT : 0);
}

void UartSocket::sync()
{
    ::fsync(_fd);
}

void UartSocket::closePort()
{
    if(_notifier)
    {
        delete _notifier;
    }

    if(_fd >= 0)
    {
        shutdown(_fd, SHUT_RDWR);
        close(_fd);
    }
}

UartSocket::UartSocket()
    :Uart(), _nonblock(true)
{

}

UartSocket::~UartSocket()
{

}
