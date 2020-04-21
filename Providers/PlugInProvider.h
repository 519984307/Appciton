/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/6/4
 **/

#pragma once
#include <QObject>
#include "SystemManagerInterface.h"

class PlugInProviderPrivate;
class Provider;
class PlugInProvider : public QObject
{
    Q_OBJECT
public:
    enum PlugInType
    {
        PLUGIN_TYPE_INVALID = -1,    // invalid packet type
        PLUGIN_TYPE_CTRL = 0xF0,     // control packet type
        PLUGIN_TYPE_SPO2 = 0xF2,
        PLUGIN_TYPE_CO2,
        PLUGIN_TYPE_IBP
    };

    enum PacketPortBaudrate
    {
        BAUDRATE_9600,
        BAUDRATE_19200,
        BAUDRATE_28800,
        BAUDRATE_38400,
        BAUDRATE_57600,
        BAUDRATE_115200,
        BAUDRATE_230400,
        BAUDRATE_NR
    };

    struct PlugInInfo
    {
        PlugInInfo() : plugIn(NULL), plugInType(PLUGIN_TYPE_INVALID) {}
        PlugInProvider *plugIn;
        PlugInType plugInType;
    };

public:
    explicit PlugInProvider(const QString &name, QObject *parent = NULL);
    virtual ~PlugInProvider();

    QString getName() const;
    /**
     * @brief connectProvider connect data dispatcher to the provider
     * @param type the plugin type
     * @param provider pointer to the provider
     */
    void connectProvider(PlugInType type, Provider *provider);

    /**
     * @brief sendData send data through the data dispatcher
     * @param buff ther data buffer
     * @param len the buffer length
     * @return the length of data have beed send
     */
    int sendData(PlugInType type, const unsigned char *buff, int len);
    /**
     * @brief addPlugInProvider add a plugin provider to the system
     * @param PlugInProvider the new added PlugInProvider
     */
    static void addPlugInProvider(PlugInProvider *plugInProvider);

    /**
     * @brief getPlugInProvider get a plugin provider from the system base on the name
     * @param name the plugin provider name
     * @return the plugin provider object or null is not exist
     */
    static PlugInProvider *getPlugInProvider(const QString &name);

    virtual void sendVersion(void) {}

    virtual void disconnected(void) {}          // 模块连接恢复时回调，之类实现。
    virtual void reconnected(void) {}        // 模块连接恢复时回调，之类实现。

    /**
     * @brief setPacketPortBaudrate 设置模块波特率
     * @param type 插件类型
     * @param baud 波特率
     * @return
     */
    bool setPacketPortBaudrate(PlugInType type, PlugInProvider::PacketPortBaudrate baud);

    /**
     * @brief updateUartBaud 更新串口波特率
     * @param baud 波特率
     */
    void updateUartBaud(unsigned int baud);

protected:
    void timerEvent(QTimerEvent *ev);

private slots:
    /**
     * @brief dataArrived 数据接收
     */
    void dataArrived();

    /**
     * @brief changeBaudrate 改变波特率
     */
    void changeBaudrate();

    /**
     * @brief startInitModule start the module initlization process
     */
    void startInitModule();

    // /**
    //  * @brief onPlugInProviderConnectParam 处理插件连接参数
    //  * @param isAttach
    //  */
    // void onPlugInProviderConnectParam(bool isAttach);

    /**
     * @brief onWorkModeChanged handle the work mode change event
     * @param curMode the current work mode
     */
    void onWorkModeChanged(WorkMode curMode);

private:
    PlugInProviderPrivate *const d_ptr;
};
