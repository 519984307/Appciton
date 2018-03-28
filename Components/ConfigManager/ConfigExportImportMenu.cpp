#include "ConfigExportImportMenu.h"
#include "ConfigMaintainMenuGrp.h"
#include "ConfigManager.h"
#include "IListWidget.h"
#include "LabelButton.h"

#define CONFIG_LIST_ITEM_H 30
#define CONFIG_LIST_ITME_W 200
#define CONFIG_MAX_NUM 3

class ConfigExportImportMenuPrivate
{
public:
    ConfigExportImportMenuPrivate()
        :configList(NULL), exportBtn(NULL), importBtn(NULL),
        lastSelectItem(NULL)
    {}

    void loadConfigs();
    void updateConfigList();

    IListWidget *configList;
    LButtonEx *exportBtn;
    LButtonEx *importBtn;

    QListWidgetItem *lastSelectItem;
    QList<ConfigManager::UserDefineConfigInfo> configs;
};

void ConfigExportImportMenuPrivate::loadConfigs()
{
    configs = configManager.getUserDefineConfigInfos();
}

void ConfigExportImportMenuPrivate::updateConfigList()
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

}

ConfigExportImportMenu::ConfigExportImportMenu()
    :SubMenu(trs("Export/Import")), d_ptr(new ConfigExportImportMenuPrivate)
{
    setDesc(trs("Export/Import"));
    startLayout();
}

ConfigExportImportMenu::~ConfigExportImportMenu()
{

}

bool ConfigExportImportMenu::eventFilter(QObject *obj, QEvent *ev)
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
                d_ptr->importBtn->setEnabled(true);
                d_ptr->exportBtn->setEnabled(false);
                d_ptr->lastSelectItem = NULL;
            }
        }
    }
    return false;
}

void ConfigExportImportMenu::layoutExec()
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
    label->setText(trs("Export/ExportConfig"));
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
    "QListWidget::item:focus {background-color: %2;}").arg("white").arg(colorManager.getHighlight().name());

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
    d_ptr->exportBtn = new LButtonEx();
    d_ptr->exportBtn->setText(trs("Export"));
    d_ptr->exportBtn->setFont(fontManager.textFont(fontSize));
    hlayout->addWidget(d_ptr->exportBtn);
    connect(d_ptr->exportBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    d_ptr->importBtn = new LButtonEx();
    d_ptr->importBtn->setText(trs("Import"));
    d_ptr->importBtn->setFont(fontManager.textFont(fontSize));
    hlayout->addWidget(d_ptr->importBtn);
    connect(d_ptr->importBtn, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    mainLayout->addLayout(hlayout);
}

void ConfigExportImportMenu::readyShow()
{
    d_ptr->loadConfigs();
    d_ptr->updateConfigList();
    if(d_ptr->configList->count() >= CONFIG_MAX_NUM)
    {
        d_ptr->importBtn->setEnabled(false);
    }
}

void ConfigExportImportMenu::onExitList(bool backTab)
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

void ConfigExportImportMenu::onConfigClick()
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
        d_ptr->exportBtn->setEnabled(true);
    }
    else
    {
        d_ptr->exportBtn->setEnabled(false);
    }
}

void ConfigExportImportMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());
    if(btn == d_ptr->exportBtn)
    {

    }
    else if(btn == d_ptr->importBtn)
    {

    }
    else
    {
        qdebug("Unknown signal sender!");
    }
}

