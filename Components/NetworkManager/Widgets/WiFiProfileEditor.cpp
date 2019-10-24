/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "WiFiProfileEditor.h"
#include <QRegExp>
#include "LabelButton.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "Debug.h"
#include "KeyBoardPanel.h"
#include "IMessageBox.h"
#include "SystemManager.h"
#include <QApplication>


#define FONT_SIZE 14
#define MAX_IP_STRING_LENGTH 15
#define DEFAULT_WIDGET_WIDTH 540

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
WiFiProfileInfo::WiFiProfileInfo():
    staticIp(QString::fromLatin1("0.0.0.0")),
    defaultGateway(QString::fromLatin1("0.0.0.0")),
    subnetMask(QString::fromLatin1("255.255.255.0")),
    preferedDNS(QString::fromLatin1("0.0.0.0")),
    alternateDNS(QString::fromLatin1("0.0.0.0")),
    authType(Wpa_psk),
    isStatic(false)
{
}

/***************************************************************************************************
 * check whether a ip string is valid
 **************************************************************************************************/
static bool isIpStrValid(const QString &ipStr)
{
    QRegExp reg("^(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\."
                "(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])$");
    return reg.exactMatch(ipStr);
}

/***************************************************************************************************
 * copy constructor
 **************************************************************************************************/
WiFiProfileInfo::WiFiProfileInfo(const WiFiProfileInfo &other)
{
    profileName = other.profileName;
    ssid = other.ssid;
    securityKey = other.securityKey;
    staticIp = other.staticIp;
    defaultGateway = other.defaultGateway;
    subnetMask = other.subnetMask;
    preferedDNS = other.preferedDNS;
    alternateDNS = other.alternateDNS;
    authType = other.authType;
    isStatic = other.isStatic;
}

/*****************************************#include "Utility.h"**********************************************************
 * assignment operation override
 **************************************************************************************************/
WiFiProfileInfo &WiFiProfileInfo::operator=(const WiFiProfileInfo &other)
{
    profileName = other.profileName;
    ssid = other.ssid;
    securityKey = other.securityKey;
    staticIp = other.staticIp;
    defaultGateway = other.defaultGateway;
    subnetMask = other.subnetMask;
    preferedDNS = other.preferedDNS;
    alternateDNS = other.alternateDNS;
    authType = other.authType;
    isStatic = other.isStatic;

    return *this;
}

/***************************************************************************************************
 * validate the profile, return ture if valid
 **************************************************************************************************/
bool WiFiProfileInfo::isValid() const
{
    if (!isStatic)
    {
        return !profileName.isEmpty() && !ssid.isEmpty() && authType >= Open && authType <= Wpa2_psk;
    }

    return !profileName.isEmpty() && !ssid.isEmpty() && authType >= Open && authType <= Wpa2_psk
            && isIpStrValid(staticIp) && isIpStrValid(defaultGateway) && isIpStrValid(subnetMask)
            && isIpStrValid(preferedDNS) && isIpStrValid(alternateDNS);
}


class WiFiProfileEditorPrivate
{
public:
    Q_DECLARE_PUBLIC(WiFiProfileEditor)
    explicit WiFiProfileEditorPrivate(WiFiProfileEditor * const q_ptr)
        :q_ptr(q_ptr), profileNameBtn(NULL), ssidBtn(NULL), authTypeComboL(NULL),
    securityKeyBtn(NULL), networkSettingComboL(NULL), staticIpBtn(NULL), defaultGatewayBtn(NULL),
    subnetMaskBtn(NULL), preferedDNSBtn(NULL), alternateDNSBtn(NULL), cancelBtn(NULL), commitBtn(NULL){}

    void init(const WiFiProfileInfo &info);
    void onSwitch(int index);
    void onClick();
    void editIpAddress();
    void editNormalText();
    void onCancel();
    void onCommit();
    WiFiProfileEditor * const q_ptr;
    LabelButton *profileNameBtn;
    LabelButton *ssidBtn;
    IComboList *authTypeComboL;
    LabelButton *securityKeyBtn;
    IComboList *networkSettingComboL;
    LabelButton *staticIpBtn;
    LabelButton *defaultGatewayBtn;
    LabelButton *subnetMaskBtn;
    LabelButton *preferedDNSBtn;
    LabelButton *alternateDNSBtn;
    LButtonEx *cancelBtn;
    LButtonEx *commitBtn;

