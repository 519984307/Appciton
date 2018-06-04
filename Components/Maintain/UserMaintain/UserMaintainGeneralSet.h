#pragma once
#include "SubMenu.h"

class LabelButton;
class IComboList;
class UserMaintainGeneralSet : public SubMenu
{
    Q_OBJECT
public:
    /**
     * @brief construction  初始化方法
     * @return
     */
    static UserMaintainGeneralSet &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new UserMaintainGeneralSet();
        }
        return *_selfObj;
    }
    static UserMaintainGeneralSet *_selfObj;
    ~UserMaintainGeneralSet();
protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  加载数据方法
     */
    virtual void readyShow(void);
private slots:
    /**
     * @brief _editMonitorName  监视名称改变方法
     */
    void _editMonitorName(void);
    /**
     * @brief _passwordReleased  密码改变方法
     */
    void _passwordReleased(void);
    /**
     * @brief _setChangeBedNumberRight 床号权限改变方法
     * @param index  选择序列号
     */
    void _setChangeBedNumberRight(int index);
    /**
     * @brief _editBedNumber 床号改变方法
     */
    void _editBedNumber(void);
    /**
     * @brief _languageSetup  语言设置改变方法
     * @param index  选择序列号
     */
    void _languageSetup(int index);
    /**
     * @brief _editDepartment  部门改变方法
     */
    void _editDepartment(void);
private:
    UserMaintainGeneralSet();
    LabelButton *_monitorName;             //监视名称
    LabelButton *_department;              //部门
    LabelButton *_bedNumber;               //床号
    LabelButton *_modifyPassword;          //更改用户维护密码
    IComboList  *_changeBedNumberRight;    //改变床号权限
    IComboList  *_language;                //语言
};
#define userMaintainGeneralSet (UserMaintainGeneralSet::construction())
