#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditSpO2MenuPrivate;
class ConfigEditSpO2Menu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditSpO2Menu();
    ~ConfigEditSpO2Menu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);

private:
    QScopedPointer<ConfigEditSpO2MenuPrivate> d_ptr;
};
