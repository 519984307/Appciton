#pragma once
#include "Provider.h"
#include "NIBPProviderIFace.h"

class BLMNIBPProvider: public Provider, public NIBPProviderIFace
{
public: // Provider的接口。
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

public: // NIBPProviderIFace的接口。
    // 起停测量。
    virtual void startMeasure(PatientType);
    virtual void stopMeasure(void);

    // 发送启动指令是否有该指令的应答。
    virtual bool needStartACK(void);
    virtual bool isStartACK(unsigned char *packet, int len);

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void);

    // 压力数据，不是压力数据返回-1。
    virtual int cuffPressure(unsigned char *packet, int len);

    // 测量是否结束。
    virtual bool isMeasureDone(unsigned char *packet, int len);

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char *packet, int len);

    // 发送获取结果请求。
    virtual void getResult(void);

    // 是否为结果包。
    virtual bool isResult(unsigned char *packet, int len,
            short &sys, short &dia, short &map, short &pr, NIBPOneShotType &err);

    BLMNIBPProvider();
    ~BLMNIBPProvider();

protected:
    virtual void disconnected(void) { }
    virtual void reconnected(void) { }

private:
    void _sendReset(void);
    bool _isInvalid(unsigned char *buff, int len);
    void _handlePacket(unsigned char *data, int len);

    static const int _minPacketLen = 6;      // 最小数据包长度。

    static const int _STX = 0x02;            // 数据包头。
    static const int _ETX = 0x03;            // 数据包尾。
    static const int _CR  = 0x0D;            // 数据包分割符。
};
