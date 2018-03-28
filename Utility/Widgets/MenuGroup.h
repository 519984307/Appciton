#pragma once
#include <QWidget>
#include <QListWidget>
#include <QKeyEvent>
#include <QEvent>
#include "IListWidget.h"
#include "Debug.h"

class QLabel;
class QListWidget;
class QStackedWidget;
class QListWidgetItem;
class SubMenu;
class QScrollArea;
class IButton;

class MenuGroup : public QWidget
{
    Q_OBJECT

public:
    // 菜单名称。
    const QString &name(void)
    {
        return _menuName;
    }

    // 菜单的描述，该描述将在菜单栏显示。
    const QString &desc(void)
    {
        return _menuDesc;
    }

    // 弹出菜单。
    void popup(int x = 0, int y = 0);

    // 弹出菜单并聚焦到指定的页面。
    void popup(SubMenu *menu, int x = 0, int y = 0);

    // 添加一个子菜单。
    void addSubMenu(SubMenu *subMenu);

    // 添加一个返回子按钮。
    void addReturnMenu(void);

    // 设置列表区和菜单区的宽度。
    void setListWidth(int w);
    void setScrollAreaSize(int w, int h);

    // 设置焦点顺序
    void setFocusOrder(bool flag);

    // 初始化菜单列表项。
    void _initMenuList();

    bool listTableFocus() { return Qt::StrongFocus == listTable->focusPolicy();}
    void listTableOrder(bool flag);

    //获取子菜单高度
    int getListWidth() {return _listWidth;}

    //获取子菜单高度
    int getSubmenuHeight() {return _submenuHeight;}

    //获取子菜单宽度
    int getSubmenuWidth() {return _submenuWidth;}

    QLabel *warn;                        //警告
    IListWidget *listTable;              //列表

    // 构造与析构。
    MenuGroup(const QString &name);
    ~MenuGroup();

signals:
    void menuGroupReturn();

protected:
    // 隐藏事件。
    virtual void hideEvent(QHideEvent *event);

    //按键事件
    void keyPressEvent(QKeyEvent *event);

    QScrollArea *_scorllArea;             //滚动区域

private slots:
    // 返回到菜单列表项。
    void _returnMenuList();

    // 子菜单页聚焦
    virtual void _itemClicked(void);

    // 页面更改。
    void _changePage(QListWidgetItem *current, QListWidgetItem *previous);

    // 关闭按钮获得焦点
    void _closeBtnSetFoucs();

    // 改变滚动区域滚动条的值
    void _changeScrollValue(int value);

    void _closeSlot(void);

private:
    static const int _titleBarHeight = 29;
    static const int _listItemHeight = 26;
    static const int _borderWidth = 4;

    QLabel *_warn;                        //警告

    QStackedWidget *_subMenus;            //子界面
    IButton *_return;

    int _listWidth;                      //列表宽度
    int _submenuWidth;                   //子菜单宽度
    int _submenuHeight;                  //高度

private:
    QString _menuName;
    QString _menuDesc;
};
