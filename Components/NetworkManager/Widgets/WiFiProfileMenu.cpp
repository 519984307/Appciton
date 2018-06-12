#include "WiFiProfileMenu.h"
#include "WiFiProfileEditor.h"
#include "FontManager.h"
#include "NetworkManager.h"
#include "Debug.h"
#include <QFocusEvent>
#include "IConfig.h"
#include "IListWidget.h"
#include "LabelButton.h"
#include "MenuManager.h"
#include "ColorManager.h"
#include "IComboList.h"
#include "WifiMaintainMenu.h"
#include <QBasicTimer>
#include <IConfig.h>

#define PROFILE_LIST_ITEM_H 30
#define PROFILE_LIST_ITEM_W 200

#define ITEM_SPACING 5

#define SMTP_SERVER "smtp.163.com"
#define SMTP_SERVER_PORT 25
#define SMTP_USER "idmtest@163.com"
#define SMTP_PASSWORD "idmtest1"
#define SMTP_TEST_RECEIVER "bingyun_idm@163.com"

class WiFiProfileMenuPrivate {
public:
    Q_DECLARE_PUBLIC(WiFiProfileMenu)
    WiFiProfileMenuPrivate(WiFiProfileMenu * const q_ptr):q_ptr(q_ptr),
    comboProfileList(NULL),selectApBtn(NULL), ipLabelBtn(NULL), macLabelBtn(NULL){}
    void loadProfiles();
    void onProfileSelect(int index);
    void onWifiConnected(const QString &ssid);
    WiFiProfileMenu * const q_ptr;
    IComboList *comboProfileList;
    LabelButton *selectApBtn;
    LabelButton *ipLabelBtn;
    LabelButton *macLabelBtn;
    QVector<WiFiProfileInfo> profiles;
    QBasicTimer timer;
};

void WiFiProfileMenu::updateWifiProfileSlot(bool isEnabled)
{
    d_ptr->loadProfiles();
    if(d_ptr->comboProfileList)
    {
        d_ptr->comboProfileList->combolist->setEnabled(isEnabled);
        if(!isEnabled)
        {
            d_ptr->comboProfileList->combolist->setCurrentIndex(0);
        }
        else
        {
            if(d_ptr->comboProfileList->combolist->count()>_wifiProfileInt)
            {
                d_ptr->comboProfileList->combolist->setCurrentIndex(_wifiProfileInt);
            }
            else
            {
                d_ptr->comboProfileList->combolist->setCurrentIndex(0);
            }
        }
    }
}
/***************************************************************************************************
 * loadProfiels : load the profile info from super config
 **************************************************************************************************/
void WiFiProfileMenuPrivate::loadProfiles()
{
    QString tmpStr;
    int tmpValue=0;
    int count;
    bool ok;
    if(!systemConfig.getStrAttr("WiFi|Profiles", "Count", tmpStr))
    {
        return;
    }

    count = tmpStr.toInt(&ok);

    if(!ok)
    {
        return;
    }

    profiles.clear();
    for(int i = 0; i < count; i++)
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
        if(profile.isStatic)
        {
            systemConfig.getStrValue(prefix + "StaticIP", profile.staticIp);
            systemConfig.getStrValue(prefix + "DefaultGateway", profile.defaultGateway);
            systemConfig.getStrValue(prefix + "SubnetMask", profile.subnetMask);
            systemConfig.getStrValue(prefix + "PreferedDNS", profile.preferedDNS);
            systemConfig.getStrValue(prefix + "AlternateDNS", profile.alternateDNS);
        }
        profiles.append(profile);
    }

    //update combo profile list

    //remove old item
    while(comboProfileList->count())
    {
        comboProfileList->removeItem(0);
    }

    //first item will be "OFF
    comboProfileList->addItem(trs("Off")) ;

    for(int i = 0; i < profiles.count(); i++)
    {
        comboProfileList->addItem(profiles.at(i).profileName);
    }

    if(systemConfig.getStrAttr("WiFi|Profiles", "CurrentSelect", tmpStr))
    {
        ok = false;
        int index = tmpStr.toInt(&ok);
        if(ok && index >= 0  && index < profiles.count())
        {
            comboProfileList->blockSignals(true);
            comboProfileList->setCurrentIndex(index + 1);
            comboProfileList->blockSignals(false);
            selectApBtn->button->setText(profiles.at(index).ssid);
            wifiProfileMenu._wifiProfileInt = index+1;
        }
    }
}

