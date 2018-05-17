#include "UserMaintainEntrance.h"
#include "SPO2Param.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include "SupervisorMenuManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "IConfig.h"
#include "MenuManager.h"

UserMaintainEntrance *UserMaintainEntrance::_selfObj = NULL;

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void UserMaintainEntrance::layoutExec(void)
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    QString userMaintainPassword;
    currentConfig.getStrValue("General|UserMaintainPassword", userMaintainPassword);

    _passwordMenuManage = new PasswordMenuManage(trs("UserMaintainPassword"));
    _passwordMenuManage->setUserMaintainPassword(userMaintainPassword);
//    connect(_passwordMenuManage, SIGNAL(enterSignal()), this, SLOT(_showSolt()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_passwordMenuManage);

    mainLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void UserMaintainEntrance::readyShow(void)
{
    QString userMaintainPassword;
    currentConfig.getStrValue("General|UserMaintainPassword", userMaintainPassword);
    _passwordMenuManage->setUserMaintainPassword(userMaintainPassword);
    _passwordMenuManage->clearPassword();
}

//void UserMaintainEntrance::_showSolt()
//{
//    menuManager.openMenuGroup(&supervisorMenuManager);
//}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
UserMaintainEntrance::UserMaintainEntrance() : SubMenu(trs("UserMaintainSystem"))
{
    setDesc(trs("UserMaintainSystem"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
UserMaintainEntrance::~UserMaintainEntrance()
{

}
