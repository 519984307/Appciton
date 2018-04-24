#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditCO2MenuPrivate;
class ConfigEditCO2Menu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditCO2Menu();
    ~ConfigEditCO2Menu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);

private:
    QScopedPointer<ConfigEditCO2MenuPrivate> d_ptr;
};
