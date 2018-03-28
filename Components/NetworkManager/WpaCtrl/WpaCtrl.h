#pragma once
#include <QObject>
#include <QScopedPointer>
#include "wpa_ctrl.h"
#include <QList>
#include "WiFiProfileEditor.h"


struct WiFiNetworkInfo
{
    WiFiNetworkInfo(const QString &id, const QString &ssid, const QString &bssid, const QString &flags)
        :id(id), ssid(ssid), bssid(bssid), flags(flags){}
    QString id;
    QString ssid;
    QString bssid;
    QString flags;
};

struct SSIDInfo
{
    SSIDInfo(const QString &ssid, const QString &bssid, const QString &flags, int freq, int signal)
            :ssid(ssid), bssid(bssid), flags(flags),freq(freq), signal(signal){}
    QString ssid;
    QString bssid;
    QString flags;
    int freq;
    int signal;
};

struct WiFiStatus{
    QString ssid;
    QString bssid;
    QString freq;
    QString id;
    QString mode;
    QString pairwise_cipher;
    QString group_cipher;
    QString address;
    QString ip_address;
    QString wpa_state;
    QString key_mgmt;
};

class WpaCtrlPrivate;
class WpaCtrl : public QObject
{
    Q_OBJECT
public:
    WpaCtrl(QObject *parent);
    ~WpaCtrl();
    //open the wifi interface, like wlan0
    int openInterface(const QString &interface);
    //check whether this control interface is valid
    bool isValid() const;
    //get the store networks
    QList<WiFiNetworkInfo> getNetworks();
    //enable network by network id
    void enableNetwork(const QString &id);
    //disable network by network id
    void disableNetwork(const QString &id);
    //select network by network id
    void selectNetwork(const QString &id);
    //add a network
    QString addNetwork();
    //set the specific network parameter
    int setNetworkParam(int id, const char *field, const char *value, bool quote);
    //remove a network by network id
    void removeNetwork(const QString &id);
    //request to perform a scan operation
    void scanRequest();
    //check whether the scan is finished
    bool scanResultReady() const;
    //get latest scan result
    QList<SSIDInfo> getScanResult() const;
    //save the network config
    void saveConfig();
    //get the wifi status
    WiFiStatus status() const;

signals:
    void connectToAp(const QString &ssid);
    void disconnectFromAp(const QString &ssid);

protected:
    const QScopedPointer<WpaCtrlPrivate> d_ptr;
    void timerEvent(QTimerEvent *e);

private:
    Q_PRIVATE_SLOT(d_func(), void receiveMsgs())
    Q_DISABLE_COPY(WpaCtrl)
    Q_DECLARE_PRIVATE(WpaCtrl)
};


