#pragma once
#include "SubMenu.h"
#include <QScopedPointer>

class ConfigExportImportMenuPrivate;
class ConfigExportImportMenu : public SubMenu
{
    Q_OBJECT
public:
    ConfigExportImportMenu();
    ~ConfigExportImportMenu();

    bool eventFilter(QObject *obj, QEvent *ev);

protected:
    virtual void layoutExec();
    virtual void readyShow();

private slots:
    void onExitList(bool);
    void onConfigClick();
    void onBtnClick();

private:
    QScopedPointer<ConfigExportImportMenuPrivate> d_ptr;

};
