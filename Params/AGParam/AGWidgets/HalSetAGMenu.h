#pragma once

#include "PopupWidget.h"
#include "MenuWindow.h"
#include <QScopedPointer>

class HalSetAGMenuPrivate;
class HalSetAGMenu : public Dialog
{
    Q_OBJECT
public:
     HalSetAGMenu();
    ~HalSetAGMenu();
public slots:
     void onComboIndexChanged(int index);
private:
    QScopedPointer<HalSetAGMenuPrivate>d_ptr;
};
