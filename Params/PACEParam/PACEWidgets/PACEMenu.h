#pragma once
#include "SubMenu.h"

class IComboList;
class LabelButton;
class LButton;
class PACEMenu : public SubMenu
{
    Q_OBJECT

public:
    static PACEMenu &Construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new PACEMenu();
        }
        return *_selfObj;
    }
    static PACEMenu *_selfObj;

public:
    ~PACEMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _rateSlot(int index);
    void _currentSlot(int index);
    void _modeSlot(int index);

private:
    PACEMenu();
    void _loadOptions(void);

    IComboList *_rate;             // 输出频率。
    IComboList *_current;          // 输出电流。
    IComboList *_mode;             // 工作模式。

    LButton *_returnBtn;           // 返回
};
#define paceMenu (PACEMenu::Construction())
