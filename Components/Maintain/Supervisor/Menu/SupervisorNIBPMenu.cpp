#include <QVBoxLayout>
#include "FontManager.h"
#include "LanguageManager.h"
#include "IComboList.h"
#include "NIBPDefine.h"
#include "NIBPSymbol.h"
#include "SupervisorNIBPMenu.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "UnitManager.h"

SupervisorNIBPMenu *SupervisorNIBPMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorNIBPMenu::SupervisorNIBPMenu() : SubMenu(trs("NIBPMenu"))
{
    setDesc(trs("NIBPMenuDesc"));
    startLayout();
}

void SupervisorNIBPMenu::_loadOptions()
{
    int unit = UNIT_MMHG;
    superConfig.getNumValue("Local|NIBPUnit", unit);

    _adultInitialCuff->clear();
    for (unsigned i = 0; i < NIBP_ADULT_INITIAL_CUFF_NR; ++i)
    {
        QString str = NIBPSymbol::convert((NIBPAdultInitialCuff)i);
        if (unit == UNIT_KPA)
        {
            str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
        }
        str = str + " " + Unit::getSymbol((UnitType)unit);
        _adultInitialCuff->addItem(str);
    }

    _prediatricInitialCuff->clear();
    for (unsigned i = 0; i < NIBP_PREDIATRIC_INITIAL_CUFF_NR; ++i)
    {
        QString str = NIBPSymbol::convert((NIBPPrediatrictInitialCuff)i);
        if (unit == UNIT_KPA)
        {
            str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
        }
        str = str + " " + Unit::getSymbol((UnitType)unit);
        _prediatricInitialCuff->addItem(str);
    }

    _neonatalInitialCuff->clear();
    for (unsigned i = 0; i < NIBP_NEONATAL_INITIAL_CUFF_NR; ++i)
    {
        QString str = NIBPSymbol::convert((NIBPNeonatalInitialCuff)i);
        if (unit == UNIT_KPA)
        {
            str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
        }
        str = str + " " + Unit::getSymbol((UnitType)unit);
        _neonatalInitialCuff->addItem(str);
    }
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorNIBPMenu::readyShow()
{
    _loadOptions();

    int index = 0;

    superConfig.getNumValue("NIBP|MeasureMode", index);
    _measureMode->setCurrentIndex(index);

    superConfig.getNumValue("NIBP|AutoInterval", index);
    _autoInterval->setCurrentIndex(index);

    superConfig.getNumValue("NIBP|AutomaticRetry", index);
    _startOnMeasureFail->setCurrentIndex(index);

    superConfig.getNumValue("NIBP|StatFunction", index);
    _startOnSTATEnabled->setCurrentIndex(index);

    superConfig.getNumValue("NIBP|AdultInitialCuffInflation", index);
    _adultInitialCuff->setCurrentIndex(index);

    superConfig.getNumValue("NIBP|PedInitialCuffInflation", index);
    _prediatricInitialCuff->setCurrentIndex(index);

    superConfig.getNumValue("NIBP|NeoInitialCuffInflation", index);
    _neonatalInitialCuff->setCurrentIndex(index);
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorNIBPMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 3;
    int labelWidth = itemW - btnWidth;

    //测量模式
    _measureMode = new IComboList(trs("SupervisorNIBPMeasureMode"));
    _measureMode->setFont(fontManager.textFont(fontSize));
    _measureMode->label->setFixedSize(labelWidth, ITEM_H);
    _measureMode->combolist->setFixedSize(btnWidth, ITEM_H);
    _measureMode->addItem(trs(NIBPSymbol::convert(NIBP_MODE_MANUAL)));
    _measureMode->addItem(trs(NIBPSymbol::convert(NIBP_MODE_AUTO)));
    connect(_measureMode, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_measureModeChangeSlot(int)));
    mainLayout->addWidget(_measureMode);

    //自动测量时间
    _autoInterval = new IComboList(trs("SupervisorNIBPAutoInterval"));
    _autoInterval->setFont(fontManager.textFont(fontSize));
    _autoInterval->label->setFixedSize(labelWidth, ITEM_H);
    _autoInterval->combolist->setFixedSize(btnWidth, ITEM_H);
    for (int i = 0; i < NIBP_AUTO_INTERVAL_NR; i++)
    {
        _autoInterval->addItem(trs(NIBPSymbol::convert((NIBPAutoInterval)i)));
    }
    connect(_autoInterval, SIGNAL(currentIndexChanged(int)), this, SLOT(_autoIntervalSlot(int)));
    mainLayout->addWidget(_autoInterval);

    //STAT测量使能
    _startOnSTATEnabled = new IComboList(trs("SupervisorSTATFunction"));
    _startOnSTATEnabled->setFont(fontManager.textFont(fontSize));
    _startOnSTATEnabled->label->setFixedSize(labelWidth, ITEM_H);
    _startOnSTATEnabled->combolist->setFixedSize(btnWidth, ITEM_H);
    _startOnSTATEnabled->addItem(trs(NIBPSymbol::convert(NIBP_PR_DISPLAY_OFF)));
    _startOnSTATEnabled->addItem(trs(NIBPSymbol::convert(NIBP_PR_DISPLAY_ON)));
    connect(_startOnSTATEnabled, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_startOnSTATEnabledChangeSlot(int)));
    mainLayout->addWidget(_startOnSTATEnabled);

    //测量失败额外增加一次测量
    _startOnMeasureFail = new IComboList(trs("SupervisorStartOnMeasureFail"));
    _startOnMeasureFail->setFont(fontManager.textFont(fontSize));
    _startOnMeasureFail->label->setFixedSize(labelWidth, ITEM_H);
    _startOnMeasureFail->combolist->setFixedSize(btnWidth, ITEM_H);
    _startOnMeasureFail->addItem(trs(NIBPSymbol::convert(NIBP_PR_DISPLAY_OFF)));
    _startOnMeasureFail->addItem(trs(NIBPSymbol::convert(NIBP_PR_DISPLAY_ON)));
    connect(_startOnMeasureFail, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_startOnMeasureFailChangeSlot(int)));
    mainLayout->addWidget(_startOnMeasureFail);

    //成人初始袖袋压
    _adultInitialCuff = new IComboList(trs("SupervisorAdultInitialCuff"));
    _adultInitialCuff->setFont(fontManager.textFont(fontSize));
    _adultInitialCuff->label->setFixedSize(labelWidth, ITEM_H);
    _adultInitialCuff->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_adultInitialCuff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_adultInitialCuffChangeSlot(int)));
    mainLayout->addWidget(_adultInitialCuff);

    //小儿初始袖袋压
    _prediatricInitialCuff = new IComboList(trs("SupervisorPrediatricInitialCuff"));
    _prediatricInitialCuff->setFont(fontManager.textFont(fontSize));
    _prediatricInitialCuff->label->setFixedSize(labelWidth, ITEM_H);
    _prediatricInitialCuff->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_prediatricInitialCuff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_prediatricInitialCuffChangeSlot(int)));
    mainLayout->addWidget(_prediatricInitialCuff);

    //新生儿初始袖袋压
    _neonatalInitialCuff = new IComboList(trs("SupervisorNeonatalInitialCuff"));
    _neonatalInitialCuff->setFont(fontManager.textFont(fontSize));
    _neonatalInitialCuff->label->setFixedSize(labelWidth, ITEM_H);
    _neonatalInitialCuff->combolist->setFixedSize(btnWidth, ITEM_H);
    connect(_neonatalInitialCuff, SIGNAL(currentIndexChanged(int)), this,
            SLOT(_neonatalInitialCuffChangeSlot(int)));
    mainLayout->addWidget(_neonatalInitialCuff);
}

