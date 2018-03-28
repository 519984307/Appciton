#pragma once
#include "SubMenu.h"

class IComboList;

//显示设置配置
class SupervisorDisplayMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorDisplayMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorDisplayMenu();
        }

        return *_selfObj;
    }
    static SupervisorDisplayMenu *_selfObj;

    ~SupervisorDisplayMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _ecgColorChangeSlot(int index);
    void _spo2ColorChangeSlot(int index);
    void _nibpColorChangeSlot(int index);
    void _co2ColorChangeSlot(int index);
    void _respColorChangeSlot(int index);
    void _tempColorChangeSlot(int index);

private:
    SupervisorDisplayMenu();

    QStringList _colorList;             // 所有颜色
    IComboList *_ecgColor;              // ECG菜单颜色
    IComboList *_spo2Color;             // SPO2菜单颜色
    IComboList *_nibpColor;             // NIBP菜单颜色
    IComboList *_co2Color;              // CO2菜单颜色
    IComboList *_respColor;             // RESP菜单颜色
    IComboList *_tempColor;             // TEMP菜单颜色
};
#define supervisorDisplayMenu (SupervisorDisplayMenu::construction())
