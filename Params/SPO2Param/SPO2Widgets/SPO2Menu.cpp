#include "SPO2Menu.h"
#include "SPO2Param.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include "MenuManager.h"
#include "IConfig.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "LoadConfigMenu.h"

SPO2Menu *SPO2Menu::_selfObj = NULL;

/**************************************************************************************************
 * 灵敏度设置。
 *************************************************************************************************/
void SPO2Menu::_sensitivitySlot(int index)
{
    spo2Param.setSensitivity((SPO2Sensitive)index);
}

/**************************************************************************************************
 * 智能音设置。
 *************************************************************************************************/
void SPO2Menu::_smartPulseToneSlot(int index)
{
    spo2Param.setSmartPulseTone((SPO2SMARTPLUSETONE)index);
}

/**************************************************************************************************
 * 显示前确定配置。
 *************************************************************************************************/
void SPO2Menu::readyShow(void)
{
    _sensitivity->setCurrentIndex(spo2Param.getSensitivity());
    _smartPulseTone->setCurrentIndex(spo2Param.getSmartPulseTone());
}

void SPO2Menu::_updateConfigSlot()
{
    readyShow();
}

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void SPO2Menu::layoutExec(void)
{
    int submenuW = menuManager.getSubmenuWidth();
    int submenuH = menuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _sensitivity = new IComboList(trs("SPO2Sensitivity"));         // 导联模式。
    _sensitivity->setFont(defaultFont());
    _sensitivity->label->setFixedSize(labelWidth, ITEM_H);
    _sensitivity->combolist->setFixedSize(btnWidth, ITEM_H);
    _sensitivity->addItem(trs(SPO2Symbol::convert(SPO2_SENS_LOW)));
    _sensitivity->addItem(trs(SPO2Symbol::convert(SPO2_SENS_MED)));
    _sensitivity->addItem(trs(SPO2Symbol::convert(SPO2_SENS_HIGH)));
    connect(_sensitivity, SIGNAL(currentIndexChanged(int)), this, SLOT(_sensitivitySlot(int)));

    QString str;
    machineConfig.getStrValue("SPO2", str);
    if (str == "BLM_TS3")
    {
        mainLayout->addWidget(_sensitivity);
    }

    _smartPulseTone = new IComboList(trs("SPO2SmartPulseTone"));         // 导联模式。
    _smartPulseTone->setFont(defaultFont());
    _smartPulseTone->label->setFixedSize(labelWidth, ITEM_H);
    _smartPulseTone->combolist->setFixedSize(btnWidth, ITEM_H);
    _smartPulseTone->addItem(trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_ON)));
    _smartPulseTone->addItem(trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_OFF)));
    connect(_smartPulseTone, SIGNAL(currentIndexChanged(int)), this, SLOT(_smartPulseToneSlot(int)));
    mainLayout->addWidget(_smartPulseTone);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SPO2Menu::SPO2Menu() : SubMenu(trs("SPO2Menu"))
{
    setDesc(trs("SPO2MenuDesc"));
    startLayout();
    connect(&loadConfigMenu, SIGNAL(updateConfigSignal()), this, SLOT(_updateConfigSlot()));
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SPO2Menu::~SPO2Menu()
{

}
