#pragma once
#include "SubMenu.h"

class IComboList;
class ISpinBox;

//时间配置
class SupervisorTimeMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorTimeMenu &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new SupervisorTimeMenu();
        }

        return *_selfObj;
    }
    static SupervisorTimeMenu *_selfObj;

    ~SupervisorTimeMenu();
    void setSysTimeTo1970_01_01_00();         //设置系统时间到1970/01/01 00:00:00。
    void setSysTimeTo2037_12_27_20();         //设置系统时间到2037/12/27 20:00:00。

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _yearChangeSlot(QString);
    void _monthChangeSlot(QString);
    void _timeChangeSlot(QString);
    void _dateFormatSlot(int index);
    void _timeFormatSlot(int index);
    void _displaySecSlot(int index);

private:
    SupervisorTimeMenu();
    int _getMaxDay(int year, int month);
    int _getMaxDay(QString year, QString month);
    void _setSysTime();

    ISpinBox *_year;              // 年
    ISpinBox *_month;             // 月
    ISpinBox *_day;               // 日
    ISpinBox *_hour;              // 时
    ISpinBox *_minute;            // 分
    ISpinBox *_second;            // 秒
    IComboList *_dateFormat;         // 日期格式
    IComboList *_timeFormat;         // 时间格式
    IComboList *_displaySec;         // 是否显示秒
};
#define supervisorTimeMenu (SupervisorTimeMenu::construction())
