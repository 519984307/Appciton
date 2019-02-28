#pragma once

#include "PopupWidget.h"
#include "MenuWindow.h"
#include <QScopedPointer>

class N2OSetAGMenuPrivate;
class N2OSetAGMenu : public Dialog
{
    Q_OBJECT
public:
     N2OSetAGMenu();
    ~N2OSetAGMenu();
public slots:
     void onComboIndexChanged(int index);
private:
    QScopedPointer<N2OSetAGMenuPrivate>d_ptr;
};
