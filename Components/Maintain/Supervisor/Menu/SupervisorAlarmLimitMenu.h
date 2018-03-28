#pragma once
#include "SubMenu.h"

#include "AlarmLimitMenu.h"

class IComboList;
class LButton;
class SetItem;
class SupervisorAlarmLimitMenu : public SubMenu
{
    Q_OBJECT

public:
    static SupervisorAlarmLimitMenu &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new SupervisorAlarmLimitMenu();
        }
        return *_selfObj;
    }
    static SupervisorAlarmLimitMenu *_selfObj;

public:
    ~SupervisorAlarmLimitMenu();

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void _limitChange(QString, int);
    void _comboListIndexChanged(int id, int index);

private:
    SupervisorAlarmLimitMenu();

    // 载入可选项的值。
    void _loadOptions(void);

    QList<SetItem*> _itemList;
};
#define supervisorAlarmLimitMenu (SupervisorAlarmLimitMenu::construction())

