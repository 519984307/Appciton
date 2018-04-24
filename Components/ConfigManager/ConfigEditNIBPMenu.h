#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditNIBPMenuPrivate;
class ConfigEditNIBPMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditNIBPMenu();
    ~ConfigEditNIBPMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);
    void onBtnClick();
private:
    QScopedPointer<ConfigEditNIBPMenuPrivate> d_ptr;
};
