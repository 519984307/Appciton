/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.14
 **/
#pragma once
#include "UserConfigEditMenuContent.h"
#include <QMap>
#include "Button.h"
#include "LanguageManager.h"
#include "ConfigManager.h"
#include "IListWidget.h"
#include <QLabel>
#include "ConfigEditMenuGrp.h"
#include "PatientManager.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include "FontManager.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

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
        : lastSelectItem(NULL), curConfig(NULL), curEditIndex(-1)
    {
        btns.clear();
        ltws.clear();
    }

    void loadConfigs();
    void updateConfigList();

    //  check whether a config exist base on the config name
    bool isConfigExist(const QString &name) const;

    //  get a default config name
    QString generateDefaultConfigName() const;

    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;

    QMap <MenuItem, Button *> btns;
    QMap <MenuItem, IListWidget *> ltws;
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
    //  remove old item
    while (ltws[ITEM_LTW_CONFIG_LIST]->count())
    {
        QListWidgetItem *item = ltws[ITEM_LTW_CONFIG_LIST]->takeItem(0);
        delete item;
    }

    for (int i = 0; i < configs.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(configs.at(i).name, ltws[ITEM_LTW_CONFIG_LIST]);
        item->setSizeHint(QSize(CONFIG_LIST_ITME_W, CONFIG_LIST_ITEM_H));
    }

    int count = configs.count();
    if (count)
    {
        ltws[ITEM_LTW_CONFIG_LIST]->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        ltws[ITEM_LTW_CONFIG_LIST]->setFocusPolicy(Qt::NoFocus);
    }

    if (count >= CONFIG_MAX_NUM)
    {
        btns[ITEM_BTN_ADD_CONFIG]->setEnabled(false);
    }
    else
    {
        btns[ITEM_BTN_ADD_CONFIG]->setEnabled(true);
    }
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
    connect(configEditMenuGrp.getCloseBtn(), SIGNAL(realReleased()), this, SLOT(onEditFinished()));
}

UserConfigEditMenuContent::~UserConfigEditMenuContent()
{
}

bool UserConfigEditMenuContent::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST])
    {
        if (ev->type() == QEvent::FocusIn)
        {
            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
            if (e->reason() == Qt::TabFocusReason)
            {
                d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setCurrentRow(0);
            }
            else if (e->reason() == Qt::BacktabFocusReason)
            {
                d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]
                ->setCurrentRow(d_ptr->ltws[UserConfigEditMenuContentPrivate
                                            ::ITEM_LTW_CONFIG_LIST]->count() - 1);
            }
        }

        if (ev->type() == QEvent::Hide)
        {
            if (d_ptr->lastSelectItem)
            {
                d_ptr->lastSelectItem->setIcon(QIcon());
                d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG]->setEnabled(false);
                d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG]->setEnabled(false);
                d_ptr->lastSelectItem = NULL;
            }
        }
    }
    return false;
}

void UserConfigEditMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

// handle config list exit event
void UserConfigEditMenuContent::onExitList(bool backTab)
{
    if (backTab)
    {
        focusPreviousChild();
    }
    else
    {
        focusNextChild();
    }
}

void UserConfigEditMenuContent::onConfigClick()
{
    QListWidgetItem *item = d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->currentItem();
    if (d_ptr->lastSelectItem)
    {
        d_ptr->lastSelectItem->setIcon(QIcon());
    }

    if (item != d_ptr->lastSelectItem)
    {
        item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        d_ptr->lastSelectItem = item;
    }
    else
    {
        d_ptr->lastSelectItem = NULL;
    }

    if (d_ptr->lastSelectItem)
    {
        d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG]->setEnabled(true);
        d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG]->setEnabled(true);
    }
    else
    {
        d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG]->setEnabled(false);
        d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG]->setEnabled(false);
    }
}


void UserConfigEditMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_ADD_CONFIG])
    {
        if (d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->count() >= CONFIG_MAX_NUM)
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

            configEditMenuGrp.setCurrentEditConfigName(d_ptr->generateDefaultConfigName());
            configEditMenuGrp.setCurrentEditConfig(d_ptr->curConfig);
            configEditMenuGrp.initializeSubMenu();
            configEditMenuGrp.popup();
        }
    }
    else if (btn == d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG])
    {
        if (d_ptr->curConfig)
        {
            delete d_ptr->curConfig;
        }

        if (d_ptr->lastSelectItem)
        {
            int index = d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->row(d_ptr->lastSelectItem);
            d_ptr->curEditIndex = index;
            d_ptr->curConfig = new Config(QString("%1/%2")
                                          .arg(CONFIG_DIR)
                                          .arg(d_ptr->configs.at(index).fileName));
            configEditMenuGrp.setCurrentEditConfigName(d_ptr->configs.at(index).name);
            configEditMenuGrp.setCurrentEditConfig(d_ptr->curConfig);
            configEditMenuGrp.initializeSubMenu();
            configEditMenuGrp.popup();
        }
    }
    else if (btn == d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG])
    {
        if (d_ptr->lastSelectItem)
        {
            int index = d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->row(d_ptr->lastSelectItem);
            QString filename  = QString("%1/%2").arg(CONFIG_DIR).arg(d_ptr->configs.at(index).fileName);

            //  delete the config file
            QFile::remove(filename);
            d_ptr->configs.removeAt(index);
            configManager.saveUserConfigInfo(d_ptr->configs);

            d_ptr->loadConfigs();
            d_ptr->updateConfigList();

            //  update widget status
            d_ptr->lastSelectItem = NULL;
            d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_ADD_CONFIG]->setEnabled(true);
            d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_EDIT_CONFIG]->setEnabled(false);
            d_ptr->btns[UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG]->setEnabled(false);
        }
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
    QString configName = configEditMenuGrp.getCurrentEditConfigName();
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
    configEditMenuGrp.setCurrentEditConfig(NULL);
    configEditMenuGrp.setCurrentEditConfigName(QString());

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

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label = new QLabel(trs("ConfigManagerment"));
    layout->addWidget(label, d_ptr->ltws.count(), 0);

    IListWidget *listWidget = new IListWidget;
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setFrameStyle(QFrame::Plain);
    listWidget->setSpacing(2);
    listWidget->setUniformItemSizes(true);
    listWidget->installEventFilter(this);
    listWidget->setIconSize(QSize(16, 16));
    QString configListStyleSheet =
        QString("QListWidget { margin-left: 0px; border:1px solid #808080; border-radius: 0px; background-color: transparent; outline: none; }\n "
                "QListWidget::item {padding: 0px; border-radius: 0px; border: none; background-color: %1;}\n"
                "QListWidget::item:focus {background-color: %2;}").arg("gray").arg("blue");
    listWidget->setStyleSheet(configListStyleSheet);
    d_ptr->ltws.insert(UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST, listWidget);
    layout->addWidget(listWidget, d_ptr->ltws.count(), 0);
    connect(d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST],
            SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST],
            SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    //  size for 5 items
    d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->setFixedHeight(174);

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
    button = new Button(trs("Del"));
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setEnabled(false);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->btns.insert(UserConfigEditMenuContentPrivate::ITEM_BTN_DEL_CONFIG, button);

    layout->addLayout(hl, (d_ptr->ltws.count() + 1), 0);

    layout->setRowStretch((d_ptr->ltws.count() + 2), 1);
}

void UserConfigEditMenuContent::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange)
    {
        d_ptr->ltws[UserConfigEditMenuContentPrivate::ITEM_LTW_CONFIG_LIST]
        ->setFont(fontManager.textFont(font().pixelSize()));
    }
}

