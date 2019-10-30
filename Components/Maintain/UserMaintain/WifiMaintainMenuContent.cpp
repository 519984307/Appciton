/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#include "WifiMaintainMenuContent.h"
#include "FontManager.h"
#include "NetworkManager.h"
#include "Debug.h"
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include "IConfig.h"
#include <QFocusEvent>
#include "WiFiProfileEditorWindow.h"
#include "MessageBox.h"
#include "ComboBox.h"
#include "Button.h"
#include "LanguageManager.h"

#define PROFILE_MAX_NUM 5
#define LISTVIEW_MAX_VISIABLE_TIME 4

class WifiMaintainMenuContentPrivate
{
public:
    Q_DECLARE_PUBLIC(WifiMaintainMenuContent)

    explicit WifiMaintainMenuContentPrivate(WifiMaintainMenuContent *const q_ptr)
        : q_ptr(q_ptr), switchCombo(NULL), profileList(NULL),
          model(NULL), addBtn(NULL), editBtn(NULL), delBtn(NULL)
    {}

    void onSwitch(int val);
    void updateProfileList();
    void onBtnClick();
    void loadProfiles();
    void saveProfiles();
    void updateBtnStatus();

    WifiMaintainMenuContent *const q_ptr;
    ComboBox *switchCombo;
    ListView *profileList;
    ListDataModel *model;
    Button *addBtn;
    Button *editBtn;
    Button *delBtn;
    QVector<WiFiProfileWindowInfo> profiles;
    bool _isEnabled;
};

/***************************************************************************************************
 * onSwitch: private slot, handle the switch combo button click event
 **************************************************************************************************/
void WifiMaintainMenuContentPrivate::onSwitch(int val)
{
    systemConfig.setNumValue("WiFi|EnableWifi", val);
    systemConfig.save();
    _isEnabled = !!val;
//    emit WifiMaintainMenuContent.updateWifiProfileSignal(_isEnabled);
}

/***************************************************************************************************
 * updateProfileList : Update the profile List item from config
 **************************************************************************************************/
void WifiMaintainMenuContentPrivate::updateProfileList()
{
    QStringList profileListName;
    for (int i = 0; i < profiles.count(); i++)
    {
        profileListName.append(profiles.at(i).profileName);
    }
    model->setStringList(profileListName);

    int count = profiles.count();

    if (count)
    {
        profileList->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        profileList->setFocusPolicy(Qt::NoFocus);
    }

    if (count >= PROFILE_MAX_NUM)
    {
        addBtn->setEnabled(false);
    }
    else
    {
        addBtn->setEnabled(true);
    }

    editBtn->setEnabled(false);
    delBtn->setEnabled(false);
}


/***************************************************************************************************
 * caseInsensitiveLessThan, use to compare profile name
 **************************************************************************************************/
static bool caseInsensitiveLessThan(const WiFiProfileWindowInfo &p1, const WiFiProfileWindowInfo &p2)
{
    return p1.profileName.toLower() < p2.profileName.toLower();
}

/***************************************************************************************************
 * onBtnClick : handle add, edit, delete button click signal
 **************************************************************************************************/
