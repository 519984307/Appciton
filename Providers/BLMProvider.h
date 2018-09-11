/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/4
 **/

#pragma once
#include "Provider.h"

/***************************************************************************************************
 * BLM 通信协议(数据收发处理)
 **************************************************************************************************/
class BLMProvider : public Provider
{
    Q_OBJECT
public:
    explicit BLMProvider(const QString &name);
    virtual ~BLMProvider();

    // 发送协议命令
    bool sendCmd(unsigned char cmdId, const unsigned char *data, unsigned int len);

    // 发送数据
    bool _sendData(const unsigned char *data, unsigned int len);


    // 接收数据
    void dataArrived(unsigned char *buff, unsigned int length);

#ifdef CONFIG_UNIT_TEST
    friend class TestECGParam;
#endif
protected:
    // 接收数据
    void dataArrived();

    // 协议命令解析
    virtual void handlePacket(unsigned char */*data*/, int /*len*/);

private:
    // 发送数据
//    void _sendData(const unsigned char *data, unsigned int len);

    // 协议数据校验
    bool _checkPacketValid(const unsigned char *data, unsigned int len);

    // 读取数据导RingBuff中
    void _readData(void);

    void _readData(unsigned char *buff, unsigned int len);

    bool _isLastSOHPaired; // 遗留在ringBuff最后一个数据（该数据为SOH）是否已经剃掉了多余的SOH。


protected:
    static const int HeadLen = 4;               // 包头长度: Head,Length,FCS
    static const int minPacketLen = 5;          // 最小数据包长度: Head,Length,Type,FCS
    static const int maxPacketLen = (1 << 9);   // 最大数据包长度: 1字节最大表示范围。
};

