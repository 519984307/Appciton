#pragma once
#include "SubMenu.h"
#include "LabelButton.h"

class IComboList;
class IButton;
class QLabel;
class NIBPManometer : public SubMenu
{
    Q_OBJECT

public:
    static NIBPManometer &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPManometer();
        }

        return *_selfObj;
    }
    static NIBPManometer *_selfObj;

    ~NIBPManometer();

    // 压力值。
    void setCuffPressure(int pressure);

    //模式应答
    void unPacket(bool flag);

    //初始化
    void init(void);

protected:
    virtual void layoutExec(void);
    virtual void focusFirstItem(void);

private:
    NIBPManometer();

    QLabel *_value;
};
#define nibpmanometer (NIBPManometer::construction())
