/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018.07.13
 **/


#include "LoadConfigMenuContent.h"
#include "Button.h"
#include <QLabel>
#include <QMap>
#include "LanguageManager.h"
#include "ConfigManager.h"
#include "IListWidget.h"
#include <QGridLayout>
#include "ColorManager.h"
#include <QList>
#include "IConfig.h"
#include <QFile>
#include "PatientManager.h"
#include "IMessageBox.h"
#include "MenuManager.h"
#include "ConfigEditMenuGrp.h"
#include <QHBoxLayout>
#include "FontManager.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3
class LoadConfigMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_LOAD_CONFIG = 0,
        ITEM_BTN_VIEW_CONFIG,

        ITEM_LTW_CONFIG_LIST = 0,
    };

    LoadConfigMenuContentPrivate();

    void loadConfigs();
    void updateConfigList();

    QMap <MenuItem, Button *> btns;
    QMap <MenuItem, IListWidget *> ltws;
    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;
};

LoadConfigMenuContentPrivate::LoadConfigMenuContentPrivate():
    lastSelectItem(NULL),
    curConfig(NULL),
    curEditIndex(0)
{
    btns.clear();
    ltws.clear();
    configs.clear();
}

void LoadConfigMenuContentPrivate::loadConfigs()
{
    configs.clear();
    ConfigManager::UserDefineConfigInfo userConfig[] =
    {
        {"AdultConfig.Original", "AdultConfig.Original.xml"},
        {"PedConfig.Original", "PedConfig.Original.xml"},
        {"NeoConfig.Original", "NeoConfig.Original.xml"}
    };
    for (int i = 0; i < 3; i++)
    {
        configs.append(userConfig[i]);
    }
    configs = configs + configManager.getUserDefineConfigInfos();
}
/**
 * @brief LoadConfigMenuContentPrivate::updateConfigList
 *        update the config list widget
 */
void LoadConfigMenuContentPrivate::updateConfigList()
{
    // remove old item
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

    if (lastSelectItem)
    {
        lastSelectItem->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
    }
    btns[ITEM_BTN_LOAD_CONFIG]->setEnabled(!!lastSelectItem);
    btns[LoadConfigMenuContentPrivate::ITEM_BTN_VIEW_CONFIG]->setEnabled(!!lastSelectItem);
}
LoadConfigMenuContent::LoadConfigMenuContent():
    MenuContent(trs("LoadConfigMenuContent"),
                trs("LoadConfigMenuContentDesc")),
    d_ptr(new LoadConfigMenuContentPrivate)
{
    connect(this, SIGNAL(configUpdated()), &configManager, SIGNAL(configUpdated()));
}

LoadConfigMenuContent::~LoadConfigMenuContent()
{
    delete d_ptr;
}

void LoadConfigMenuContent::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

void LoadConfigMenuContent::layoutExec()
{
    // already install layout
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    // load config
    QLabel *label = new QLabel(trs("LoadConfig"));
    layout->addWidget(label, d_ptr->ltws.count(), 0);

    // config list
    IListWidget *listWidget = new IListWidget();
    listWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    listWidget->setFrameStyle(QFrame::Plain);
    listWidget->setFocusPolicy(Qt::StrongFocus);
    listWidget->setSpacing(2);
    listWidget->setIconSize(QSize(16, 16));
    listWidget->setUniformItemSizes(true);
    QString listWidgetStyleSheet = QString("QListWidget { margin-left: 0px; border:1px solid #808080; "
                                           "border-radius: 0px; background-color: transparent; "
                                           "outline: none; }\n "
                                           "QListWidget::item {padding:0px; border-radius:0px; border: none;"
                                           "background-color: %1;}\n"
                                           "QListWidget::item:focus {background-color: %2;}").
                                   arg("gray").arg("blue");
    listWidget->setStyleSheet(listWidgetStyleSheet);
    connect(listWidget, SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    listWidget->setFixedHeight(289);// size for 8 items
    d_ptr->ltws.insert(LoadConfigMenuContentPrivate::ITEM_LTW_CONFIG_LIST, listWidget);
    layout->addWidget(listWidget, d_ptr->ltws.count(), 0);

    int row = d_ptr->ltws.count();
    Button *button;

    QHBoxLayout *hl = new QHBoxLayout;
    // load buton
    button = new Button();
    button->setText(trs("Load"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    d_ptr->btns.insert(LoadConfigMenuContentPrivate::ITEM_BTN_LOAD_CONFIG, button);
    hl->addWidget(button);

    // view buton
    button = new Button();
    button->setText(trs("View"));
    button->setButtonStyle(Button::ButtonTextOnly);
    connect(button, SIGNAL(released()), this, SLOT(onBtnClick()));
    hl->addWidget(button);
    d_ptr->btns.insert(LoadConfigMenuContentPrivate::ITEM_BTN_VIEW_CONFIG, button);

    layout->addLayout(hl, row + 1, 0);
    layout->setRowStretch((row + d_ptr->btns.count()), 1);
}

void LoadConfigMenuContent::hideEvent(QHideEvent *ev)
{
    if (ev->type() == QEvent::Hide)
    {
        if (d_ptr->lastSelectItem)
        {
            d_ptr->lastSelectItem = NULL;
        }
    }
    MenuContent::hideEvent(ev);
}

void LoadConfigMenuContent::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange)
    {
        d_ptr->ltws[LoadConfigMenuContentPrivate::ITEM_LTW_CONFIG_LIST]
        ->setFont(fontManager.textFont(font().pixelSize()));
    }
}

void LoadConfigMenuContent::onConfigClick()
{
    QListWidgetItem *item = d_ptr->ltws[LoadConfigMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->currentItem();
    if (!item)
    {
        return;
    }
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
    d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_LOAD_CONFIG]->setEnabled(!!d_ptr->lastSelectItem);
    d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_VIEW_CONFIG]->setEnabled(!!d_ptr->lastSelectItem);
}

