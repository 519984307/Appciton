#pragma once
#include "Provider.h"
#include "SPO2ProviderIFace.h"

class BLMSPO2Provider: public Provider, public SPO2ProviderIFace
{
public: // Provider的接口。
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

public: // SPO2ProviderIFace 的接口。
    virtual void setSensitive(SPO2Sensitive /*sens*/) { }
    virtual int getSPO2WaveformSample(void) {return 60;}
    virtual int getSPO2BaseLine(void) {return (50 * 100);}      //由于0～100的基线为飘，放大100倍，使其放大倍数为整数
    virtual int getSPO2MaxValue(void) {return (100 * 100);}

    //获取版本号
    virtual void sendVersion(void) { }

    BLMSPO2Provider();
    ~BLMSPO2Provider();

protected:
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    static const int _packetLen = 9;      // 数据包长度。
};