/***************************************************************************************************
 * onProfileSelect : handle profile select issue
 **************************************************************************************************/
void WiFiProfileMenuPrivate::onProfileSelect(int index)
{
    Q_Q(WiFiProfileMenu);
    if(index<0)
    {
        index = 0;
    }
    int select = index - 1;
    //index 0 means turn off
    wifiProfileMenu._wifiProfileInt = index;
    if(index)
    {
        emit q->selectProfile(profiles.at(index - 1));
        selectApBtn->button->setText(profiles.at(index - 1).ssid);
    }
    else {
        emit q->selectProfile(WiFiProfileInfo());
        selectApBtn->button->setText(QString());
    }
    ipLabelBtn->button->setText(QString());
    macLabelBtn->button->setText(QString());
    systemConfig.setNumAttr("WiFi|Profiles", "CurrentSelect", select);
}

/***************************************************************************************************
 * onWifiConnected : handle wifi connected issue
 **************************************************************************************************/
void WiFiProfileMenuPrivate::onWifiConnected(const QString &ssid)
{
    //the wifi will auto connect on startup if the network is not disabled.
    //So we need to check wether the select profile contain the ssid or not
    int select = 0;

    systemConfig.getNumAttr("WiFi|Profiles", "CurrentSelect", select);

    if(profiles.count() > 0 && select < profiles.count() && select >= 0)
    {
        if(profiles[select].ssid == ssid)
        {
            if(comboProfileList->currentIndex() != select + 1)  //the first item is the off option, need to skip
            {
                comboProfileList->blockSignals(true);
                comboProfileList->setCurrentIndex(select + 1);
                comboProfileList->blockSignals(false);
                selectApBtn->button->setText(ssid);
            }
        }
    }
    else
    {
        qDebug()<<Q_FUNC_INFO<<"Wifi Connected to "<<ssid<<", which doesn't belong to the profile.";
        onProfileSelect(0); //set to off
    }

}

/***************************************************************************************************
 * singleton
 **************************************************************************************************/
WiFiProfileMenu &WiFiProfileMenu::construction()
{
    static WiFiProfileMenu *_instance = NULL;
    if(_instance == NULL)
    {
        _instance = new WiFiProfileMenu;
    }
    return *_instance;
}

/***************************************************************************************************
 * destructor
 **************************************************************************************************/
WiFiProfileMenu::~WiFiProfileMenu()
{

}

/***************************************************************************************************
 * get the curret select wifi profile
 **************************************************************************************************/
WiFiProfileInfo WiFiProfileMenu::getCurrentWifiProfile() const
{
    Q_D(const WiFiProfileMenu);
    if(d->comboProfileList->currentIndex() == 0)
    {
        return WiFiProfileInfo();
    }
    else
    {
        return d->profiles.at(d->comboProfileList->currentIndex() - 1);
    }
}

/***************************************************************************************************
 * layoutExec : do the layout issue
 **************************************************************************************************/
