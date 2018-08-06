/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2018/8/2
 **/


#pragma once
#include <QWidget>
#include "IButton.h"
#include "MenuGroup.h"
#include "PopupWidget.h"

#define TITLE_H (30)           //标题高度
#define HELP_BUTTON_H (30)     //帮助按钮高度
#define ITEM_H (30)            //子项高度
#define BORDER_W (4)           //边宽

class QVBoxLayout;
class QStackedWidget;
class QLabel;
class MenuManager : public QObject
{
    Q_OBJECT

public:
    static MenuManager &construction(void)
    {
        if (_selfObj == NULL)
        {
            _selfObj = new MenuManager();
        }
        return *_selfObj;
    }
    static MenuManager *_selfObj;

    MenuManager();
    ~MenuManager();

public:
    void popupWidegt(QWidget *widget, int x = 0, int y = 0);

    void returnPrevious(void);

    void close(void);

    //获取子菜单高度
    int getListWidth() {return _listWidth;}

    //获取子菜单高度
    int getSubmenuHeight() {return _submenuHeight;}

    //获取子菜单宽度
    int getSubmenuWidth() {return _submenuWidth;}

private slots:
    void _currentMenuGroup(int index);

private:
    //初始化子菜单
    void _addSubMenu();

    QStackedWidget *_subMenus;           // 子菜单
    MenuGroup *_menuGroup;
    QList<QWidget *> _widgetList;

    int _listWidth;                      // 列表宽度
    int _submenuWidth;                   // 子菜单宽度
    int _submenuHeight;                  // 高度
};
#define menuManager (MenuManager::construction())
#define deleteMenuManager() (delete MenuManager::_selfObj)
