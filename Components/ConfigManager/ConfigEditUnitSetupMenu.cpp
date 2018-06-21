#include <QHBoxLayout>
#include <QVBoxLayout>
#include "ConfigEditCO2Menu.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "IComboList.h"
#include "LabelButton.h"
#include "ISpinBox.h"
#include "LanguageManager.h"
#include "MenuManager.h"
#include "ConfigEditMenuGrp.h"
#include "Config.h"
#include "ConfigEditUnitSetupMenu.h"
#include "UnitManager.h"
#include "IConfig.h"
#include "LoadConfigMenu.h"
#include "ConfigManager.h"
ConfigEditUnitSetupMenu *ConfigEditUnitSetupMenu::_selfObj = NULL;


/**************************************************************************************************
 * 高度单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_hightUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_CM;
    }
    else if(index==1)
    {
        index = UNIT_INCH;
    }
    systemConfig.setNumValue("Unit|HightUnit",index);
}

/**************************************************************************************************
 * 体重单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_weightUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_KG;
    }
    else if(index==1)
    {
        index = UNIT_LB;
    }
    systemConfig.setNumValue("Unit|WeightUnit",index);
}

/**************************************************************************************************
 * ST单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_stUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_MV;
    }
    else if(index==1)
    {
        index = UNIT_MM;
    }
    systemConfig.setNumValue("Unit|STUnit",index);
}

/**************************************************************************************************
 * 压力单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_pressureUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_MMHG;
    }
    else if(index==1)
    {
        index = UNIT_KPA;
    }
    else
    {
        index = UNIT_MMHG;
    }
    systemConfig.setNumValue("Unit|PressureUnit",index);
}

/**************************************************************************************************
 * 温度单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_temperatureUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_TF;
    }
    else if(index==1)
    {
        index = UNIT_TC;
    }
    else
    {
        index = UNIT_TF;
    }
    systemConfig.setNumValue("Unit|TemperatureUnit",index);
}

/**************************************************************************************************
 * CVP单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_cvpUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_MMHG;
    }
    else if(index==1)
    {
        index = UNIT_KPA;
    }
    else if(index ==2)
    {
        index = UNIT_CMH2O;
    }
    else
    {
        index = UNIT_MMHG;
    }
    systemConfig.setNumValue("Unit|CVPUnit",index);
}

/**************************************************************************************************
 * ICP单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_icpUnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_MMHG;
    }
    else if(index==1)
    {
        index = UNIT_KPA;
    }
    else if(index ==2)
    {
        index = UNIT_CMH2O;
    }
    else
    {
        index = UNIT_MMHG;
    }
    systemConfig.setNumValue("Unit|ICPUnit",index);
}

/**************************************************************************************************
 * CO2单位改变。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_co2UnitSlot(int index)
{
    if(index==0)
    {
        index = UNIT_MMHG;
    }
    else if(index==1)
    {
        index = UNIT_KPA;
    }
    else if(index ==2)
    {
        index = UNIT_PERCENT;
    }
    else
    {
        index = UNIT_MMHG;
    }
    systemConfig.setNumValue("Unit|CO2Unit",index);
}


/**************************************************************************************************
 * 载入当前配置。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::_loadOptions(void)
{
    int index  = 0;
    systemConfig.getNumValue("Unit|HightUnit",index);

    _hightUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));

    index=0;
    systemConfig.getNumValue("Unit|WeightUnit",index);
    _weightUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));

    index=0;
    systemConfig.getNumValue("Unit|STUnit",index);
    _stUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));

    index=0;
    systemConfig.getNumValue("Unit|PressureUnit",index);
    _pressureUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));

    index=0;
    systemConfig.getNumValue("Unit|TemperatureUnit",index);
    _temperatureUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));
    index=0;
    systemConfig.getNumValue("Unit|CVPUnit",index);
    _cvpUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));

    index=0;
    systemConfig.getNumValue("Unit|ICPUnit",index);
    _icpUnit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));

    index=0;
    systemConfig.getNumValue("Unit|CO2Unit",index);
    _co2Unit->setCurrentItem(trs(Unit::getSymbol(UnitType(index))));
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::readyShow(void)
{
    _loadOptions();

    bool preStatusBool = !configManager.getWidgetsPreStatus();

    _hightUnit->setEnabled(preStatusBool);
    _weightUnit->setEnabled(preStatusBool);
    _stUnit->setEnabled(preStatusBool);
    _pressureUnit->setEnabled(preStatusBool);
    _temperatureUnit->setEnabled(preStatusBool);
    _cvpUnit->setEnabled(preStatusBool);
    _icpUnit->setEnabled(preStatusBool);
    _co2Unit->setEnabled(preStatusBool);
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ConfigEditUnitSetupMenu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    _hightUnit = new IComboList(trs("HightUnit"));
    _hightUnit->setFont(defaultFont());
    _hightUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_CM))));
    _hightUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_INCH))));
    connect(_hightUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_hightUnitSlot(int)));
    _hightUnit->label->setFixedSize(labelWidth, ITEM_H);
    _hightUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_hightUnit);


    _weightUnit = new IComboList(trs("WeightUnit"));
    _weightUnit->setFont(defaultFont());
    _weightUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_KG))));
    _weightUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_LB))));
    connect(_weightUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_weightUnitSlot(int)));
    _weightUnit->label->setFixedSize(labelWidth, ITEM_H);
    _weightUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_weightUnit);

    _stUnit = new IComboList(trs("STUnit"));
    _stUnit->setFont(defaultFont());
    _stUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_MV))));
    _stUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_MM))));
    connect(_stUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_stUnitSlot(int)));
    _stUnit->label->setFixedSize(labelWidth, ITEM_H);
    _stUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_stUnit);

    _pressureUnit = new IComboList(trs("PressureUnit"));
    _pressureUnit->setFont(defaultFont());
    _pressureUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_MMHG))));
    _pressureUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_KPA))));
    connect(_pressureUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_pressureUnitSlot(int)));
    _pressureUnit->label->setFixedSize(labelWidth, ITEM_H);
    _pressureUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_pressureUnit);

    _temperatureUnit = new IComboList(trs("TemperatureUnit"));
    _temperatureUnit->setFont(defaultFont());
    _temperatureUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_TF))));
    _temperatureUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_TC))));
    connect(_temperatureUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_temperatureUnitSlot(int)));
    _temperatureUnit->label->setFixedSize(labelWidth, ITEM_H);
    _temperatureUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_temperatureUnit);

    _cvpUnit = new IComboList(trs("CVPUnit"));
    _cvpUnit->setFont(defaultFont());
    _cvpUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_MMHG))));
    _cvpUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_KPA))));
    _cvpUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_CMH2O))));
    connect(_cvpUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_cvpUnitSlot(int)));
    _cvpUnit->label->setFixedSize(labelWidth, ITEM_H);
    _cvpUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_cvpUnit);

    _icpUnit = new IComboList(trs("ICPUnit"));
    _icpUnit->setFont(defaultFont());
    _icpUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_MMHG))));
    _icpUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_KPA))));
    _icpUnit->addItem(trs(Unit::getSymbol(UnitType(UNIT_CMH2O))));
    connect(_icpUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_icpUnitSlot(int)));
    _icpUnit->label->setFixedSize(labelWidth, ITEM_H);
    _icpUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_icpUnit);

    _co2Unit = new IComboList(trs("CO2Unit"));
    _co2Unit->setFont(defaultFont());
    _co2Unit->addItem(trs(Unit::getSymbol(UnitType(UNIT_MMHG))));
    _co2Unit->addItem(trs(Unit::getSymbol(UnitType(UNIT_KPA))));
    _co2Unit->addItem(trs(Unit::getSymbol(UnitType(UNIT_PERCENT))));
    connect(_co2Unit, SIGNAL(currentIndexChanged(int)), this, SLOT(_co2UnitSlot(int)));
    _co2Unit->label->setFixedSize(labelWidth, ITEM_H);
    _co2Unit->combolist->setFixedSize(btnWidth, ITEM_H);
    mainLayout->addWidget(_co2Unit);
}



/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ConfigEditUnitSetupMenu::ConfigEditUnitSetupMenu() : SubMenu(trs("UnitSetup")),
                                                     _hightUnit(NULL),
                                                     _weightUnit(NULL),
                                                     _stUnit(NULL),
                                                     _pressureUnit(NULL),
                                                     _temperatureUnit(NULL),
                                                     _cvpUnit(NULL),
                                                     _icpUnit(NULL),
                                                     _co2Unit(NULL)
{
    setDesc(trs("UnitSetupDesc"));
    startLayout();
}
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ConfigEditUnitSetupMenu::~ConfigEditUnitSetupMenu()
{

}
