#include "UserMaintainEntrance.h"
#include "SPO2Param.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include "SupervisorMenuManager.h"
#include "UserMaintainManager.h"
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
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    QString userMaintainPassword;
    systemConfig.getStrValue("General|UserMaintainPassword", userMaintainPassword);

    _passwordMenuManage = new PasswordMenuManage(trs("UserMaintainPassword"));
    _passwordMenuManage->setUserMaintainPassword(userMaintainPassword);

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
    QString superPassword;
    systemConfig.getStrValue("General|SuperPassword", superPassword);
    systemConfig.getStrValue("General|UserMaintainPassword", userMaintainPassword);
    _passwordMenuManage->setSuperPassword(superPassword);
    _passwordMenuManage->setUserMaintainPassword(userMaintainPassword);
    _passwordMenuManage->clearPassword();
    _passwordMenuManage->setOkBtn('U');
}

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
