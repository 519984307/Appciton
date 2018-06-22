#pragma once
#include "SubMenu.h"

class IComboList;
class LabelButton;
class ISpinBox;
class ConfigCO2Menu : public SubMenu
{
    Q_OBJECT

public:
    /**
     * @brief ConfigCO2Menu 构造方法
     */
    ConfigCO2Menu();
    /**
     * @brief construction 初始化方法
     * @return
     */
    static ConfigCO2Menu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ConfigCO2Menu();
        }
        return *_selfObj;
    }
    static ConfigCO2Menu *_selfObj;

public:
    ~ConfigCO2Menu();

protected:
    /**
     * @brief layoutExec  菜单布局方法
     */
    virtual void layoutExec(void);
    /**
     * @brief readyShow  装载数据方法
     */
    virtual void readyShow(void);

private slots:
    /**
     * @brief _speedSlot 编辑速度改变触发方法
     * @param index 速度条目序列号
     */
    void _speedSlot(int index);
    /**
     * @brief _fico2DisplaySlot FiCO2显示开关改变触发方法
     * @param index 开关选择项值
     */
    void _fico2DisplaySlot(int index);
    /**
     * @brief _o2CompenReleased O2补偿改变触发方法
     * @param strValue 补偿值的字符串形式
     */
    void _o2CompenReleased(QString strValue);
    /**
     * @brief _n2oCompenReleased NO2补偿改变触发方法
     * @param strValue 补偿值的字符串形式
     */
    void _n2oCompenReleased(QString strValue);
    /**
     * @brief _alarmLbtnSlot 报警设置槽函数
     */
    void _alarmLbtnSlot(void);

private:
    /**
     * @brief _loadOptions 装载数据
     */
    void _loadOptions(void);

    IComboList  *_speed;                  // 波形速度。
    IComboList  *_fico2Display;           // FiCO2显示开关。
    ISpinBox    *_o2Compen;               // 氧气补偿。
    ISpinBox    *_n2oCompen;              // 笑气补偿。
    LabelButton *_alarmLbtn;            //跳到报警项设置按钮
};
