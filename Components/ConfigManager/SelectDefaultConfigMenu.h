#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class SelectDefaultConfigMenuPrivate;
class SelectDefaultConfigMenu : public SubMenu
{
    Q_OBJECT
public:
    SelectDefaultConfigMenu();
    ~SelectDefaultConfigMenu();

protected:
    virtual void layoutExec(void);

private slots:
    void onCurrentIndexChanged(int index);
    void onUserDefinedConfigChanged();

private:
    QScopedArrayPointer<SelectDefaultConfigMenuPrivate> d_ptr;
};