void WiFiProfileMenu::layoutExec()
{
    Q_D(WiFiProfileMenu);

    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    QFont font = fontManager.textFont(15);
    setFixedSize(submenuW, submenuH);

    int itemW = submenuW - ITEM_SPACING;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    d->comboProfileList = new IComboList(trs("AccessPointProfile"));
    d->comboProfileList->setFont(font);
    d->comboProfileList->label->setFixedSize(labelWidth, ITEM_H);
    d->comboProfileList->combolist->setFixedSize(btnWidth, ITEM_H);
    d->comboProfileList->combolist->setDrawCheckMark(false);
    connect(d->comboProfileList, SIGNAL(currentIndexChanged(int)), this, SLOT(onProfileSelect(int)));
    mainLayout->addWidget(d->comboProfileList);

    d->selectApBtn= new LabelButton(trs("SelectAccessPoint"));
    d->selectApBtn->setFont(font);
    d->selectApBtn->label->setFixedWidth(labelWidth);
    QPalette pal = d->selectApBtn->button->palette();
    pal.setColor(QPalette::Disabled, QPalette::WindowText, Qt::black);
    pal.setColor(QPalette::Disabled, QPalette::Window, QColor("#d0d0d0"));
    d->selectApBtn->button->setPalette(pal);
    d->selectApBtn->button->setEnabled(false);
    d->selectApBtn->button->setFixedWidth(btnWidth);
    mainLayout->addWidget(d->selectApBtn);

    d->ipLabelBtn = new LabelButton(trs("LocalIp"));
    d->ipLabelBtn->setFont(font);
    d->ipLabelBtn->label->setFixedWidth(labelWidth);
    d->ipLabelBtn->button->setFixedWidth(btnWidth);
    d->ipLabelBtn->button->setPalette(pal);
    d->ipLabelBtn->button->setEnabled(false);
    mainLayout->addWidget(d->ipLabelBtn);

    d->macLabelBtn = new LabelButton(trs("Mac"));
    d->macLabelBtn->setFont(font);
    d->macLabelBtn->label->setFixedWidth(labelWidth);
    d->macLabelBtn->button->setFixedWidth(btnWidth);
    d->macLabelBtn->button->setPalette(pal);
    d->macLabelBtn->button->setEnabled(false);
    mainLayout->addWidget(d->macLabelBtn);

    mainLayout->addStretch();
}

void WiFiProfileMenu::timerEvent(QTimerEvent *e)
{
    Q_D(WiFiProfileMenu);
    if(d->timer.timerId() == e->timerId())
    {
        if(this->isVisible() && networkManager.isWiFiConnected() && d->comboProfileList->currentIndex() != 0)
        {
            d->ipLabelBtn->button->setText(networkManager.getIpString(NETWORK_WIFI));
            d->macLabelBtn->button->setText(networkManager.getMacAddress(NETWORK_WIFI));
        }
        else
        {
            d->ipLabelBtn->button->setText(QString());
            d->macLabelBtn->button->setText(QString());
        }
    }
}

void WiFiProfileMenu::showEvent(QShowEvent *e)
{
    Q_D(WiFiProfileMenu);
    d->timer.start(1000, this);
    connect(d->comboProfileList, SIGNAL(currentIndexChanged(int)), this, SLOT(onProfileSelect(int)));
    SubMenu::showEvent(e);
}

void WiFiProfileMenu::hideEvent(QHideEvent *e)
{
    Q_D(WiFiProfileMenu);
    d->timer.stop();
    disconnect(d->comboProfileList, SIGNAL(currentIndexChanged(int)), this, SLOT(onProfileSelect(int)));
    SubMenu::hideEvent(e);
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
WiFiProfileMenu::WiFiProfileMenu()
    :SubMenu(trs("WiFiMenu")),
      d_ptr(new WiFiProfileMenuPrivate(this))
{
    Q_D(WiFiProfileMenu);
    setDesc(trs("WiFiMenuDesc"));
    startLayout();
    d->loadProfiles();

    connect(this, SIGNAL(selectProfile(WiFiProfileInfo)), &networkManager, SLOT(connectWiFiProfile(WiFiProfileInfo)), Qt::QueuedConnection);
    connect(&networkManager, SIGNAL(wifiConnectToAp(QString)), this, SLOT(onWifiConnected(QString)), Qt::QueuedConnection);
    connect(&wifiMaintainMenu, SIGNAL(updateWifiProfileSignal(bool)), this, SLOT(updateWifiProfileSlot(bool)));
}

#include "moc_WiFiProfileMenu.cpp"

