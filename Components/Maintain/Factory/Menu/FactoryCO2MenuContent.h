#pragma once
#include "MenuContent.h"

class FactoryCO2MenuContentPrivate;
class FactoryCO2MenuContent : public MenuContent
{
    Q_OBJECT

public:
    FactoryCO2MenuContent();

    ~FactoryCO2MenuContent();

    void showError(QString str);

protected:

    virtual void readyShow();

    virtual void layoutExec();

    void timerEvent(QTimerEvent *ev);

private slots:
    void onBtnReleased();

    void updateCO2Contect(bool status);

private:
    FactoryCO2MenuContentPrivate *const d_ptr;
};
