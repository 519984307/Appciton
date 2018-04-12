#include "Debug.h"
#include "crc8.h"
#include "BLMProvider.h"

#include <sys/time.h>
#include <unistd.h>

#define SOH             (0x01)  // packet header

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
BLMProvider::BLMProvider(const QString &name)
    : Provider(name)
{
    _isLastSOHPaired = false;
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
BLMProvider::~BLMProvider()
{

}

#if 0
void BLMProvider::dataArrived()
{
    readData(); // 读取数据到RingBuff中

    while (!ringBuff.isEmpty()) {
        if (ringBuff.at(0) != SOH)
        { // 协议数据以SOH字节开始
            debug("Invalid packet header!\n");
            ringBuff.pop(1);
            continue;
        }

        unsigned int dataSize = ringBuff.dataSize();
        if (dataSize < minPacketLen)
        { // 数据不够
            break;
        }

        unsigned int len = ringBuff.at(1);
        if (SOH == len)
        { // 协议数据长度值不可能等于SOH
            debug("Invalid packet len: SOH!\n");
            ringBuff.pop(1);
            continue;
        }

        if (len > maxPacketLen)
        {
            debug("Invalid packet len: too long!\n");
            ringBuff.pop(2);
            continue;
        }

        unsigned int index = 0;
        unsigned char packet[maxPacketLen];
        packet[index++] = ringBuff.at(0);
        packet[index++] = ringBuff.at(1);

        unsigned int i = index;
        for (; (i < dataSize) && (index < len); i++)
        {
            if (ringBuff.at(i) == SOH)
            { // 移除多余的SOH转义字节
                i++;
            }

            packet[index++] = ringBuff.at(i);
        }

        if (index < len)
        { // 数据不够
            break;
        }

        ringBuff.pop(i);

        if (_checkPacketValid(packet, len))
        {
            handlePacket(packet, len);
        }
    }
}
#else

/***************************************************************************************************
 * 接收数据
 **************************************************************************************************/
void BLMProvider::dataArrived()
{
    _readData(); // 读取数据到RingBuff中

    unsigned char packet[570];

    while (ringBuff.dataSize() >= minPacketLen)
    {
        if (ringBuff.at(0) != SOH)
        {
            debug("discard (%s:%x)\n", qPrintable(getName()), ringBuff.at(0));
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
        if (len > (int)sizeof(packet))
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
            outHex(packet, len);
            debug("FCS error (%s)\n", qPrintable(getName()));
            ringBuff.pop(1);
        }
    }
}
#endif

/***************************************************************************************************
 * handlePacket : handle packet, should be called in derived
 **************************************************************************************************/
void BLMProvider::handlePacket(unsigned char *data, int len)
{
    if(data[0] == 0x11 && len > 1) //version data, all BLMProvidor share the same version respond code
    {
        versionInfoData.setRawData((const char *)(data + 1), len - 1);
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
    { // 协议数据以SOH字节开始
        debug("Invalid command!");
        return false;
    }

    int index = 0;
    unsigned char sendBuf[2 * len];

    sendBuf[index++] = data[0];
    for (unsigned int i = 1; i < len; i++)
    {
        if (SOH == data[i])
        { // 对SOH字节进行转义
            sendBuf[index++] = SOH;
        }

        sendBuf[index++] = data[i];
    }
    return writeData(sendBuf, index) == index;
}

/***************************************************************************************************
 * getVersionInfoData: get the version info data
 **************************************************************************************************/
QByteArray BLMProvider::getVersionInfoData()
{
    return versionInfoData;
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
    if (data[len-1] != crc)
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
