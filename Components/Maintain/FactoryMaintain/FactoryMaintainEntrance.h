#pragma once
#include "SubMenu.h"
#include "PasswordMenuManage.h"



class IComboList;
class QStackedWidget;
class FactoryMaintainEntrance : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static FactoryMaintainEntrance &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new FactoryMaintainEntrance();
        }
        return *_selfObj;
    }
    static FactoryMaintainEntrance *_selfObj;
public:
    ~FactoryMaintainEntrance();
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
     * @brief FactoryMaintainEntrance  构造方法
     */
    FactoryMaintainEntrance();
    QStackedWidget      *_subMenu;           //子菜单
    PasswordMenuManage  *_passwordMenuManage;
};
#define factoryMaintainEntrance (FactoryMaintainEntrance::construction())
