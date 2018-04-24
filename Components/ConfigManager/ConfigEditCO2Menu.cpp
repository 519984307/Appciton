#include "ConfigEditCO2Menu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "CO2Symbol.h"

class ConfigEditCO2MenuPrivate
{
public:
    enum ComboListId {
        AsphyxiaDelay,
        EquilibriumGas,
        OxygenConcentration,
        ConcentrationOfAnaesthetizedGas,
        MaximumRetention,
        OperationMode,
        WaveformMode,
        WaveformRuler,
        WaveVelocity,
        RRSource,
        Zero,
        IntubationMode,
        ComboListMax,
    };

    ConfigEditCO2MenuPrivate()
    {
        memset(combos, 0, sizeof(combos));

        //常量存放在静态变量区
        comboLabels[AsphyxiaDelay] = "AsphyxiaDelay";
        comboLabels[EquilibriumGas] = "EquilibriumGas";
        comboLabels[OxygenConcentration] = "OxygenConcentration";
        comboLabels[ConcentrationOfAnaesthetizedGas] = "ConcentrationOfAnaesthetizedGas";
        comboLabels[MaximumRetention] = "MaximumRetention";
        comboLabels[OperationMode] = "OperationMode";
        comboLabels[WaveformMode] = "WaveformMode";
        comboLabels[WaveformRuler] = "WaveformRuler";
        comboLabels[WaveVelocity] = "WaveVelocity";
        comboLabels[RRSource] = "RRSource";
        comboLabels[Zero] = "Zero";
        comboLabels[IntubationMode] = "IntubationMode";
    }

    void loadOptions();

    IComboList *combos[ComboListMax];
    const char *comboLabels[ComboListMax];
};

void ConfigEditCO2MenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;

    if (combos[AsphyxiaDelay]->count() == 0)
    {
        for(int i = 0; i < CO2_APNEA_TIME_NR; i++)
        {
            combos[AsphyxiaDelay]->addItem(CO2Symbol::convert((CO2ApneaTime)i));
        }
        config->getNumValue("CO2|CO2ApneaTime", index);
        combos[AsphyxiaDelay]->setCurrentIndex(index);
    }

    if (combos[EquilibriumGas]->count() == 0)
    {
        for(int i = 0; i < CO2_BALANCE_GAS_NR; i++)
        {
            combos[EquilibriumGas]->addItem(CO2Symbol::convert((BalanceGas)i));
        }
        config->getNumValue("CO2|BalanceGas", index);
        combos[EquilibriumGas]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[OxygenConcentration]->count() == 0)
    {
        for(int i = 0; i < OXYGEN_DENSITY_NR; i++)
        {
            combos[OxygenConcentration]->addItem(CO2Symbol::convert((Oxygendensity)i));
        }
        config->getNumValue("CO2|Oxygendensity", index);
        combos[OxygenConcentration]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[ConcentrationOfAnaesthetizedGas]->count() == 0)
    {
        for(int i = 0; i < GAS_STRENGTH_NR; i++)
        {
            combos[ConcentrationOfAnaesthetizedGas]->addItem(CO2Symbol::convert((GasStrength)i));
        }
        config->getNumValue("CO2|GasStrength", index);
        combos[ConcentrationOfAnaesthetizedGas]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[MaximumRetention]->count() == 0)
    {
        for(int i = 0; i < MAX_HOLD_NR; i++)
        {
            combos[MaximumRetention]->addItem(CO2Symbol::convert((MaxHold)i));
        }
        config->getNumValue("CO2|MaxHold", index);
        combos[MaximumRetention]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[OperationMode]->count() == 0)
    {
        for(int i = 0; i < OPERATION_MODE_NR; i++)
        {
            combos[OperationMode]->addItem(CO2Symbol::convert((CO2OperationMode)i));
        }
        config->getNumValue("CO2|CO2OperationMode", index);
        combos[OperationMode]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[WaveformMode]->count() == 0)
    {
        for(int i = 0; i < CO2_SWEEP_MODE_NR; i++)
        {
            combos[WaveformMode]->addItem(CO2Symbol::convert((CO2SweepMode)i));
        }
        config->getNumValue("CO2|CO2SweepMode", index);
        combos[WaveformMode]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[WaveformRuler]->count() == 0)
    {
        for(int i = 0; i < 14; i++)
        {
            combos[WaveformRuler]->addItem(QString::number((15+5*i)));
        }
        config->getNumValue("CO2|WaveformRuler", index);
        combos[WaveformRuler]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[WaveVelocity]->count() == 0)
    {
        for(int i = 0; i < CO2_SWEEP_SPEED_NR; i++)
        {
            combos[WaveVelocity]->addItem(CO2Symbol::convert((CO2SweepSpeed)i));
        }
        config->getNumValue("CO2|CO2SweepSpeed", index);
        combos[WaveVelocity]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[RRSource]->count() == 0)
    {

        combos[RRSource]->addItem("AUTO");
        combos[RRSource]->addItem("DATA");
        config->getNumValue("CO2|RRSource", index);
        combos[RRSource]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[Zero]->count() == 0)
    {
        combos[Zero]->addItem("OFF");
        combos[Zero]->addItem("ON");
        config->getNumValue("CO2|Zero", index);
        combos[Zero]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[IntubationMode]->count() == 0)
    {
        combos[IntubationMode]->addItem("OFF");
        combos[IntubationMode]->addItem("ON");
        config->getNumValue("CO2|IntubationMode", index);
        combos[IntubationMode]->setCurrentIndex(index);
    }
}

