#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditGeneralMenuPrivate;
class ConfigEditGeneralMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditGeneralMenu();
    ~ConfigEditGeneralMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void editConfigName();
    void disableWidgets(void);

private:
    QScopedPointer<ConfigEditGeneralMenuPrivate> d_ptr;
};
