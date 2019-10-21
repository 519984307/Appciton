/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/9/27
 **/

#pragma once
#include "Uart.h"

/* create a fake uart port, data transmit through the TCP socket */
class UartSocket : public Uart
{
    Q_OBJECT
public:
    /* overriden */
    bool initPort(const QString &port, const UartAttrDesc &desc, bool needNotify);
    /* overriden */
    int write(const unsigned char buff[], int len);
    /* overriden */
    int read(unsigned char *buff, int nbytes);
    /* overriden */
    void sync();

    /* overriden */
    void closePort(void);

    UartSocket();
    ~UartSocket();

private:
    bool _nonblock;
};

class UartSocketControllerPrivate;
class UartSocketController : public QObject
{
    Q_OBJECT
public:
    explicit UartSocketController(QObject *parent = NULL);
    ~UartSocketController();

    /**
     * @brief connectControlServer connect UART socket control server
     */
    void connectControlServer();

    /**
     * @brief isConnected check whether connected to the control server
     * @return true when connected to server, otherwise, return false
     */
    bool isConnected();


    /**
     * @brief openPort open the remove uart port
     * @param port the uart device path
     */
    void openPort(const QString &port);

    /**
     * @brief configPort config port
     * @param port the uart port
     * @param desc the port desc
     */
    void configPort(const QString &port, const UartAttrDesc &desc);

    /**
     * @brief portConfigSuccess check specific port config is successfully
     * @param port the port
     * @return  true is configurate successfully
     */
    bool isPortConfigSuccess(const QString &port);

    /**
     * @brief getServerPort get the remove server port
     * @param port the uart device in remote device
     * @return server port or 0 if not any server
     */
    quint16 getServerPort(const QString &port);

signals:
    /**
     * @brief portConfigSuceess emit when port configure is success
     */
    void receiveCmdResponse();

private slots:
    /**
     * @brief onDataReady handle the tcp receive data
     */
    void onDataReady();

    /**
     * @brief onDisconnected handle disconnect event from the control server
     */
    void onDisconnected();

private:
    Q_DECLARE_PRIVATE(UartSocketController)
    UartSocketControllerPrivate * const d_ptr;
};
