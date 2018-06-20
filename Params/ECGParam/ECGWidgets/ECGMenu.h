#pragma once
#include "SubMenu.h"


class IComboList;
class LabelButton;
class ECGMenu : public SubMenu
{
    Q_OBJECT

public:
    static ECGMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new ECGMenu();
        }
        return *_selfObj;
    }
    static ECGMenu *_selfObj;
    ~ECGMenu();

public:
    // 刷新各个选项。
    void refresh(void);

    //设置ECG导联模式
    void setECGLeadMode(int leadMode);

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _loadOptionsInSlot(void);
    void _leadModeSlot(int index);
    void _filterModeSlot(int index);
    void _notchSlot(int index);
    void _paceMakerSlot(int index);
    void _12LpaceMakerSlot(int index);
    void _sweepSpeedSlot(int index);
    void _qrsVolumSlot(int index);
    void _onConfigUpdated();

private:
    ECGMenu();

    // 添加其他选项。
    void _addFixedOptions(void);

    // 载入可选项的值。
    void _loadOptions(void);

    IComboList *_leadMode;              // 导联模式。
    IComboList *_filterMode;            // fitler mode
    IComboList *_notch;                 // 滤波
    IComboList *_sweepSpeed;            // 波形速度。
    IComboList *_pacemaker;             // 起搏
    IComboList *_12Lpacemaker;          // 12导起搏
    IComboList *_qrsTone;               // 心跳音量。
};
 #define ecgMenu (ECGMenu::construction())
