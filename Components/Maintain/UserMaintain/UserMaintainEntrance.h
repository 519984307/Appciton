#pragma once
#include "SubMenu.h"
#include "PasswordMenuManage.h"



class IComboList;
class QStackedWidget;
class UserMaintainEntrance : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
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
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  数据加载方法
     */
    virtual void readyShow(void);
private slots:
private:
    /**
     * @brief UserMaintainEntrance  构造方法
     */
    UserMaintainEntrance();
    QStackedWidget      *_subMenu;           //子菜单
    PasswordMenuManage  *_passwordMenuManage;
};
#define userMaintainEntrance (UserMaintainEntrance::construction())
