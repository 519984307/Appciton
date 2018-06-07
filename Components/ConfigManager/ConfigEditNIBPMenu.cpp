#include "ConfigEditNIBPMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "NIBPSymbol.h"
#include "LabelButton.h"
#include "MenuGroup.h"

class ConfigEditNIBPMenuPrivate
{
public:
    enum ComboListId {
        NIBPMeasureMode,/*测量模式*/
        IntervalTime,/*自动间隔时间*/
        NBIPInitialCuff,/*初始化袖带气压*/
        ComboListMax,
    };

    ConfigEditNIBPMenuPrivate()
    {
        memset(combos, 0, sizeof(combos));

        //常量存放在静态变量区
        comboLabels[NBIPInitialCuff] = "NBIPInitialCuff";
        comboLabels[IntervalTime] = "IntervalTime";
        comboLabels[NIBPMeasureMode] = "NIBPMeasureMode";
    }

    void loadOptions();

    IComboList *combos[ComboListMax];
    const char *comboLabels[ComboListMax];
};

void ConfigEditNIBPMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;
    if (combos[NIBPMeasureMode]->count() == 0)
    {
        combos[NIBPMeasureMode]->addItem(trs("manaul"));
        combos[NIBPMeasureMode]->addItem(trs("Auto"));
        config->getNumValue("NIBP|NIBPMeasureMode", index);
        combos[NIBPMeasureMode]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[IntervalTime]->count() == 0)
    {
        for (int i = 0; i < NIBP_AUTO_INTERVAL_NR; i++)
        {
            combos[IntervalTime]->addItem(NIBPSymbol::convert((NIBPAutoInterval)i));
        }
        config->getNumValue("NIBP|IntervalTime", index);
        combos[IntervalTime]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[NBIPInitialCuff]->count() == 0)
    {
        for(int i = 0; i < NIBP_INIT_PRE_NR; i++)
        {
            int time = 120 + 20 * i;
            combos[NBIPInitialCuff]->addItem(QString::number(time) + " mmHg");
        }
        config->getNumValue("NIBP|NBIPInitialCuff", index);
        combos[NBIPInitialCuff]->setCurrentIndex(index);
    }
}

ConfigEditNIBPMenu::ConfigEditNIBPMenu()
    :SubMenu(trs("NIBPMenu")),
    d_ptr(new ConfigEditNIBPMenuPrivate())
{
    setDesc(trs("NIBPMenuDesc"));
    startLayout();
}

ConfigEditNIBPMenu::~ConfigEditNIBPMenu()
{

}


void ConfigEditNIBPMenu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    for(int i = 0; i< ConfigEditNIBPMenuPrivate::ComboListMax; i++)
    {
        //申请一个标签下拉框，用于填充子菜单布局中
        IComboList *combo = new IComboList(trs(d_ptr->comboLabels[i]));
        combo->setFont(defaultFont());
        //设置标签的形状大小
        combo->label->setFixedSize(labelWidth, ITEM_H);
        //设置下拉框的形状大小
        combo->combolist->setFixedSize(btnWidth, ITEM_H);
        //建立槽函数
        connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
        //对成员变量进行设置数值（安全设置） 后续再看。。。
        combo->setProperty("comboId", qVariantFromValue(i));
        //将此布局加入到子菜单中的链表节点上
        mainLayout->addWidget(combo);
        //存放每个下拉框的指针
        d_ptr->combos[i] = combo;
    }
    //按照比例分配空余空间
    mainLayout->addStretch(1);
}

void ConfigEditNIBPMenu::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditNIBPMenu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }

    ConfigEditNIBPMenuPrivate::ComboListId comboId = (ConfigEditNIBPMenuPrivate::ComboListId) combo->property("comboId").toInt();
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    switch (comboId)
    {

    case ConfigEditNIBPMenuPrivate::NBIPInitialCuff:
        config->setNumValue("NIBP|NBIPInitialCuff", index);
        break;
    case ConfigEditNIBPMenuPrivate::IntervalTime:
        config->setNumValue("NIBP|IntervalTime", index);
        break;
    case ConfigEditNIBPMenuPrivate::NIBPMeasureMode:
        config->setNumValue("NIBP|NIBPMeasureMode", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}
