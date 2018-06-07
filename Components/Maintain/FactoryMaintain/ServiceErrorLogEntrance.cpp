#include "ServiceErrorLogEntrance.h"
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
#include "ServiceErrorLogMenu.h"

ServiceErrorLogEntrance *ServiceErrorLogEntrance::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceErrorLogEntrance::ServiceErrorLogEntrance() : SubMenu(trs("ServiceErrorLogEntrance")),
                                             _button(NULL)
{

    _button = new LabelButton(trs(""));

    _button->button->setText(QString("%1%2").arg(trs("ServiceErrorLogEntrance")).arg(">>"));
    connect(_button->button, SIGNAL(realReleased()), this, SLOT(_onBtnSlot()));

    setDesc(trs("ServiceErrorLogEntranceDesc"));
    startLayout();
}


void ServiceErrorLogEntrance::readyShow()
{

}

void ServiceErrorLogEntrance::layoutExec()
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

void ServiceErrorLogEntrance::_onBtnSlot()
{
    serviceErrorLogMenu.popup();
    serviceErrorLogMenu.init();
}
/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceErrorLogEntrance::~ServiceErrorLogEntrance()
{
    ;
}
