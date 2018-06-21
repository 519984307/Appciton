#include "AGMenu.h"
#include "LanguageManager.h"
#include "MenuManager.h"
#include "IComboList.h"
#include "AGSymbol.h"
#include "AGParam.h"
#include "IConfig.h"
#include "AlarmLimitMenu.h"
#include "PublicMenuManager.h"
#include "LoadConfigMenu.h"

AGMenu *AGMenu::_selfObj = NULL;

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
AGMenu::~AGMenu()
{

}

void AGMenu::readyShow()
{
    _loadOptions();
}

void AGMenu::_loadOptions()
{
    int index=0;
    systemConfig.getNumValue("PrimaryCfg|AG|AGModule", index);

    _isEnableAGCombo->combolist->setCurrentIndex(index);
    _speedIcomb->setEnabled(!index);
    for(int i=0; i<AG_TYPE_NR; i++)
    {
        _agGasTypeLbtn[i]->setEnabled(!index);
    }
    _isEnable = (!index);

    systemConfig.getNumValue("PrimaryCfg|AG|SweepSpeed", index);
    _speedIcomb->combolist->setCurrentIndex((AGSweepSpeed)index);

}

void AGMenu::_onConfigUpdated()
{
    readyShow();
}

void AGMenu::readyhide(){}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void AGMenu::layoutExec()
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _isEnableAGCombo = new IComboList(trs("AGModule"));
    _isEnableAGCombo->setFont(defaultFont());
    _isEnableAGCombo->label->setFixedSize(labelWidth, ITEM_H);
    _isEnableAGCombo->combolist->setFixedSize(btnWidth, ITEM_H);
    _isEnableAGCombo->addItem(trs("Enable"));
    _isEnableAGCombo->addItem(trs("Disable"));
    connect(_isEnableAGCombo->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_isEnableAGComboSlot(int)));
    mainLayout->addWidget(_isEnableAGCombo);

    for(int i=0; i<AG_TYPE_NR; i++)
    {
        _agGasTypeLbtn[i] = new LabelButton(trs(AGSymbol::convert((AGTypeGas)i)));
        _agGasTypeLbtn[i]->setFont(defaultFont());
        _agGasTypeLbtn[i]->label->setFixedSize(labelWidth, ITEM_H);
        _agGasTypeLbtn[i]->button->setFixedSize(btnWidth, ITEM_H);
        _agGasTypeLbtn[i]->button->setText(trs("SetUp"));
        connect(_agGasTypeLbtn[i]->button, SIGNAL(realReleased()), this, SLOT(_lBtonSlot()));
        _agGasTypeLbtn[i]->button->setProperty("Btn", qVariantFromValue(i));
        mainLayout->addWidget(_agGasTypeLbtn[i]);
    }

    _speedIcomb = new IComboList(trs("AGSweepSpeed"));
    _speedIcomb->setFont(defaultFont());
    for (int i = 0; i < AG_SWEEP_SPEED_NR; i ++)
    {
        _speedIcomb->addItem(AGSymbol::convert((AGSweepSpeed)i));
    }
    connect(_speedIcomb, SIGNAL(currentIndexChanged(int)), this, SLOT(_speedIcombSlot(int)));
    _speedIcomb->label->setFixedSize(labelWidth, ITEM_H);
    _speedIcomb->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_speedIcomb);

    _alarmLbtn = new LabelButton("");
    _alarmLbtn->setFont(defaultFont());
    _alarmLbtn->label->setFixedSize(labelWidth, ITEM_H);
    _alarmLbtn->button->setFixedSize(btnWidth, ITEM_H);
    _alarmLbtn->button->setText(trs("AlarmLimitSetUp"));
    connect(_alarmLbtn->button, SIGNAL(realReleased()), this, SLOT(_alarmLbtnSlot()));
    mainLayout->addWidget(_alarmLbtn);

}

/**************************************************************************************************
 * 波形速度槽函数。
 *************************************************************************************************/
void AGMenu::_speedIcombSlot(int index)
{
    agParam.setSweepSpeed((AGSweepSpeed)index);
    systemConfig.setNumValue("PrimaryCfg|AG|SweepSpeed", index);
}

/**************************************************************************************************
 * 模块是否使能槽函数。
 *************************************************************************************************/
void AGMenu::_isEnableAGComboSlot(int index)
{
//    agParam.setSweepSpeed((AGSweepSpeed)index);
    if((!index) != _isEnable)
    {
        _speedIcomb->setEnabled(!index);
        for(int i=0; i<AG_TYPE_NR; i++)
        {
            _agGasTypeLbtn[i]->setEnabled(!index);
        }
        _isEnable = (!index);
    }
    systemConfig.setNumValue("PrimaryCfg|AG|AGModule", index);
}
/**************************************************************************************************
 * 气体类型设置槽函数。
 *************************************************************************************************/
void AGMenu::_lBtonSlot()
{
    LButton *gasTypeBtn = qobject_cast<LButton*>(sender());
    int gasTypeNum = (gasTypeBtn->property("Btn")).toInt();
    switch(gasTypeNum)
    {
    case AG_TYPE_CO2:
        break;
    case AG_TYPE_N2O:
        break;
    case AG_TYPE_AA1:
        break;
    case AG_TYPE_AA2:
        break;
    case AG_TYPE_O2:
        break;
    }
    if(gasTypeNum<AG_TYPE_NR)
    {

    }

}

//报警项设置
void AGMenu::_alarmLbtnSlot()
{
    alarmLimitMenu.setFocusIndex(SUB_PARAM_ETCO2+1);
    publicMenuManager.changePage(&alarmLimitMenu, &agMenu);
}
/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
AGMenu::AGMenu() : SubMenu(trs("AGMenu"))
{
    setDesc(trs("AGMenuDesc"));
    startLayout();
    connect(&configManager, SIGNAL(configUpdated()), this, SLOT(_onConfigUpdated()));
}
