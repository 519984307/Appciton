#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditSeqMeaMenuPrivate;
class ConfigEditSeqMeaMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditSeqMeaMenu();
    ~ConfigEditSeqMeaMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);
    void onBtnClick();
private:
    QScopedPointer<ConfigEditSeqMeaMenuPrivate> d_ptr;
};
