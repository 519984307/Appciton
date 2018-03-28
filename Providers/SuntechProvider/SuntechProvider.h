#pragma once
#include "Provider.h"
#include "NIBPProviderIFace.h"

class QTimer;
class SuntechProvider: public Provider, public NIBPProviderIFace
{
    Q_OBJECT

public:
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

    virtual void sendVersion(void) { }

public:
    // 起停测量。
    virtual void startMeasure(PatientType type);
    virtual void stopMeasure(void);

    // 设置预充气压力值。
    virtual void setInitPressure(short pressure);

    // 发送启动指令是否有该指令的应答。
    virtual bool needStartACK(void);
    virtual bool isStartACK(unsigned char *packet);

    // 发送停止指令是否有该指令的应答。
    virtual bool needStopACK(void);
    virtual bool isStopACK(unsigned char *packet);

    // 压力数据，不是压力数据返回-1。
    virtual short cuffPressure(unsigned char *packet);

    // 测量是否结束。
    virtual bool isMeasureDone(unsigned char *packet);

    // 是否为错误数据包。
    virtual NIBPOneShotType isMeasureError(unsigned char *packet);

    // 发送获取结果请求。
    virtual void getResult(void);

    // 是否为结果包。
    virtual bool isResult(unsigned char *packet,short &sys,
                          short &dia, short &map, short &pr, NIBPOneShotType &err);

    // 构造与析构。
    SuntechProvider();
    ~SuntechProvider();

protected:
    virtual void disconnected(void) { }
    virtual void reconnected(void) { }

private slots:
    void _getCuffPressure(void);

private:
    void _sendCmd(const unsigned char *data, unsigned int len);

    // 协议数据校验
    unsigned char _calcCheckSum(const unsigned char *data, int len);

    void _sendReset(void);

    bool _NIBPStart;

    static const int _minPacketLen = 3;      // 最小数据包长度。

    QTimer *_timer;
};
