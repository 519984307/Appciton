/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/4
 **/

#include "DataDispatcher.h"
#include "Uart.h"
#include "IConfig.h"
#include "crc8.h"
#include "Provider.h"
#include "Debug.h"

#define SOH             (0x02)  // packet header
#define MIN_PACKET_LEN   5      // 最小数据包长度: SOH,Length,Type,FCS
#define RING_BUFFER_LENGTH 4096
#define MAXIMUM_PACKET_SIZE 256  // largest packet size, should be larger enough


enum PacketPortCommand
{
    PORT_CMD_RESET  = 0x10,         // reset port
    PORT_CMD_RESET_RSP = 0x11,      // reset port response
    PORT_CMD_BAUDRATE = 0x12,       // baudrate
    PORT_CMD_BAUDRATE_RSP = 0x13,   // baudrate
};

class DataDispatcherPrivate
{
public:
    DataDispatcherPrivate(const QString &name, DataDispatcher *const q_ptr)
        : isLastByteSOH(false), name(name),
          uart(new Uart(q_ptr)),
          ringBuff(RING_BUFFER_LENGTH)
    {
    }

    bool initPort(const UartAttrDesc &desc)
    {
        QString port;
        machineConfig.getStrValue(name + "Port", port);
        debug("%s", qPrintable(name));
        debug("%s", qPrintable(port));
        return uart->initPort(port, desc, true);
    }

    void readData()
    {
        unsigned char buff[MAXIMUM_PACKET_SIZE];
        int len = uart->read(buff, MAXIMUM_PACKET_SIZE);
        if (len <= 0)
        {
            return;
        }

        for (int i = 0; i < len; ++i)
        {
            if (buff[i] == SOH)
            {
                if (isLastByteSOH)
                {
                    /* conitnuous receiving two SOH, drop one */
                    isLastByteSOH = false;
                    continue;
                }
                else
                {
                    isLastByteSOH = true;
                }
            }
            else
            {
                isLastByteSOH = false;
            }
            ringBuff.push(buff[i]);
        }
    }

    bool checkPacketValid(const unsigned char *data, unsigned int len)
    {
        if ((NULL == data) || (len < MIN_PACKET_LEN))
        {
            debug("Invalid packet!\n");
            debug("%s: FCS not match!\n", qPrintable(name));
            return false;
        }

        unsigned char crc = calcCRC(data, (len - 1));
        if (data[len - 1] != crc)
        {
            // outHex(data, (int)len);
            // debug("%s: FCS not match : %x!\n", qPrintable(getName()), crc);
            return false;
        }

        return true;
    }

    void handlePacket(unsigned char *data, int len)
    {
        DataDispatcher::PacketType type = static_cast<DataDispatcher::PacketType>(data[0]);
        if (type == DataDispatcher::PACKET_TYPE_CTRL)
        {
            // control packet responsoe
            unsigned char *responseData = data + 1;
            DataDispatcher::PacketType portType = static_cast<DataDispatcher::PacketType>(responseData[0]);
            PacketPortCommand portCmdRSP = static_cast<PacketPortCommand>(responseData[1]);
            switch (portCmdRSP)
            {
            case PORT_CMD_RESET_RSP:
            {
                qDebug() << "DataDispatcher: reset response of packet port 0x" << hex << portType;
                dataHandlers[portType]->dispatchPortHasReset();
            }
                break;
            case PORT_CMD_BAUDRATE_RSP:
                qDebug() << "DataDispatcher: baudrate response of packet port 0x" << hex << portType;
                break;
            default:
                break;
            }
        }
        else if (dataHandlers.contains(type))
        {
            dataHandlers[type]->dataArrived(data + 1, len - 1);
        }
        else
        {
            qWarning() << Q_FUNC_INFO << "Invalid dispatch packet type 0x" << hex << data[0];
        }
    }

    bool isLastByteSOH;    // 记录上一个字节是否是SOH
    QString name;
    Uart *uart;
    QMap<DataDispatcher::PacketType, Provider *> dataHandlers;
    RingBuff<unsigned char> ringBuff;
    static QMap<QString, DataDispatcher *> dispatchers;

private:
    DataDispatcherPrivate(const DataDispatcherPrivate &);  // use to pass the cpplint check only, no implementation
};

QMap<QString, DataDispatcher *> DataDispatcherPrivate::dispatchers;

DataDispatcher::DataDispatcher(const QString &name, QObject *parent)
    : QObject(parent), d_ptr(new DataDispatcherPrivate(name, this))
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    d_ptr->initPort(portAttr);
    connect(d_ptr->uart, SIGNAL(activated(int)), this, SLOT(dataArrived()));
}

DataDispatcher::~DataDispatcher()
{
    delete d_ptr;
}

QString DataDispatcher::getName() const
{
    return d_ptr->name;
}

void DataDispatcher::connectProvider(DataDispatcher::PacketType type, Provider *provider)
{
    if (type == PACKET_TYPE_INVALID)
    {
        qDebug() << "provider " << provider->getName() << " connect to invalid packet type!";
    }
    d_ptr->dataHandlers[type] = provider;
}

