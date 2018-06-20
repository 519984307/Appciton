#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigEditRespMenuPrivate;
class ConfigEditRespMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigEditRespMenu();
    ~ConfigEditRespMenu();

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onComboListConfigChanged(int index);

private:
    QScopedPointer<ConfigEditRespMenuPrivate> d_ptr;
};
