/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#include "WiFiProfileEditorWindow.h"

#include <QRegExp>
#include "LanguageManager.h"
#include "FontManager.h"
#include "WindowManager.h"
#include "Debug.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"
#include "SystemManager.h"
#include <QApplication>
#include "Button.h"
#include "ComboBox.h"
#include <QLabel>
#include <QGridLayout>
#include <QKeyEvent>

#define FONT_SIZE 14
#define MAX_IP_STRING_LENGTH 15
#define DEFAULT_WIDGET_WIDTH 540


class WiFiProfileEditorWindowPrivate
{
public:
    Q_DECLARE_PUBLIC(WiFiProfileEditorWindow)
    explicit WiFiProfileEditorWindowPrivate(WiFiProfileEditorWindow *const q_ptr)
        : q_ptr(q_ptr), profileNameBtn(NULL), ssidBtn(NULL), authTypeComboL(NULL),
          securityKeyBtn(NULL), networkSettingComboL(NULL), staticIpBtn(NULL), defaultGatewayBtn(NULL),
          subnetMaskBtn(NULL), preferedDNSBtn(NULL), alternateDNSBtn(NULL), cancelBtn(NULL), commitBtn(NULL) {}

    void init(const WiFiProfileInfo &info);
    void onSwitch(int index);
    void onClick();
    void editIpAddress();
    void editNormalText();
    void onCancel();
    void onCommit();
    WiFiProfileEditorWindow *const q_ptr;
    Button *profileNameBtn;
    Button *ssidBtn;
    ComboBox *authTypeComboL;
    Button *securityKeyBtn;
    ComboBox *networkSettingComboL;
    Button *staticIpBtn;
    Button *defaultGatewayBtn;
    Button *subnetMaskBtn;
    Button *preferedDNSBtn;
    Button *alternateDNSBtn;
    Button *cancelBtn;
    Button *commitBtn;

    QSize dhcpWindowSize;
};

/***************************************************************************************************
 * init, do the layout job
 **************************************************************************************************/
void WiFiProfileEditorWindowPrivate::init(const WiFiProfileInfo &profile)
{
    Q_Q(WiFiProfileEditorWindow);

    q->resize(windowManager.getPopWindowWidth(), windowManager.getPopWindowHeight());
    q->setToolTip(trs("WiFiProfileEdit"));

    QGridLayout *layout = new QGridLayout;
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;

    QLabel *label;

    label = new QLabel(trs("ProfileName"));
    layout->addWidget(label, 0, 0);
    profileNameBtn = new Button(profile.profileName);
    profileNameBtn->setButtonStyle(Button::ButtonTextOnly);
    profileNameBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(profileNameBtn, 0, 1);

    label = new QLabel(trs("WiFiAP"));
    layout->addWidget(label, 0, 2);
    ssidBtn = new Button(profile.ssid);
    ssidBtn->setButtonStyle(Button::ButtonTextOnly);
    ssidBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(ssidBtn, 0, 3);

    label = new QLabel(trs("EncryptType"));
    layout->addWidget(label, 1, 0);
    authTypeComboL = new ComboBox;
    authTypeComboL->addItems(QStringList()
                             << "None"
                             << "WPA PSK"
                             << "WPA2 PSK");
    authTypeComboL->setCurrentIndex(profile.authType);
    authTypeComboL->setFixedWidth(q->width() / 4);
    layout->addWidget(authTypeComboL, 1, 1);

    label = new QLabel(trs("PassWord"));
    layout->addWidget(label, 1, 2);
    securityKeyBtn = new Button(profile.securityKey);
    securityKeyBtn->setButtonStyle(Button::ButtonTextOnly);
    if (profile.authType == WiFiProfileInfo::Open)
    {
        securityKeyBtn->setEnabled(false);
    }
    securityKeyBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(securityKeyBtn, 1, 3);

    label = new QLabel(trs("NetworkSetting"));
    layout->addWidget(label, 2, 0);
    networkSettingComboL = new ComboBox;
    networkSettingComboL->addItems(QStringList()
                                   << trs("DHCP")
                                   << trs("StaticIp"));
    networkSettingComboL->setCurrentIndex(profile.isStatic);
    networkSettingComboL->setFixedWidth(q->width() / 4);
    layout->addWidget(networkSettingComboL, 2, 1);

    label = new QLabel(trs("StaticIp"));
    layout->addWidget(label, 2, 2);
    staticIpBtn = new Button(profile.staticIp);
    staticIpBtn->setButtonStyle(Button::ButtonTextOnly);
    staticIpBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(staticIpBtn, 2, 3);

    label = new QLabel(trs("GateWay"));
    layout->addWidget(label, 3, 0);
    defaultGatewayBtn = new Button(profile.defaultGateway);
    defaultGatewayBtn->setButtonStyle(Button::ButtonTextOnly);
    defaultGatewayBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(defaultGatewayBtn, 3, 1);

    label = new QLabel(trs("SubnetMask"));
    layout->addWidget(label, 3, 2);
    subnetMaskBtn = new Button(profile.subnetMask);
    subnetMaskBtn->setButtonStyle(Button::ButtonTextOnly);
    subnetMaskBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(subnetMaskBtn, 3, 3);

    label = new QLabel(trs("PreferredDNS"));
    layout->addWidget(label, 4, 0);
    preferedDNSBtn = new Button(profile.preferedDNS);
    preferedDNSBtn->setButtonStyle(Button::ButtonTextOnly);
    preferedDNSBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(preferedDNSBtn, 4, 1);

    label = new QLabel(trs("AlternateDNS"));
    layout->addWidget(label, 4, 2);
    alternateDNSBtn = new Button(profile.alternateDNS);
    alternateDNSBtn->setButtonStyle(Button::ButtonTextOnly);
    alternateDNSBtn->setFixedWidth(q->width() / 4);
    layout->addWidget(alternateDNSBtn, 4, 3);

    commitBtn = new Button(trs("Enter"));
    commitBtn->setButtonStyle(Button::ButtonTextOnly);
    commitBtn->setFixedWidth(q->width() / 3);
    hLayout->addWidget(commitBtn);
    cancelBtn = new Button(trs("Cancel"));
    cancelBtn->setButtonStyle(Button::ButtonTextOnly);
    cancelBtn->setFixedWidth(q->width() / 3);
    hLayout->addWidget(cancelBtn);

    layout->setMargin(2);
    layout->setSpacing(10);
    vLayout->addLayout(layout);
    vLayout->addStretch();
    vLayout->addLayout(hLayout);
    q->setWindowLayout(vLayout);

    staticIpBtn->setEnabled(profile.isStatic);
    defaultGatewayBtn->setEnabled(profile.isStatic);
    subnetMaskBtn->setEnabled(profile.isStatic);
    preferedDNSBtn->setEnabled(profile.isStatic);
    alternateDNSBtn->setEnabled(profile.isStatic);
}

