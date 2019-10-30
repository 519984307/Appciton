/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QProcess>
#include <QBasicTimer>
#ifdef Q_WS_QWS
#include <QNetworkInterface>
#else
#include <QtNetwork/QNetworkInterface>
#endif
#include "NetworkDefine.h"
#include "WpaCtrl.h"
#include "WiFiProfileInfo.h"

/***************************************************************************************************
 * 网络连接管理类，支持有线/WiFi两种网络，暂不支持移动数据连接
 **************************************************************************************************/
class NetworkManager : public QObject
{
    Q_OBJECT
public:
    static NetworkManager &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NetworkManager();
        }

        return *_selfObj;
    }

    static NetworkManager *_selfObj;
    ~NetworkManager();

    // 设置网络连接类型
    void setNetworkType(NetworkType type);
    // 设置有线网络静态IP
    void setStaticIp(unsigned int ip);
    // 设置有线动态获取IP
    void setDynamicIp();

    // 获取指定网络类型的IP地址
    int getIp(NetworkType type);
    // 获取指定网络类型的IP地址
    QString getIpString(NetworkType type);
    // 获取指定网络类型的MAC地址
    QString getMacAddress(NetworkType type);
    // 根据网络类型返回网卡信息
    QNetworkInterface getInterface(NetworkType type);

    // 打开网络
    void open(NetworkType type);
    // 关闭网络
    void close(NetworkType type);

    // get the wifi status, on or off
    static bool isWifiTurnOn();

    // check whether the wifi is working
    bool isWifiWorking() const;

    // check wheter the wifi is connected to ap
    bool isWiFiConnected() const;

    // set the wifi status, on or off
    void setWifiState(bool onOrOFF);

public slots:
    void connectWiFiProfile(const WiFiProfileInfo &profile);

signals:
    // open network operation
    void openNetwork(NetworkType type);
    // close network operation
    void closeNetwork(NetworkType type);
    // connected to ap signal
    void wifiConnectToAp(const QString &ssid);

    // 网络类型发生改变
    void networkTypeChanged(NetworkType type);
    // 网络连接成功信号, type网络类型
    void connected(NetworkType type);

protected:
    void timerEvent(QTimerEvent *e);

private slots:
    void _onWifiApConnected(const QString &ssid);
    void _onWifiApDisconnected(const QString &ssid);
    // open network operation
    void _openNetwork(NetworkType type);
    // close network operation
    void _closeNetwork(NetworkType type);
    // 有线网络udhcpc执行结束
    void _udhcpcFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    explicit NetworkManager(QObject *parent = 0);

    // 根据网络类型返回网卡名称
    QString _getInterfaceName(NetworkType type);
private:
    IpMode _ipMode;                     // 有线IP获取方式
    NetworkType _netType;               // 网络类型
    int _signalLevel;                   // 信号强度
    bool _isWiFiConnected;              // wifi is connected

    WpaCtrl *_wpaCtrl;                  // wpa control interface
    WiFiProfileInfo _curProfile;        // wifi profile
    bool _lastProfileIsStatic;          // last  wifi profile is static ip
    QProcess *_udhcpc;                  // udhcpc进程
    QBasicTimer _timer;                 // 1秒定时器
};

Q_DECLARE_METATYPE(NetworkType);

#define networkManager (NetworkManager::construction())
#define deleteNetworkManager() (delete NetworkManager::_selfObj)

#endif  // NETWORKMANAGER_H

