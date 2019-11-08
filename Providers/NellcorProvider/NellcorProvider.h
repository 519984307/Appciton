/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/10/18
 **/

#pragma once
#include "Provider.h"
#include "SPO2ProviderIFace.h"
#include <QDate>
class NellcorSetProvider : public Provider, public SPO2ProviderIFace
{
public:
    virtual bool attachParam(Param *param);
    virtual void dataArrived(void);

    /**
     * @brief setSensitive  设置灵敏度--相当于设置血氧值的更新速率
     * @param sens 灵敏度级别
     */
    virtual void setSensitive(SPO2Sensitive sens);

    virtual int getSPO2WaveformSample(void)
    {
        return 76;
    }
    virtual int getSPO2BaseLine(void)
    {
        return 128;
    }
    virtual int getSPO2MaxValue(void)
    {
        return 256;
    }

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
     * @brief getSatSeconds  获取高低限值容忍的时间
     * @param pack  数据包
     */
    virtual void getSatSeconds(unsigned char *pack);
    /**
     * @brief sendCmdData  发送协议指令
     * @param cmdId  指令ID
     * @param data  数据包
     * @param len  数据包长度
     */
    virtual void sendCmdData(unsigned char cmdId, const unsigned char *data, unsigned int len);
    /**
     * @brief resetNecllcorModule 初始化复位nellcor模块
     */
    virtual void resetNecllcorModule(void);
    /**
     * @brief sendResetInfo  获取模块复位信息
     */
    virtual void sendResetInfo(void);
    /**
     * @brief sendHostSensorKeyInfo  获取主机传感器密钥信息
     */
    virtual void sendHostSensorKeyInfo(void);
    /**
     * @brief setHostSensorKeyInfo  设置主机传感器密钥信息
     * @param keyInfo  密钥信息
     */
    virtual void setHostSensorKeyInfo(const unsigned char *keyInfo);
    /**
     * @brief sendWaveUpdateInfo  获取波形更新率信息
     */
    virtual void sendWaveUpdateInfo(void);
    /**
     * @brief setWaveUpdateInfo  设置波形更新率信息
     * @param infoIndex  更新率选项
     */
    virtual void setWaveUpdateInfo(unsigned char infoIndex);
    /**
     * @brief sendWaveSelectInfo  获取波形选择信息
     */
    virtual void sendWaveSelectInfo(void);
    /**
     * @brief setWaveSelectInfo  选择波形
     * @param index 选择项
     */
    virtual void setWaveSelectInfo(unsigned char index);
    /**
     * @brief sendCClockInfo  获取C-CLOCK当前状态
     */
    virtual void sendCClockInfo(void);
    /**
     * @brief setCClockInfo  设置CCLOCK功能的禁用或实现
     * @param isEnabled  状态位
     */
    virtual void setCClockInfo(bool isEnabled);
    /**
     * @brief sendTrigerInfo  提示软件ECG触发器已经实现
     */
    virtual void sendTriggerInfo(void);
    /**
     * @brief disabledSatRateStatus  禁用该类报告，减少带宽占有率
     */
    virtual void disabledSatRateStatus(void);
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
    /**
     * @brief _analyzeHostSeneorKeyInfo  解析主机传感器密钥信息
     * @param pack  数据包
     */
    void _analyzeHostSeneorKeyInfo(unsigned char *pack);
    /**
     * @brief _analyzeVersionInfo  解析版本信息
     * @param pack  数据包
     */
    void _analyzeVersionInfo(unsigned char *pack);
    /**
     * @brief _analyzeSatRateInfo  解析血氧、脉率值及其状态、读取模块复位信息
     * @param pack  数据包
     */
    void _analyzeSatRateInfo(unsigned char *pack);
    /**
     * @brief _analyzeIrPerModulValue  解析IR百分比调制值
     * @param pack  数据包
     */
    void _analyzeIrPerModulValue(unsigned char *pack);
    /**
     * @brief _analyzeWaveUpdateInfo  解析波形更新率
     * @param pack  数据包
     */
    void _analyzeWaveUpdateInfo(unsigned char *pack);
    /**
     * @brief _analyzewaveSelectInfo  解析波形选择
     * @param pack  数据包
     */
    void _analyzewaveSelectInfo(unsigned char *pack);
    /**
     * @brief _analyzeCClockInfo  解析CCLOCK信息
     * @param pack  数据包
     */
    void _analyzeCClockInfo(unsigned char *pack);
    /**
     * @brief _analyzeSensorEventInfo  解析传感器时间信息
     * @param pack  数据包
     */
    void _analyzeSensorEventInfo(unsigned char *pack);
    // 协议数据校验
    unsigned char _calcCheckSum(const unsigned char *data, int len);

    static const int _minPacketLen = 5;      // 最小数据包长度。

    unsigned char _spo2ValueLimitStatus;  // 血氧高低限报警状态 0：无效 1：高限 2：低限

    unsigned char _prValueLimitStatus;  // 脉率高低限报警状态 0：无效 1：高限 2：低限

    bool _isInterfere;  // 是否有干扰

    bool _isLowPerfusionFlag;

    unsigned char _volumeWarnPriority;  // 声音报警优先级

    unsigned char _spo2Low;  // 血氧低限

    unsigned char _spo2High;  // 血氧高限

    unsigned short _prLow;  // 脉率低限

    unsigned short _prHigh;  // 脉率高限

    unsigned char _hostSensorKeyOne;  // 主机传感器密钥信息
    unsigned char _hostSensorKeyTwo;  // 主机传感器密钥信息
    unsigned char _hostSensorKeyThree;  // 主机传感器密钥信息
    unsigned char _hostSensorKeyFour;  // 主机传感器密钥信息

    QString _versionInfo;  // 版本信息

    bool _isreset;  // 模块是否复位

    unsigned char _satSeconds;  // 高低限值容忍时间

    unsigned char _irPerModulValue;  // IR百分比调制值

    WaveUpdateType _waveUpdateType;  // 波形更新速率

    unsigned char _waveSelectType;  // 波形选择

    bool _isEnableCClock;  // 是否使能CCLCOK功能

    bool _isEnableSensorEventRecord;  // 是否启用传感器事件记录功能

    QDateTime _dataTime;    // 日期时间

    // 传感器事件状态
    unsigned char _sensorEventStatus;  // 传感器事件状态
    unsigned char _sensorEventType;  // 传感器事件类型
    unsigned char _sensorType;  // 传感器类型
};
