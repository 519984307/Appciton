/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/27
 **/

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
#include <QTcpSocket>
#include "UartSocketDefine.h"
#include "crc8.h"
#include "Framework/Utility/RingBuff.h"
#include <QMap>
#include <QEventLoop>
#include <QTimer>

#define DEVICE_IP "192.168.10.2"
#define CONTROL_PORT 8000

static UartSocketController *controller = NULL;

/***************************************************************************************************
 * create tcp connection here
 **************************************************************************************************/
bool UartSocket::initPort(const QString &port, const UartAttrDesc &desc, bool needNotify)
{
    _port = port;
    if (controller == NULL) {
        controller = new UartSocketController();
        controller->connectControlServer();
    }

    if (!controller->isConnected()) {
        qDebug() << "UartSocket control server not connect";
        return false;
    }

    controller->openPort(port);

    quint16 portnum = controller->getServerPort(port);
    if (portnum == 0) {
        return false;
    }

    controller->configPort(port, desc);

    _nonblock = desc.nonBlocking;

    struct sockaddr_in remoteAddr;
    memset(&remoteAddr, 0, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_addr.s_addr = inet_addr(DEVICE_IP);
    remoteAddr.sin_port = htons((uint16_t)portnum);

    _fd = socket(PF_INET, SOCK_STREAM, 0);

    if (_fd < 0)
    {
        perror("UartSocket");
        debug("uartSocket:%s, %s:%d connect fail!", qPrintable(port), DEVICE_IP, portnum);
        return false;
    }

    if (::connect(_fd, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr)) < 0)
    {
        perror("UartSocket");
        debug("uartSocket:%s, %s:%d connect fail!", qPrintable(port), DEVICE_IP, portnum);
        close(_fd);
        _fd = -1;
        return false;
    }

    int enable = 1;
    setsockopt(_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));

    if (needNotify)
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

    int remain = len;
    int retry = 0;
    const unsigned char *current = buff;

    // 确保数据能够全部写往端口。
    while (remain > 0)
    {
        int ret = TEMP_FAILURE_RETRY(::send(_fd, current, remain, _nonblock ? MSG_DONTWAIT : 0));
        if (ret == -1)
        {
            if (errno == EAGAIN)
            {
                if (++retry <= FAIL_RETRY_LIMIT)
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
    if (_notifier)
    {
        delete _notifier;
    }

    if (_fd >= 0)
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

class UartSocketControllerPrivate
{
public:
    explicit UartSocketControllerPrivate(UartSocketController * const q_ptr)
        : q_ptr(q_ptr), socket(NULL), isReceivedFrameHead(false)
    {
        ringbuff.resize(1024);
    }

    /**
     * @brief handlePacket handle the recieve frame
     * @param type frame type
     * @param payload data payload
     * @param len length of data payload
     */
    void handlePacket(UartSocketFrameType type, quint8 *payload, int len)
    {
        Q_UNUSED(len)
        switch (type) {
        case UARTSOCKET_CTRL_RSP_OPEN_PORT:
        {
            if (payload[0] == 1) {
                quint16 port = payload[1] + (payload[2] << 8);
                ports.insert(lastConfigPort, port);
                qDebug() << lastConfigPort << "UartSocket server port" << port;
            } else {
                qDebug() << "Open port" << lastConfigPort <<"failed!";
            }
        }
            break;
        case UARTSOCKET_CTRL_RSP_CONF_PORT:
            qDebug() << lastConfigPort << "config" << (payload[0] ? "success" : "fail");
            break;
        default:
            break;
        }
        q_ptr->receiveCmdResponse();
    }

    /**
     * @brief parseData parse data in the ringbuffer
     */
    void parseData() {
        quint8 packet[UARTSOCKET_MAX_FRAME_LEN];
        RingBuff<quint8> *buf = &ringbuff;
        while (buf->dataSize() >= UARTSOCKET_MIN_FRAME_LEN) {
            /* try to find the frame head */
            if (buf->at(0) != UARTSOCKET_FRAME_HEAD) {
                qDebug() << "Invalid head, drop";
                buf->pop(1);
                continue;
            }

            int len = buf->at(1) + (buf->at(2) << 8);
            if (len > UARTSOCKET_MAX_FRAME_LEN) {
                /* exceed the maximum frame len */
                qDebug() << "Invalid length, drop";
                buf->pop(1);
                continue;
            }

            if (len > buf->dataSize()) {
                /* no enough data */
                break;
            }

            /* read data to buffer */
            for (int i = 0; i < len; i++) {
                packet[i] = buf->at(i);
            }

            /* calculate crc */
            if (calcCRC(packet, len - 1) == packet[len - 1]) {
                UartSocketFrameType type = static_cast<UartSocketFrameType>(packet[3]);
                quint8 *payload = &packet[4];
                int payloadLen = len - UARTSOCKET_MIN_FRAME_LEN;

                handlePacket(type, payload, payloadLen);

                /* data is already handle, drop frame */
                buf->pop(len);
            } else {
                qDebug() << "checksum fail, drop";
                buf->pop(1);
            }
        }
    }

    /**
     * @brief sendControlFrame send control frame to the server
     * @param socket the socket
     * @param type the frame type
     * @param payload the data payload
     * @param len the payload length
     */
    void sendControlFrame(QTcpSocket *socket, UartSocketFrameType type, quint8 *payload, int len)
    {
        quint8 packet[UARTSOCKET_MAX_FRAME_LEN];

        quint16 frameLen = UARTSOCKET_MIN_FRAME_LEN + len;
        packet[0] = UARTSOCKET_FRAME_HEAD;
        packet[1] = frameLen & 0xFF;
        packet[2] = (frameLen >> 8) & 0xFF;
        packet[3] = type;
        for (int i = 0; i < len; i++)
        {
            packet[i + 4] = payload[i];
        }
        packet[frameLen - 1] = calcCRC(packet, frameLen - 1);

        quint8 sendBuf[UARTSOCKET_MAX_FRAME_LEN * 2];
        quint16 sendLen = 0;
        sendBuf[sendLen++] = packet[0];
        for (int i = 1; i < frameLen; i++)
        {
            sendBuf[sendLen++] = packet[i];
            if (packet[i] == UARTSOCKET_FRAME_HEAD)
            {
                sendBuf[sendLen++] = packet[i];
            }
        }

        if (socket->write(reinterpret_cast<char *>(&sendBuf[0]), sendLen) != sendLen)
        {
            qWarning() << Q_FUNC_INFO << "send imcomplete data!";
        }
    }

    /**
     * @brief receiveData receive data from socket
     * @param socket the socket
     */
    void receiveData(QTcpSocket *socket)
    {
        /* we consider the whole frame is received a once */
        QByteArray data = socket->readAll();
        for (int i = 0; i < data.length(); i++)
        {
            quint8 ch = data.at(i);
            if (ch == UARTSOCKET_FRAME_HEAD)
            {
                if (isReceivedFrameHead)
                {
                    /* receive two continuous frame head byte */
                    isReceivedFrameHead = false;
                    /* drop this byte */
                    continue;
                }
                else
                {
                    ringbuff.push(ch);
                    isReceivedFrameHead = true;
                }
            }
            else
            {
                isReceivedFrameHead = false;
                ringbuff.push(ch);
            }
        }
    }

    UartSocketController * const q_ptr;
    QTcpSocket *socket;
    bool isReceivedFrameHead;
    QString lastConfigPort;
    QMap<QString, quint16> ports;
    RingBuff<quint8> ringbuff;
};

UartSocketController::UartSocketController(QObject *parent)
    : QObject(parent), d_ptr(new UartSocketControllerPrivate(this))
{
    d_ptr->socket = new QTcpSocket(this);
    d_ptr->socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(d_ptr->socket, SIGNAL(readyRead()), this, SLOT(onDataReady()));
    connect(d_ptr->socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
}

UartSocketController::~UartSocketController()
{
}

void UartSocketController::connectControlServer()
{
    d_ptr->socket->connectToHost(DEVICE_IP, CONTROL_PORT);
    QEventLoop loop;
    connect(d_ptr->socket, SIGNAL(connected()), &loop, SLOT(quit()));
    connect(d_ptr->socket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();
}

bool UartSocketController::isConnected()
{
    return d_ptr->socket->state() == QAbstractSocket::ConnectedState;
}

void UartSocketController::openPort(const QString &port)
{
    if (!isConnected()) {
        qDebug() << "contorl server is not connected";
        return;
    }

    quint8 payload[32] = {0};

    d_ptr->lastConfigPort = port;

    memcpy(payload, qPrintable(port), port.length());
    d_ptr->sendControlFrame(d_ptr->socket, UARTSOCKET_CTRL_CMD_OPEN_PORT, payload, sizeof(payload));

    QEventLoop loop;
    connect(d_ptr->socket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
    connect(d_ptr->socket, SIGNAL(disconnected()), &loop, SLOT(quit()));
    connect(this, SIGNAL(receiveCmdResponse()), &loop, SLOT(quit()));
    QTimer::singleShot(3000, &loop, SLOT(quit()));
    loop.exec();
}

void UartSocketController::configPort(const QString &port, const UartAttrDesc &desc)
{
    if (!isConnected()) {
        qDebug() << "contorl server is not connected";
        return;
    }

    quint16 serverPort = d_ptr->ports.value(port, 0);
    if (serverPort)
    {
        d_ptr->lastConfigPort = port;
        quint8 payload[11];
        /* set server port */
        payload[0] = serverPort & 0xFF;
        payload[1] = (serverPort >> 8) & 0xFF;
        /* set baudrate */
        payload[2] = desc.baud & 0xFF;
        payload[3] = (desc.baud >> 8) & 0xFF;
        payload[4] = (desc.baud >> 16) & 0xFF;
        payload[5] = (desc.baud >> 24) & 0xFF;
        /* set data len */
        payload[6] = desc.dataLen;
        /* set parity */
        payload[7] = desc.parity;
        /* set stop bits */
        payload[8] = desc.stopLen;
        /* set vmin */
        payload[9] = desc.vmin;
        /* set flow control */
        payload[10] = desc.flowCtrl;
        d_ptr->sendControlFrame(d_ptr->socket, UARTSOCKET_CTRL_CMD_CONF_PORT, payload, sizeof(payload));

        QEventLoop loop;
        connect(d_ptr->socket, SIGNAL(error(QAbstractSocket::SocketError)), &loop, SLOT(quit()));
        connect(d_ptr->socket, SIGNAL(disconnected()), &loop, SLOT(quit()));
        connect(this, SIGNAL(receiveCmdResponse()), &loop, SLOT(quit()));
        QTimer::singleShot(3000, &loop, SLOT(quit()));
        loop.exec();
    }
    else
    {
        qDebug() << port <<"config failed";
    }
}

bool UartSocketController::isPortConfigSuccess(const QString &port)
{
    return d_ptr->ports.value(port, 0);
}

quint16 UartSocketController::getServerPort(const QString &port)
{
    return d_ptr->ports.value(port, 0);
}

void UartSocketController::onDataReady()
{
    d_ptr->receiveData(d_ptr->socket);
    d_ptr->parseData();
}

void UartSocketController::onDisconnected()
{
    d_ptr->socket->abort();
    d_ptr->ports.clear();
    /* try reconnected */
    connectControlServer();
}
