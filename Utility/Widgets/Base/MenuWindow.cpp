#include "MenuWindow.h"
#include "FontManager.h"
#include <QBoxLayout>
#include "MenuSidebar.h"

class MenuWindowPrivate
{
public:
    MenuWindowPrivate()
        :sidebar(NULL)
    {

    }

    MenuSidebar *sidebar;
};

MenuWindow::MenuWindow()
    :Window(), d_ptr(new MenuWindowPrivate())
{
    setWindowTitle("Menu Window");
    resize(640, 480);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0,0,0,0);
    setWindowLayout(layout);

    d_ptr->sidebar = new MenuSidebar();
    d_ptr->sidebar->setFixedWidth(180);
    layout->addWidget(d_ptr->sidebar);
    layout->addStretch();
}

MenuWindow::~MenuWindow()
{

}

void MenuWindow::addMenuGroup(const QString &title)
{
    d_ptr->sidebar->addItem(title);
}