/***************************************************************************************************
 * onSwitch, handle switch signal, like authentication type or network type
 **************************************************************************************************/
void WiFiProfileEditorWindowPrivate::onSwitch(int index)
{
    Q_Q(WiFiProfileEditorWindow);
    ComboBox *sender = qobject_cast<ComboBox *>(q->sender());
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
            securityKeyBtn->setText(QString());
            securityKeyBtn->setEnabled(false);
        }
        else
        {
            securityKeyBtn->setEnabled(true);
        }
    }
}

/***************************************************************************************************
 * editIpAddress, edit ip address input, like static ip, gateway, subnet mask, dns
 **************************************************************************************************/
void WiFiProfileEditorWindowPrivate::editIpAddress()
{
    Q_Q(WiFiProfileEditorWindow);
    Button *sender = qobject_cast<Button *>(q->sender());
    if (!sender)
    {
        return;
    }

    KeyInputPanel englishPanel;
    englishPanel.setMaxInputLength(MAX_IP_STRING_LENGTH);
    englishPanel.setInitString(sender->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setBtnEnable("[0-9.]");
    englishPanel.setCheckValueHook(WiFiProfileInfo::isIpStrValid);

    if (sender == staticIpBtn)
    {
        englishPanel.setWindowTitle(trs("StaticIp"));
    }
    else if (sender == defaultGatewayBtn)
    {
        englishPanel.setWindowTitle(trs("GateWay"));
    }
    else if (sender == subnetMaskBtn)
    {
        englishPanel.setWindowTitle(trs("SubnetMask"));
    }
    else if (sender == preferedDNSBtn)
    {
        englishPanel.setWindowTitle(trs("PreferredDNS"));
    }
    else if (sender == alternateDNSBtn)
    {
        englishPanel.setWindowTitle(trs("AlternateDNS"));
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
void WiFiProfileEditorWindowPrivate::editNormalText()
{
    Q_Q(WiFiProfileEditorWindow);
    Button *sender = qobject_cast<Button *>(q->sender());
    if (!sender)
    {
        return;
    }

    KeyInputPanel englishPanel;
    if (sender == profileNameBtn)
    {
        englishPanel.setMaxInputLength(24);
        englishPanel.setWindowTitle(trs("ProfileName"));
    }
    else if (sender == ssidBtn)
    {
        englishPanel.setMaxInputLength(32);
        englishPanel.setWindowTitle(trs("WiFiAP"));
    }
    else if (sender == securityKeyBtn)
    {
        englishPanel.setMaxInputLength(63);
        englishPanel.setWindowTitle(trs("PassWord"));
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
void WiFiProfileEditorWindowPrivate::onCancel()
{
    Q_Q(WiFiProfileEditorWindow);
    q->done(0);
}

void WiFiProfileEditorWindowPrivate::onCommit()
{
    Q_Q(WiFiProfileEditorWindow);

    // check input
    if (profileNameBtn->text().isEmpty())
    {
        MessageBox(QString(trs("NoProfileName")), QString(trs("PleaseInputProfileName")),
                    QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }

    if (ssidBtn->text().isEmpty())
    {
        MessageBox(QString(trs("NoSSID")), QString(trs("PleaseInputSSIDName")),
                    QStringList(trs("EnglishYESChineseSURE"))).exec();
        return;
    }

    if (authTypeComboL->currentIndex() == WiFiProfileInfo::Wpa2_psk
            || authTypeComboL->currentIndex() == WiFiProfileInfo::Wpa_psk)
    {
        int keylen = securityKeyBtn->text().length();
        if (keylen < 8 || keylen >= 64)
        {
            MessageBox(QString(trs("SecurityKeyError")), QString(trs("WpaPskRequires")),
                        QStringList(trs("EnglishYESChineseSURE")), true).exec();
            return;
        }
    }
    q->done(1);
}


/***************************************************************************************************
 * constructor
 **************************************************************************************************/
WiFiProfileEditorWindow::WiFiProfileEditorWindow(const WiFiProfileInfo &profile)
    : Dialog(),
      d_ptr(new WiFiProfileEditorWindowPrivate(this))
{
    Q_D(WiFiProfileEditorWindow);
    d->init(profile);
    connect(d->networkSettingComboL, SIGNAL(currentIndexChanged(int)), this, SLOT(onSwitch(int)));
    connect(d->authTypeComboL, SIGNAL(currentIndexChanged(int)), this, SLOT(onSwitch(int)));
    connect(d->staticIpBtn, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->defaultGatewayBtn, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->subnetMaskBtn, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->preferedDNSBtn, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->alternateDNSBtn, SIGNAL(clicked()), this, SLOT(editIpAddress()));
    connect(d->profileNameBtn, SIGNAL(clicked()), this, SLOT(editNormalText()));
    connect(d->ssidBtn, SIGNAL(clicked()), this, SLOT(editNormalText()));
    connect(d->securityKeyBtn, SIGNAL(clicked()), this, SLOT(editNormalText()));
    connect(d->cancelBtn, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(d->commitBtn, SIGNAL(clicked()), this, SLOT(onCommit()));
}

/***************************************************************************************************
 * destructor, do nothing, but needed of QScopePointer
 **************************************************************************************************/
WiFiProfileEditorWindow::~WiFiProfileEditorWindow()
{
}

/***************************************************************************************************
 * getProfileInfo, get the edit result
 **************************************************************************************************/
WiFiProfileInfo WiFiProfileEditorWindow::getProfileInfo() const
{
    Q_D(const WiFiProfileEditorWindow);
    WiFiProfileInfo profile;
    profile.profileName = d->profileNameBtn->text();
    profile.ssid = d->ssidBtn->text();
    profile.authType = (WiFiProfileInfo::AuthenticationType)d->authTypeComboL->currentIndex();
    profile.securityKey = d->securityKeyBtn->text();
    profile.isStatic = d->networkSettingComboL->currentIndex();
    if (profile.isStatic)
    {
        profile.staticIp = d->staticIpBtn->text();
        profile.defaultGateway = d->defaultGatewayBtn->text();
        profile.subnetMask = d->subnetMaskBtn->text();
        profile.preferedDNS = d->preferedDNSBtn->text();
        profile.alternateDNS = d->alternateDNSBtn->text();
    }
    return profile;
}

/***************************************************************************************************
 * keyPressEvent : handle child widget focus issue
 **************************************************************************************************/
void WiFiProfileEditorWindow::keyPressEvent(QKeyEvent *event)
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
    Dialog::keyPressEvent(event);
}


#include "moc_WiFiProfileEditorWindow.cpp"