void LoadConfigMenuContent::onBtnClick()
{
    Button *btn = qobject_cast<Button *>(sender());

    if (btn == NULL || d_ptr->lastSelectItem == NULL)
    {
        return;
    }

    if (btn == d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_LOAD_CONFIG]) //加载配置
    {
        //加入最新配置文件
        int index = d_ptr->ltws[LoadConfigMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->row(d_ptr->lastSelectItem);
        d_ptr->curEditIndex = index;
        QFile::remove(systemConfig.getCurConfigName());
        QFile::copy(QString("%1/%2").arg(CONFIG_DIR).arg(d_ptr->configs.at(index).fileName),
                    systemConfig.getCurConfigName());

        //更新加载的病人类型
        int patitentTypeInt = 255;
        QString fileNameStr(d_ptr->configs.at(index).fileName);
        if (fileNameStr.indexOf("Adult") >= 0)
        {
            patitentTypeInt = 0;
        }
        else if (fileNameStr.indexOf("Ped") >= 0)
        {
            patitentTypeInt = 1;
        }
        else if (fileNameStr.indexOf("Neo") >= 0)
        {
            patitentTypeInt = 2;
        }
        else
        {
            patitentTypeInt = 0;
        }
        systemConfig.setNumValue("General|PatientType", patitentTypeInt);
        systemConfig.updateCurConfigName();
        patientManager.setType((PatientType)patitentTypeInt);
        currentConfig.reload();
        //发送更新加载配置信号
        emit configUpdated();

        d_ptr->lastSelectItem->setIcon(QIcon());
        d_ptr->lastSelectItem = NULL;

        d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_LOAD_CONFIG]->setEnabled(!!d_ptr->lastSelectItem);
        d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_VIEW_CONFIG]->setEnabled(!!d_ptr->lastSelectItem);

        QString title(trs("LoadConfig"));
        QString text(trs("SuccessToLoad"));
        IMessageBox iMessageBox(title, text, false);
        int subWidth = menuManager.getSubmenuWidth();
        int subHeight = menuManager.getSubmenuHeight();
        iMessageBox.setFixedSize(subWidth / 2, subHeight / 3);
        iMessageBox.exec();
    }
    else if (btn == d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_VIEW_CONFIG])//查看配置
    {
        if (d_ptr->lastSelectItem)
        {
            int index = d_ptr->ltws[LoadConfigMenuContentPrivate::ITEM_LTW_CONFIG_LIST]->row(d_ptr->lastSelectItem);
            d_ptr->curEditIndex = index;
            d_ptr->curConfig = new Config(QString("%1/%2")
                                          .arg(CONFIG_DIR)
                                          .arg(d_ptr->configs.at(index).fileName));
            d_ptr->lastSelectItem->setIcon(QIcon());
            d_ptr->lastSelectItem = NULL;
            configEditMenuGrp.setCurrentEditConfigName(d_ptr->configs.at(index).name);
            configEditMenuGrp.setCurrentEditConfig(d_ptr->curConfig);
            configManager.setWidgetStatus(true);
            configEditMenuGrp.initializeSubMenu();
            configEditMenuGrp.popup();

            d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_LOAD_CONFIG]->setEnabled(!!d_ptr->lastSelectItem);
            d_ptr->btns[LoadConfigMenuContentPrivate::ITEM_BTN_VIEW_CONFIG]->setEnabled(!!d_ptr->lastSelectItem);
        }
    }
    else
    {
        qdebug("Unknown singal sender!");
    }
}





















