#pragma once
#include "SubMenu.h"
#include "LabelButton.h"

class IComboList;
class NIBPMenu : public SubMenu
{
    Q_OBJECT

public:
    static NIBPMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new NIBPMenu();
        }
        return *_selfObj;
    }
    static NIBPMenu *_selfObj;

public:
    ~NIBPMenu();

    void statBtnShow(void);

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _measureModeSlot(int index);
    void _autoIntervalSlot(int index);
    void _initialCuffChangeSlot(int index);
    void _statBtnSlot(void);
    void _alarmLbtnSlot(void);

private:
    NIBPMenu();

    // 载入可选项的值。
    void _loadOptions(void);

    IComboList *_intelligentInflate;         // 智能充气。
    IComboList *_measureMode;                // 测量模式。
    IComboList *_autoInterval;               // 自动模式下的时间间隔。
    IComboList *_initialCuff;                // 预充气值。
    LButtonEx *_statBtn;                     //STAT启动停止按钮

    LabelButton *_alarmLbtn;                 //跳到报警项设置按钮
};
#define nibpMenu (NIBPMenu::construction())
