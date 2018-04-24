#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditCOMenuPrivate;
class ConfigEditCOMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditCOMenu();
    ~ConfigEditCOMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);
    void onButtonClicked();
private:
    QScopedPointer<ConfigEditCOMenuPrivate> d_ptr;
};
