/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/4
 **/

#include "Debug.h"
#include "crc8.h"
#include "BLMProvider.h"

#include <sys/time.h>
#include <unistd.h>

#define SOH             (0x01)  // packet header
#define NACK            (0x00)  // 错误应答
#define ACK             (0x01)  // 正确应答

QMap<QString, BLMProvider *> BLMProvider::providers;

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
BLMProvider::BLMProvider(const QString &name)
    : Provider(name), upgradeIface(NULL),
      rxdTimer(NULL), _noResponseCount(0),
      _cmdId(-1)
{
    providers.insert(name, this);
    _isLastSOHPaired = false;
    rxdTimer = new QTimer();
    connect(rxdTimer, SIGNAL(timeout()), this, SLOT(noResponseTimeout()));
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
BLMProvider::~BLMProvider()
{
}

/***************************************************************************************************
 * 接收数据
 **************************************************************************************************/
void BLMProvider::dataArrived()
{
    _readData();  // 读取数据到RingBuff中

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
        if (len > ringBuff.dataSize())  // 数据还不够，继续等待。
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
            if (upgradeIface)
            {
                upgradeIface->handlePacket(&packet[3], len - 4);
            }
            else
            {
                handlePacket(&packet[3], len - 4);
            }
        }
        else
        {
            // outHex(packet, len);
            debug("FCS error (%s)", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

/***************************************************************************************************
 * handlePacket : handle packet, should be called in derived
 **************************************************************************************************/
void BLMProvider::handlePacket(unsigned char *data, int len)
{
    if (data[0] == _cmdId + 1)
    {
        resetTimer();
    }

    // version data, all BLMProvidor share the same version respond code
    if (data[0] == 0x11 && len >= 80 + 1)   // version info length + data packet head offset
    {
        const char *p = reinterpret_cast<char *>(&data[1]);
        versionInfo.clear();
        versionInfo.append(p);          // software git version
        versionInfo.append(" ");
        p += (30 + 1);                  // software git version offset + data packet head offset

        versionInfo.append(p);          // build date
        versionInfo.append(" ");
        p += (15 + 1);                  // build date offset + data packet head offset

        p += (15 + 1);                  // build time offset + data packet head offset

        versionInfo.append(p);          // hardware version
        versionInfo.append(" ");
        p += (1 + 1);                   // hardware version offset + data packet head offset

        versionInfo.append(p);          // bootloader git version

        if (len == 160 + 1)             // 适用于N5从片版本信息
        {
            const char *p = reinterpret_cast<char *>(&data[1 + 80]);
            versionInfoEx.clear();
            versionInfoEx.append(p);    // software git version
            versionInfoEx.append(" ");
            p += (30 + 1);              // software git version offset + data packet head offset

            versionInfoEx.append(p);    // build date
            versionInfoEx.append(" ");
            p += (15 + 1);              // build date offset + data packet head offset

            p += (15 + 1);              // build time offset + data packet head offset

            versionInfoEx.append(p);    // hardware version
            versionInfoEx.append(" ");
            p += (1 + 1);               // hardware version offset + data packet head offset

            versionInfoEx.append(p);    // bootloader git version
        }
    }
}

void BLMProvider::noResponseTimeout()
{
    _noResponseCount++;
    if (_noResponseCount < 2)
    {
        _sendData(_blmCmd.cmd, _blmCmd.cmdLen);
    }
    else
    {
        qWarning() << Q_FUNC_INFO << getName() << "no response for cmd: " << showbase << hex << _cmdId;
        sendDisconnected();
        resetTimer();
    }
}

/***************************************************************************************************
 * 发送数据
 **************************************************************************************************/
bool BLMProvider::_sendData(const unsigned char *data, unsigned int len)
{
    if ((NULL == data) || (0 == len))
    {
        return false;
    }

    if (SOH != data[0])
    {
        // 协议数据以SOH字节开始
        debug("Invalid command!");
        return false;
    }

    int index = 0;
    unsigned char sendBuf[2 * len];

    sendBuf[index++] = data[0];
    for (unsigned int i = 1; i < len; i++)
    {
        if (SOH == data[i])
        {
            // 对SOH字节进行转义
            sendBuf[index++] = SOH;
        }

        sendBuf[index++] = data[i];
    }
    return writeData(sendBuf, index) == index;
}



void BLMProvider::dataArrived(unsigned char *buff, unsigned int length)
{
    _readData(buff, length);   // 读取数据到RingBuff中

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
        if (len > ringBuff.dataSize())  // 数据还不够，继续等待。
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
            if (upgradeIface)
            {
                upgradeIface->handlePacket(&packet[3], len - 4);
            }
            else
            {
                handlePacket(&packet[3], len - 4);
            }
        }
        else
        {
            // outHex(packet, len);
            debug("FCS error (%s)", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}

void BLMProvider::setUpgradeIface(BLMProviderUpgradeIface *iface)
{
    if (iface == NULL)
    {
        stopCheckConnect(false);
    }
    else
    {
        stopCheckConnect(true);
    }
    upgradeIface = iface;
}

BLMProvider *BLMProvider::findProvider(const QString &name)
{
    return providers.value(name, NULL);
}

void BLMProvider::resetTimer()
{
    rxdTimer->stop();
    _blmCmd.reset();
    _noResponseCount = 0;
    _cmdId = -1;
}

/***************************************************************************************************
 * 发送协议命令
 * 参数:
 *      cmdId: 命令类型
 *      data:  命令参数内容指针，可以为空(NULL）。
 *      len:   命令参数长度，单位:字节。若data指针为空，则len为0。
 **************************************************************************************************/
bool BLMProvider::sendCmd(unsigned char cmdId, const unsigned char *data, unsigned int len)
{
    // 数据包长度包括包头、包长、帧类型、数据和校验和，即：数据长度＋4。
    unsigned int cmdLen = len + minPacketLen;
    unsigned char cmdBuf[maxPacketLen] = {0};
    if (cmdLen > maxPacketLen)
    {
        debug("Comand too long!");
        return false;
    }

    cmdBuf[0] = SOH;
    cmdBuf[1] = (cmdLen & 0xFF);
    cmdBuf[2] = ((cmdLen >> 8) & 0xFF);
    cmdBuf[3] = cmdId;
    if ((NULL != data) && (0 != len))
    {
        qMemCopy(cmdBuf + 4, data, len);
    }

    cmdBuf[cmdLen - 1] = calcCRC(cmdBuf, (cmdLen - 1));

    if (cmdId != NACK && cmdId != ACK && upgradeIface == NULL)
    {
        _cmdId = cmdId;
        _blmCmd.reset();
        for (unsigned int i = 0; i < cmdLen; i++)
        {
            _blmCmd.cmd[i] = cmdBuf[i];
        }
        _blmCmd.cmdLen = cmdLen;
        rxdTimer->start(1000);
    }
    return _sendData(cmdBuf, cmdLen);
}

/***************************************************************************************************
 * 协议数据校验
 * 返回:
 *   true - 校验成功; false - 校验失败。
 **************************************************************************************************/
bool BLMProvider::_checkPacketValid(const unsigned char *data, unsigned int len)
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

/***************************************************************************************************
 * 读取协议数据
 **************************************************************************************************/
void BLMProvider::_readData(void)
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

void BLMProvider::_readData(unsigned char *buff, unsigned int len)
{
    if (!buff)
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

    for (unsigned int i = startIndex; i < len; i++)
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
