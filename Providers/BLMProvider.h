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

struct BlmCmd
{
    BlmCmd() : cmdLen(0)
    {
        memset(cmd, 0, 150);
    }
    unsigned char cmd[150];
    unsigned int cmdLen;
    void reset()
    {
        memset(cmd, 0, 150);
        cmdLen = 0;
    }
};

/**
 * @brief The BLMProviderUpgradeIface class BLMProvider upgrade interface
 */
class BLMProviderUpgradeIface
{
public:
    virtual ~BLMProviderUpgradeIface(){}

    virtual void handlePacket(unsigned char * data, int len) = 0;
};

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

    // 接收数据
    void dataArrived(unsigned char *buff, unsigned int length);

    /**
     * @brief setUpgradeIface set the upgrade iface to handle the upgrade packet
     * @note set iface to NULL to stop upgrade
     * @param iface the upgrade iface
     */
    void setUpgradeIface(BLMProviderUpgradeIface *iface);

    /**
     * @brief findProvider find a blmprovider in the system base on the provider name
     * @param name the provider name
     * @return pointer to the provider if found, otherwise, NULL
     */
    static BLMProvider *findProvider(const QString &name);

    /**
     * @brief resetTimer 复位定时器
     */
    void resetTimer();

protected:
    // 接收数据
    void dataArrived();

    // 协议命令解析
    virtual void handlePacket(unsigned char */*data*/, int /*len*/);

    // 发送通信中断
    virtual void sendDisconnected(){}

protected slots:
    void noResponseTimeout();

private:
    // 发送数据
    bool _sendData(const unsigned char *data, unsigned int len);

    // 协议数据校验
    bool _checkPacketValid(const unsigned char *data, unsigned int len);

    // 读取数据导RingBuff中
    void _readData(void);

    void _readData(unsigned char *buff, unsigned int len);

    bool _isLastSOHPaired; // 遗留在ringBuff最后一个数据（该数据为SOH）是否已经剃掉了多余的SOH。

    BLMProviderUpgradeIface *upgradeIface;

    static QMap<QString, BLMProvider*> providers;

    QTimer *rxdTimer;           // 发送命令定时器
    int _noResponseCount;              // 重发次数
    unsigned char _cmdId;        // 命令帧数
    BlmCmd _blmCmd;

protected:
    static const int HeadLen = 4;               // 包头长度: Head,Length,FCS
    static const int minPacketLen = 5;          // 最小数据包长度: Head,Length,Type,FCS
    static const int maxPacketLen = (1 << 9);   // 最大数据包长度: 1字节最大表示范围。
};