ConfigEditCO2Menu::ConfigEditCO2Menu()
    :SubMenu(trs("CO2Menu")),
    d_ptr(new ConfigEditCO2MenuPrivate())
{
    setDesc(trs("CO2MenuDesc"));
    startLayout();
}

ConfigEditCO2Menu::~ConfigEditCO2Menu()
{

}

void ConfigEditCO2Menu::layoutExec()
{
    int submenuW = configEditMenuGrp.getSubmenuWidth();
    int submenuH = configEditMenuGrp.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    for(int i = 0; i< ConfigEditCO2MenuPrivate::ComboListMax; i++)
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

void ConfigEditCO2Menu::readyShow()
{
    d_ptr->loadOptions();
}


void ConfigEditCO2Menu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }
    //根据索引设置对应列表的参数值
    ConfigEditCO2MenuPrivate::ComboListId comboId = (ConfigEditCO2MenuPrivate::ComboListId) combo->property("comboId").toInt();

    Config *config = configEditMenuGrp.getCurrentEditConfig();

    switch (comboId)
    {

    case ConfigEditCO2MenuPrivate::AsphyxiaDelay:
        config->setNumValue("CO2|CO2ApneaTime", index);
        break;
    case ConfigEditCO2MenuPrivate::EquilibriumGas:
        config->setNumValue("CO2|BalanceGas", index);
        break;
    case ConfigEditCO2MenuPrivate::OxygenConcentration:
        config->setNumValue("CO2|Oxygendensity", index);
        break;
    case ConfigEditCO2MenuPrivate::ConcentrationOfAnaesthetizedGas:
        config->setNumValue("CO2|GasStrength", index);
        break;
    case ConfigEditCO2MenuPrivate::MaximumRetention:
        config->setNumValue("CO2|MaxHold", index);
        break;
    case ConfigEditCO2MenuPrivate::OperationMode:
        config->setNumValue("CO2|CO2OperationMode", index);
        break;
    case ConfigEditCO2MenuPrivate::WaveformMode:
        config->setNumValue("CO2|CO2SweepMode", index);
        break;
    case ConfigEditCO2MenuPrivate::WaveformRuler:
        config->setNumValue("CO2|WaveformRuler", index);
        break;
    case ConfigEditCO2MenuPrivate::WaveVelocity:
        config->setNumValue("CO2|CO2SweepSpeed", index);
        break;
    case ConfigEditCO2MenuPrivate::RRSource:
        config->setNumValue("CO2|RRSource", index);
        break;
    case ConfigEditCO2MenuPrivate::Zero:
        config->setNumValue("CO2|Zero", index);
        break;
    case ConfigEditCO2MenuPrivate::IntubationMode:
        config->setNumValue("CO2|IntubationMode", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}