void WifiMaintainMenuContentPrivate::onBtnClick()
{
    Q_Q(WifiMaintainMenuContent);
    QObject *sender = q->sender();

    if (sender == addBtn)
    {
        WiFiProfileEditorWindow editor;
        while (editor.exec())
        {
            QVector<WiFiProfileWindowInfo>::ConstIterator iter;
            WiFiProfileWindowInfo editProfile = editor.getProfileInfo();
            bool duplicate = false;
            for (iter = profiles.constBegin(); iter != profiles.constEnd(); iter ++)
            {
                if (iter->profileName == editProfile.profileName)
                {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate)
            {
                QString title = trs("ProfileConflict");
                MessageBox msgBox(title, QString("%1 %2 %3").arg(trs("Profile"))
                                  .arg(editProfile.profileName).arg(trs("AlreadyExist")),
                                  false);
                msgBox.exec();
                continue;
            }
            else
            {
                profiles.append(editor.getProfileInfo());
                qSort(profiles.begin(), profiles.end(), caseInsensitiveLessThan);
                updateProfileList();
                saveProfiles();
                break;
            }
        }
    }
    else if (sender == editBtn)
    {
        int index = profileList->curCheckedRow();
        WiFiProfileEditorWindow editor(profiles[index]);
        while (editor.exec())
        {
            bool duplicate = false;
            QString profileName = editor.getProfileInfo().profileName;
            for (int i = 0;  i < profiles.size(); i++)
            {
                if (i != index && profileName == profiles[i].profileName)
                {
                    duplicate = true;
                    break;
                }
            }

            if (duplicate)
            {
                QString title = trs("ProfileConflict");
                MessageBox msgBox(title, QString("%1 %2 %3").arg(trs("Profile"))
                                  .arg(profileName).arg(trs("AlreadyExist")), false);
                msgBox.exec();
                continue;
            }

            profiles[index] = editor.getProfileInfo();
            qSort(profiles.begin(), profiles.end(), caseInsensitiveLessThan);
            updateProfileList();
            saveProfiles();
            break;
        }
    }
    else if (sender == delBtn)
    {
        MessageBox messageBox(trs("Prompt"), trs("DeleteSelectedWifiProfile"));
        if (messageBox.exec() == 0)
        {
            return;
        }

        int index = profileList->curCheckedRow();
        profiles.remove(index);
        qSort(profiles.begin(), profiles.end(), caseInsensitiveLessThan);
        updateProfileList();
        saveProfiles();
        delBtn->setEnabled(false);
        editBtn->setEnabled(false);
    }
    else
    {
        qDebug() << Q_FUNC_INFO << "Unknown singal sender!";
    }
}

/***************************************************************************************************
 * loadProfiles, load the profiles from the config file
 **************************************************************************************************/
void WifiMaintainMenuContentPrivate::loadProfiles()
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
        WiFiProfileWindowInfo profile;
        systemConfig.getStrValue(prefix + "ProfileName", profile.profileName);
        systemConfig.getStrValue(prefix + "SSID", profile.ssid);
        systemConfig.getNumValue(prefix + "AuthType", tmpValue);
        profile.authType = (WiFiProfileWindowInfo::AuthenticationType) tmpValue;
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
    qSort(profiles.begin(), profiles.end(), caseInsensitiveLessThan);
}

/***************************************************************************************************
 * saveProfiles : save the profiles, it will delete all the old profiles and add the new profiles
 **************************************************************************************************/
void WifiMaintainMenuContentPrivate::saveProfiles()
{
    bool ok;
    QString tmpStr;
    if (!systemConfig.getStrAttr("WiFi|Profiles", "Count", tmpStr))
    {
        return;
    }

    int count = tmpStr.toInt(&ok);

    if (!ok)
    {
        return;
    }

    // remove old profile
    for (int i = 0; i < count; i++)
    {
        QString prefix = QString("WiFi|Profiles|Profile%1").arg(i);
        systemConfig.removeNode(prefix);
    }

    // add new profile
    for (int j = 0; j < profiles.count(); j++)
    {
        QString prefix("WiFi|Profiles");
        QString nodeName = QString("Profile%1").arg(j);
        systemConfig.addNode(prefix, nodeName);
        prefix = prefix + "|" + nodeName;
        systemConfig.addNode(prefix, "ProfileName", profiles.at(j).profileName);
        systemConfig.addNode(prefix, "SSID", profiles.at(j).ssid);
        systemConfig.addNode(prefix, "AuthType", QString::number(static_cast<int>(profiles.at(j).authType)));
        systemConfig.addNode(prefix, "SecurityKey", profiles.at(j).securityKey);
        systemConfig.addNode(prefix, "IsStatic", QString::number(profiles.at(j).isStatic));
        if (profiles.at(j).isStatic)
        {
            systemConfig.addNode(prefix, "StaticIP", profiles.at(j).staticIp);
            systemConfig.addNode(prefix, "DefaultGateway", profiles.at(j).defaultGateway);
            systemConfig.addNode(prefix, "SubnetMask", profiles.at(j).subnetMask);
            systemConfig.addNode(prefix, "PreferedDNS", profiles.at(j).preferedDNS);
            systemConfig.addNode(prefix, "AlternateDNS", profiles.at(j).alternateDNS);
        }
    }

    systemConfig.setStrAttr("WiFi|Profiles", "Count", QString::number(profiles.count()));
    int currentSelect = -1;  // default select nothing if wifi profile changes
    systemConfig.setNumAttr("WiFi|Profiles", "CurrentSelect", currentSelect);

    systemConfig.save();

// emit WifiMaintainMenuContent.updateWifiProfileSignal(_isEnabled);
}

