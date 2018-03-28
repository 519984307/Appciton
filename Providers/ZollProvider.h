#pragma once
#include "Provider.h"

/***************************************************************************************************
 * Zoll 通信协议(数据收发处理)
 **************************************************************************************************/
class ZollProvider : public Provider
{
    Q_OBJECT
public:
    ZollProvider(const QString &name);
    virtual ~ZollProvider();

    // 发送协议消息
    void sendMsg(unsigned char groupId, unsigned char type, unsigned char sequence,
                 const unsigned char *data, unsigned int len);

protected:
    // 接收数据
    void dataArrived();

    // 消息处理
    virtual void handlePacket(const unsigned char */*data*/, unsigned int /*len*/)
    {
    }

private:
    // 发送数据
    void _sendData(const unsigned char *data, unsigned int len);

    // 数据校验
    bool _checkPacketValid(const unsigned char *data, unsigned int len);

protected:
    static const int minPacketLen = 6;          // 最小数据包长度: SOH(1),Control(2),Length(2),FCS(1)
    static const int maxPacketLen = (1 << 16);  // 最大数据包长度: 2字节最大表示范围。
};
