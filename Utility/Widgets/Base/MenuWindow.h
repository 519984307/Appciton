#pragma once
#include "Window.h"

class MenuWindowPrivate;
class MenuWindow : public Window
{
    Q_OBJECT
public:
    MenuWindow();
    ~MenuWindow();

    void addMenuGroup(const QString &title);

private:
    QScopedPointer<MenuWindowPrivate> d_ptr;
};
