/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include <QTimerEvent>
#include "IConfig.h"
#include "ConfigManager.h"
#include <QThread>
#include <QFile>
#include <sys/types.h>
#include <signal.h>
#include "PatientManager.h"
#include "SystemManager.h"
#include "Debug.h"
#include "NetworkManager.h"

#define TIMER_INTERVAL     (1000)   // 1s
#define AUTO_START_WIFI_TIME    (5*1000)    // auto start wifi after 5s from start up if the wifi is configured on
#define WIFI_UDHCPC_PID_FILE "/var/run/udhcpc_wifi.pid"
#define RESOLV_CONF_FILE "/etc/resolv.conf"

#define SMTP_SERVER "smtp.163.com"
#define SMTP_SERVER_PORT 25
#define SMTP_USER "idmtest@163.com"
#define SMTP_PASSWORD "idmtest1"
#define SMTP_TEST_RECEIVER "bingyun_idm@163.com"

NetworkManager *NetworkManager::_selfObj = NULL;

/***************************************************************************************************
 * 构造函数
 **************************************************************************************************/
NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , _ipMode(IPMODE_STATIC)
    , _netType(NETWORK_WIRED)
    , _isWiFiConnected(false)
    , _wpaCtrl(NULL)
    , _lastProfileIsStatic(false)
    , _udhcpc(NULL)
{
    _timer.start(TIMER_INTERVAL, this);
    // queued signal-slots need to know how to handle object creation and destruction at runtime.
    // the follwing register function is needed.
    qRegisterMetaType<NetworkType>();
    qRegisterMetaType<WiFiProfileInfo>();
    connect(this, SIGNAL(openNetwork(NetworkType)), this, SLOT(_openNetwork(NetworkType)));
    connect(this, SIGNAL(closeNetwork(NetworkType)), this, SLOT(_closeNetwork(NetworkType)));
}

/***************************************************************************************************
 * 析构函数
 **************************************************************************************************/
NetworkManager::~NetworkManager()
{
    _timer.stop();
    if (_wpaCtrl != NULL)
    {
        delete _wpaCtrl;
    }

    if (_udhcpc != NULL)
    {
        _udhcpc->close();
        delete _udhcpc;
    }
}

/***************************************************************************************************
 * 打开指定类型的网络
 **************************************************************************************************/
void NetworkManager::_openNetwork(NetworkType type)
{
    _netType = type;
    QString name = _getInterfaceName(type);

    switch (type)
    {
    case NETWORK_WIFI:
        if (!_wpaCtrl)
        {
            _wpaCtrl = new WpaCtrl(this);
            connect(_wpaCtrl, SIGNAL(connectToAp(QString)), this, SLOT(_onWifiApConnected(QString)));
            connect(_wpaCtrl, SIGNAL(disconnectFromAp(QString)), this, SLOT(_onWifiApDisconnected(QString)));
        }

        if (_wpaCtrl->openInterface(name))
        {
            qDebug() << Q_FUNC_INFO << "Open interface " << name << " failed.";
        }
        break;
    case NETWORK_MOBILEDATA:
        break;
    case NETWORK_WIRED:
    default:
        QProcess::startDetached(QString("ifconfig %1 up").arg(name));
        break;
    }
}

/***************************************************************************************************
 * 关闭指定类型的网络
 **************************************************************************************************/
void NetworkManager::_closeNetwork(NetworkType type)
{
    QString name = _getInterfaceName(type);

    switch (type)
    {
    case NETWORK_WIFI:
        if (_wpaCtrl)
        {
            delete _wpaCtrl;
            _wpaCtrl = NULL;
        }
        break;
    case NETWORK_MOBILEDATA:
        break;
    case NETWORK_WIRED:
    default:
        QProcess::startDetached(QString("ifconfig %1 down").arg(name));
        break;
    }
}

/***************************************************************************************************
 * 设置网络类型
 **************************************************************************************************/
void NetworkManager::setNetworkType(NetworkType type)
{
    if (_netType == type)
    {
        return;
    }

    close(_netType);

    _netType = type;

    open(_netType);
    emit networkTypeChanged(type);
}

/***************************************************************************************************
 * 设置有线网络静态IP
 **************************************************************************************************/
void NetworkManager::setStaticIp(unsigned int ip)
{
    if (0 == ip)
    {
        return;
    }

    _ipMode = IPMODE_STATIC;

    QString cmd = QString("ifconfig eth0 %1.%2.%3.%4\n").
            number((ip & 0xFF000000) >> 24).
            number((ip & 0x00FF0000) >> 16).
            number((ip & 0x0000FF00) >> 8).
            number(ip & 0x000000FF);
    QProcess::startDetached(cmd);
}

/***************************************************************************************************
 * 设置有线网络动态获取IP
 **************************************************************************************************/
