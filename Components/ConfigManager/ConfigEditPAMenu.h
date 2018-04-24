#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditPAMenuPrivate;
class ConfigEditPAMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditPAMenu();
    ~ConfigEditPAMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);
    void onButtonClicked();
private:
    QScopedPointer<ConfigEditPAMenuPrivate> d_ptr;
};
