/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "WiFiProfileWindow.h"
#include "ComboBox.h"
#include <QLabel>
#include "Button.h"
#include "LanguageManager.h"
#include <QMap>
#include <QGridLayout>
#include <QPalette>
#include <QBasicTimer>
#include "ConfigManager.h"
#include "IConfig.h"
#include "NetworkManager.h"
#include <QTimerEvent>

class WiFiProfileWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ACCESSPOINT_PROFILE = 0,

        ITEM_BTN_SELECT_ACCESSPOINT = 0,
        ITEM_BTN_LOCAL_IP,
        ITEM_BTN_MAC_ADDRESS,
    };

    Q_DECLARE_PUBLIC(WiFiProfileWindow)
    explicit WiFiProfileWindowPrivate(WiFiProfileWindow *const q_ptr): q_ptr(q_ptr) {}
    void loadProfiles();
    void onProfileSelect(int index);
    void onWifiConnected(const QString &ssid);
    void updateWifiProfileSlot(bool isEnabled);
    void onConfigUpdated(void);
    WiFiProfileWindow *const q_ptr;
    QVector<WiFiProfileInfo> profiles;
    QBasicTimer timer;
    int _wifiProfileInt;
    QMap <MenuItem, ComboBox *> combos;
    QMap <MenuItem, Button *> btns;
};

WiFiProfileWindow::WiFiProfileWindow()
    : Dialog(),
      d_ptr(new WiFiProfileWindowPrivate(this))
{
    Q_D(WiFiProfileWindow);
    d->combos.clear();
    d->btns.clear();

    connect(this, SIGNAL(selectProfile(WiFiProfileInfo)), &networkManager, SLOT(connectWiFiProfile(WiFiProfileInfo)),
            Qt::QueuedConnection);
    connect(&networkManager, SIGNAL(wifiConnectToAp(QString)), this, SLOT(onWifiConnected(QString)),
            Qt::QueuedConnection);
    connect(&configManager, SIGNAL(configUpdated()), this, SLOT(d->onConfigUpdated()));

    layoutExec();
    readyShow();
}