void NetworkManager::setDynamicIp()
{
    _ipMode = IPMODE_DHCP;

    if (NULL == _udhcpc)
    {
        _udhcpc = new QProcess(this);
        connect(_udhcpc, SIGNAL(finished(int, QProcess::ExitStatus)),
                SLOT(_udhcpcFinished(int, QProcess::ExitStatus)));
    }

    QString cmd = "udhcpc -q -i eth0";
    _udhcpc->start(cmd);
}

/***************************************************************************************************
 * 获取指定网络类型的IP地址
 * 参数：
 *      type:网络类型
 * 返回值:
 *      如果找到IP地址，返回整型IP数值，否则返回0
 **************************************************************************************************/
int NetworkManager::getIp(NetworkType type)
{
    QNetworkInterface interface = getInterface(type);

    if (!interface.isValid()
            || interface.addressEntries().isEmpty())
    {
        return 0;
    }

    return interface.addressEntries().first().ip().toIPv4Address();
}

/***************************************************************************************************
 * 获取指定网络类型的IP地址
 * 参数：
 *      type:网络类型
 * 返回值:
 *      如果找到IP地址，返回IP地址字符串，否则返回空字符串
 **************************************************************************************************/
QString NetworkManager::getIpString(NetworkType type)
{
    QNetworkInterface interface = getInterface(type);

    if (!interface.isValid()
            || interface.addressEntries().isEmpty())
    {
        return QString();
    }

    return interface.addressEntries().first().ip().toString();
}

/***************************************************************************************************
 * 获取指定网络类型的MAC地址
 * 参数：
 *      type:网络类型
 * 返回值:
 *      如果找到MAC地址，返回MAC地址字符串，否则返回空字符串
 **************************************************************************************************/
QString NetworkManager::getMacAddress(NetworkType type)
{
    QNetworkInterface interface = getInterface(type);
    return (interface.isValid() ? interface.hardwareAddress() : "");
}

/***************************************************************************************************
 * 根据网络类型返回网卡信息
 **************************************************************************************************/
QNetworkInterface NetworkManager::getInterface(NetworkType type)
{
    QString ifname = _getInterfaceName(type);
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();

    foreach(QNetworkInterface iface, interfaceList)
    {
        if (iface.name() == ifname)
        {
            return iface;
        }
    }

    return QNetworkInterface();
}

/***************************************************************************************************
 * Just send a signal to open the network
 **************************************************************************************************/
void NetworkManager::open(NetworkType type)
{
    emit openNetwork(type);
}

/***************************************************************************************************
 * just send a signal to close the network
 **************************************************************************************************/
void NetworkManager::close(NetworkType type)
{
    emit closeNetwork(type);
}

/***************************************************************************************************
 * isWifiTurnOn : check whether the wifi is turn on
 **************************************************************************************************/
bool NetworkManager::isWifiTurnOn()
{
    int st = 0;
    systemConfig.getNumValue("WiFi|EnableWifi", st);
    return st;
}

/***************************************************************************************************
 * isWifiWorking : check whether the wifi is working
 **************************************************************************************************/
bool NetworkManager::isWifiWorking() const
{
#if defined (Q_WS_X11)
    return true;
#else
    if (_wpaCtrl)
    {
        return _wpaCtrl->isValid();
    }
    return false;
#endif
}

bool NetworkManager::isWiFiConnected() const
{
#if defined (Q_WS_X11)
    return true;
#else
    return _isWiFiConnected;
#endif
}

/***************************************************************************************************
 * setWifiState : set the wifi state, on or off
 **************************************************************************************************/
void NetworkManager::setWifiState(bool onOrOFF)
{
    int st = onOrOFF;
    systemConfig.setNumValue("WiFi|EnableWifi", st);
}


static QString authTypeToString(WiFiProfileInfo::AuthenticationType type)
{
    switch (type)
    {
    case WiFiProfileInfo::Open:
        return QString("NONE");
        break;
    case WiFiProfileInfo::Wpa2_psk:
    case WiFiProfileInfo::Wpa_psk:
        return QString("WPA-PSK");
    default:
        break;
    }
    return QString("");
}

/***************************************************************************************************
 * connectWiFiProfile : connect to profile specific network
 **************************************************************************************************/
