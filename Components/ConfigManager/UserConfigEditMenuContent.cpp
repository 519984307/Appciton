/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#include "UserConfigEditMenuContent.h"
#include <QMap>
#include "Button.h"
#include "LanguageManager.h"
#include "ConfigManager.h"
#include <QLabel>
#include "PatientManager.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include "FontManager.h"
#include "ConfigEditMenuWindow.h"
#include "WindowManager.h"
#include <QHideEvent>
#include "ListView.h"
#include "ListDataModel.h"
#include "ListViewItemDelegate.h"
#include "ConfigManagerMenuWindow.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"
#define CONFIG_MAX_NUM 3
#define LISTVIEW_MAX_VISIABLE_TIME 6

class UserConfigEditMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_ADD_CONFIG = 0,
        ITEM_BTN_EDIT_CONFIG,
        ITEM_BTN_DEL_CONFIG,

        ITEM_LTW_CONFIG_LIST = 0,
    };

    UserConfigEditMenuContentPrivate()
        :  curConfig(NULL), curEditIndex(-1),
           configDataModel(NULL), configListView(NULL),
           editWindow(NULL)
    {
        btns.clear();
    }

    void loadConfigs();
    void updateConfigList();

    //  check whether a config exist base on the config name
    bool isConfigExist(const QString &name) const;

    //  get a default config name
    QString generateDefaultConfigName() const;

    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;

    QMap <MenuItem, Button *> btns;

    ListDataModel *configDataModel;
    ListView *configListView;
    ConfigEditMenuWindow *editWindow;
};

void UserConfigEditMenuContentPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

/**
 * @brief UserConfigEditMenuContentPrivate::updateConfigList
 *           update the config list widget
 */
void UserConfigEditMenuContentPrivate::updateConfigList()
{
    QStringList configNameList;
    for (int i = 0; i < configs.count(); i++)
    {
        configNameList.append(configs.at(i).name);
    }
    configDataModel->setStringList(configNameList);

    int curSelectedRow = configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    btns[ITEM_BTN_EDIT_CONFIG]->setEnabled(isEnable);
    btns[ITEM_BTN_DEL_CONFIG]->setEnabled(isEnable);

    if (configDataModel->getRowCount() >= CONFIG_MAX_NUM)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    btns[ITEM_BTN_ADD_CONFIG]->setEnabled(isEnable);
}

bool UserConfigEditMenuContentPrivate::isConfigExist(const QString &name) const
{
    for (int i = 0 ; i < configs.size(); i++)
    {
        if (configs.at(i).name == name)
        {
            return true;
        }
    }
    return false;
}

QString UserConfigEditMenuContentPrivate::generateDefaultConfigName() const
{
    QString prefix("UserConfig");
    for (int i = 1; i <= CONFIG_MAX_NUM; i++)
    {
        QString name = prefix + QString::number(i);
        if (!isConfigExist(name))
        {
            return name;
        }
    }
    qdebug("Warning, can't generate default config name.");
    return prefix;
}

UserConfigEditMenuContent::UserConfigEditMenuContent()
    : MenuContent(trs("ConfigManagerment"),
                  trs("ConfigManagermentDesc")),
      d_ptr(new UserConfigEditMenuContentPrivate())
{
}

UserConfigEditMenuContent::~UserConfigEditMenuContent()
{
    delete d_ptr;
}


void UserConfigEditMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

void UserConfigEditMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_ADD_CONFIG])
    {
        if (d_ptr->configDataModel->getRowCount() >= CONFIG_MAX_NUM)
        {
        }
        else
        {
            if (d_ptr->curConfig)
            {
                delete d_ptr->curConfig;
            }

            QFile myFile(QString("%1/%2")
                         .arg(CONFIG_DIR)
                         .arg(configManager.runningConfigFilename(patientManager.getType())));
            if (!myFile.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                d_ptr->curConfig = new Config(QString("%1/%2")
                                              .arg(CONFIG_DIR)
                                              .arg(configManager.factoryConfigFilename(patientManager.getType())));
            }
            else
            {

                d_ptr->curConfig = new Config(QString("%1/%2")
                                              .arg(CONFIG_DIR)
                                              .arg(configManager.runningConfigFilename(patientManager.getType())));
            }

            d_ptr->editWindow = new ConfigEditMenuWindow();
            d_ptr->editWindow->setCurrentEditConfigName(d_ptr->generateDefaultConfigName());
            d_ptr->editWindow->setCurrentEditConfig(d_ptr->curConfig);
            d_ptr->editWindow->initializeSubMenu();
            windowManager.showWindow(d_ptr->editWindow, WindowManager::WINDOW_TYPE_NONMODAL);
            connect(d_ptr->editWindow , SIGNAL(finished(int)) , this , SLOT(onEditFinished()));
        }
    }
    else if (btn == d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG])
    {
        if (d_ptr->curConfig)
        {
            delete d_ptr->curConfig;
        }
        int index = d_ptr->configListView->curCheckedRow();
        d_ptr->curEditIndex = index;
        d_ptr->curConfig = new Config(QString("%1/%2")
                                      .arg(CONFIG_DIR)
                                      .arg(d_ptr->configs.at(index).fileName));
        d_ptr->editWindow = new ConfigEditMenuWindow();
        d_ptr->editWindow->setCurrentEditConfigName(d_ptr->configs.at(index).name);
        d_ptr->editWindow->setCurrentEditConfig(d_ptr->curConfig);
        d_ptr->editWindow->initializeSubMenu();
        windowManager.showWindow(d_ptr->editWindow, WindowManager::WINDOW_TYPE_NONMODAL);
        connect(d_ptr->editWindow , SIGNAL(finished(int)) , this , SLOT(onEditFinished()));
    }
    else if (btn == d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG])
    {
        int index = d_ptr->configListView->curCheckedRow();
        QString filename  = QString("%1/%2").arg(CONFIG_DIR).arg(d_ptr->configs.at(index).fileName);

        //  delete the config file
        QFile::remove(filename);
        d_ptr->configs.removeAt(index);
        configManager.saveUserConfigInfo(d_ptr->configs);

        d_ptr->loadConfigs();
        d_ptr->updateConfigList();
    }
    else
    {
        qdebug("Unknown singal sender!");
    }
}

void UserConfigEditMenuContent::onEditFinished()
{
    //  can't add, too many
    //  TODO show some message
    QString configName = d_ptr->editWindow->getCurrentEditConfigName();
    if (configName.isEmpty())
    {
        return;
    }
    if (d_ptr->curEditIndex >= 0)
    {
        //  edit exist config
        d_ptr->curConfig->saveToDisk();
        if (d_ptr->configs.at(d_ptr->curEditIndex).name != configName)
        {
            d_ptr->configs[d_ptr->curEditIndex].name = configName;
            configManager.saveUserConfigInfo(d_ptr->configs);
        }
    }
    else
    {
        //  add new config
        configManager.saveUserDefineConfig(configName, d_ptr->curConfig);
    }

    d_ptr->editWindow->setCurrentEditConfig(NULL);
    d_ptr->editWindow->setCurrentEditConfigName(QString());

    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
    d_ptr->curEditIndex = -1;
}

void UserConfigEditMenuContent::layoutExec()
{
    if (layout())
    {
        //  already install layout
        return;
    }

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(10);
    layout->setAlignment(Qt::AlignTop);

    QLabel *label = new QLabel(trs("ConfigManagerment"));
    layout->addWidget(label);

    ListView *listView = new ListView(this);
    listView->setItemDelegate(new ListViewItemDelegate(listView));
    ListDataModel *model = new ListDataModel(this);
    listView->setModel(model);
    listView->setFixedHeight(LISTVIEW_MAX_VISIABLE_TIME * model->getRowHeightHint()
                         + listView->spacing() * (LISTVIEW_MAX_VISIABLE_TIME * 2));
    d_ptr->configDataModel = model;
    connect(listView, SIGNAL(enterSignal()), this, SLOT(updateBtnStatus()));
    d_ptr->configListView = listView;
    layout->addWidget(listView);

    //  buttons
    QHBoxLayout *hl = new QHBoxLayout;
    Button *button;

    //  add buttons
    button = new Button(trs("Add"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->btns.insert(UserConfigEditMenuContentPrivate::ITEM_BTN_ADD_CONFIG, button);

    //  edit buttons
    button = new Button(trs("Edit"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->btns.insert(UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG, button);

    //  del buttons
    button = new Button(trs("Delete"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->btns.insert(UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG, button);

    layout->addLayout(hl);
    layout->addStretch(1);
}

void UserConfigEditMenuContent::updateBtnStatus()
{
    int curSelectedRow = d_ptr->configListView->curCheckedRow();
    bool isEnable;
    if (curSelectedRow == -1)
    {
        isEnable = false;
    }
    else
    {
        isEnable = true;
    }
    d_ptr->btns[d_ptr->ITEM_BTN_DEL_CONFIG]->setEnabled(isEnable);
    d_ptr->btns[d_ptr->ITEM_BTN_EDIT_CONFIG]->setEnabled(isEnable);
}
