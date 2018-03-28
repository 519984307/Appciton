#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditAlarmLimitMenuPrivate;
class ConfigEditAlarmLimitMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditAlarmLimitMenu();
    ~ConfigEditAlarmLimitMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void _limitChanged(QString, int);
    void _comboListIndexChanged(int id, int index);

private:
    QScopedPointer<ConfigEditAlarmLimitMenuPrivate> d_ptr;
};
