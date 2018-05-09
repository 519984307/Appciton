#include <QLabel>
#include "UserConfigEditMenu.h"
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
#include "SystemMenu.h"
#include "SystemManager.h"
#include "MenuWidget.h"

#define CONFIG_DIR "/usr/local/nPM/etc"
#define USER_DEFINE_CONFIG_NAME "UserConfig"

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

class UserConfigEditMenuPrivate {
public:
    UserConfigEditMenuPrivate()
        :configList(NULL), addBtn(NULL), editBtn(NULL),
          delBtn(NULL), lastSelectItem(NULL), curConfig(NULL), curEditIndex(-1)
    {

    }

    void loadConfigs();
    void updateConfigList();

    //check whether a config exist base on the config name
    bool isConfigExist(const QString &name) const;

    //get a default config name
    QString generateDefaultConfigName() const;


    IListWidget *configList;
    LButtonEx *addBtn;
    LButtonEx *editBtn;
    LButtonEx *delBtn;
    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
    Config *curConfig;
    int curEditIndex;
};

void UserConfigEditMenuPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

/**
 * @brief UserConfigEditMenuPrivate::updateConfigList
 *        update the config list widget
 */
void UserConfigEditMenuPrivate::updateConfigList()
{
    //remove old item
    while(configList->count())
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
    if(count)
    {
        configList->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        configList->setFocusPolicy(Qt::NoFocus);
    }

    if(count >= CONFIG_MAX_NUM)
    {
        addBtn->setEnabled(false);
    }
    else
    {
        addBtn->setEnabled(true);
    }

}

bool UserConfigEditMenuPrivate::isConfigExist(const QString &name) const
{
    for(int i = 0 ; i < configs.size(); i++)
    {
        if(configs.at(i).name == name)
        {
            return true;
        }
    }
    return false;
}

QString UserConfigEditMenuPrivate::generateDefaultConfigName() const
{
    QString prefix("UserConfig");
    for(int i = 1; i <= CONFIG_MAX_NUM; i++)
    {
        QString name = prefix+QString::number(i);
        if(!isConfigExist(name))
        {
            return name;
        }
    }
    qdebug("Warning, can't generate default config name.");
    return prefix;
}

UserConfigEditMenu::UserConfigEditMenu()
    :SubMenu(trs("ConfigManagement")), d_ptr(new UserConfigEditMenuPrivate())
{
    setDesc(trs("ConfigManagement"));
    startLayout();

    //保存配置管理的数据
    connect(configEditMenuGrp.getCloseBtn(), SIGNAL(realReleased()), this, SLOT(onEditFinished()));

    //connect(&configEditMenuGrp, SIGNAL(menuexitsignal()), this, SLOT(onEditFinished()));
}

UserConfigEditMenu::~UserConfigEditMenu()
{

}

/***************************************************************************************************
 * eventFilter : handle the config list focus in event, the operation of the list will be more natrual.
 *              If we don't handle the focus in event, the focus item will be the last item of the list
 *              when the focus reason is tab, or the first item when the focus reason is back tab.
 *
 *              Handle the config list hide event, clear the select item when the widget is hidden.
 **************************************************************************************************/
bool UserConfigEditMenu::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == d_ptr->configList)
    {
        if (ev->type() == QEvent::FocusIn)
        {
            QFocusEvent *e = static_cast<QFocusEvent *>(ev);
            if(e->reason() == Qt::TabFocusReason)
            {
                d_ptr->configList->setCurrentRow(0);
            }
            else if (e->reason() == Qt::BacktabFocusReason)
            {
                d_ptr->configList->setCurrentRow(d_ptr->configList->count() - 1);
            }
        }

        if (ev->type() == QEvent::Hide)
        {
            if(d_ptr->lastSelectItem)
            {
                d_ptr->lastSelectItem->setIcon(QIcon());
                d_ptr->delBtn->setEnabled(false);
                d_ptr->editBtn->setEnabled(false);
                d_ptr->lastSelectItem = NULL;
            }
        }
    }
    return false;
}

