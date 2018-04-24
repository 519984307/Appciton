#include "ConfigEditSpO2Menu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "SPO2Symbol.h"

class ConfigEditSpO2MenuPrivate
{
public:
    enum ComboListId {
        Sensitivity,
        NIBPSameSide,
        WaveVelocity,
        PRSource,
        AlarmSource,
        PulseVolume,
        PIMagnifyingDisplay,
        ComboListMax,
    };

    ConfigEditSpO2MenuPrivate()
    {
        memset(combos, 0, sizeof(combos));

        //常量存放在静态变量区
        comboLabels[Sensitivity] = "Sensitivity";
        comboLabels[NIBPSameSide] = "NIBPSameSide";
        comboLabels[WaveVelocity] = "WaveVelocity";
        comboLabels[PRSource] = "PRSource";
        comboLabels[AlarmSource] = "AlarmSource";
        comboLabels[PulseVolume] = "PulseVolume";
        comboLabels[PIMagnifyingDisplay] = "PIMagnifyingDisplay";
    }

    void loadOptions();

    IComboList *combos[ComboListMax];
    const char *comboLabels[ComboListMax];
};

void ConfigEditSpO2MenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;

    if (combos[Sensitivity]->count() == 0)
    {
        combos[Sensitivity]->addItem(trs("HIGH"));
        combos[Sensitivity]->addItem(trs("NORMAL"));
        combos[Sensitivity]->addItem(trs("LOW"));

        config->getNumValue("AlarmSource|SPO2|Sensitivity", index);
        combos[Sensitivity]->setCurrentIndex(index);
    }

    if (combos[NIBPSameSide]->count() == 0)
    {
        combos[NIBPSameSide]->addItem(trs("OFF"));
        combos[NIBPSameSide]->addItem(trs("ON"));
        config->getNumValue("AlarmSource|SPO2|NIBPSameSide", index);
        combos[NIBPSameSide]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[WaveVelocity]->count() == 0)
    {
        for(int i = 0; i < SPO2_WAVE_VELOCITY_NR; i++)
        {
            combos[WaveVelocity]->addItem(SPO2Symbol::convert((SPO2WaveVelocity)i));
        }
        config->getNumValue("AlarmSource|SPO2|WaveVelocity", index);
        combos[WaveVelocity]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[PRSource]->count() == 0)
    {
        for(int i = 0; i < PRSource_NR; i++)
        {
            combos[PRSource]->addItem(SPO2Symbol::convert((SPO2PRSource)i));
        }
        config->getNumValue("AlarmSource|SPO2|PRSource", index);
        combos[PRSource]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[AlarmSource]->count() == 0)
    {
        combos[AlarmSource]->addItem(trs("HR"));
        combos[AlarmSource]->addItem(trs("PR"));
        combos[AlarmSource]->addItem(trs("AUTO"));
        config->getNumValue("AlarmSource|SPO2|AlarmSource", index);
        combos[AlarmSource]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[PulseVolume]->count() == 0)
    {
        for(int i = 0; i < 10; i++)
        {
            combos[PulseVolume]->addItem(QString::number(i));
        }
        config->getNumValue("AlarmSource|SPO2|PulseVolume", index);
        combos[PulseVolume]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[PIMagnifyingDisplay]->count() == 0)
    {
        combos[PIMagnifyingDisplay]->addItem(trs("YES"));
        combos[PIMagnifyingDisplay]->addItem(trs("NO"));
        config->getNumValue("AlarmSource|SPO2|PIMagnifyingDisplay", index);
        combos[PIMagnifyingDisplay]->setCurrentIndex(index);
    }
}

ConfigEditSpO2Menu::ConfigEditSpO2Menu()
    :SubMenu(trs("SPO2Menu")),
    d_ptr(new ConfigEditSpO2MenuPrivate())
{
    setDesc(trs("SPO2MenuDesc"));
    startLayout();
}

ConfigEditSpO2Menu::~ConfigEditSpO2Menu()
{

}

void ConfigEditSpO2Menu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    for(int i = 0; i< ConfigEditSpO2MenuPrivate::ComboListMax; i++)
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

void ConfigEditSpO2Menu::readyShow()
{
    d_ptr->loadOptions();
}


void ConfigEditSpO2Menu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }
    //根据索引设置对应列表的参数值
    ConfigEditSpO2MenuPrivate::ComboListId comboId = (ConfigEditSpO2MenuPrivate::ComboListId) combo->property("comboId").toInt();

    Config *config = configEditMenuGrp.getCurrentEditConfig();

    switch (comboId)
    {

    case ConfigEditSpO2MenuPrivate::Sensitivity:
        config->setNumValue("AlarmSource|Adult|SPO2|Sensitivity", index);
        break;
    case ConfigEditSpO2MenuPrivate::NIBPSameSide:
        config->setNumValue("AlarmSource|Adult|SPO2|NIBPSameSide", index);
        break;
    case ConfigEditSpO2MenuPrivate::WaveVelocity:
        config->setNumValue("AlarmSource|Adult|SPO2|WaveVelocity", index);
        break;
    case ConfigEditSpO2MenuPrivate::PRSource:
        config->setNumValue("AlarmSource|Adult|SPO2|PRSource", index);
        break;
    case ConfigEditSpO2MenuPrivate::AlarmSource:
        config->setNumValue("AlarmSource|Adult|SPO2|AlarmSource", index);
        break;
    case ConfigEditSpO2MenuPrivate::PulseVolume:
        config->setNumValue("AlarmSource|Adult|SPO2|PulseVolume", index);
        break;
    case ConfigEditSpO2MenuPrivate::PIMagnifyingDisplay:
        config->setNumValue("AlarmSource|Adult|SPO2|PIMagnifyingDisplay", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}
