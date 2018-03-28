#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "UnitManager.h"
#include "SupervisorLocalMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorCodeMarker.h"
#include "SupervisorMenuManager.h"
#include "PatientManager.h"

SupervisorLocalMenu *SupervisorLocalMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorLocalMenu::SupervisorLocalMenu() : SubMenu(trs("SupervisorLocalMenu"))
{
    setDesc(trs("SupervisorLocalMenuDesc"));

    startLayout();
}

/**************************************************************************************************superConfig.getNumValue("Local|CO2Unit", index);
    if (index == UNIT_PERCENT)
    {
        _co2Unit->setCurrentIndex(0);
    }
    else if (index == UNIT_MMHG)
    {
        _co2Unit->setCurrentIndex(1);
    }
    else if (index == UNIT_KPA)
    {
        _co2Unit->setCurrentIndex(2);
    }
 * 显示。
 *************************************************************************************************/
void SupervisorLocalMenu::readyShow()
{
    int index = 0;
    superConfig.getNumAttr("Local|Language", "CurrentOption", index);
    _language->setCurrentIndex(index);

    // NIBP.
    superConfig.getNumValue("Local|NIBPUnit", index);
    _nibpUnit->setCurrentIndex((index == UNIT_MMHG) ? 0 : 1);

    // CO2单位。
    superConfig.getNumValue("Local|CO2Unit", index);
    if (index == UNIT_PERCENT)
    {
        _co2Unit->setCurrentIndex(0);
    }
    else if (index == UNIT_MMHG)
    {
        _co2Unit->setCurrentIndex(1);
    }
    else if (index == UNIT_KPA)
    {
        _co2Unit->setCurrentIndex(2);
    }

    // TEMP单位。
    superConfig.getNumValue("Local|TEMPUnit", index);
    _tempUnit->setCurrentIndex((index == UNIT_TC) ? 0 : 1);

    // weight单位。
    superConfig.getNumValue("Local|WEIGHTUnit", index);
    _weightUnit->setCurrentIndex((index == UNIT_KG) ? 0 : 1);

    // height单位。
    superConfig.getNumValue("Local|HEIGHTUnit", index);
    if (index == UNIT_CM)
    {
        _heightUnit->setCurrentIndex(0);
    }
    else if (index == UNIT_MM)
    {
        _heightUnit->setCurrentIndex(1);
    }
    else if (index == UNIT_INCH)
    {
        _heightUnit->setCurrentIndex(2);
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorLocalMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //语言
    _language = new IComboList(trs("SupervisorLanguage"));
    _language->setFont(fontManager.textFont(fontSize));
    _language->label->setFixedSize(labelWidth, ITEM_H);
    _language->combolist->setFixedSize(btnWidth, ITEM_H);
    QVector <QString> options;
    superConfig.getOptionList("Local|Language", options);
    for (int i = 0; i < options.size(); i++)
    {
        _language->addItem(options[i]);
    }
    connect(_language, SIGNAL(currentIndexChanged(int)), this, SLOT(_languageChangeSlot(int)));
    mainLayout->addWidget(_language);

    //添加显示单位。
    // NIBP单位。
    _nibpUnit = new IComboList(trs("SupervisorNIBPUnit"));
    _nibpUnit->setFont(fontManager.textFont(fontSize));
    _nibpUnit->label->setFixedSize(labelWidth, ITEM_H);
    _nibpUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    _nibpUnit->addItem(Unit::localeSymbol(UNIT_MMHG));
    _nibpUnit->addItem(Unit::localeSymbol(UNIT_KPA));
    connect(_nibpUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_nibpUnitChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        mainLayout->addWidget(_nibpUnit);
    }

    // CO2单位。
    _co2Unit = new IComboList(trs("SupervisorCO2Unit"));
    _co2Unit->setFont(fontManager.textFont(fontSize));
    _co2Unit->label->setFixedSize(labelWidth, ITEM_H);
    _co2Unit->combolist->setFixedSize(btnWidth, ITEM_H);
    _co2Unit->addItem(Unit::localeSymbol(UNIT_PERCENT));
    _co2Unit->addItem(Unit::localeSymbol(UNIT_MMHG));
    _co2Unit->addItem(Unit::localeSymbol(UNIT_KPA));
    connect(_co2Unit, SIGNAL(currentIndexChanged(int)), this, SLOT(_co2UnitSlot(int)));
    if (systemManager.isSupport(CONFIG_CO2))
    {
        mainLayout->addWidget(_co2Unit);
    }

    // 体温单位。
    _tempUnit = new IComboList(trs("SupervisorTEMPUnit"));
    _tempUnit->setFont(fontManager.textFont(fontSize));
    _tempUnit->label->setFixedSize(labelWidth, ITEM_H);
    _tempUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    _tempUnit->addItem(Unit::localeSymbol(UNIT_TC));
    _tempUnit->addItem(Unit::localeSymbol(UNIT_TF));
    connect(_tempUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_tempUnitChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        mainLayout->addWidget(_tempUnit);
    }

    // 体重单位。
    _weightUnit = new IComboList(trs("SupervisorWeightUnit"));
    _weightUnit->setFont(fontManager.textFont(fontSize));
    _weightUnit->label->setFixedSize(labelWidth, ITEM_H);
    _weightUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    _weightUnit->addItem(Unit::localeSymbol(UNIT_KG));
    _weightUnit->addItem(Unit::localeSymbol(UNIT_LB));
    connect(_weightUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_weightUnitChangeSlot(int)));
    mainLayout->addWidget(_weightUnit);

    // 身高单位。
    _heightUnit = new IComboList(trs("SupervisorHeightUnit"));
    _heightUnit->setFont(fontManager.textFont(fontSize));
    _heightUnit->label->setFixedSize(labelWidth, ITEM_H);
    _heightUnit->combolist->setFixedSize(btnWidth, ITEM_H);
    _heightUnit->addItem(Unit::localeSymbol(UNIT_CM));
    _heightUnit->addItem(Unit::localeSymbol(UNIT_MM));
    _heightUnit->addItem(Unit::localeSymbol(UNIT_INCH));
    connect(_heightUnit, SIGNAL(currentIndexChanged(int)), this, SLOT(_heightUnitChangeSlot(int)));
    mainLayout->addWidget(_heightUnit);
}

