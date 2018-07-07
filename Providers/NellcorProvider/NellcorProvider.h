#pragma once
#include "Provider.h"
#include "SPO2ProviderIFace.h"

class NellcorSetProvider : public Provider, public SPO2ProviderIFace
{
public:
    virtual bool attachParam(Param &param);
    virtual void dataArrived(void);

    /**
     * @brief setSensitive  设置灵敏度--相当于设置血氧值的更新速率
     * @param sens 灵敏度级别
     */
    virtual void setSensitive(SPO2Sensitive sens);

    virtual int getSPO2WaveformSample(void) {return 62;}
    virtual int getSPO2BaseLine(void) {return 128;}
    virtual int getSPO2MaxValue(void) {return 256;}

    /**
     * @brief sendVersion  获取版本号
     */
    virtual void sendVersion(void);

    /**
     * @brief setWarnLimitValue  设置报警限值
     * @param spo2Low  血氧低限
     * @param spo2High  血氧高限
     * @param prLow  脉率低限
     * @param prHigh  脉率高限
     */
    virtual void setWarnLimitValue(char spo2Low, char spo2High, short prLow, short prHigh);

    /**
     * @brief getWarnLimitValue 查询报警限值
     */
    virtual void getWarnLimitValue(void);
    /**
     * @brief isResultSPO2PR  是否是血氧值与脉率值
     * @param packet  数据包
     * @return  是返回true  否返回false
     */
    virtual bool isResultSPO2PR(unsigned char *packet);
    /**
      * @brief isResult_BAR  描记波 棒图 报警音
      * @param packet  数据包
      * @return  是返回true  否返回false
      */
    virtual bool isResult_BAR(unsigned char *packet);
    /**
      * @brief isStatus  接收传感器的状态
      * @param packet 数据包
      * @return 是返回true  否返回false
      */
    virtual bool isStatus(unsigned char *packet);
    /**
     * @brief setSatSeconds 设置过高低限值的容忍时间
     * @param type 时间类型
     */
    virtual void setSatSeconds(Spo2SatSecondsType type);
    /**
     * @brief sendCmdData  发送协议指令
     * @param cmdId  指令ID
     * @param data  数据包
     * @param len  数据包长度
     */
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);

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

    static const int _minPacketLen = 5;      // 最小数据包长度。

    unsigned char _spo2ValueLimitStatus;  //血氧高低限报警状态 0：无效 1：高限 2：低限

    unsigned char _prValueLimitStatus;  //脉率高低限报警状态 0：无效 1：高限 2：低限

    bool _isInterfere;  //是否有干扰

    bool _isLowPerfusionFlag;

    unsigned char _volumeWarnPriority;  //声音报警优先级

    unsigned char _spo2Low;

    unsigned char _spo2High;

    unsigned short _prLow;

    unsigned short _prHigh;
};
