#pragma once
#include "SubMenu.h"

class LabelButton;
class IComboList;
class UserMaintainGeneralSet : public SubMenu
{
    Q_OBJECT

public:
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
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _editMonitorName(void);
    void _passwordReleased(void);
    void _setChangeBedNumberRight(int);
    void _editBedNumber(void);
    void _languageSetup(int);
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
