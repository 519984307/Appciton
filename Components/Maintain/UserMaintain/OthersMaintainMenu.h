#pragma once
#include "SubMenu.h"

class LabelButton;
class IComboList;
class OthersMaintainMenu : public SubMenu
{
    Q_OBJECT

public:
    static OthersMaintainMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new OthersMaintainMenu();
        }

        return *_selfObj;
    }
    static OthersMaintainMenu *_selfObj;

    ~OthersMaintainMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _editCombolistSlot(int);

private:
    OthersMaintainMenu();

    IComboList  *_waveLine;                //波形线条
    IComboList  *_ecgStandard;             //ECG标准
    IComboList  *_frequencyNotch;          //工频陷波
    IComboList  *_paraSwitchPrem;          //参数开关权限
    IComboList  *_confImpactItemSettings;  //受配置影响项设置
    IComboList  *_nurseCallSetting;        //护士呼叫设置
    IComboList  *_signalType;              //信号类型
    IComboList  *_triggerMode;             //触发方式
    IComboList  *_alarmLevel;              //报警级别
    IComboList  *_alarmType;               //报警类型

};
#define othersMaintainMenu (OthersMaintainMenu::construction())
