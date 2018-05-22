#pragma once

#include <QScopedPointer>
#include "SubMenu.h"

class WifiMaintainMenuPrivate;
class WifiMaintainMenu: public SubMenu
{
    Q_OBJECT
public:
    static WifiMaintainMenu &construction(void);
    void focusFirstItem();
    bool eventFilter(QObject *obj, QEvent *ev);

protected:
    void layoutExec();
    void readyShow();
    const QScopedPointer<WifiMaintainMenuPrivate> d_ptr;

private:
    WifiMaintainMenu();
    Q_DECLARE_PRIVATE(WifiMaintainMenu)
    Q_DISABLE_COPY(WifiMaintainMenu)

    Q_PRIVATE_SLOT(d_func(), void onSwitch(int))
    Q_PRIVATE_SLOT(d_func(), void onListExit(bool))
    Q_PRIVATE_SLOT(d_func(), void onProfileItemClick())
    Q_PRIVATE_SLOT(d_func(), void onBtnClick())
};

#define wifiMaintainMenu (WifiMaintainMenu::construction())
