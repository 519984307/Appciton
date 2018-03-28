#pragma once

#include <QScopedPointer>
#include "SubMenu.h"

class SupervisorMailMenuPrivate;
class SupervisorMailMenu: public SubMenu
{
    Q_OBJECT
public:
    static SupervisorMailMenu &construction();
    ~SupervisorMailMenu();
    void focusFirstItem();
    bool eventFilter(QObject *obj, QEvent *ev);
protected:
    void layoutExec();
    void readyShow();
    const QScopedPointer<SupervisorMailMenuPrivate> d_ptr;
private:
    SupervisorMailMenu();
    Q_DECLARE_PRIVATE(SupervisorMailMenu)
    Q_DISABLE_COPY(SupervisorMailMenu)

    Q_PRIVATE_SLOT(d_func(), void onListExit(bool))
    Q_PRIVATE_SLOT(d_func(), void onRecipientItemClick())
    Q_PRIVATE_SLOT(d_func(), void onConnectionSecuritySwitch(int))
    Q_PRIVATE_SLOT(d_func(), void onBtnClick())
    Q_PRIVATE_SLOT(d_func(), void onEditText())
};

#define supervisorMailMenu (SupervisorMailMenu::construction())
