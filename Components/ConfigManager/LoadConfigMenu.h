#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class LoadConfigMenuPrivate;
class LoadConfigMenu : public SubMenu
{
    Q_OBJECT
public:
    static LoadConfigMenu &construction()
    {
        if(selfObj == NULL)
        {
            selfObj = new LoadConfigMenu();
        }
        return *selfObj;
    }
    static LoadConfigMenu *selfObj;

    LoadConfigMenu();
    ~LoadConfigMenu();

    bool eventFilter(QObject *obj, QEvent *ev);

signals:
    void configUpdated(void);
protected:
    virtual void layoutExec(void);
    virtual void readyShow(void);

private slots:
    void onExitList(bool);
    void onConfigClick();
    void onBtnClick();

private:
    QScopedPointer<LoadConfigMenuPrivate> d_ptr;
};
#define loadConfigMenu (LoadConfigMenu::construction())
