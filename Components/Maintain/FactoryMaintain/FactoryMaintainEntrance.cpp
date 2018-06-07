#include "FactoryMaintainEntrance.h"
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

FactoryMaintainEntrance *FactoryMaintainEntrance::_selfObj = NULL;

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void FactoryMaintainEntrance::layoutExec(void)
{
    int submenuW = factoryMaintainManager.getSubmenuWidth();
    int submenuH = factoryMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    QString factoryMaintainPassword;
    systemConfig.getStrValue("General|FactoryMaintainPassword", factoryMaintainPassword);

    _passwordMenuManage = new PasswordMenuManage(trs("FactoryMaintainPassword"));
    _passwordMenuManage->setFactoryMaintainPassword(factoryMaintainPassword);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_passwordMenuManage);

    mainLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void FactoryMaintainEntrance::readyShow(void)
{
    QString factoryMaintainPassword;
    QString superPassword;
    systemConfig.getStrValue("General|SuperPassword", superPassword);
    systemConfig.getStrValue("General|FactoryMaintainPassword", factoryMaintainPassword);
    _passwordMenuManage->setSuperPassword(superPassword);
    _passwordMenuManage->setFactoryMaintainPassword(factoryMaintainPassword);
    _passwordMenuManage->clearPassword();
    _passwordMenuManage->setOkBtn('F');
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactoryMaintainEntrance::FactoryMaintainEntrance() : SubMenu(trs("FactoryMaintainSystem"))
{
    setDesc(trs("FactoryMaintainSystem"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactoryMaintainEntrance::~FactoryMaintainEntrance()
{

}
