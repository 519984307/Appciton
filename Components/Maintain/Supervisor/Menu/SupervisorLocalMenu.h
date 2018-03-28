#pragma once
#include "SubMenu.h"

class IComboList;

//显示设置配置
class SupervisorLocalMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorLocalMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorLocalMenu();
        }

        return *_selfObj;
    }
    static SupervisorLocalMenu *_selfObj;

    ~SupervisorLocalMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _languageChangeSlot(int index);
    void _nibpUnitChangeSlot(int index);
    void _tempUnitChangeSlot(int index);
    void _weightUnitChangeSlot(int index);
    void _heightUnitChangeSlot(int index);
    void _co2UnitSlot(int index);

private:
    SupervisorLocalMenu();

    IComboList *_language;              // 语言
    IComboList *_nibpUnit;              // NIBP单位。
    IComboList *_co2Unit;               // CO2单位
    IComboList *_tempUnit;              // 体温单位。
    IComboList *_weightUnit;            // 体重单位。
    IComboList *_heightUnit;            // 身高单位。

};
#define supervisorLocalMenu (SupervisorLocalMenu::construction())

