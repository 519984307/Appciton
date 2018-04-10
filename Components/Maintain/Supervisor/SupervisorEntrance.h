#pragma once
#include "SubMenu.h"
#include "PasswordMenuManage.h"


class IComboList;
class QStackedWidget;
class SupervisorEntrance : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorEntrance &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SupervisorEntrance();
        }
        return *_selfObj;
    }
    static SupervisorEntrance *_selfObj;

public:
    ~SupervisorEntrance();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
//    void _showSolt(void);

private:
    SupervisorEntrance();

    QStackedWidget *_subMenu;           //子菜单

    PasswordMenuManage *_passwordMenuManage;
};
#define supervisorEntrance (SupervisorEntrance::construction())
