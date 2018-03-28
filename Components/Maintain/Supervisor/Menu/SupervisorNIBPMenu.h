#pragma once
#include "SubMenu.h"

class IComboList;

//NIBP配置菜单
class SupervisorNIBPMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorNIBPMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorNIBPMenu();
        }

        return *_selfObj;
    }
    static SupervisorNIBPMenu *_selfObj;

    ~SupervisorNIBPMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _measureModeChangeSlot(int index);
    void _autoIntervalSlot(int index);
    void _startOnMeasureFailChangeSlot(int index);
    void _startOnSTATEnabledChangeSlot(int index);
    void _adultInitialCuffChangeSlot(int index);
    void _prediatricInitialCuffChangeSlot(int index);
    void _neonatalInitialCuffChangeSlot(int index);

private:
    SupervisorNIBPMenu();

    // 载入可选项的值。
    void _loadOptions(void);

    IComboList *_measureMode;           // HR报警时触发NIBP测量
    IComboList *_autoInterval;          //自动测量时间
    IComboList *_startOnMeasureFail;    // 测量失败时增加额外的NIBP测量
    IComboList *_startOnSTATEnabled;    // STAT使能
    IComboList *_adultInitialCuff;      // 成人初始袖袋压
    IComboList *_prediatricInitialCuff; // 小儿初始袖袋压
    IComboList *_neonatalInitialCuff;   // 新生儿初始袖袋压
};
#define supervisorNIBPMenu (SupervisorNIBPMenu::construction())
