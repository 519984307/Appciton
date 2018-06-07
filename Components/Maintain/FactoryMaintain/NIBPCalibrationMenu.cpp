#include "NIBPCalibrationMenu.h"
#include <QVBoxLayout>
#include <QRegExp>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "IComboList.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "PatientDefine.h"
#include "KeyBoardPanel.h"
#include "Debug.h"
#include "UserMaintainManager.h"
#include "FactoryMaintainManager.h"
#include "NIBPRepairMenuManager.h"
#include "NIBPCalibrate.h"
#include "NIBPManometer.h"
#include "NIBPZeroPoint.h"
#include "NIBPPressureControl.h"

NIBPCalibrationMenu *NIBPCalibrationMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
NIBPCalibrationMenu::NIBPCalibrationMenu() : SubMenu(trs("NIBPCalibrationMenu")),
                                             _button(NULL)
{

    nibpRepairMenuManager.Construation();
    nibpcalibrate.construction();
    nibpRepairMenuManager.addSubMenu(&nibpcalibrate);
    nibpmanometer.construction();
    nibpRepairMenuManager.addSubMenu(&nibpmanometer);
    nibpzeropoint.construction();
    nibpRepairMenuManager.addSubMenu(&nibpzeropoint);
    nibppressurecontrol.construction();
    nibpRepairMenuManager.addSubMenu(&nibppressurecontrol);

    _button = new LabelButton(trs(""));

    _button->button->setText(QString("%1 >>").arg(trs("NIBPCalibration")));
    connect(_button->button, SIGNAL(realReleased()), this, SLOT(_onBtnSlot()));

    setDesc(trs("NIBPCalibrationMenuDesc"));
    startLayout();
}


void NIBPCalibrationMenu::readyShow()
{

}

void NIBPCalibrationMenu::layoutExec()
{
    int submenuW = factoryMaintainManager.getSubmenuWidth();
    int submenuH = factoryMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _button->label->setFixedSize(labelWidth,ITEM_H);
    _button->button->setFixedSize(btnWidth,ITEM_H);
    _button->setFont(fontManager.textFont(fontSize));

    mainLayout->addWidget(_button,0,Qt::AlignRight);
    mainLayout->addStretch();
}

void NIBPCalibrationMenu::_onBtnSlot()
{

    nibpRepairMenuManager.popup();
    nibpRepairMenuManager.init();
}
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
NIBPCalibrationMenu::~NIBPCalibrationMenu()
{
    ;
}