void NetworkManager::connectWiFiProfile(const WiFiProfileInfo &profile)
{
    if (!profile.isValid())
    {
        _lastProfileIsStatic = _curProfile.isStatic;
    }
    else
    {
        _lastProfileIsStatic = profile.isStatic;
    }

    _curProfile = profile;

    if (!_wpaCtrl)
    {
        this->_openNetwork(NETWORK_WIFI);
    }
    if (_wpaCtrl->isValid())
    {
        int id;
        QString idStr;
        QList<WiFiNetworkInfo> infos = _wpaCtrl->getNetworks();
        if (!infos.isEmpty())
        {
            // always using the first network config
            id = infos.at(0).id.toInt();
            idStr = QString::number(id);
        }
        else
        {
            idStr = _wpaCtrl->addNetwork();
            if (idStr.isEmpty())
            {
                qdebug("add network failed.");
                return;
            }
            id = idStr.toInt();
        }

        if (!profile.isValid())
        {
            // invalid profile, disable the network
            _wpaCtrl->disableNetwork(idStr);
        }
        else
        {
            if (_wpaCtrl->setNetworkParam(id, "ssid", profile.ssid.toAscii().constData(), true))
            {
                qdebug("set ssid failed.");
                return;
            }
            if (_wpaCtrl->setNetworkParam(id, "key_mgmt",
                                          authTypeToString(profile.authType).toAscii().constData(), false))
            {
                qdebug("set key_mgmt failed.");
                return;
            }
            if (profile.authType == WiFiProfileInfo::Wpa_psk || profile.authType == WiFiProfileInfo::Wpa2_psk)
            {
                if (_wpaCtrl->setNetworkParam(id, "psk", profile.securityKey.toAscii().constData(), true))
                {
                    qdebug("set psk failed");
                    return;
                }
            }
            else if (profile.authType == WiFiProfileInfo::Open)
            {
                /* TODO */
            }

            _wpaCtrl->disableNetwork(idStr);
            _wpaCtrl->enableNetwork(idStr);
            _wpaCtrl->selectNetwork(idStr);
        }

        _wpaCtrl->saveConfig();
    }
}

/***************************************************************************************************
 * getUDHCPCPid : get the udhcpc process id
 **************************************************************************************************/
static int getUDHCPCPid()
{
    bool ok = false;
    int pid = -1;
    if (QFile::exists(WIFI_UDHCPC_PID_FILE))
    {
        // get the pid
        QFile pidfile(WIFI_UDHCPC_PID_FILE);
        pidfile.open(QIODevice::ReadOnly | QIODevice::Text);
        pid = pidfile.readLine().trimmed().toInt(&ok);
    }
    return ok ? pid : -1;
}

/***************************************************************************************************
 * _onWifiApConnected : handle wifi ap connected signal
 **************************************************************************************************/
void NetworkManager::_onWifiApConnected(const QString &ssid)
{
    Q_UNUSED(ssid)

    if (!_curProfile.isValid())
    {
        // automatic connect to the last select AP at power up
        // try to load the ap info of the last connected AP

        bool ok;
        int index;
        int count;
        int tmpValue = 0;
        QString tmpStr;
        if (!currentConfig.getStrAttr("WiFi|Profiles", "CurrentSelect", tmpStr))
        {
            // failed to find current select AP attribute
            connectWiFiProfile(WiFiProfileInfo());
            return;
        }

        index = tmpStr.toInt(&ok);

        if (!ok || index < 0)
        {
            // failed to find current select AP attribute
            connectWiFiProfile(WiFiProfileInfo());
            return;
        }

        if (!currentConfig.getStrAttr("WiFi|Profiles", "Count", tmpStr))
        {
            // failed to find current select AP attribute
            connectWiFiProfile(WiFiProfileInfo());
            return;
        }

        count = tmpStr.toInt(&ok);

        if (!ok || count <= index)
        {
            // failed to find current select AP attribute
            connectWiFiProfile(WiFiProfileInfo());
            return;
        }

        QString prefix = QString("WiFi|Profiles|Profile%1|").arg(index);
        WiFiProfileInfo profile;
        currentConfig.getStrValue(prefix + "ProfileName", profile.profileName);
        currentConfig.getStrValue(prefix + "SSID", profile.ssid);
        currentConfig.getNumValue(prefix + "AuthType", tmpValue);
        profile.authType = (WiFiProfileInfo::AuthenticationType) tmpValue;
        currentConfig.getStrValue(prefix + "SecurityKey", profile.securityKey);
        currentConfig.getNumValue(prefix + "IsStatic", tmpValue);
        profile.isStatic = tmpValue;
        if (profile.isStatic)
        {
            currentConfig.getStrValue(prefix + "StaticIP", profile.staticIp);
            currentConfig.getStrValue(prefix + "DefaultGateway", profile.defaultGateway);
            currentConfig.getStrValue(prefix + "SubnetMask", profile.subnetMask);
            currentConfig.getStrValue(prefix + "PreferedDNS", profile.preferedDNS);
            currentConfig.getStrValue(prefix + "AlternateDNS", profile.alternateDNS);
        }

        _curProfile = profile;
    }

    if (_curProfile.isStatic)
    {
        // stop udhcpc process if exist
        int pid = getUDHCPCPid();
        if (pid >= 0)
        {
            kill(pid, SIGTERM);
            qdebug("Kill udhcpc...");
        }
        QString cmdIpconfig = QString("ifconfig %1 %2 netmask %3").arg(_getInterfaceName(NETWORK_WIFI)).arg(
                                  _curProfile.staticIp).arg(_curProfile.subnetMask);
        QString cmdRoute = QString("route add default gw %1 dev %2").arg(_curProfile.defaultGateway).arg(
                    _getInterfaceName(NETWORK_WIFI));

        qdebug("add static ip...");
        QProcess::execute(cmdIpconfig);
        QProcess::execute("route del default");  // delete exists default gw
        QProcess::execute(cmdRoute);

        QFile resovFile(RESOLV_CONF_FILE);
        resovFile.open(QIODevice::WriteOnly | QIODevice::Truncate);
        if (_curProfile.preferedDNS != QString("0.0.0.0"))
        {
            resovFile.write(QString("nameserver %1\n").arg(_curProfile.preferedDNS).toAscii());
        }
        if (_curProfile.alternateDNS != QString("0.0.0.0"))
        {
            resovFile.write(QString("nameserver %1\n").arg(_curProfile.alternateDNS).toAscii());
        }
        resovFile.flush();
    }
    else
    {
        // perform dhcp
        int pid = getUDHCPCPid();
        if (pid >= 0)
        {
            // send a SIGUSR1 to udhcpc will cause a renew
            kill(pid, SIGUSR1);
            qdebug("udhcpc renew...");
        }
        else
        {
            // start a new dhcpc process, if get the ip address successfully, udhcpc will create a deamon
            // process and exit, the daemon process will handle the renew issue.
            // Otherwise, udhcpc will loop until get the ip.
            QProcess::startDetached(QString("udhcpc -i %1 -p %2 ").arg(_getInterfaceName(NETWORK_WIFI))
                                    .arg(WIFI_UDHCPC_PID_FILE));
        }
    }

    _isWiFiConnected = true;
    emit wifiConnectToAp(ssid);
    emit connected(NETWORK_WIFI);
}

