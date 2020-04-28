/**
 ** This file is part of the Project project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2020/4/28
 **/

#include "SmartIBPProvider.h"
#include <QDebug>

#define MAX_PACKET_LENGTH       12      /* maximum packet length */
#define MIN_PARSE_LENGTH        2       /* minimum length of data to start parse */
#define FRAME_HEAD              0xFF    /* frame head */
#define MODULE_CMD_ZERO         0x00    /* channel zero */
#define MODULE_CMD_CALIBRATE    0x01    /* channel calibrate */

#define MODULE_IBP_CHN_1        0x00    /* IBP channel 1 */
#define MODULE_IBP_CHN_2        0x01    /* IBP channel 2 */

#define MODULE_ID               0xF9    /* moudle ID */
#define MODULE_CMD_RSP          0xFE    /* command response */

#define DATA_TYPE_SYS       0x00
#define DATA_TYPE_DIA       0x01
#define DATA_TYPE_MAP       0x02
#define DATA_TYPE_PR        0x03

struct IBPChannelData
{
    quint16 sys;
    quint16 dia;
    quint16 map;
    quint16 pr;
    bool sensorOff;
};

class SmartIBPProviderPrivate
{
public:
    explicit SmartIBPProviderPrivate(SmartIBPProvider * const q_ptr)
        : q_ptr(q_ptr)
    {
        chn1Data.sys = 0;
        chn1Data.dia = 0;
        chn1Data.map = 0;
        chn1Data.pr = 0;
        chn1Data.sensorOff = true;

        chn2Data.sys = 0;
        chn2Data.dia = 0;
        chn2Data.map = 0;
        chn2Data.pr = 0;
        chn2Data.sensorOff = true;
    }

    /**
     * @brief calcCheckSum calculate the checksum
     * @param data the data to calculate checksum
     * @param len the data len
     * @return the checksum
     * @note when the checksum is 0xFF, need to minus 1 to avoid conflcting
     * the FRAME_HEAD_BYTE
     */
    quint8 calcCheckSum(quint8 *data, int len)
    {
        quint8 sum = 0;
        for (int i = 0; i < len; i++)
        {
            sum += data[i];
        }

        return sum == FRAME_HEAD ? sum - 1 : sum;
    }

    /**
     * @brief handlePacket handle packet data
     * @param data the packet data buffer
     * @param len length of the packet data
     */
    void handlePacket(const quint8 *data, int len);

    /**
     * @brief setChannelType set the channel data
     * @param chn the channel data struct
     * @param type the data type
     * @param val the chanel value
     */
    void setChannelData(IBPChannelData *chn, quint8 type, quint16 val)
    {
        switch (type) {
        case DATA_TYPE_SYS:
            chn->sys = val;
            break;
        case DATA_TYPE_DIA:
            chn->dia = val;
            break;
        case DATA_TYPE_MAP:
            chn->map = val;
            break;
        case DATA_TYPE_PR:
            chn->pr = val;
            break;
        default:
            break;
        }
    }

    /**
     * @brief sendCmd send command to the module
     * @param cmdByte the command byte
     * @param the channel number
     */
    void sendCmd(quint8 cmdByte, quint8 chn)
    {
        unsigned char buf[6];
        buf[0] = FRAME_HEAD;
        buf[1] = 0x04;
        buf[2] = MODULE_ID;
        buf[3] = cmdByte;
        buf[4] = chn;
        buf[5] = calcCheckSum(&buf[1], buf[1]);
        q_ptr->writeData(buf, sizeof(buf));
    }

    SmartIBPProvider * const q_ptr;
    IBPChannelData chn1Data;        /* data of channel 1 */
    IBPChannelData chn2Data;        /* data of channel 2 */
};

SmartIBPProvider::SmartIBPProvider(const QString &port)
    :Provider("SMART_IBP"), pimpl(new SmartIBPProviderPrivate(this))
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);
}

SmartIBPProvider::~SmartIBPProvider()
{
}

void SmartIBPProvider::sendVersion()
{
    /* no supported */
}

void SmartIBPProvider::disconnected()
{
}

void SmartIBPProvider::reconnected()
{
}

int SmartIBPProvider::getIBPWaveformSample()
{
    return 250;
}

int SmartIBPProvider::getIBPBaseLine()
{
    return 0;
}

int SmartIBPProvider::getIBPMaxWaveform()
{
    return 10000;
}

void SmartIBPProvider::dataArrived()
{
    readData();
    quint8 buf[MAX_PACKET_LENGTH];
    while (ringBuff.dataSize() > MIN_PARSE_LENGTH)
    {
        /* find frame head */
        if (ringBuff.at(0) != FRAME_HEAD)
        {
            ringBuff.pop(1);
            continue;
        }

        quint8 length = ringBuff.at(1);
        if (length != 0x04 && length != 0x0A)
        {
            /* possible length field value of packet from module is 0x04 0r 0x0A */
            qDebug() << "Invalid Packet length:" << length;
            ringBuff.pop(1);
            continue;
        }

        if (ringBuff.dataSize() < length)
        {
            /* no enough data */
            break;
        }

        ringBuff.copy(0, buf, length + 2);
        quint8 checksum = pimpl->calcCheckSum(buf + 1, length);
        if (checksum == buf[length + 1])
        {
            /* packet is valid, pop the packet data */
            ringBuff.pop(length + 2);

            pimpl->handlePacket(buf + 2, length - 1);
        }
        else
        {
            qDebug() << this->getName() << "CheckSum failed!";
            ringBuff.pop(1);
        }
    }
}

void SmartIBPProviderPrivate::handlePacket(const quint8 *data, int len)
{
    switch (data[0])
    {
    case MODULE_ID:
    {
        /* periodic data */
        bool ch1SensorOff = data[1] & 0x40;
        quint16 ch1Wave = ((data[1] & 0x1F) << 7) + (data[2] & 0x7F);
        quint8 ch1Type = (data[3] & 0x60) >> 5;
        quint16 ch1Val = ((data[3] &0x1F) << 7) + (data[4] & 0x7F);

        chn1Data.sensorOff = ch1SensorOff;
        setChannelData(&chn1Data, ch1Type, ch1Val);

        bool ch2SensorOff = data[5] & 0x40;
        quint16 ch2Wave = ((data[5] & 0x1F) << 7) + (data[6] & 0x7F);
        quint8 ch2Type = (data[7] & 0x60) >> 5;
        quint16 ch2Val = ((data[7] &0x1F) << 7) + (data[8] & 0x7F);

        chn2Data.sensorOff = ch2SensorOff;
        setChannelData(&chn2Data, ch2Type, ch2Val);
    }
        break;
    case MODULE_CMD_RSP:
    {
        /* comman respone */
        quint8 cmd = data[2];
        quint8 result = data[4];
        if (result == 0)
        {
            qDebug() << Q_FUNC_INFO << "CMD" << cmd << "success!";
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "CMD" << cmd << "fail!";
        }
    }
        break;
    default:
        /* unknow packet id */
        qDebug() << Q_FUNC_INFO << "unknown packet type";
        break;
    }
}
