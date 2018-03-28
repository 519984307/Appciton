#pragma once
#include "SubMenu.h"
#include "PasswordMenuManage.h"

#define SERVER_PASSWORD ("11111111")          //服务密码

class IComboList;
class QStackedWidget;
class ServiceEntrance : public SubMenu
{
    Q_OBJECT

public:
    static ServiceEntrance &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ServiceEntrance();
        }
        return *_selfObj;
    }
    static ServiceEntrance *_selfObj;

public:
    ~ServiceEntrance();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _showSolt(void);

private:
    ServiceEntrance();

    QStackedWidget *_subMenu;           //子菜单

    PasswordMenuManage *_passwordMenuManage;
};
#define serviceEntrance (ServiceEntrance::construction())