/***************************************************************************************************
 * _onWifiApDisconnected : handle wifi ap disconnected signal
 **************************************************************************************************/
void NetworkManager::_onWifiApDisconnected(const QString &ssid)
{
    Q_UNUSED(ssid)
    if (_lastProfileIsStatic)
    {
        QString cmdIpconfig = QString("ifconfig %1 0.0.0.0").arg(_getInterfaceName(NETWORK_WIFI));
        QString cmdRoute = QString("route del default gw dev %1").arg(_getInterfaceName(NETWORK_WIFI));
        qdebug("remove static ip...");
        QProcess::execute(cmdIpconfig);
        QProcess::execute(cmdRoute);
    }
    else
    {
        // stop udhcpc
        int pid = getUDHCPCPid();
        if (pid >= 0)
        {
            kill(pid, SIGTERM);
            qdebug("Kill udhcpc...");
        }
    }
    _isWiFiConnected = false;
}

/***************************************************************************************************
 * 定时器事件
 **************************************************************************************************/
void NetworkManager::timerEvent(QTimerEvent *e)
{
    static qint32 timeCounter = 0;
    if (e->timerId() != _timer.timerId())
    {
        return;
    }

    if (timeCounter >= 0 && timeCounter < AUTO_START_WIFI_TIME)
    {
        timeCounter += TIMER_INTERVAL;
    }

    if (timeCounter >= AUTO_START_WIFI_TIME)
    {
        timeCounter = -1;
        if (_wpaCtrl == NULL && isWifiTurnOn() && systemManager.isSupport(CONFIG_WIFI))
        {
            /* TODO: connect to last profile */
            // this->connectWiFiProfile(WiFiProfileMenuContent::getInstance()->getCurrentWifiProfile());
        }
    }
}

/***************************************************************************************************
 * 有线网络udhcpc执行结束
 **************************************************************************************************/
void NetworkManager::_udhcpcFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    bool isNormalExit = ((exitStatus == QProcess::NormalExit) && (exitCode == 0));

    if (isNormalExit && IPMODE_DHCP == _ipMode)
    {
        // 发出信号，主要用于IP刷新
        emit connected(NETWORK_WIRED);
    }
    else
    {
        setDynamicIp();
    }
}

/***************************************************************************************************
 * 根据网络类型返回网卡名称
 **************************************************************************************************/
QString NetworkManager::_getInterfaceName(NetworkType type)
{
    switch (type)
    {
    // 根据网络类型返回其名称
    case NETWORK_WIRED:
        return QString("eth0");
    case NETWORK_MOBILEDATA:
        return QString("ppp0");
    case NETWORK_WIFI:
        return QString("wlan0");
    default:
        return QString("Unknown");
    }
}
