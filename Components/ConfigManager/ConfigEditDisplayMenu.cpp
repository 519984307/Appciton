#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "UnitManager.h"
#include "ConfigEditDisplayMenu.h"
#include "SupervisorConfigManager.h"
#include "SupervisorCodeMarker.h"
#include "SupervisorMenuManager.h"

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ConfigEditDisplayMenu::ConfigEditDisplayMenu() : SubMenu(trs("ConfigEditDisplayMenu"))
{
    setDesc(trs("ConfigEditDisplayMenuDesc"));

    QString color;
    currentConfig.getStrValue("Display|AllColors", color);
    _colorList = color.split(',', QString::KeepEmptyParts);

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ConfigEditDisplayMenu::readyShow()
{
    QString color;
    currentConfig.getStrValue("Display|ECGColor", color);
    _ecgColor->setCurrentIndex(_colorList.indexOf(color));

    currentConfig.getStrValue("Display|SPO2Color", color);
    _spo2Color->setCurrentIndex(_colorList.indexOf(color));

    currentConfig.getStrValue("Display|NIBPColor", color);
    _nibpColor->setCurrentIndex(_colorList.indexOf(color));

    currentConfig.getStrValue("Display|CO2Color", color);
    _co2Color->setCurrentIndex(_colorList.indexOf(color));

    currentConfig.getStrValue("Display|RESPColor", color);
    _respColor->setCurrentIndex(_colorList.indexOf(color));

    currentConfig.getStrValue("Display|TEMPColor", color);
    _tempColor->setCurrentIndex(_colorList.indexOf(color));

    bool preStatusBool = !configManager.getWidgetsPreStatus();

    _ecgColor->setEnabled(preStatusBool);
    _spo2Color->setEnabled(preStatusBool);
    _nibpColor->setEnabled(preStatusBool);
    _co2Color->setEnabled(preStatusBool);
    _respColor->setEnabled(preStatusBool);
    _tempColor->setEnabled(preStatusBool);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void ConfigEditDisplayMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;
    int size = _colorList.size();

    //ecg color
    _ecgColor = new IComboList("ECG");
    _ecgColor->setFont(fontManager.textFont(fontSize));
    _ecgColor->label->setFixedSize(labelWidth, ITEM_H);
    _ecgColor->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < size; ++i)
    {
        _ecgColor->addItem(trs(_colorList.at(i)));
    }
    connect(_ecgColor, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_ecgColorChangeSlot(int)));
    mainLayout->addWidget(_ecgColor);

    //spo2 color
    _spo2Color = new IComboList("SPO2");
    _spo2Color->setFont(fontManager.textFont(fontSize));
    _spo2Color->label->setFixedSize(labelWidth, ITEM_H);
    _spo2Color->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < size; ++i)
    {
        _spo2Color->addItem(trs(_colorList.at(i)));
    }
    connect(_spo2Color, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_spo2ColorChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        mainLayout->addWidget(_spo2Color);
    }

    //nibp color
    _nibpColor = new IComboList("NIBP");
    _nibpColor->setFont(fontManager.textFont(fontSize));
    _nibpColor->label->setFixedSize(labelWidth, ITEM_H);
    _nibpColor->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < size; ++i)
    {
        _nibpColor->addItem(trs(_colorList.at(i)));
    }
    connect(_nibpColor, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_nibpColorChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        mainLayout->addWidget(_nibpColor);
    }

    //co2 color
    _co2Color = new IComboList("CO2");
    _co2Color->setFont(fontManager.textFont(fontSize));
    _co2Color->label->setFixedSize(labelWidth, ITEM_H);
    _co2Color->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < size; ++i)
    {
        _co2Color->addItem(trs(_colorList.at(i)));
    }
    connect(_co2Color, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_co2ColorChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_CO2))
    {
        mainLayout->addWidget(_co2Color);
    }

    //resp color
    _respColor = new IComboList("RESP");
    _respColor->setFont(fontManager.textFont(fontSize));
    _respColor->label->setFixedSize(labelWidth, ITEM_H);
    _respColor->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < size; ++i)
    {
        _respColor->addItem(trs(_colorList.at(i)));
    }
    connect(_respColor, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_respColorChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_RESP))
    {
        mainLayout->addWidget(_respColor);
    }

    //temp color
    _tempColor = new IComboList("TEMP");
    _tempColor->setFont(fontManager.textFont(fontSize));
    _tempColor->label->setFixedSize(labelWidth, ITEM_H);
    _tempColor->combolist->setFixedSize(btnWidth, ITEM_H);

    for (int i = 0; i < size; ++i)
    {
        _tempColor->addItem(trs(_colorList.at(i)));
    }
    connect(_tempColor, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_tempColorChangeSlot(int)));
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        mainLayout->addWidget(_tempColor);
    }
}

/**************************************************************************************************
 * ecg color改变。
 *************************************************************************************************/
void ConfigEditDisplayMenu::_ecgColorChangeSlot(int index)
{
    currentConfig.setStrValue("Display|ECGColor", _colorList.at(index));
}

/**************************************************************************************************
 * spo2 color改变。
 *************************************************************************************************/
void ConfigEditDisplayMenu::_spo2ColorChangeSlot(int index)
{
    currentConfig.setStrValue("Display|SPO2Color", _colorList.at(index));
}

/**************************************************************************************************
 * nibp color 改变。
 *************************************************************************************************/
void ConfigEditDisplayMenu::_nibpColorChangeSlot(int index)
{
    currentConfig.setStrValue("Display|NIBPColor", _colorList.at(index));
}

/**************************************************************************************************
 * co2 color改变。
 *************************************************************************************************/
void ConfigEditDisplayMenu::_co2ColorChangeSlot(int index)
{
    currentConfig.setStrValue("Display|CO2Color", _colorList.at(index));
}

/**************************************************************************************************
 * resp color改变。
 *************************************************************************************************/
void ConfigEditDisplayMenu::_respColorChangeSlot(int index)
{
    currentConfig.setStrValue("Display|RESPColor", _colorList.at(index));
}

/**************************************************************************************************
 * temp color改变。
 *************************************************************************************************/
void ConfigEditDisplayMenu::_tempColorChangeSlot(int index)
{
    currentConfig.setStrValue("Display|TEMPColor", _colorList.at(index));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ConfigEditDisplayMenu::~ConfigEditDisplayMenu()
{

}
