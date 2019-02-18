#pragma once

#include "PopupWidget.h"
#include "MenuWindow.h"
#include <QScopedPointer>

class CO2SetAGMenuPrivate;
class CO2SetAGMenu : public Dialog
{
    Q_OBJECT
public:
     CO2SetAGMenu();
    ~CO2SetAGMenu();
public slots:
     void onComboIndexChanged(int index);
private:
    QScopedPointer<CO2SetAGMenuPrivate>d_ptr;
};
