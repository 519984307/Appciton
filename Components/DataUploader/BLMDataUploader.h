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
#include <QAbstractSocket>
#include "DataUploaderIFace.h"
#include <QSslError>
#include <QList>

class BLMDataUploaderPrivate;
class BLMDataUploader : public QObject, public DataUploaderIface
{
    Q_OBJECT
public:
    BLMDataUploader();
    ~BLMDataUploader();

    /* reimplement */
    void connectToServer(const QHostAddress &addr, unsigned short port);

    /* reimplement */
    void disconnectFromServer();

    /* reimplement */
    bool isConnectedToServer() const;

    /**
     * @brief setSecuredMode set the work mode
     * @param enable true to enable secured mode, should be set before connecting
     */
    void setSecuredMode(bool enable);

private slots:
    /* handle the connect event when connect to the server */
    void onConnected();

    /* handle the disconnect event when disconnect from the server */
    void onDisconnected();

    /* handle error */
    void onError(QAbstractSocket::SocketError err);

    /* handle ssl error */
    void onSslErrors(const QList<QSslError> &errs);

    /* reconnected slot */
    void reconnected();

private:
    BLMDataUploaderPrivate * const pimpl;
};
