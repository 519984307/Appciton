#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class UserConfigEditMenuPrivate;
class UserConfigEditMenu : public SubMenu
{
    Q_OBJECT
public:
    UserConfigEditMenu();
    ~UserConfigEditMenu();

    bool eventFilter(QObject *obj, QEvent *ev);

protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void onExitList(bool);
    void onConfigClick();
    void onBtnClick();
    void onEditFinished();

private:
    QScopedPointer<UserConfigEditMenuPrivate> d_ptr;
};