    QSize dhcpWindowSize;
};

/***************************************************************************************************
 * init, do the layout job
 **************************************************************************************************/
void WiFiProfileEditorPrivate::init(const WiFiProfileInfo &profile)
{
    Q_Q(WiFiProfileEditor);

    int submenuW;
    submenuW = DEFAULT_WIDGET_WIDTH;
    int itemW = submenuW - 20;
    int btnWidth = itemW / 2;
    int labelWidth = btnWidth / 2;

    QFont font = fontManager.textFont(FONT_SIZE);

    q->setTitleBarText(trs("WiFiProfileEdit"));

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(10, 4, 10, 4);
    contentLayout->setSpacing(2);

    profileNameBtn = new LabelButton(trs("ProfileName"));
    profileNameBtn->setFont(font);
    profileNameBtn->setValue(profile.profileName);
    profileNameBtn->label->setFixedWidth(labelWidth);
    profileNameBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(profileNameBtn);

    ssidBtn = new LabelButton(trs("WiFiAP"));
    ssidBtn->setFont(font);
    ssidBtn->setValue(profile.ssid);
    ssidBtn->label->setFixedWidth(labelWidth);
    ssidBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(ssidBtn);

    authTypeComboL = new IComboList(trs("EncryptType"));
    authTypeComboL->setFont(font);
    authTypeComboL->addItems(QStringList() << "None" << "WPA PSK" << "WPA2 PSK");
    authTypeComboL->setCurrentIndex(profile.authType);
    authTypeComboL->label->setFixedWidth(labelWidth);
    authTypeComboL->combolist->setFixedWidth(btnWidth);
    contentLayout->addWidget(authTypeComboL);

    securityKeyBtn = new LabelButton(trs("PassWord"));
    securityKeyBtn->setFont(font);
    securityKeyBtn->setValue(profile.securityKey);
    securityKeyBtn->label->setFixedWidth(labelWidth);
    securityKeyBtn->button->setFixedWidth(btnWidth);
    if (profile.authType == WiFiProfileInfo::Open)
    {
        securityKeyBtn->setEnabled(false);
    }
    contentLayout->addWidget(securityKeyBtn);

    networkSettingComboL = new IComboList(trs("NetworkSetting"));
    networkSettingComboL->setFont(font);
    networkSettingComboL->addItems(QStringList() << trs("DHCP") << trs("StaticIp"));
    networkSettingComboL->setCurrentIndex(profile.isStatic);
    networkSettingComboL->label->setFixedWidth(labelWidth);
    networkSettingComboL->combolist->setFixedWidth(btnWidth);
    contentLayout->addWidget(networkSettingComboL);

    staticIpBtn = new LabelButton(trs("StaticIp"));
    staticIpBtn->setFont(font);
    staticIpBtn->setValue(profile.staticIp);
    staticIpBtn->label->setFixedWidth(labelWidth);
    staticIpBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(staticIpBtn);

    defaultGatewayBtn = new LabelButton(trs("GateWay"));
    defaultGatewayBtn->setFont(font);
    defaultGatewayBtn->setValue(profile.defaultGateway);
    defaultGatewayBtn->label->setFixedWidth(labelWidth);
    defaultGatewayBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(defaultGatewayBtn);

    subnetMaskBtn = new LabelButton(trs("SubnetMask"));
    subnetMaskBtn->setFont(font);
    subnetMaskBtn->setValue(profile.subnetMask);
    subnetMaskBtn->label->setFixedWidth(labelWidth);
    subnetMaskBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(subnetMaskBtn);

    preferedDNSBtn = new LabelButton(trs("PreferredDNS"));
    preferedDNSBtn->setFont(font);
    preferedDNSBtn->setValue(profile.preferedDNS);
    preferedDNSBtn->label->setFixedWidth(labelWidth);
    preferedDNSBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(preferedDNSBtn);

    alternateDNSBtn = new LabelButton(trs("AlternateDNS"));
    alternateDNSBtn->setFont(font);
    alternateDNSBtn->setValue(profile.alternateDNS);
    alternateDNSBtn->label->setFixedWidth(labelWidth);
    alternateDNSBtn->button->setFixedWidth(btnWidth);
    contentLayout->addWidget(alternateDNSBtn);

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(20, 4, 20, 4);
    hlayout->setSpacing(20);
    commitBtn = new LButtonEx();
    commitBtn->setFont(font);
    commitBtn->setText(trs("Enter"));
    hlayout->addWidget(commitBtn);
    cancelBtn = new LButtonEx();
    cancelBtn->setFont(font);
    cancelBtn->setText(trs("Cancel"));
    hlayout->addWidget(cancelBtn);

    contentLayout->addLayout(hlayout);
    q->contentLayout->addLayout(contentLayout);

    staticIpBtn->setEnabled(profile.isStatic);
    defaultGatewayBtn->setEnabled(profile.isStatic);
    subnetMaskBtn->setEnabled(profile.isStatic);
    preferedDNSBtn->setEnabled(profile.isStatic);
    alternateDNSBtn->setEnabled(profile.isStatic);
}

