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
#include "BLMProvider.h"

class DataDispatcher : public Provider
{
    Q_OBJECT
public:
    enum PacketType
    {
        REV_TYPE_TEMP = 0xF1,       // 温度
        REV_TYPE_SPO2 = 0xF2        // 血氧
    };

public: // Provider的接口。
    virtual bool attachParam(Param &param);

public:
    DataDispatcher();
    virtual ~DataDispatcher();

    //获取版本号
    virtual void sendVersion(void);

protected:
    // 接收数据
    void dataArrived();

    virtual void handlePacket(unsigned char *data, int len);
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    // 读取数据导RingBuff中
    void _readData(void);

    // 协议数据校验
    bool _checkPacketValid(const unsigned char *data, unsigned int len);

    bool _isLastSOHPaired; // 遗留在ringBuff最后一个数据（该数据为SOH）是否已经剃掉了多余的SOH。

private:
    QMap<PacketType, BLMProvider *> providers;
    static const int minPacketLen = 5;          // 最小数据包长度: Head,Length,Type,FCS
};
