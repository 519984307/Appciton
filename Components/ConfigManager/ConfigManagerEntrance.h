#pragma once
#include "SubMenu.h"
#include "PasswordMenuManage.h"



class IComboList;
class QStackedWidget;
class ConfigManagerEntrance : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static ConfigManagerEntrance &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ConfigManagerEntrance();
        }
        return *_selfObj;
    }
    static ConfigManagerEntrance *_selfObj;
public:
    ~ConfigManagerEntrance();
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
     * @brief ConfigManagerEntrance  构造方法
     */
    ConfigManagerEntrance();
    QStackedWidget      *_subMenu;           //子菜单
    PasswordMenuManage  *_passwordMenuManage;
};
#define configManagerEntrance (ConfigManagerEntrance::construction())
