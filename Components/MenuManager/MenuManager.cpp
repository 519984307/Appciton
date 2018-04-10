#include "MenuManager.h"
#include "WindowManager.h"
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QLabel>
#include <QPainter>
#include "ColorManager.h"
#include "SystemMenu.h"
#include "SupervisorEntrance.h"
#include "PublicMenuManager.h"
#include "SupervisorMenuManager.h"
#include "MenuGroup.h"
#include "NIBPRepair.h"

MenuManager *MenuManager::_selfObj = NULL;

/**************************************************************************************************
 * 弹出新菜单。
 *************************************************************************************************/
void MenuManager::popup(QWidget *menuGroup, int x, int y)
{
    if (!widgetList.isEmpty())
    {
        widgetList.last()->hide();
    }
    widgetList.append(menuGroup);

    if (x == 0 && y == 0)
    {
        QRect r = windowManager.getMenuArea();
        x = r.x() + (r.width() - menuGroup->width()) / 2;
        y = r.y() + (r.height() - menuGroup->height());
    }

    menuGroup->move(x, y);
    menuGroup->show();
}

void MenuManager::returnPrevious()
{
//    int i = _subMenus->count();
//    if (i > 1)
//    {
//        if (_subMenus->currentWidget() != &publicMenuManager)
//        {
//            _subMenus->removeWidget(_subMenus->currentWidget());
//        }
//    }
//    else
//    {
//        close();
//    }
    if (!widgetList.isEmpty())
    {
        widgetList.last()->hide();
        widgetList.removeLast();
    }
    if (!widgetList.isEmpty())
    {
        widgetList.last()->show();
    }
}

void MenuManager::close()
{
    widgetList.clear();
}

void MenuManager::_currentMenuGroup(int /*index*/)
{
//    _menuGroup = (MenuGroup*)_subMenus->currentWidget();
    _menuGroup = dynamic_cast<MenuGroup *>(_subMenus->currentWidget());
}

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
MenuManager::MenuManager()
{
//    setFixedSize(windowManager.getPopMenuWidth(), windowManager.getPopMenuHeight());
    _listWidth = windowManager.getPopMenuWidth() / 4;
    _submenuWidth = windowManager.getPopMenuWidth() - _listWidth - 20;
    _submenuHeight = windowManager.getPopMenuHeight() - 30 - 30 - 20;

    widgetList.clear();

    _subMenus = new QStackedWidget();
}

/**************************************************************************************************
* 功能: 添加子菜单
*************************************************************************************************/
void MenuManager::_addSubMenu(void)
{

}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
MenuManager::~MenuManager()
{

}

