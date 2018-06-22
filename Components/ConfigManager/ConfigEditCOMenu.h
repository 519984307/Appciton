#pragma once
#include "SubMenu.h"
#include "CODefine.h"

class IComboList;
class LabelButton;
class IButton;
class ConfigCOMenu : public SubMenu
{
    Q_OBJECT

public:
    /**
     * @brief construction 初始化方法
     * @return
     */
    static ConfigCOMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ConfigCOMenu();
        }
        return *_selfObj;
    }
    static ConfigCOMenu *_selfObj;
    /**
     * @brief ConfigCOMenu  构造方法
     */
    ConfigCOMenu();
    ~ConfigCOMenu();

protected:
    /**
     * @brief layoutExec  布局方法
     */
    virtual void layoutExec(void);

    virtual void readyShow(void);

private slots:
    /**
     * @brief _ductRatioReleased  CO系数改变方法
     */
    void _ductRatioReleased(void);
    /**
     * @brief _inputModeSlot  Ti输入模式改变方法
     * @param index  选择序列号
     */
    void _inputModeSlot(int index);
    /**
     * @brief _injectionTempReleased 注射液温度改变方法
     */
    void _injectionTempReleased(void);
    /**
     * @brief _injectionVolumnReleased 注射液体积改变方法
     */
    void _injectionVolumnReleased(void);
    /**
     * @brief _alarmLbtnSlot 报警设置槽函数
     */
    void _alarmLbtnSlot(void);

private:
    LabelButton  *_ductRatio;             // CO系数(漂浮导管系数)
    IComboList   *_inputMode;             // 注射液温度来源(Ti输入模式设置)
    LabelButton  *_injectionTemp;         // 注射液温度
    LabelButton  *_injectionVolumn;       // 注射液体积
    IComboList   *_measureMode;           // 测量模式
    LabelButton *_alarmLbtn;            //跳到报警项设置按钮
};