/***************************************************************************************************
 * onSwitch, handle switch signal, like authentication type or network type
 **************************************************************************************************/
void WiFiProfileEditorPrivate::onSwitch(int index)
{
    Q_Q(WiFiProfileEditor);
    IComboList *sender = qobject_cast<IComboList*>(q->sender());
    if (!sender)
    {
        return;
    }

    if (sender == networkSettingComboL)
    {
        staticIpBtn->setEnabled(index);
        defaultGatewayBtn->setEnabled(index);
        subnetMaskBtn->setEnabled(index);
        preferedDNSBtn->setEnabled(index);
        alternateDNSBtn->setEnabled(index);
    }
    else if (sender == authTypeComboL)
    {
        if (authTypeComboL->currentIndex() == WiFiProfileInfo::Open)
        {
            securityKeyBtn->button->setText(QString());
            securityKeyBtn->setEnabled(false);
        } else{
            securityKeyBtn->setEnabled(true);
        }
    }
}

/***************************************************************************************************
 * editIpAddress, edit ip address input, like static ip, gateway, subnet mask, dns
 **************************************************************************************************/
void WiFiProfileEditorPrivate::editIpAddress()
{
    Q_Q(WiFiProfileEditor);
    LButton *sender = qobject_cast<LButton*>(q->sender());
    if (!sender)
    {
        return;
    }

    KeyBoardPanel englishPanel;
    englishPanel.setMaxInputLength(MAX_IP_STRING_LENGTH);
    englishPanel.setInitString(sender->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setBtnEnable("[0-9.]");
    englishPanel.setCheckValueHook(isIpStrValid);

    if (sender == staticIpBtn->button)
    {
        englishPanel.setTitleBarText(trs("StaticIp"));
    }
    else if (sender == defaultGatewayBtn->button)
    {
        englishPanel.setTitleBarText(trs("GateWay"));
    }
    else if (sender == subnetMaskBtn->button)
    {
        englishPanel.setTitleBarText(trs("SubnetMask"));
    }
    else if (sender == preferedDNSBtn->button)
    {
        englishPanel.setTitleBarText(trs("PreferredDNS"));
    }
    else if (sender == alternateDNSBtn->button)
    {
        englishPanel.setTitleBarText(trs("AlternateDNS"));
    }

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        sender->setText(text);
    }
}

/***************************************************************************************************
 * editNormalText, edit normal text, like profile name, ssid, security key
 **************************************************************************************************/
void WiFiProfileEditorPrivate::editNormalText()
{
    Q_Q(WiFiProfileEditor);
    LButton *sender = qobject_cast<LButton*>(q->sender());
    if (!sender)
    {
        return;
    }

    KeyBoardPanel englishPanel;
    if (sender == profileNameBtn->button)
    {
        englishPanel.setMaxInputLength(24);
        englishPanel.setTitleBarText(trs("ProfileName"));
    }
    else if (sender == ssidBtn->button)
    {
        englishPanel.setMaxInputLength(32);
        englishPanel.setTitleBarText(trs("WiFiAP"));
    }
    else if (sender == securityKeyBtn->button)
    {
        englishPanel.setMaxInputLength(63);
        englishPanel.setTitleBarText(trs("PassWord"));
    }

    englishPanel.setInitString(sender->text());
    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        sender->setText(text);
    }
}

