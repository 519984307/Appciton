#pragma once
#include "SubMenu.h"

class IComboList;
class RESPMenu : public SubMenu
{
    Q_OBJECT

public:
    static RESPMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new RESPMenu();
        }
        return *_selfObj;
    }
    static RESPMenu *_selfObj;

public:
    ~RESPMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _sweepSpeedSlot(int index);
//    void _apneaTimeSlot(int index);
    void _zoomSlot(int index);
    void _calcLeadChange(int index);
    void _monitorSlot(int index);

private:
    RESPMenu();

    // 载入可选项的值。
    void _loadOptions(void);

    IComboList *_sweepSpeed;          // 波形速度。
//    IComboList *_apneaTime;           // 窒息时间。
//    IComboList *_zoom;                // 波形放大倍数。
//    IComboList *_calcLead;            // 呼吸导联
    IComboList *_monitor;             // 呼吸测量功能。
};
#define respMenu (RESPMenu::construction())
