#pragma once
#include "SubMenu.h"
#include "PasswordMenuManage.h"



class IComboList;
class QStackedWidget;
class UserMaintainEntrance : public SubMenu
{
    Q_OBJECT

public:
    static UserMaintainEntrance &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new UserMaintainEntrance();
        }
        return *_selfObj;
    }
    static UserMaintainEntrance *_selfObj;

public:
    ~UserMaintainEntrance();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
//    void _showSolt(void);

private:
    UserMaintainEntrance();

    QStackedWidget *_subMenu;           //子菜单

    PasswordMenuManage *_passwordMenuManage;
};
#define userMaintainEntrance (UserMaintainEntrance::construction())
