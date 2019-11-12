/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/9/27
 **/

#pragma once
#include "Provider.h"
#include "SPO2ProviderIFace.h"

enum sensityList
{
    SENSITY_MAX,
    SENSITY_NORMAL,
    APOD,
};

enum fastsatList
{
    FASTSAT_OFF,
    FASTSAT_ON,
};

enum avgTimeList
{
    SECOND2_4,
    SECOND4_6,
    SECOND8,
    SECOND10,
};

class MasimoSetProvider : public Provider, public SPO2ProviderIFace
{
public:
    virtual bool attachParam(Param *param);
    virtual void dataArrived(void);
    virtual void dataArrived(unsigned char *buf, unsigned int length);

    virtual void setSensitive(SPO2Sensitive /*sens*/) { }

    virtual int getSPO2WaveformSample(void) {return 62;}
    virtual int getSPO2BaseLine(void) {return 128;}
    virtual int getSPO2MaxValue(void) {return 128;}

    //获取版本号
    virtual void sendVersion(void) { }

    // 设置灵敏度和FastSat。
    virtual void setSensitivityFastSat(SensitivityMode mode, bool fastSat);

    // 设置平均时间。
    virtual void setAverageTime(AverageTime mode);

    // 设置SMart Tone使能选项。
    virtual void setSmartTone(bool enable);

public:
    // 构造与构造
    MasimoSetProvider();
    virtual ~MasimoSetProvider();

protected:
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    void handlePacket(unsigned char *data, int len);

    void _sendCmd(const unsigned char *data, unsigned int len);

    // 读取数据导RingBuff中
    void _readData(unsigned char *buff, unsigned int len);

    // 协议数据校验
    unsigned char _calcCheckSum(const unsigned char *data, int len);

    static const int _minPacketLen = 14;      // 最小数据包长度。

    bool _isLowPerfusionFlag;                     // 低弱冠注
};