/**************************************************************************************************
 * 测量模式。
 *************************************************************************************************/
void SupervisorNIBPMenu::_measureModeChangeSlot(int index)
{
    superConfig.setNumValue("NIBP|MeasureMode", index);
}

/**************************************************************************************************
 * 自动测量时间。
 *************************************************************************************************/
void SupervisorNIBPMenu::_autoIntervalSlot(int index)
{
    superConfig.setNumValue("NIBP|AutoInterval", index);
}

/**************************************************************************************************
 * 测量失败额外增加一次测量开关改变。
 *************************************************************************************************/
void SupervisorNIBPMenu::_startOnMeasureFailChangeSlot(int index)
{
    superConfig.setNumValue("NIBP|AutomaticRetry", index);
}

/**************************************************************************************************
 * STAT使能开关。
 *************************************************************************************************/
void SupervisorNIBPMenu::_startOnSTATEnabledChangeSlot(int index)
{
    superConfig.setNumValue("NIBP|StatFunction", index);
}

/**************************************************************************************************
 * 成人初始袖袋压改变。
 *************************************************************************************************/
void SupervisorNIBPMenu::_adultInitialCuffChangeSlot(int index)
{
    superConfig.setNumValue("NIBP|AdultInitialCuffInflation", index);
}

/**************************************************************************************************
 * 新生儿初始袖袋压改变。
 *************************************************************************************************/
void SupervisorNIBPMenu::_neonatalInitialCuffChangeSlot(int index)
{
    superConfig.setNumValue("NIBP|NeoInitialCuffInflation", index);
}

/**************************************************************************************************
 * 小儿初始袖袋压改变。
 *************************************************************************************************/
void SupervisorNIBPMenu::_prediatricInitialCuffChangeSlot(int index)
{
    superConfig.setNumValue("NIBP|PedInitialCuffInflation", index);
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorNIBPMenu::~SupervisorNIBPMenu()
{

}
