#pragma once
#include "SubMenu.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include "IComboList.h"
#include "LabelButton.h"

class ISpinBox;
class NIBPPressureControl : public SubMenu
{
    Q_OBJECT

public:
    static NIBPPressureControl &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPPressureControl();
        }

        return *_selfObj;
    }
    static NIBPPressureControl *_selfObj;

    ~NIBPPressureControl();

public:
    // 压力计压力。
    void setCuffPressure(int pressure);

    //进入模式应答
    void unPacket(bool flag);

    //充气/放气按钮切换
    void btnSwitch(bool inflate);

    //初始化
    void init(void);

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem(void);

private slots:
    void _InflateBtnReleased(void);            //充气、放气控制按钮信号
    void _pressureChange(QString);             //充气压力值
    void _patientInflate(int index);           //病人类型信号
//    void Inflate_Btn(void);

private:
    NIBPPressureControl();

    ISpinBox *_chargePressure;          // 设定充气压力值
    IComboList *_patientType;           // 病人类型
    LButtonEx *_InflateBtn;             //充气、放气控制按钮
    QLabel *_value;

    void _inflateReleased(void);        //充气指令
    void _deflateReleased(void);        //放气指令
    void _changeReleased(void);         //病人类型，充气值改变指令

    int _inflatePressure;               //充气压力值
    int _patientVaulue;                  //病人类型
    bool _inflateSwitch;                 //充气、放气标志
    bool _pressureControlFlag;          //进入模式标志
};
#define nibppressurecontrol (NIBPPressureControl::construction())
