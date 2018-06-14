#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditEcgMenuPrivate;
class ConfigEditEcgMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditEcgMenu();
    ~ConfigEditEcgMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);
    void _alarmLbtnSlot(void);

private:
    QScopedPointer<ConfigEditEcgMenuPrivate> d_ptr;
};
