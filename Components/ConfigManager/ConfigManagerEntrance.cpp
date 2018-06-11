#include "ConfigManagerEntrance.h"
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

ConfigManagerEntrance *ConfigManagerEntrance::_selfObj = NULL;

/**************************************************************************************************
 * 执行布局。
 *************************************************************************************************/
void ConfigManagerEntrance::layoutExec(void)
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    QString normalPassword;
    systemConfig.getStrValue("General|ConfigManagerPassword", normalPassword);

    _passwordMenuManage = new PasswordMenuManage(trs("ConfigManagerPassword"));
    _passwordMenuManage->setPassword(normalPassword);

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(_passwordMenuManage);

    mainLayout->addLayout(vLayout);
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void ConfigManagerEntrance::readyShow(void)
{
    QString normalPassword;
    systemConfig.getStrValue("General|ConfigManagerPassword", normalPassword);
    _passwordMenuManage->setPassword(normalPassword);
    _passwordMenuManage->clearPassword();
    _passwordMenuManage->setOkBtn('G');
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
ConfigManagerEntrance::ConfigManagerEntrance() : SubMenu(trs("ConfigManager"))
{
    setDesc(trs("ConfigManagerDesc"));
    startLayout();
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
ConfigManagerEntrance::~ConfigManagerEntrance()
{

}
