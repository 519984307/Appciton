#pragma once
#include <QScopedPointer>
#include "PopupWidget.h"

class FreezeWidgetPrivate;
class FreezeWidget : public PopupWidget
{
    Q_OBJECT
public:
    FreezeWidget();
    ~FreezeWidget();

protected:
    virtual void showEvent(QShowEvent *ev);
    virtual void hideEvent(QHideEvent *ev);
    virtual void keyPressEvent(QKeyEvent *e);

private slots:
    void onSelectWaveChanged(const QString &waveName);
    void onBtnClick();

private:
    QScopedPointer<FreezeWidgetPrivate> d_ptr;

};