/***************************************************************************************************
 * focusFirstItem : set the first widget to get focus
 **************************************************************************************************/
void WifiMaintainMenuContent::focusFirstItem()
{
    // has set the focus proxy, just set this widget foucs will make the proxy get focus
    this->setFocus();
}

/***************************************************************************************************
 * layoutExec : layout issue
 **************************************************************************************************/
void WifiMaintainMenuContent::layoutExec()
{
    Q_D(WifiMaintainMenuContent);

    // 强制窗口不聚焦
    this->setFocusPolicy(Qt::NoFocus);

    QHBoxLayout *hLayout;
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *label;

    // switch combo list
    label = new QLabel(trs("WiFiMenu"));
    d->switchCombo = new ComboBox;
    d->switchCombo->addItems(QStringList()
                             << trs("Off")
                             << trs("On"));
    connect(d->switchCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onSwitch(int)));

    hLayout = new QHBoxLayout;
    hLayout->addWidget(label);
    hLayout->addWidget(d->switchCombo);
    layout->addLayout(hLayout);

    // configure label
    label = new QLabel(trs("ConfiguredAccessPointProfiles"));
    layout->addWidget(label);

    // profile list
    ListView *listView = new ListView();
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    ListDataModel *model = new ListDataModel(this);
    listView->setModel(model);
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                             + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    layout->addWidget(listView);
    d->profileList = listView;
    d->model = model;

    // buttons
    d->addBtn = new Button(trs("Add"));
    d->addBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d->addBtn, SIGNAL(released()), this, SLOT(onBtnClick()));

    d->editBtn = new Button(trs("Edit"));
    d->editBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d->editBtn, SIGNAL(released()), this, SLOT(onBtnClick()));

    d->delBtn = new Button(trs("Delete"));
    d->delBtn->setButtonStyle(Button::ButtonTextOnly);
    connect(d->delBtn, SIGNAL(released()), this, SLOT(onBtnClick()));

    hLayout = new QHBoxLayout;
    hLayout->addWidget(d->addBtn);
    hLayout->addWidget(d->editBtn);
    hLayout->addWidget(d->delBtn);
    layout->addLayout(hLayout);
    layout->addStretch();
}

/***************************************************************************************************
 * call before display, load configurations here
 **************************************************************************************************/
void WifiMaintainMenuContent::readyShow()
{
    Q_D(WifiMaintainMenuContent);
    bool enableWifi = false;
    systemConfig.getNumValue("WiFi|EnableWifi", enableWifi);
    d->_isEnabled = enableWifi;
    d->switchCombo->setCurrentIndex(enableWifi);
    d->loadProfiles();
    d->updateProfileList();
}

/***************************************************************************************************
 * constructor
 **************************************************************************************************/
WifiMaintainMenuContent::WifiMaintainMenuContent()
    : MenuContent(trs("WiFiMenu"), trs("WiFiMenuDesc")),
      d_ptr(new WifiMaintainMenuContentPrivate(this))
{
    setFocusPolicy(Qt::StrongFocus);
}

WifiMaintainMenuContent::~WifiMaintainMenuContent()
{
}

void WifiMaintainMenuContentPrivate::updateBtnStatus()
{
    int row = profileList->curCheckedRow();
    bool isEnabled;
    if (row == -1)
    {
        isEnabled = false;
    }
    else
    {
        isEnabled = true;
    }
    editBtn->setEnabled(isEnabled);
    delBtn->setEnabled(isEnabled);
}

#include "moc_WifiMaintainMenuContent.cpp"
