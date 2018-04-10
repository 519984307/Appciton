#include "ServiceEntrance.h"
#include "SPO2Param.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include "MenuManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "ServiceMenuManager.h"

ServiceEntrance *ServiceEntrance::_selfObj = NULL;

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ServiceEntrance::layoutExec(void)
{
    int submenuW = serviceMenuManager.getSubmenuWidth();
    int submenuH = serviceMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    _passwordMenuManage = new PasswordMenuManage("ServicePassword");
//    _passwordMenuManage->setPassword(SERVER_PASSWORD);
//    connect(_passwordMenuManage, SIGNAL(enterSignal()), this, SLOT(_showSolt()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_passwordMenuManage);

    mainLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ServiceEntrance::readyShow(void)
{
    _passwordMenuManage->clearPassword();
}

//void ServiceEntrance::_showSolt(void)
//{
////    menuManager.openMenuGroup(&serviceMenuManager);
//    menuManager.openWidget(&serviceMenu);
//}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ServiceEntrance::ServiceEntrance() : SubMenu(trs("ServiceMenu"))
{
    setDesc(trs("ServiceMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ServiceEntrance::~ServiceEntrance()
{

}