int DataDispatcher::sendData(DataDispatcher::PacketType type, const unsigned char *buff, int len)
{
    if (buff == NULL || len == 0)
    {
        return 0;
    }

    unsigned char crc = 0;
    unsigned char sendBuf[2 * len + 9];     // larger enough to hold the duplicated SOH
    int i = 0;
    unsigned short length = len + 5;
    sendBuf[i] = SOH;
    crc = crcDigest(crc, sendBuf[i++]);
    sendBuf[i] = length & 0xFF;
    if (sendBuf[i] == SOH)
    {
        sendBuf[++i] = SOH;
    }
    crc = crcDigest(crc, sendBuf[i++]);

    sendBuf[i] = length >> 8;
    if (sendBuf[i] == SOH)
    {
        sendBuf[++i] = SOH;
    }
    crc = crcDigest(crc, sendBuf[i++]);

    sendBuf[i] = type;
    if (sendBuf[i] == SOH)
    {
        sendBuf[++i] = SOH;
    }
    crc = crcDigest(crc, sendBuf[i++]);

    for (int j = 0; j < len; ++j)
    {
        sendBuf[i++] = buff[j];
        crc = crcDigest(crc, buff[j]);

        if (buff[j] == SOH)
        {
            sendBuf[i++] = buff[j];     // duplicated SOH
        }
    }

    sendBuf[i++] = crc;
    if (crc == SOH)
    {
        sendBuf[i++] = SOH;
    }

    int sendLength =  d_ptr->uart->write(sendBuf, i);

    if (sendLength != i)
    {
        // partially send
        return sendLength - 5;      // not an accurate return value, but should be fine
    }

    // all data is sent, return the buffer length
    return len;
}

void DataDispatcher::addDataDispatcher(DataDispatcher *dispatcher)
{
    if (dispatcher)
    {
        DataDispatcherPrivate::dispatchers.insert(dispatcher->getName(), dispatcher);
    }
}

DataDispatcher *DataDispatcher::getDataDispatcher(const QString &name)
{
    return DataDispatcherPrivate::dispatchers.value(name, NULL);
}

#define ArraySize(arr) ((int)(sizeof(arr)/sizeof(arr[0])))      // NOLINT
bool DataDispatcher::resetPacketPort(DataDispatcher::PacketType type)
{
    unsigned char crc = 0;
    unsigned char sendBuf[16];
    unsigned char frameData[]  = { 0, 0, PACKET_TYPE_CTRL, 0, PORT_CMD_RESET};
    unsigned short length = 1 + ArraySize(frameData) + 1;   // frame head byte + frame data field + crc byte
    frameData[0] = length & 0xFF;
    frameData[1] = length >> 8;
    frameData[3] = type;

    int i = 0;
    sendBuf[i] = SOH;
    crc = crcDigest(crc, sendBuf[i++]);

    for (int j = 0; j < ArraySize(frameData); j++)
    {
        sendBuf[i++] = frameData[j];
        crc = crcDigest(crc, frameData[j]);

        if (frameData[j] == SOH)
        {
            sendBuf[i++] = SOH;     // duplicated SOH
        }
    }

    sendBuf[i++] = crc;
    if (crc == SOH)
    {
        sendBuf[i++] = SOH;
    }

    return d_ptr->uart->write(sendBuf, i) == i;
}

bool DataDispatcher::setPacketPortBaudrate(DataDispatcher::PacketType type, DataDispatcher::PacketPortBaudrate baud)
{
    unsigned char crc = 0;
    unsigned char sendBuf[16];
    unsigned char frameData[]  = { 0, 0, PACKET_TYPE_CTRL, 0, PORT_CMD_BAUDRATE, 0};
    unsigned short length = 1 + ArraySize(frameData) + 1;   // frame head byte + frame data field + crc byte
    frameData[0] = length & 0xFF;
    frameData[1] = length >> 8;
    frameData[3] = type;
    frameData[5] = baud;

    int i = 0;
    sendBuf[i] = SOH;
    crc = crcDigest(crc, sendBuf[i++]);

    for (int j = 0; j < ArraySize(frameData); j++)
    {
        sendBuf[i++] = frameData[j];
        crc = crcDigest(crc, frameData[j]);

        if (frameData[j] == SOH)
        {
            sendBuf[i++] = SOH;     // duplicated SOH
        }
    }

    sendBuf[i++] = crc;
    if (crc == SOH)
    {
        sendBuf[i++] = SOH;
    }

    return d_ptr->uart->write(sendBuf, i) == i;
}

void DataDispatcher::dataArrived()
{
    d_ptr->readData();  // 读取数据到RingBuff中

    unsigned char packet[256];

    while (d_ptr->ringBuff.dataSize() >= MIN_PACKET_LEN)
    {
        if (d_ptr->ringBuff.at(0) != SOH)
        {
            // debug("discard (%s:%x)\n", qPrintable(getName()), ringBuff.at(0));
            d_ptr->ringBuff.pop(1);
            continue;
        }

        int len = (d_ptr->ringBuff.at(1) + (d_ptr->ringBuff.at(2) << 8));
        if ((len <= 0) || (len > static_cast<int>(sizeof(packet))))
        {
            d_ptr->ringBuff.pop(1);
            continue;
        }
        if (len > d_ptr->ringBuff.dataSize())  // 数据还不够，继续等待。
        {
            break;
        }

        // 数据包不会超过packet长度，当出现这种情况说明发生了不可预料的错误，直接丢弃该段数据。
        if (len > static_cast<int>(sizeof(packet)))
        {
            d_ptr->ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < len; i++)
        {
            packet[i] = d_ptr->ringBuff.at(0);
            d_ptr->ringBuff.pop(1);
        }

        if (d_ptr->checkPacketValid(packet, len))
        {
            d_ptr->handlePacket(&packet[3], len - 4);
        }
        else
        {
            debug("FCS error (%s)\n", qPrintable(d_ptr->name));
            d_ptr->ringBuff.pop(1);
        }
    }
}
