/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include <QLabel>
#include "LoadConfigMenu.h"
#include "ConfigMaintainMenuGrp.h"
#include "IListWidget.h"
#include "IButton.h"
#include "LabelButton.h"
#include "ConfigManager.h"
#include "MenuManager.h"
#include "ConfigEditMenuGrp.h"
#include "PatientManager.h"
#include "PublicMenuManager.h"
#include "Debug.h"
#include "MenuManager.h"
#include "SystemManager.h"
#include "MenuWidget.h"
#include "IConfig.h"
#include "IMessageBox.h"
#include "ConfigManager.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

LoadConfigMenu *LoadConfigMenu::selfObj = NULL;

class LoadConfigMenuPrivate
{
public:
    LoadConfigMenuPrivate()
        : configList(NULL), loadBtn(NULL), viewBtn(NULL),
          lastSelectItem(NULL), curConfig(NULL), curEditIndex(0)
    {
    }

    void loadConfigs();
    void updateConfigList();

    // check whether a config exist base on the config name
    bool isConfigExist(const QString &name) const;

    IListWidget *configList;
    LButtonEx *loadBtn;
    LButtonEx *viewBtn;
    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;
};

void LoadConfigMenuPrivate::loadConfigs()
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
 * @brief LoadConfigMenuPrivate::updateConfigList
 *        update the config list widget
 */
void LoadConfigMenuPrivate::updateConfigList()
{
    // remove old item
    while (configList->count())
    {
        QListWidgetItem *item = configList->takeItem(0);
        delete item;
    }

    for (int i = 0; i < configs.count(); i++)
    {
        QListWidgetItem *item = new QListWidgetItem(configs.at(i).name, configList);
        item->setSizeHint(QSize(CONFIG_LIST_ITME_W, CONFIG_LIST_ITEM_H));
    }

    int count = configs.count();
    if (count)
    {
        configList->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        configList->setFocusPolicy(Qt::NoFocus);
    }

    if (lastSelectItem)
    {
        lastSelectItem->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
    }
    loadBtn->setEnabled(!!lastSelectItem);
    viewBtn->setEnabled(!!lastSelectItem);
}


LoadConfigMenu::LoadConfigMenu()
    : SubMenu(trs("LoadConfig")), d_ptr(new LoadConfigMenuPrivate())
{
    setDesc(trs("LoadConfigDesc"));/*更改标题栏标题*/
    startLayout();/*布局*/

    connect(this, SIGNAL(configUpdated()), &configManager, SIGNAL(configUpdated()));
}

LoadConfigMenu::~LoadConfigMenu()
{
}

/***************************************************************************************************
 * eventFilter : handle the config list focus in event, the operation of the list will be more natrual.
 *              If we don't handle the focus in event, the focus item will be the last item of the list
 *              when the focus reason is tab, or the first item when the focus reason is back tab.
 *
 *              Handle the config list hide event, clear the select item when the widget is hidden.
 **************************************************************************************************/
bool LoadConfigMenu::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj == d_ptr->configList)
    {
        if (ev->type() == QEvent::Hide)
        {
            d_ptr->lastSelectItem = NULL;
        }
    }
    return false;
}

// 对应界面的ConfigManagerment选项
void LoadConfigMenu::layoutExec()
{
    int submenuW = configMaintainMenuGrp.getSubmenuWidth();
    int submenuH = configMaintainMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int fontSize = fontManager.getFontSize(1);

    // configure label
    QLabel *label = new QLabel();
    label->setFont(fontManager.textFont(fontSize));
    QMargins margin = label->contentsMargins();
    margin.setTop(10);
    margin.setLeft(15);
    margin.setBottom(10);
    label->setContentsMargins(margin);
    label->setText(trs("LoadConfig"));
    mainLayout->addWidget(label);

    // config list
    d_ptr->configList = new IListWidget();
    d_ptr->configList->setFont(fontManager.textFont(fontSize));
    d_ptr->configList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->configList->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->configList->setFrameStyle(QFrame::Plain);
    d_ptr->configList->setSpacing(2);
    d_ptr->configList->setUniformItemSizes(true);
    d_ptr->configList->setIconSize(QSize(16, 16));

    QString configListStyleSheet =
        QString("QListWidget { margin-left: 15px; border:1px solid #808080; border-radius: 2px; background-color: transparent; outline: none; }\n "
                "QListWidget::item {padding: 5px; border-radius: 2px; border: none; background-color: %1;}\n"
                "QListWidget::item:focus {background-color: %2;}").arg("blue").arg(colorManager.getHighlight().name());

    d_ptr->configList->setStyleSheet(configListStyleSheet);
    connect(d_ptr->configList, SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(d_ptr->configList, SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    d_ptr->configList->installEventFilter(this);
    d_ptr->configList->setFixedHeight(289); // size for 8 items
    mainLayout->addWidget(d_ptr->configList);

    // buttons
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(QMargins(15, 10, 0, 40));
    hlayout->setSpacing(10);
    d_ptr->loadBtn = new LButtonEx();
    d_ptr->loadBtn->setText(trs("Load"));
    d_ptr->loadBtn->setFont(fontManager.textFont(fontSize));
    hlayout->addWidget(d_ptr->loadBtn);
    connect(d_ptr->loadBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d_ptr->viewBtn = new LButtonEx();
    d_ptr->viewBtn->setText(trs("View"));
    d_ptr->viewBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->viewBtn->setEnabled(false);
    d_ptr->viewBtn->setEnabled(true);
    hlayout->addWidget(d_ptr->viewBtn);
    connect(d_ptr->viewBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));

    mainLayout->addLayout(hlayout);
}

void LoadConfigMenu::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

// handle config list exit event
void LoadConfigMenu::onExitList(bool backTab)
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

void LoadConfigMenu::onConfigClick()
{
    QListWidgetItem *item = d_ptr->configList->currentItem();
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
    d_ptr->loadBtn->setEnabled(!!d_ptr->lastSelectItem);
    d_ptr->viewBtn->setEnabled(!!d_ptr->lastSelectItem);
}

void LoadConfigMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());

    if (btn == NULL || d_ptr->lastSelectItem == NULL)
    {
        return;
    }

    if (btn == d_ptr->loadBtn) //加载配置
    {
        //加入最新配置文件
        int index = d_ptr->configList->row(d_ptr->lastSelectItem);
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

        d_ptr->loadBtn->setEnabled(!!d_ptr->lastSelectItem);
        d_ptr->viewBtn->setEnabled(!!d_ptr->lastSelectItem);

        QString title(trs("LoadConfig"));
        QString text(trs("SuccessToLoad"));
        IMessageBox iMessageBox(title, text, false);
        int subWidth = menuManager.getSubmenuWidth();
        int subHeight = menuManager.getSubmenuHeight();
        iMessageBox.setFixedSize(subWidth / 2, subHeight / 3);
        iMessageBox.exec();
    }
    else if (btn == d_ptr->viewBtn)//查看配置
    {
        if (d_ptr->lastSelectItem)
        {
            int index = d_ptr->configList->row(d_ptr->lastSelectItem);
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

            d_ptr->loadBtn->setEnabled(!!d_ptr->lastSelectItem);
            d_ptr->viewBtn->setEnabled(!!d_ptr->lastSelectItem);
        }
    }
    else
    {
        qdebug("Unknown singal sender!");
    }
}