/**************************************************************************************************
 * 语言改变。
 *************************************************************************************************/
void SupervisorLocalMenu::_languageChangeSlot(int index)
{
    superConfig.setNumAttr("Local|Language", "CurrentOption", index);

//    QStringList codeMarkerTypeList;

//    QString codemarkerStr;
//    QString indexStr = "CodeMarker|Marker";
//    indexStr += QString::number(index, 10);

//    superConfig.getStrValue(indexStr, codemarkerStr);
//    codeMarkerTypeList = codemarkerStr.split(',', QString::KeepEmptyParts);
//    codeMarkerTypeList.append("");

//    while (CODEMARKER_MAX < codeMarkerTypeList.size())
//    {
//        codeMarkerTypeList.takeLast();
//    }

//    QStringList tmpList(codeMarkerTypeList);
//    while (tmpList.removeOne(""))
//    {

//    }
//    QString strValue = tmpList.join(",");


//    QString markerStr = "CodeMarker|SelectMarker|Language";
//    markerStr += QString::number(index, 10);
//    superConfig.setStrValue(markerStr, strValue);
}

/**************************************************************************************************
 * 血压单位改变。
 *************************************************************************************************/
void SupervisorLocalMenu::_nibpUnitChangeSlot(int index)
{
    superConfig.setNumValue("Local|NIBPUnit", (int)((index == 0) ? UNIT_MMHG : UNIT_KPA));
}

/**************************************************************************************************
 * 二氧化碳单位改变。
 *************************************************************************************************/
void SupervisorLocalMenu::_co2UnitSlot(int index)
{
    if (index == 0)
    {
        superConfig.setNumValue("Local|CO2Unit", (int)UNIT_PERCENT);
    }
    else if (index == 1)
    {
        superConfig.setNumValue("Local|CO2Unit", (int)UNIT_MMHG);
    }
    else if (index == 2)
    {
        superConfig.setNumValue("Local|CO2Unit", (int)UNIT_KPA);
    }
}

/**************************************************************************************************
 * 体温单位改变。
 *************************************************************************************************/
void SupervisorLocalMenu::_tempUnitChangeSlot(int index)
{
    superConfig.setNumValue("Local|TEMPUnit", (int)((index == 0) ? UNIT_TC : UNIT_TF));
}

/**************************************************************************************************
 * 体重单位改变。
 *************************************************************************************************/
void SupervisorLocalMenu::_weightUnitChangeSlot(int index)
{
    patientManager.setWeight(0);
    superConfig.setNumValue("Local|WEIGHTUnit", (int)((index == 0) ? UNIT_KG : UNIT_LB));
}

/**************************************************************************************************
 * 身高单位改变。
 *************************************************************************************************/
void SupervisorLocalMenu::_heightUnitChangeSlot(int index)
{
    patientManager.setHeight(0);
    if (index == 0)
    {
        superConfig.setNumValue("Local|HEIGHTUnit", (int)UNIT_CM);
    }
    else if (index == 1)
    {
        superConfig.setNumValue("Local|HEIGHTUnit", (int)UNIT_MM);
    }
    else if (index == 2)
    {
        superConfig.setNumValue("Local|HEIGHTUnit", (int)UNIT_INCH);
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorLocalMenu::~SupervisorLocalMenu()
{

}