//对应界面的ConfigManagerment选项
void UserConfigEditMenu::layoutExec()
{
    int submenuW = configMaintainMenuGrp.getSubmenuWidth();
    int submenuH = configMaintainMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int fontSize = fontManager.getFontSize(1);

    //configure label
    QLabel *label = new QLabel();
    label->setFont(fontManager.textFont(fontSize));
    QMargins margin = label->contentsMargins();
    margin.setTop(10);
    margin.setLeft(15);
    margin.setBottom(10);
    label->setContentsMargins(margin);
    label->setText(trs("ConfigManagement55"));
    mainLayout->addWidget(label);

    //config list
    d_ptr->configList = new IListWidget();
    d_ptr->configList->setFont(fontManager.textFont(fontSize));
    d_ptr->configList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d_ptr->configList->setSelectionMode(QAbstractItemView::SingleSelection);
    d_ptr->configList->setFrameStyle(QFrame::Plain);
    d_ptr->configList->setSpacing(2);
    d_ptr->configList->setUniformItemSizes(true);
    d_ptr->configList->setIconSize(QSize(16,16));

    QString configListStyleSheet = QString("QListWidget { margin-left: 15px; border:1px solid #808080; border-radius: 2px; background-color: transparent; outline: none; }\n "
    "QListWidget::item {padding: 5px; border-radius: 2px; border: none; background-color: %1;}\n"
    "QListWidget::item:focus {background-color: %2;}").arg("blue").arg(colorManager.getHighlight().name());

    d_ptr->configList->setStyleSheet(configListStyleSheet);
    connect(d_ptr->configList, SIGNAL(exitList(bool)), this, SLOT(onExitList(bool)));
    connect(d_ptr->configList, SIGNAL(realRelease()), this, SLOT(onConfigClick()));
    d_ptr->configList->installEventFilter(this);
    d_ptr->configList->setFixedHeight(174); //size for 5 items
    mainLayout->addWidget(d_ptr->configList);

    //buttons
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(QMargins(15, 10, 0, 40));
    hlayout->setSpacing(10);
    d_ptr->addBtn = new LButtonEx();
    d_ptr->addBtn->setText(trs("Add"));
    d_ptr->addBtn->setFont(fontManager.textFont(fontSize));
    hlayout->addWidget(d_ptr->addBtn);
    connect(d_ptr->addBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d_ptr->editBtn = new LButtonEx();
    d_ptr->editBtn->setText(trs("Edit"));
    d_ptr->editBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->editBtn->setEnabled(false);
    hlayout->addWidget(d_ptr->editBtn);
    connect(d_ptr->editBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d_ptr->delBtn =  new LButtonEx();
    d_ptr->delBtn->setText((trs("Delete")));
    d_ptr->delBtn->setFont(fontManager.textFont(fontSize));
    d_ptr->delBtn->setEnabled(false);
    hlayout->addWidget(d_ptr->delBtn);
    connect(d_ptr->delBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));

    mainLayout->addLayout(hlayout);


}

void UserConfigEditMenu::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
}

// handle config list exit event
void UserConfigEditMenu::onExitList(bool backTab)
{
    if(backTab)
    {
        focusPreviousChild();
    }
    else
    {
        focusNextChild();
    }
}

void UserConfigEditMenu::onConfigClick()
{
    QListWidgetItem *item = d_ptr->configList->currentItem();
    if(d_ptr->lastSelectItem)
    {
        d_ptr->lastSelectItem->setIcon(QIcon());
    }

    if(item != d_ptr->lastSelectItem)
    {
        item->setIcon(QIcon("/usr/local/nPM/icons/select.png"));
        d_ptr->lastSelectItem = item;
    }
    else
    {
        d_ptr->lastSelectItem = NULL;
    }

    if(d_ptr->lastSelectItem)
    {
        d_ptr->delBtn->setEnabled(true);
        d_ptr->editBtn->setEnabled(true);
    }
    else
    {
        d_ptr->delBtn->setEnabled(false);
        d_ptr->editBtn->setEnabled(false);
    }

}

void UserConfigEditMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());

    if(btn == d_ptr->addBtn)
    {
        if(d_ptr->configList->count() >= CONFIG_MAX_NUM)
        {

            //can't add, too many
            //TODO show some message
        }
        else
        {
            if(d_ptr->curConfig)
            {
                delete d_ptr->curConfig;
            }

            //将当前配置改为工厂配置
            d_ptr->curConfig = new Config(QString("%1/%2")
                                          .arg(CONFIG_DIR)
                                          .arg(configManager.factoryConfigFilename(patientManager.getType())));

            configEditMenuGrp.setCurrentEditConfigName(d_ptr->generateDefaultConfigName());
            configEditMenuGrp.setCurrentEditConfig(d_ptr->curConfig);
            configEditMenuGrp.initializeSubMenu();
            configEditMenuGrp.popup();
        }

    }
    else if (btn == d_ptr->editBtn)
    {
        if(d_ptr->curConfig)
        {
            delete d_ptr->curConfig;
        }

        if(d_ptr->lastSelectItem)
        {
            int index = d_ptr->configList->row(d_ptr->lastSelectItem);
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
    else if (btn == d_ptr->delBtn)
    {
        if(d_ptr->lastSelectItem)
        {
            int index = d_ptr->configList->row(d_ptr->lastSelectItem);
            QString filename  = QString("%1/%2").arg(CONFIG_DIR).arg(d_ptr->configs.at(index).fileName);

            //delete the config file
            QFile::remove(filename);
            d_ptr->configs.removeAt(index);
            configManager.saveUserConfigInfo(d_ptr->configs);

            d_ptr->loadConfigs();
            d_ptr->updateConfigList();

            //update widget status
            d_ptr->lastSelectItem = NULL;
            d_ptr->addBtn->setEnabled(true);
            d_ptr->editBtn->setEnabled(false);
            d_ptr->delBtn->setEnabled(false);
        }
    }
    else
    {
        qdebug("Unknown singal sender!");
    }
}

void UserConfigEditMenu::onEditFinished()
{
    QString configName = configEditMenuGrp.getCurrentEditConfigName();
    if(d_ptr->curEditIndex >= 0)
    {
        //edit exist config
        d_ptr->curConfig->saveToDisk();
        if(d_ptr->configs.at(d_ptr->curEditIndex).name != configName)
        {
            d_ptr->configs[d_ptr->curEditIndex].name = configName;
            configManager.saveUserConfigInfo(d_ptr->configs);
        }
    }
    else
    {
        //add new config
        configManager.saveUserDefineConfig(configName, d_ptr->curConfig);
    }
    configEditMenuGrp.setCurrentEditConfig(NULL);
    configEditMenuGrp.setCurrentEditConfigName(QString());

    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
    d_ptr->curEditIndex = -1;
}

