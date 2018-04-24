#include "ConfigEditNIBPMenu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "NIBPSymbol.h"
#include "LabelButton.h"
#include "ConfigEditSeqMeaMenu.h"
#include "MenuGroup.h"

class ConfigEditNIBPMenuPrivate
{
public:
    enum ComboListId {
        InitialInflatingPressure,
        IntervalTime,
        NIBPcompleteSound,
        WholePointMeasure,
        StartUpMeasurement,
        ContinuousMeasurement,
        AuxiliaryVenipuncture,
        SequenceMeasurementSetting,
        ComboListMax,
    };

    ConfigEditNIBPMenuPrivate()
    {
        memset(combos, 0, sizeof(combos));

        //常量存放在静态变量区
        comboLabels[InitialInflatingPressure] = "InitialInflatingPressure";
        comboLabels[IntervalTime] = "IntervalTime";
        comboLabels[NIBPcompleteSound] = "NIBPcompleteSound";
        comboLabels[WholePointMeasure] = "WholePointMeasure";
        comboLabels[StartUpMeasurement] = "StartUpMeasurement";
        comboLabels[ContinuousMeasurement] = "ContinuousMeasurement";
        comboLabels[AuxiliaryVenipuncture] = "AuxiliaryVenipuncture";
        comboLabels[SequenceMeasurementSetting] = "SequenceMeasurementSetting";
    }

    void loadOptions();

    IComboList *combos[ComboListMax];
    const char *comboLabels[ComboListMax];
    LButtonEx  *AuxButton;
    LButtonEx  *SeqButton;
};

void ConfigEditNIBPMenuPrivate::loadOptions()
{
    Config *config = configEditMenuGrp.getCurrentEditConfig();
    int index  = 0;

    if (combos[InitialInflatingPressure]->count() == 0)
    {
        combos[InitialInflatingPressure]->addItem(trs("OFF"));
        for(int i = 0; i < NIBP_INIT_PRE_NR; i++)
        {
            int time = 80 + 10 * i;
            combos[InitialInflatingPressure]->addItem(QString::number(time) + " mmHg");
        }
        config->getNumValue("NIBP|InitialInflatingPressure", index);
        combos[InitialInflatingPressure]->setCurrentIndex(index);
    }

    if (combos[IntervalTime]->count() == 0)
    {
        for (int i = 0; i < NIBP_INT_TIM_NR; i++)
        {
            combos[IntervalTime]->addItem(NIBPSymbol::convert((NIBPIntervalTime)i));
        }
        config->getNumValue("NIBP|IntervalTime", index);
        combos[IntervalTime]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[NIBPcompleteSound]->count() == 0)
    {
        combos[NIBPcompleteSound]->addItem(trs("OFF"));
        combos[NIBPcompleteSound]->addItem(trs("ON"));
        config->getNumValue("NIBP|NIBPcompleteSound", index);
        combos[NIBPcompleteSound]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[WholePointMeasure]->count() == 0)
    {
        combos[WholePointMeasure]->addItem(trs("OFF"));
        combos[WholePointMeasure]->addItem(trs("ON"));
        config->getNumValue("NIBP|WholePointMeasure", index);
        combos[WholePointMeasure]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[StartUpMeasurement]->count() == 0)
    {
        combos[StartUpMeasurement]->addItem(trs("OFF"));
        combos[StartUpMeasurement]->addItem(trs("ON"));
        config->getNumValue("NIBP|StartUpMeasurement", index);
        combos[StartUpMeasurement]->setCurrentIndex(index);
    }

    index  = 0;
    if (combos[ContinuousMeasurement]->count() == 0)
    {
        combos[ContinuousMeasurement]->addItem(trs("OFF"));
        combos[ContinuousMeasurement]->addItem(trs("ON"));
        config->getNumValue("NIBP|ContinuousMeasurement", index);
        combos[ContinuousMeasurement]->setCurrentIndex(index);
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

    for(int i = 0; i< ConfigEditNIBPMenuPrivate::ComboListMax-2; i++)
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

    QVBoxLayout *hlayout = new QVBoxLayout();
    hlayout->setContentsMargins(QMargins(submenuW-btnWidth, 0, 0, 0));
    hlayout->setSpacing(1);
    //辅助静脉穿刺
    d_ptr->AuxButton=new LButtonEx();
    d_ptr->AuxButton->setText(trs("AuxiliaryVenipuncture>>"));
    d_ptr->AuxButton->setFont(defaultFont());
    d_ptr->AuxButton->setFixedSize(btnWidth,ITEM_H);
    hlayout->addWidget(d_ptr->AuxButton);
    connect(d_ptr->AuxButton, SIGNAL(realReleased()), this, SLOT(onBtnClick()));
    //序列测量设置
    d_ptr->SeqButton=new LButtonEx();
    d_ptr->SeqButton->setText(trs("SequenceMeasurementSetting>>"));
    d_ptr->SeqButton->setFont(defaultFont());
    d_ptr->SeqButton->setFixedSize(btnWidth,ITEM_H);
    hlayout->addWidget(d_ptr->SeqButton);
    connect(d_ptr->SeqButton, SIGNAL(realReleased()), this, SLOT(onBtnClick()));

    mainLayout->addLayout(hlayout);
}

void ConfigEditNIBPMenu::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditNIBPMenu::onBtnClick()
{
    LButtonEx *btn = qobject_cast<LButtonEx *>(sender());
    if(btn == d_ptr->AuxButton)//辅助静脉穿刺按钮
    {

    }
    else if(btn == d_ptr->SeqButton)//序列测量设置按钮
    {

        //MenuGroup::addSubMenu(new ConfigEditNIBPMenu());
    }
    else
    {

    }
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

    case ConfigEditNIBPMenuPrivate::InitialInflatingPressure:
        config->setNumValue("NIBP|InitialInflatingPressure", index);
        break;
    case ConfigEditNIBPMenuPrivate::IntervalTime:
        config->setNumValue("NIBP|IntervalTime", index);
        break;
    case ConfigEditNIBPMenuPrivate::NIBPcompleteSound:
        config->setNumValue("NIBP|NIBPcompleteSound", index);
        break;
    case ConfigEditNIBPMenuPrivate::WholePointMeasure:
        config->setNumValue("NIBP|WholePointMeasure", index);
        break;
    case ConfigEditNIBPMenuPrivate::StartUpMeasurement:
        config->setNumValue("NIBP|StartUpMeasurement", index);
        break;
    case ConfigEditNIBPMenuPrivate::ContinuousMeasurement:
        config->setNumValue("NIBP|ContinuousMeasurement", index);
        break;
    case ConfigEditNIBPMenuPrivate::AuxiliaryVenipuncture:
        config->setNumValue("NIBP|AuxiliaryVenipuncture", index);
        break;
    case ConfigEditNIBPMenuPrivate::SequenceMeasurementSetting:
        config->setNumValue("NIBP|SequenceMeasurementSetting", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}
