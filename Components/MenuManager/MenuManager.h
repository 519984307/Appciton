#pragma once
#include <QDialog>
#include "IButton.h"
#include "MenuGroup.h"

#define TITLE_H (30)           //标题高度
#define HELP_BUTTON_H (30)     //帮助按钮高度
#define ITEM_H (30)            //子项高度
#define BORDER_W (4)           //边宽

class QVBoxLayout;
class QStackedWidget;
class QLabel;
class MenuManager : public QDialog
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
    //设置文本及图片
    void setCloseBtnTxt(const QString &txt);
    void setCloseBtnPic(const QImage &pic);
    void setCloseBtnColor(QColor color);

    // 添加一个子菜单。
    void addMenuGroup(MenuGroup *menuGroup);
    void openMenuGroup(MenuGroup *menuGroup);
    void openWidget(QWidget *menuGroup);

    void returnPrevious(void);

    // 关闭按钮获得焦点
    void closeBtnSetFoucs();

    //获取子菜单高度
    int getListWidth() {return _listWidth;}

    //获取子菜单高度
    int getSubmenuHeight() {return _submenuHeight;}

    //获取子菜单宽度
    int getSubmenuWidth() {return _submenuWidth;}

    IButton *closeBtn;

    bool closeBtnHasFocus() { return closeBtn->hasFocus(); }

    //血压服务模式警告
    void NIBPWarn(bool enable);

protected:
    // 重绘。
    virtual void paintEvent(QPaintEvent *event);

    // 隐藏事件。
    virtual void hideEvent(QHideEvent *event);

    //按键事件
    void keyPressEvent(QKeyEvent *event);

private slots:
    // 更新标题
    void _titleChanged(void);
    void _closeSlot(void);
    void _currentMenuGroup(int index);

private:
    //初始化子菜单
    void _addSubMenu();

    QVBoxLayout *_mainLayout;           //主布局器
    QLabel *_titleLabel;
    QLabel *_warn;                       //警告
    QStackedWidget *_subMenus;           //子菜单
    MenuGroup *_menuGroup;

    int _listWidth;                      //列表宽度
    int _submenuWidth;                   //子菜单宽度
    int _submenuHeight;                  //高度

};
#define menuManager (MenuManager::construction())
#define deleteMenuManager() (delete MenuManager::_selfObj)
