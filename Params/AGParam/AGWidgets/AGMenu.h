#pragma once
#include "SubMenu.h"
#include "AGDefine.h"

class IComboList;
class AGMenu : public SubMenu
{
    Q_OBJECT

public:
    static AGMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new AGMenu();
        }
        return *_selfObj;
    }
    static AGMenu *_selfObj;

public:
    ~AGMenu();

protected:
    // Show之前的准备工作。
    virtual void readyShow(void);

    // Hide之前的清理工作。
    virtual void readyhide(void);

    //布局
    virtual void layoutExec(void);

private slots:
    void _speedSlot(int index);

private:
    AGMenu();

    IComboList *_speed;             // 波形速度。

};
#define agMenu (AGMenu::construction())