/***************************************************************************************************
 * onCancel : handle cancel button clicked signal
 **************************************************************************************************/
void WiFiProfileEditorPrivate::onCancel()
{
    Q_Q(WiFiProfileEditor);
    q->done(0);
}

void WiFiProfileEditorPrivate::onCommit()
{
    Q_Q(WiFiProfileEditor);

    // check input
    if (profileNameBtn->button->text().isEmpty())
    {
        IMessageBox(QString(trs("NoProfileName")),
                    QString(trs("PleaseInputProfileName")),
                    QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }

    if (ssidBtn->button->text().isEmpty())
    {
        IMessageBox(QString(trs("NoSSID")),
                    QString(trs("PleaseInputSSIDName")),
                    QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }

    if (authTypeComboL->currentIndex() == WiFiProfileInfo::Wpa2_psk ||
            authTypeComboL->currentIndex() == WiFiProfileInfo::Wpa_psk)
    {
        int keylen = securityKeyBtn->button->text().length();
        if (keylen < 8 || keylen >=64)
        {
            IMessageBox(QString(trs("SecurityKeyError")),
                        QString(trs("WpaPskRequires")),
                        QStringList(trs("EnglishYESChineseSURE"))).exec();
            return;
        }
    }
    q->done(1);
}


/***************************************************************************************************
 * constructor
 **************************************************************************************************/
WiFiProfileEditor::WiFiProfileEditor(const WiFiProfileInfo &profile)
    :PopupWidget(),
      d_ptr(new WiFiProfileEditorPrivate(this))
{
    Q_D(WiFiProfileEditor);
    d->init(profile);
    connect(d->networkSettingComboL, SIGNAL(currentIndexChanged(int)), this, SLOT(onSwitch(int)));
    connect(d->authTypeComboL, SIGNAL(currentIndexChanged(int)), this, SLOT(onSwitch(int)));
    connect(d->staticIpBtn->button, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->defaultGatewayBtn->button, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->subnetMaskBtn->button, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->preferedDNSBtn->button, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->alternateDNSBtn->button, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->profileNameBtn->button, SIGNAL(clicked()), this, SLOT(editNormalText()));
    connect(d->ssidBtn->button, SIGNAL(clicked()), this, SLOT(editNormalText()));
    connect(d->securityKeyBtn->button, SIGNAL(clicked()), this, SLOT(editNormalText()));
    connect(d->cancelBtn, SIGNAL(realReleased()), this, SLOT(onCancel()));
    connect(d->commitBtn, SIGNAL(realReleased()), this, SLOT(onCommit()));
}

/***************************************************************************************************
 * destructor, do nothing, but needed of QScopePointer
 **************************************************************************************************/
WiFiProfileEditor::~WiFiProfileEditor()
{
}

/***************************************************************************************************
 * getProfileInfo, get the edit result
 **************************************************************************************************/
WiFiProfileInfo WiFiProfileEditor::getProfileInfo() const
{
    Q_D(const WiFiProfileEditor);
    WiFiProfileInfo profile;
    profile.profileName = d->profileNameBtn->button->text();
    profile.ssid = d->ssidBtn->button->text();
    profile.authType = (WiFiProfileInfo::AuthenticationType)d->authTypeComboL->combolist->currentIndex();
    profile.securityKey = d->securityKeyBtn->button->text();
    profile.isStatic = d->networkSettingComboL->currentIndex();
    if (profile.isStatic)
    {
        profile.staticIp = d->staticIpBtn->button->text();
        profile.defaultGateway = d->defaultGatewayBtn->button->text();
        profile.subnetMask = d->subnetMaskBtn->button->text();
        profile.preferedDNS = d->preferedDNSBtn->button->text();
        profile.alternateDNS = d->alternateDNSBtn->button->text();
    }
    return profile;
}

/***************************************************************************************************
 * keyPressEvent : handle child widget focus issue
 **************************************************************************************************/
void WiFiProfileEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            break;

        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            break;
        default:
            break;
    }
    PopupWidget::keyPressEvent(event);
}


#include "moc_WiFiProfileEditor.cpp"
