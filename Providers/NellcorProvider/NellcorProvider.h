#pragma once
#include "Provider.h"
#include "SPO2ProviderIFace.h"
#include <QScopedPointer>

class NellcorSetProviderPrivate;

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

class NellcorSetProvider : public Provider, public SPO2ProviderIFace
{
public:
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

    virtual void setSensitive(SPO2Sensitive /*sens*/) { }

    virtual int getSPO2WaveformSample(void) {return 62;}
    virtual int getSPO2BaseLine(void) {return 128;}
    virtual int getSPO2MaxValue(void) {return 256;}

    /**
     * @brief sendVersion  获取版本号
     */
    virtual void sendVersion(void);
    /**
     * @brief sendCmd  发送指令ID方法，查询或设置底部模块
     * @param cmdKey  指令密钥
     * @param data  待发送的数据
     * @param len  待发送数据的长度
     * @return 发送成功返回 true；发送失败返回 false
     */
    bool sendCmd(unsigned char cmdKey, const unsigned char *data, unsigned int len);

    // 设置灵敏度和FastSat。
    virtual void setSensitivityFastSat(SensitivityMode mode, bool fastSat);

    // 设置平均时间。
    virtual void setAverageTime(AverageTime mode);

    // 设置SMart Tone使能选项。
    virtual void setSmartTone(bool enable);

public:
    // 构造与构造
    NellcorSetProvider();
    virtual ~NellcorSetProvider();

protected:
    virtual void disconnected(void);
    virtual void reconnected(void);

private:
    void handlePacket(unsigned char *data, int len);

    void _sendCmd(const unsigned char *data, unsigned int len);

    // 协议数据校验
    unsigned char _calcCheckSum(const unsigned char *data, int len);

    static const int _minPacketLen = 14;      // 最小数据包长度。

    bool _isLowPerfusionFlag;                     // 低弱冠注

    QScopedPointer<NellcorSetProviderPrivate> d_ptr;
};