void WiFiProfileWindow::layoutExec()
{
    setWindowTitle(trs("WiFiProfile"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    setFixedSize(480, 480);

    QLabel *label;
    Button *button;
    ComboBox *comboBox;

    // access point profile
    label = new QLabel();
    label->setText(trs("AccessPointProfile"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(WiFiProfileWindowPrivate::ITEM_CBO_ACCESSPOINT_PROFILE,
                         comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onProfileSelect()));


    int row = d_ptr->combos.count();

    // select access point
    label = new QLabel();
    label->setText(trs("SelectAccessPoint"));
    layout->addWidget(label, (row + d_ptr->btns.count()), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    layout->addWidget(button, (row + d_ptr->btns.count()), 1);
    d_ptr->btns.insert(WiFiProfileWindowPrivate::ITEM_BTN_SELECT_ACCESSPOINT,
                       button);

    // local ip
    label = new QLabel();
    label->setText(trs("LocalIp"));
    layout->addWidget(label, (row + d_ptr->btns.count()), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    layout->addWidget(button, (row + d_ptr->btns.count()), 1);
    d_ptr->btns.insert(WiFiProfileWindowPrivate::ITEM_BTN_LOCAL_IP,
                       button);

    // mac
    label = new QLabel();
    label->setText(trs("Mac"));
    layout->addWidget(label, (row + d_ptr->btns.count()), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    layout->addWidget(button, (row + d_ptr->btns.count()), 1);
    d_ptr->btns.insert(WiFiProfileWindowPrivate::ITEM_BTN_MAC_ADDRESS,
                       button);

    layout->setRowStretch((row + d_ptr->btns.count()), 1);
    setWindowLayout(layout);
}




void WiFiProfileWindowPrivate::updateWifiProfileSlot(bool isEnabled)
{
    loadProfiles();
    if (combos[ITEM_CBO_ACCESSPOINT_PROFILE])
    {
        combos[ITEM_CBO_ACCESSPOINT_PROFILE]->setEnabled(isEnabled);
        if (!isEnabled)
        {
            combos[ITEM_CBO_ACCESSPOINT_PROFILE]->setCurrentIndex(0);
        }
        else
        {
            if (combos[ITEM_CBO_ACCESSPOINT_PROFILE]->count() > _wifiProfileInt)
            {
                combos[ITEM_CBO_ACCESSPOINT_PROFILE]->setCurrentIndex(_wifiProfileInt);
            }
            else
            {
                combos[ITEM_CBO_ACCESSPOINT_PROFILE]->setCurrentIndex(0);
            }
        }
    }
}

void WiFiProfileWindowPrivate::onConfigUpdated()
{
    loadProfiles();
}

void WiFiProfileWindow::readyShow()
{
    d_ptr->loadProfiles();
}
/***************************************************************************************************
 * loadProfiels : load the profile info from super config
 **************************************************************************************************/
void WiFiProfileWindowPrivate::loadProfiles()
{
    QString tmpStr;
    int tmpValue = 0;
    int count;
    bool ok;
    if (!systemConfig.getStrAttr("WiFi|Profiles", "Count", tmpStr))
    {
        return;
    }

    count = tmpStr.toInt(&ok);

    if (!ok)
    {
        return;
    }

    profiles.clear();
    for (int i = 0; i < count; i++)
    {
        QString prefix = QString("WiFi|Profiles|Profile%1|").arg(i);
        WiFiProfileInfo profile;
        systemConfig.getStrValue(prefix + "ProfileName", profile.profileName);
        systemConfig.getStrValue(prefix + "SSID", profile.ssid);
        systemConfig.getNumValue(prefix + "AuthType", tmpValue);
        profile.authType = (WiFiProfileInfo::AuthenticationType) tmpValue;
        systemConfig.getStrValue(prefix + "SecurityKey", profile.securityKey);
        systemConfig.getNumValue(prefix + "IsStatic", tmpValue);
        profile.isStatic = tmpValue;
        if (profile.isStatic)
        {
            systemConfig.getStrValue(prefix + "StaticIP", profile.staticIp);
            systemConfig.getStrValue(prefix + "DefaultGateway", profile.defaultGateway);
            systemConfig.getStrValue(prefix + "SubnetMask", profile.subnetMask);
            systemConfig.getStrValue(prefix + "PreferedDNS", profile.preferedDNS);
            systemConfig.getStrValue(prefix + "AlternateDNS", profile.alternateDNS);
        }
        profiles.append(profile);
    }

    // update combo profile list
    combos[ITEM_CBO_ACCESSPOINT_PROFILE]->blockSignals(true);
    // remove old item
    while (combos[ITEM_CBO_ACCESSPOINT_PROFILE]->count())
    {
        combos[ITEM_CBO_ACCESSPOINT_PROFILE]->removeItem(0);
    }

    // first item will be "OFF
    combos[ITEM_CBO_ACCESSPOINT_PROFILE]->addItem(trs("Off"));

    for (int i = 0; i < profiles.count(); i++)
    {
        combos[ITEM_CBO_ACCESSPOINT_PROFILE]->addItem(profiles.at(i).profileName);
    }

    combos[ITEM_CBO_ACCESSPOINT_PROFILE]->blockSignals(false);

    if (systemConfig.getStrAttr("WiFi|Profiles", "CurrentSelect", tmpStr))
    {
        ok = false;
        int index = tmpStr.toInt(&ok);
        if (ok && index >= 0  && index < profiles.count())
        {
            combos[ITEM_CBO_ACCESSPOINT_PROFILE]->blockSignals(true);
            combos[ITEM_CBO_ACCESSPOINT_PROFILE]->setCurrentIndex(index + 1);
            combos[ITEM_CBO_ACCESSPOINT_PROFILE]->blockSignals(false);
            btns[ITEM_BTN_SELECT_ACCESSPOINT]->setText(profiles.at(index).ssid);
            _wifiProfileInt = index + 1;
        }
    }
}

/***************************************************************************************************
 * onProfileSelect : handle profile select issue
 **************************************************************************************************/
void WiFiProfileWindowPrivate::onProfileSelect(int index)
{
    Q_Q(WiFiProfileWindow);
    if (index < 0)
    {
        index = 0;
    }
    int select = index - 1;
    // index 0 means turn off
    _wifiProfileInt = index;
    if (index)
    {
        emit q->selectProfile(profiles.at(index - 1));
        btns[ITEM_BTN_SELECT_ACCESSPOINT]->setText(profiles.at(index - 1).ssid);
    }
    else
    {
        emit q->selectProfile(WiFiProfileInfo());
        btns[ITEM_BTN_SELECT_ACCESSPOINT]->setText(QString());
    }
    btns[ITEM_BTN_LOCAL_IP]->setText(QString());
    btns[ITEM_BTN_MAC_ADDRESS]->setText(QString());
    systemConfig.setNumAttr("WiFi|Profiles", "CurrentSelect", select);
}

/***************************************************************************************************
 * onWifiConnected : handle wifi connected issue
 **************************************************************************************************/
void WiFiProfileWindowPrivate::onWifiConnected(const QString &ssid)
{
    // the wifi will auto connect on startup if the network is not disabled.
    // So we need to check wether the select profile contain the ssid or not
    int select = 0;

    systemConfig.getNumAttr("WiFi|Profiles", "CurrentSelect", select);

    if (profiles.count() > 0 && select < profiles.count() && select >= 0)
    {
        if (profiles[select].ssid == ssid)
        {
            // the first item is the off option, need to skip
            if (combos[ITEM_CBO_ACCESSPOINT_PROFILE]->currentIndex() != select + 1)
            {
                combos[ITEM_CBO_ACCESSPOINT_PROFILE]->blockSignals(true);
                combos[ITEM_CBO_ACCESSPOINT_PROFILE]->setCurrentIndex(select + 1);
                combos[ITEM_CBO_ACCESSPOINT_PROFILE]->blockSignals(false);
                btns[ITEM_BTN_SELECT_ACCESSPOINT]->setText(ssid);
            }
        }
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Wifi Connected to " << ssid
                 << ", which doesn't belong to the profile.";
        onProfileSelect(0);  // set to off
    }
}


/***************************************************************************************************
 * destructor
 **************************************************************************************************/
WiFiProfileWindow::~WiFiProfileWindow()
{
}

/***************************************************************************************************
 * get the curret select wifi profile
 **************************************************************************************************/
WiFiProfileInfo WiFiProfileWindow::getCurrentWifiProfile() const
{
    Q_D(const WiFiProfileWindow);
    if (d->combos[WiFiProfileWindowPrivate::ITEM_CBO_ACCESSPOINT_PROFILE]->currentIndex() == 0)
    {
        return WiFiProfileInfo();
    }
    else
    {
        return d->profiles.at(d->combos[WiFiProfileWindowPrivate::ITEM_CBO_ACCESSPOINT_PROFILE]->currentIndex() - 1);
    }
}

void WiFiProfileWindow::timerEvent(QTimerEvent *e)
{
    Q_D(WiFiProfileWindow);
    if (d->timer.timerId() == e->timerId())
    {
        if (this->isVisible() && networkManager.isWiFiConnected()
                && d->combos[WiFiProfileWindowPrivate::ITEM_CBO_ACCESSPOINT_PROFILE]->currentIndex() != 0)
        {
            d->btns[WiFiProfileWindowPrivate::ITEM_BTN_LOCAL_IP]->setText(networkManager.getIpString(NETWORK_WIFI));
            d->btns[WiFiProfileWindowPrivate::ITEM_BTN_MAC_ADDRESS]->setText(
                        networkManager.getMacAddress(NETWORK_WIFI));
        }
        else
        {
            d->btns[WiFiProfileWindowPrivate::ITEM_BTN_LOCAL_IP]->setText(QString());
            d->btns[WiFiProfileWindowPrivate::ITEM_BTN_MAC_ADDRESS]->setText(QString());
        }
    }
}

void WiFiProfileWindow::showEvent(QShowEvent *e)
{
    Q_D(WiFiProfileWindow);
    d->timer.start(1000, this);
    connect(d->combos[WiFiProfileWindowPrivate::ITEM_CBO_ACCESSPOINT_PROFILE], SIGNAL(currentIndexChanged(int)), this,
            SLOT(onProfileSelect(int)));
    Dialog::showEvent(e);
}

void WiFiProfileWindow::hideEvent(QHideEvent *e)
{
    Q_D(WiFiProfileWindow);
    d->timer.stop();
    disconnect(d->combos[WiFiProfileWindowPrivate::ITEM_CBO_ACCESSPOINT_PROFILE], SIGNAL(currentIndexChanged(int)),
               this, SLOT(onProfileSelect(int)));
    Dialog::hideEvent(e);
}


#include "moc_WiFiProfileWindow.cpp"
