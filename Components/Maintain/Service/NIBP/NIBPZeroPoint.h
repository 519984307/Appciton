#pragma once
#include "SubMenu.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include "IComboList.h"
#include "LabelButton.h"

class NIBPZeroPoint : public SubMenu
{
    Q_OBJECT

public:
    static NIBPZeroPoint &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPZeroPoint();
        }

        return *_selfObj;
    }
    static NIBPZeroPoint *_selfObj;

    ~NIBPZeroPoint();

public:
    // 压力计压力。
    void setCuffPressure(int pressure);

    //进入模式应答
    void unPacket(bool flag);

    //压力值
    void startSwitch(bool flag);

    //初始化
    void init(void);

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem(void);

private slots:
    void _btnReleased(void);           //关闭气阀、校零按钮信号槽

private:
    NIBPZeroPoint();

    QLabel *_value;
    LButtonEx *_startSwitchBtn;        //关闭气阀、校零按钮

    void _startReleased(void);         //关闭气阀指令
    void _zeroReleased(void);          //校零指令

    bool _startSwitch;                 //关闭气阀、校零按钮切换标志
    bool _zeroPointFlag;               //进入模式标记
};
#define nibpzeropoint (NIBPZeroPoint::construction())
