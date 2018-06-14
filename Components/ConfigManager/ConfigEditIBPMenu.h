#pragma once
#include "SubMenu.h"
#include "CODefine.h"
#include <QLabel>
#include "IBPDefine.h"

class IComboList;
class LabelButton;
class IButton;
class ConfigIBPMenu : public SubMenu
{
    Q_OBJECT

public:
    /**
     * @brief construction 初始化方法
     * @return
     */
    static ConfigIBPMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ConfigIBPMenu();
        }
        return *_selfObj;
    }
    static ConfigIBPMenu *_selfObj;
    /**
     * @brief ConfigIBPMenu  构造方法
     */
    ConfigIBPMenu();
    ~ConfigIBPMenu();

    // 显示校零校准提示信息
    void displayZeroRev(QString info);

    // 标名下拉菜单选中改变
    void comboListChangeIBP1(IBPPressureName);
    void comboListChangeIBP2(IBPPressureName);

protected:
    // Show之前的准备工作。
    virtual void readyShow(void);

    // Hide之前的清理工作。
    virtual void readyhide(void);

    //布局
    virtual void layoutExec(void);

private slots:
    void _speedSlot(int index);
    void _entitleSlot1(int index);
    void _entitleSlot2(int index);
    void _filterSlot(int index);
    void _sensitivitySlot(int index);
    void _zeroCalibReleased(void);
    void _calibrationReleased(void);
    void _alarmLbtnSlot(void);

private:
    IComboList *_entitle1;                  // 通道１标名设置。
    IComboList *_entitle2;                  // 通道２标名设置。
    IComboList *_speed;                     // 波形速度。
    IComboList *_filter;                    // 滤波设置。
    IComboList *_sensitivity;               // 灵敏度设置。
    LabelButton *_zeroCalib;                // 校零。
    LabelButton *_calibration;              // 校准
    QLabel *_zeroRev;                       // 校零校准回复信息
    LabelButton *_alarmLbtn;                //跳到报警项设置按钮
};
