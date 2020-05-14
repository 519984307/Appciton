/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2019/12/19
 **/

#include "BLMDataUploader.h"
#include "WebSockets/websocket.h"
#include "BLMMessageHandler.h"
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QSslKey>

#define RESOURCE_URL    "/monitor/Vitavue-10"
#define SERVER_NAME     "Better View Server"
#define SSL_PEER_NAME   "betterview.blmed.cn"
#define RECONNECT_DURATION  5000

#define SSL_FILE_LOCATION   "/usr/local/nPM/etc/"

class BLMDataUploaderPrivate
{
public:
    BLMDataUploaderPrivate()
        : socket(NULL), handler(NULL), stopReconnectServer(false), securedMode(false)
    {
    }

    /**
     * @brief getSslConfiguration set the client ssl configuration
     * @return the ssl configuration
     */
    QSslConfiguration getSslConfiguration();

    /**
     * @brief seServerSslPeerName set the server ssl peer name during the handshake
     */
    void updateServerSslPeerName();

    WebSocket *socket;
    BLMMessageHandler *handler;
    bool stopReconnectServer;     /* stop auto reconnect the server */
    bool securedMode;
};

BLMDataUploader::BLMDataUploader()
    : pimpl(new BLMDataUploaderPrivate())
{
}

BLMDataUploader::~BLMDataUploader()
{
    delete pimpl;
}

void BLMDataUploader::connectToServer(const QHostAddress &addr, unsigned short port)
{
    if (!pimpl->socket)
    {
        pimpl->socket = new WebSocket(QString(), WebSocketProtocol::V_LATEST, this);
        pimpl->handler = new BLMMessageHandler(pimpl->socket, this);
        connect(pimpl->socket, SIGNAL(connected()), this, SLOT(onConnected()));
        connect(pimpl->socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
        connect(pimpl->socket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(onError(QAbstractSocket::SocketError)));
        connect(pimpl->socket, SIGNAL(sslErrors(QList<QSslError>)), this, SLOT(onSslErrors(QList<QSslError>)));

        pimpl->socket->setSslConfiguration(pimpl->getSslConfiguration());
    }
    if (pimpl->socket->isValid())
    {
        pimpl->socket->close();
    }

    pimpl->stopReconnectServer = false;

    QUrl url;
    url.setHost(addr.toString());
    url.setPath(RESOURCE_URL);
    url.setPort(port);
    url.setScheme(pimpl->securedMode ? "wss" : "ws");
    if (url == pimpl->socket->getRequestUrl())
    {
        if (pimpl->socket->state() == QAbstractSocket::ConnectedState
                || pimpl->socket->state() == QAbstractSocket::ConnectingState)
        {
            /* already connected or connecting to the same url, do nothing */
            return;
        }
    }

    pimpl->socket->open(url);
    pimpl->updateServerSslPeerName();
    qDebug() << Q_FUNC_INFO << "Connecting data server" << url.toString();
}

void BLMDataUploader::disconnectFromServer()
{
    if (pimpl->socket)
    {
        pimpl->socket->close();
        pimpl->stopReconnectServer = true;
    }
}

bool BLMDataUploader::isConnectedToServer() const
{
    return pimpl->socket && pimpl->socket->isValid();
}

void BLMDataUploader::setSecuredMode(bool enable)
{
    pimpl->securedMode = enable;
}

void BLMDataUploader::onConnected()
{
    QNetworkRequest req = pimpl->socket->getRequest();
    if (req.rawHeader("Server") == SERVER_NAME)
    {
        qDebug() << Q_FUNC_INFO << "Connected to server";
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Unknown server, disconnected";
        pimpl->socket->close();
    }
}

void BLMDataUploader::onDisconnected()
{
    QTimer::singleShot(RECONNECT_DURATION, this, SLOT(reconnected()));
}

void BLMDataUploader::onError(QAbstractSocket::SocketError err)
{
    Q_UNUSED(err)
    qDebug() << "Data upload error:"<< pimpl->socket->errorString();
}

void BLMDataUploader::onSslErrors(const QList<QSslError> &errs)
{
    // QSslSocket *sslSocket = qobject_cast<QSslSocket *>(sender());
    QSslSocket *sslSocket = pimpl->socket->findChild<QSslSocket *>();
    if (sslSocket)
    {
        QSslConfiguration sslConf = sslSocket->sslConfiguration();
        QSslCertificate peerCert = sslConf.peerCertificate();
        qDebug() << Q_FUNC_INFO << "server name"
                 << peerCert.subjectInfo(QSslCertificate::CommonName)
                 << "peer verify name" << sslSocket->peerVerifyName();
    }

    for (int i = 0; i < errs.count(); i++)
    {
        qDebug() << Q_FUNC_INFO << QSslError(errs.at(i)).errorString();
    }
}

void BLMDataUploader::reconnected()
{
    if (!pimpl->stopReconnectServer)
    {
        qDebug() << "Try to reconnect server...";
        pimpl->socket->open(pimpl->socket->getRequestUrl());
        pimpl->updateServerSslPeerName();
    }
}

QSslConfiguration BLMDataUploaderPrivate::getSslConfiguration()
{
     /* ssl configuration */
    QSslConfiguration sslConf;

    /* setup the ca certificate*/
    QFile caFile(QLatin1String(SSL_FILE_LOCATION"ca.crt"));
    if (caFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QSslCertificate ca(&caFile);
        if (!ca.isNull())
        {
            QList<QSslCertificate> cas;
            cas.append(ca);
            sslConf.setCaCertificates(cas);
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "ca certificate error";
        }
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Fail to load CA certificate";
    }

    /* set client certificate */
    QFile crtFile(QLatin1String(SSL_FILE_LOCATION"client.crt"));
    if (crtFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QSslCertificate crt(&crtFile);
        if (!crt.isNull())
        {
            sslConf.setLocalCertificate(crt);
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "server certifcate error";
        }
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Fail to load server certificate";
    }

    /* set client private key */
    QFile prvKeyFile(QLatin1String(SSL_FILE_LOCATION"client.key"));
    if (prvKeyFile.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        QSslKey key(&prvKeyFile, QSsl::Rsa);
        if (!key.isNull())
        {
            sslConf.setPrivateKey(key);
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "server private key error";
        }
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Fail to load server private key";
    }

    return sslConf;
}

void BLMDataUploaderPrivate::updateServerSslPeerName()
{
    if (securedMode && socket)
    {
        QList<QSslSocket *> sslSockets = socket->findChildren<QSslSocket *>();
        if (!sslSockets.isEmpty())
        {
            foreach(QSslSocket *sslSocket, sslSockets)
            {
                /* update the peer verify name to the server url, this url is the same as the one on the certificate */
                sslSocket->setPeerVerifyName(SSL_PEER_NAME);
            }
        }
        else
        {
            qDebug() << Q_FUNC_INFO << "Fail to update server peer verify name";
        }
    }
}
