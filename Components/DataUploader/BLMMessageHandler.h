/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#pragma once

#include <QObject>
#include <QVariantMap>

class BLMMessageHandlerPrivate;
class WebSocket;
class BLMMessageHandler : public QObject
{
    Q_OBJECT
public:
    explicit BLMMessageHandler(WebSocket *socket, QObject * parent = NULL);

    ~BLMMessageHandler();


protected:
    void timerEvent(QTimerEvent *ev);

private slots:
    /**
     * @brief onConnected handle the socket connect event
     */
    void onConnected();

    /**
     * @brief onDisconnected handle the socket disconnect event
     */
    void onDisconnected();

    /**
     * @brief sendMessage  send message to the data server
     * @param message the message
     */
    void sendMessage(const QString &message);

    /**
     * @brief receiveMessage handle the received message
     * @param map variant map converted from the received message
     */
    void receiveMessage(const QVariantMap &map);

private:
    BLMMessageHandlerPrivate * const pimpl;
};
