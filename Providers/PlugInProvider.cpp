/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/6/4
 **/

#include "PlugInProvider.h"
#include "Uart.h"
#include "IConfig.h"
#include "RingBuff.h"
#include "crc8.h"

#define SOH             (0x02)  // packet header
#define MIN_PACKET_LEN   5      // 最小数据包长度: SOH,Length,Type,FCS
#define RING_BUFFER_LENGTH 4096
#define MAXIMUM_PACKET_SIZE 256 // largest packet size, should be larger enough

class PlugInProviderPrivate
{
public:
    PlugInProviderPrivate(const QString &name, PlugInProvider *const q_ptr)
        : isLastSOHPaired(false),
          name(name), uart(new Uart(q_ptr))
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

        int startIndex = 0;
        bool isok;
        unsigned char v = ringBuff.head(isok);

        if (isok && len > 0)
        {
            if ((!isLastSOHPaired) && (v == SOH) && (buff[0] == SOH))  // SOH为数据包起始数据。
            {
                isLastSOHPaired = true;
                startIndex = 1;                  // 说明有连续两个SOH出现，需要丢弃一个。
            }
        }

        for (int i = startIndex; i < len; i++)
        {
            ringBuff.push(buff[i]);
            if (buff[i] != SOH)
            {
                isLastSOHPaired = false;
                continue;
            }

            isLastSOHPaired = false;
            i++;
            if (i >= len)
            {
                break;
            }

            if (buff[i] == SOH)                    // 剔除。
            {
                isLastSOHPaired = true;
                continue;
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
    }

    bool isLastSOHPaired; // 遗留在ringBuff最后一个数据（该数据为SOH）是否已经剃掉了多余的SOH
    QString name;
    Uart *uart;
    RingBuff<unsigned char> ringBuff;

private:
    PlugInProviderPrivate(const PlugInProviderPrivate &);  // use to pass the cpplint check only, no implementation
};

PlugInProvider::PlugInProvider(const QString &name, QObject *parent)
    : QObject(parent), d_ptr(new PlugInProviderPrivate(name, this))
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    d_ptr->initPort(portAttr);
    connect(d_ptr->uart, SIGNAL(activated(int)), this, SLOT(dataArrived()));
}

PlugInProvider::~PlugInProvider()
{
    delete d_ptr;
}

void PlugInProvider::dataArrived()
{
    d_ptr->readData(); // 读取数据到RingBuff中

    unsigned char packet[256];

    while (d_ptr->ringBuff.at(0) >= MIN_PACKET_LEN)
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
            break;
        }
        if (len > d_ptr->ringBuff.dataSize()) // 数据还不够，继续等待。
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
