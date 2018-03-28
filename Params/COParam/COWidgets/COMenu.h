#pragma once
#include "SubMenu.h"
#include "CODefine.h"

class IComboList;
class LabelButton;
class IButton;
class COMenu : public SubMenu
{
    Q_OBJECT

public:
    static COMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new COMenu();
        }
        return *_selfObj;
    }
    static COMenu *_selfObj;

    COMenu();
    ~COMenu();

protected:
    //布局
    virtual void layoutExec(void);

private slots:
    void _ductRatioReleased(void);
    void _inputModeSlot(int index);
    void _injectionTempReleased(void);
    void _injectionVolumnReleased(void);
    void _startReleased();

private:

    LabelButton *_ductRatio;            // CO系数(漂浮导管系数)
    IComboList *_inputMode;             // 注射液温度来源(Ti输入模式设置)
    LabelButton *_injectionTemp;        // 注射液温度
    LabelButton *_injectionVolumn;      // 注射液体积
    IComboList *_measureMode;           // 测量模式
    IButton *_start;                    // 开始测量

    COInstCtl _measureSta;              // 测量状态

};
#define coMenu (COMenu::construction())
