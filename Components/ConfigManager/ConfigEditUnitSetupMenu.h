#pragma once
#include "SubMenu.h"

class IComboList;
class LabelButton;
class ISpinBox;
class ConfigEditUnitSetupMenu : public SubMenu
{
    Q_OBJECT

public:
    /**
     * @brief ConfigEditUnitSetupMenu  构造方法
     */
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
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  装载数据方法
     */
    virtual void readyShow(void);

private slots:
    /**
     * @brief _hightUnitSlot  身高单位改变方法
     * @param index  选择序列号
     */
    void _hightUnitSlot(int index);
    /**
     * @brief _weightUnitSlot  体重单位改变方法
     * @param index  选择序列号
     */
    void _weightUnitSlot(int index);
    /**
     * @brief _stUnitSlot  ST单位改变方法
     * @param index  选择序列号
     */
    void _stUnitSlot(int index);
    /**
     * @brief _pressureUnitSlot  压力单位改变方法
     * @param index  选择序列号
     */
    void _pressureUnitSlot(int index);
    /**
     * @brief _temperatureUnitSlot  温度单位改变方法
     * @param index  选择序列号
     */
    void _temperatureUnitSlot(int index);
    /**
     * @brief _cvpUnitSlot  CVP单位改变方法
     * @param index  选择序列号
     */
    void _cvpUnitSlot(int index);
    /**
     * @brief _icpUnitSlot  ICP单位改变方法
     * @param index  选择序列号
     */
    void _icpUnitSlot(int index);
    /**
     * @brief _co2UnitSlot  CO2单位改变方法
     * @param index  选择序列号
     */
    void _co2UnitSlot(int index);

private:
    /**
     * @brief _loadOptions  装载数据方法
     */
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
