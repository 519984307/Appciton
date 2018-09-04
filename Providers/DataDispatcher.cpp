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
#include "TEMPParam.h"
#include "SPO2Param.h"
#include "T5Provider.h"
#include "S5Provider.h"
#include "Debug.h"
#include "crc8.h"

#define SOH             (0x02)  // packet header

bool DataDispatcher::attachParam(Param &param)
{
    if (param.getParamID() == PARAM_TEMP)
    {
        T5Provider *provider = new T5Provider();
        tempParam.setProvider(provider);
        providers.insert(REV_TYPE_TEMP, provider);
        return true;
    }
    else if (param.getParamID() == PARAM_SPO2)
    {
        S5Provider *provider = new S5Provider();
        spo2Param.setProvider(provider);
        providers.insert(REV_TYPE_SPO2, provider);
        return true;
    }
    return false;
}

DataDispatcher::DataDispatcher() : Provider("DataDispatcher"), _isLastSOHPaired(0)
{
    UartAttrDesc portAttr(115200, 8, 'N', 1);
    initPort(portAttr);
}

DataDispatcher::~DataDispatcher()
{
}

void DataDispatcher::sendVersion()
{
}

void DataDispatcher::dataArrived()
{
    _readData(); // 读取数据到RingBuff中

    unsigned char packet[570];

    while (ringBuff.dataSize() >= minPacketLen)
    {
        if (ringBuff.at(0) != SOH)
        {
            // debug("discard (%s:%x)\n", qPrintable(getName()), ringBuff.at(0));
            ringBuff.pop(1);
            continue;
        }

        int len = (ringBuff.at(1) + (ringBuff.at(2) << 8));
        if ((len <= 0) || (len > 570))
        {
            ringBuff.pop(1);
            break;
        }
        if (len > ringBuff.dataSize()) // 数据还不够，继续等待。
        {
            break;
        }

        // 数据包不会超过packet长度，当出现这种情况说明发生了不可预料的错误，直接丢弃该段数据。
        if (len > static_cast<int>(sizeof(packet)))
        {
            ringBuff.pop(1);
            continue;
        }

        // 将数据包读到buff中。
        for (int i = 0; i < len; i++)
        {
            packet[i] = ringBuff.at(0);
            ringBuff.pop(1);
        }

        if (_checkPacketValid(packet, len))
        {
            handlePacket(&packet[3], len - 4);
        }
        else
        {
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

void DataDispatcher::handlePacket(unsigned char *data, int len)
{
    switch (data[0])
    {
    case REV_TYPE_TEMP:
        providers[REV_TYPE_TEMP]->dataArrived(data + 1, len - 1);
        break;
    case REV_TYPE_SPO2:
        providers[REV_TYPE_SPO2]->dataArrived(data + 1, len - 1);
        break;
    default:
        break;
    }
}

void DataDispatcher::disconnected()
{
}

void DataDispatcher::reconnected()
{
}

void DataDispatcher::_readData()
{
    unsigned char buff[ringBuffLen];
    int len = uart->read(buff, ringBuffLen);
    if (len <= 0)
    {
        return;
    }

    int startIndex = 0;
    bool isok;
    unsigned char v = ringBuff.head(isok);

    if (isok && len > 0)
    {
        if ((!_isLastSOHPaired) && (v == SOH) && (buff[0] == SOH))  // SOH为数据包起始数据。
        {
            _isLastSOHPaired = true;
            startIndex = 1;                  // 说明有连续两个SOH出现，需要丢弃一个。
        }
    }

    for (int i = startIndex; i < len; i++)
    {
        ringBuff.push(buff[i]);
        if (buff[i] != SOH)
        {
            _isLastSOHPaired = false;
            continue;
        }

        _isLastSOHPaired = false;
        i++;
        if (i >= len)
        {
            break;
        }

        if (buff[i] == SOH)                    // 剔除。
        {
            _isLastSOHPaired = true;
            continue;
        }

        ringBuff.push(buff[i]);
    }
}

bool DataDispatcher::_checkPacketValid(const unsigned char *data, unsigned int len)
{
    if ((NULL == data) || (len < minPacketLen))
    {
        debug("Invalid packet!\n");
        debug("%s: FCS not match!\n", qPrintable(getName()));
        return false;
    }

    unsigned char crc = calcCRC(data, (len - 1));
    if (data[len - 1] != crc)
    {
//        outHex(data, (int)len);
//        debug("%s: FCS not match : %x!\n", qPrintable(getName()), crc);
        return false;
    }

    return true;
}
