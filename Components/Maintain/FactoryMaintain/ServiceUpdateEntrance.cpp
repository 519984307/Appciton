#include "ServiceUpdateEntrance.h"
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
#include "ServiceUpgrade.h"

ServiceUpdateEntrance  *ServiceUpdateEntrance ::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceUpdateEntrance ::ServiceUpdateEntrance () : SubMenu(trs("ServiceUpdateEntrance ")),
                                             _button(NULL)
{

    _button = new LabelButton(trs(""));

    _button->button->setText(trs("ServiceUpdateEntrance >>"));
    connect(_button->button, SIGNAL(realReleased()), this, SLOT(_onBtnSlot()));

    setDesc(trs("ServiceUpdateEntrance Desc"));
    startLayout();
}


void ServiceUpdateEntrance ::readyShow()
{

}

void ServiceUpdateEntrance ::layoutExec()
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

void ServiceUpdateEntrance ::_onBtnSlot()
{
    serviceUpgrade.popup();
    serviceUpgrade.init();
}
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceUpdateEntrance ::~ServiceUpdateEntrance ()
{
    ;
}
