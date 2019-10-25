/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/9/5
 **/

#pragma once
#include <QObject>
#include <QString>
#include <termios.h>
#include "Param.h"
#include "Uart.h"
#include "RingBuff.h"
#include "DataDispatcher.h"
#include "PlugInProvider.h"

class Provider: public QObject
{
    Q_OBJECT

public:
    // 获取Provider的名称。
    QString &getName(void);

    // 关联Provider和Param对象，具体内容由派生类实现。
    virtual bool attachParam(Param &param);  // NOLINT

    // 脱离Provider和Param对象，具体内容由派生类实现。
    virtual void detachParam(Param &param);  // NOLINT

    // 检查连接状态。
    virtual void checkConnection(void);

    // 获取版本号
    virtual void sendVersion(void) = 0;

    virtual QString getVersionString() const { return versionInfo;}

    // 获取额外版本号
    virtual QString getExtraVersionString() const { return versionInfoEx;}

    // 关闭串口
    void closePort(void);

    // 是否连接
    bool connected();

    /**
     * @brief connectedToParam 是否连接到参数
     * @return
     */
    bool connectedToParam();

    // 构造与析构。
    explicit Provider(const QString &name);
    virtual ~Provider();

    // 当DataDispatcher 端口有数据时调用此方法，派生类实现。
    virtual void dataArrived(unsigned char *data, unsigned int length)
    {
        Q_UNUSED(data)
        Q_UNUSED(length)
    }

    /**
     * @brief dispatchPortHasReset called when the dispatch port reset is complete
     */
    virtual void dispatchPortHasReset() {}

    virtual void disconnected(void) = 0;          // 模块连接恢复时回调，之类实现。
    virtual void reconnected(void) = 0;          // 模块连接恢复时回调，之类实现。

    /**
     * @brief stopCheckConnect 是否停止检查连接模块连接状态
     * @param flag
     */
    void stopCheckConnect(bool flag);

    /**
     * @brief isConnectionCheckStop check whether the connecion checking is stopped
     * @return  true if stop, otherwise, false
     */
    bool isConnectionCheckStop() const { return _stopCheckConnect; }

protected:
    // 初始化端口。
    bool initPort(const UartAttrDesc &desc, bool needNotify = true);

    // 写数据到端口。
    int writeData(const unsigned char buff[], int len);

    // 读取数据到RingBuff中。
    void readData(void);

    // 数据端口的描述符。
    RingBuff<unsigned char> ringBuff;
    static const int ringBuffLen = 4096;
    Uart *uart;
    DataDispatcher::DispatchInfo disPatchInfo;
    PlugInProvider::PlugInInfo plugInInfo;

    void setFirstCheck(bool flag);

protected slots:
    // 当端口有数据时调用此方法，派生类实现。
    virtual void dataArrived(void)
    {
    }

protected:
    void setDisconnectThreshold(int second);
    void feed(void);                              // 有数据时调用清除连接计数器。
    bool isConnected;
    QString versionInfo;
    QString versionInfoEx;   // 版本信息扩展
    bool isConnectedToParam;
    bool needConnectedToParam;               // 需要连接参数

private:
    QString _name;
    bool _firstCheck;

    int _disconnectCount;
    int _disconnectThreshold;
    bool _stopCheckConnect;     // 待机标志
};
