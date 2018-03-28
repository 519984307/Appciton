#include "SupervisorEntrance.h"
#include "SPO2Param.h"
#include "IComboList.h"
#include "LanguageManager.h"
#include "SupervisorMenuManager.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "IConfig.h"
#include "MenuManager.h"

SupervisorEntrance *SupervisorEntrance::_selfObj = NULL;

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void SupervisorEntrance::layoutExec(void)
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    QString _normalPassword;
    superConfig.getStrValue("General|Password", _normalPassword);

    _passwordMenuManage = new PasswordMenuManage("SupervisorPassword");
    _passwordMenuManage->setPassword(_normalPassword);
    connect(_passwordMenuManage, SIGNAL(enterSignal()), this, SLOT(_showSolt()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_passwordMenuManage);

    mainLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SupervisorEntrance::readyShow(void)
{
    QString _normalPassword;
    superConfig.getStrValue("General|Password", _normalPassword);
    _passwordMenuManage->setPassword(_normalPassword);
    _passwordMenuManage->clearPassword();
}

void SupervisorEntrance::_showSolt()
{
    menuManager.openMenuGroup(&supervisorMenuManager);
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorEntrance::SupervisorEntrance() : SubMenu(trs("SupervisorMenu"))
{
    setDesc(trs("SupervisorMenuDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SupervisorEntrance::~SupervisorEntrance()
{

}
