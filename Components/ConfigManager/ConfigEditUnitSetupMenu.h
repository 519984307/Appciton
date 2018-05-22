#pragma once
#include "SubMenu.h"

class IComboList;
class LabelButton;
class ISpinBox;
class ConfigEditUnitSetupMenu : public SubMenu
{
    Q_OBJECT

public:
    ConfigEditUnitSetupMenu();
    static ConfigEditUnitSetupMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ConfigEditUnitSetupMenu();
        }
        return *_selfObj;
    }
    static ConfigEditUnitSetupMenu *_selfObj;

public:
    ~ConfigEditUnitSetupMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
//    void _apneaTimeSlot(int index);
    void _hightUnitSlot(int index);
    void _weightUnitSlot(int index);
    void _stUnitSlot(int index);
    void _pressureUnitSlot(int index);
    void _temperatureUnitSlot(int index);
    void _cvpUnitSlot(int index);
    void _icpUnitSlot(int index);
    void _co2UnitSlot(int index);

private:
    //CO2Menu();
    void _loadOptions(void);

    IComboList *_hightUnit;              // 身高单位。
    IComboList *_weightUnit;             // 体重单位。
    IComboList *_stUnit;                 // ST单位。
    IComboList *_pressureUnit;           // 压力单位。
    IComboList *_temperatureUnit;        // 温度单位。
    IComboList *_cvpUnit;                // CVP单位。
    IComboList *_icpUnit;                // ICP单位。
    IComboList *_co2Unit;                // CO2单位。
};
