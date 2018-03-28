#pragma once

#include <QScopedPointer>
#include "SubMenu.h"

class SupervisorWifiMenuPrivate;
class SupervisorWifiMenu: public SubMenu
{
    Q_OBJECT
public:
    static SupervisorWifiMenu &construction(void);
    void focusFirstItem();
    bool eventFilter(QObject *obj, QEvent *ev);

protected:
    void layoutExec();
    void readyShow();
    const QScopedPointer<SupervisorWifiMenuPrivate> d_ptr;

private:
    SupervisorWifiMenu();
    Q_DECLARE_PRIVATE(SupervisorWifiMenu)
    Q_DISABLE_COPY(SupervisorWifiMenu)

    Q_PRIVATE_SLOT(d_func(), void onSwitch(int))
    Q_PRIVATE_SLOT(d_func(), void onListExit(bool))
    Q_PRIVATE_SLOT(d_func(), void onProfileItemClick())
    Q_PRIVATE_SLOT(d_func(), void onBtnClick())
};

#define supervisorWifiMenu (SupervisorWifiMenu::construction())
