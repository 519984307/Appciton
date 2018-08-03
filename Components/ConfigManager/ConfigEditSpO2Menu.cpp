/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/3
 **/


#include "ConfigEditSpO2Menu.h"
#include "ConfigEditMenuGrp.h"
#include "IComboList.h"
#include "SPO2Symbol.h"
#include "ConfigEditAlarmLimitMenu.h"
#include "LabelButton.h"
#include "ConfigManager.h"

class ConfigEditSpO2MenuPrivate
{
public:
    enum ComboListId
    {
        Sensitivity,  /*灵敏度*/
        SmartPluseTone,/*智能脉搏音*/
        WaveVelocity,/*血氧波形*/
        Gain,/*波形增益系数*/
        ModuleControl,/*模块功能控制 使能或关闭*/
        ComboListMax,
    };

    ConfigEditSpO2MenuPrivate():_alarmLbtn(NULL)
    {
        memset(combos, 0, sizeof(combos));

        //常量存放在静态变量区
        comboLabels[Sensitivity] = "Sensitivity";
        comboLabels[SmartPluseTone] = "SmartPluseTone";
        comboLabels[WaveVelocity] = "WaveVelocity";
        comboLabels[Gain] = "Gain";
        comboLabels[ModuleControl] = "ModuleControl";
    }

    void loadOptions();

    IComboList *combos[ComboListMax];
    const char *comboLabels[ComboListMax];
    LabelButton *_alarmLbtn;            //跳到报警项设置按钮
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

        config->getNumValue("SPO2|Sensitivity", index);
        combos[Sensitivity]->setCurrentIndex(index);
    }

    if (combos[SmartPluseTone]->count() == 0)
    {
        combos[SmartPluseTone]->addItem(trs("OFF"));
        combos[SmartPluseTone]->addItem(trs("ON"));
        config->getNumValue("SPO2|SmartPluseTone", index);
        combos[SmartPluseTone]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[WaveVelocity]->count() == 0)
    {
        for (int i = 0; i < SPO2_WAVE_VELOCITY_NR; i++)
        {
            combos[WaveVelocity]->addItem(SPO2Symbol::convert((SPO2WaveVelocity)i));
        }
        config->getNumValue("SPO2|WaveVelocity", index);
        combos[WaveVelocity]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[Gain]->count() == 0)
    {
        for (int i = 0; i < SPO2_GAIN_NR; i++)
        {
            combos[Gain]->addItem(SPO2Symbol::convert((SPO2Gain)i));
        }
        config->getNumValue("SPO2|Gain", index);
        combos[Gain]->setCurrentIndex(index);
    }
    index  = 0;
    if (combos[ModuleControl]->count() == 0)
    {
        combos[ModuleControl]->addItem(trs("Disable"));
        combos[ModuleControl]->addItem(trs("Enable"));
        config->getNumValue("SPO2|ModuleControl", index);
        combos[ModuleControl]->setCurrentIndex(index);
    }
}

ConfigEditSpO2Menu::ConfigEditSpO2Menu()
    : SubMenu(trs("SPO2Menu")),
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

    for (int i = 0; i < ConfigEditSpO2MenuPrivate::ComboListMax; i++)
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
    d_ptr->_alarmLbtn = new LabelButton("");
    d_ptr->_alarmLbtn->setFont(defaultFont());
    d_ptr->_alarmLbtn->label->setFixedSize(labelWidth, ITEM_H);
    d_ptr->_alarmLbtn->button->setFixedSize(btnWidth, ITEM_H);
    d_ptr->_alarmLbtn->button->setText(trs("AlarmLimitSetUp"));
    connect(d_ptr->_alarmLbtn->button, SIGNAL(realReleased()), this, SLOT(_alarmLbtnSlot()));
    mainLayout->addWidget(d_ptr->_alarmLbtn);
    //按照比例分配空余空间
    mainLayout->addStretch(1);
}
//报警项设置
void ConfigEditSpO2Menu::_alarmLbtnSlot()
{
    SubMenu *subMenuPrevious = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditSpO2Menu"];
    SubMenu *subMenuCurrent = (configEditMenuGrp.getCurrentEditConfigItem())["ConfigEditAlarmLimitMenu"];
    ConfigEditAlarmLimitMenu *alarmLimit = qobject_cast<ConfigEditAlarmLimitMenu *>(subMenuCurrent);
    alarmLimit->setFocusIndex(SUB_PARAM_SPO2 + 1);
    configEditMenuGrp.changePage(subMenuCurrent, subMenuPrevious);
}
void ConfigEditSpO2Menu::readyShow()
{
    d_ptr->loadOptions();

    bool preStatusBool = !configManager.getWidgetsPreStatus();

    for (int i = 0; i < ConfigEditSpO2MenuPrivate::ComboListMax; i++)
    {
        d_ptr->combos[i]->setEnabled(preStatusBool);
    }
}


void ConfigEditSpO2Menu::onComboListConfigChanged(int index)
{
    IComboList *combo = qobject_cast<IComboList *>(sender());
    if (!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }
    //根据索引设置对应列表的参数值
    ConfigEditSpO2MenuPrivate::ComboListId comboId = (ConfigEditSpO2MenuPrivate::ComboListId)
            combo->property("comboId").toInt();

    Config *config = configEditMenuGrp.getCurrentEditConfig();

    switch (comboId)
    {
    case ConfigEditSpO2MenuPrivate::Sensitivity:
        config->setNumValue("SPO2|Sensitivity", index);
        break;
    case ConfigEditSpO2MenuPrivate::SmartPluseTone:
        config->setNumValue("SPO2|SmartPluseTone", index);
        break;
    case ConfigEditSpO2MenuPrivate::WaveVelocity:
        config->setNumValue("SPO2|WaveVelocity", index);
        break;
    case ConfigEditSpO2MenuPrivate::Gain:
        config->setNumValue("SPO2|Gain", index);
        break;
    case ConfigEditSpO2MenuPrivate::ModuleControl:
        config->setNumValue("SPO2|ModuleControl", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}
