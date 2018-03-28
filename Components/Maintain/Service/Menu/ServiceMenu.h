#pragma once
#include <QPushButton>
#include <QWidget>
#include "MenuGroup.h"
#include "LabelButton.h"

class QLabel;
class IListWidget;
class QStackedWidget;
class SubMenu;
class QScrollArea;
class PButton;
class ServiceMenu : public QWidget
{
    Q_OBJECT

public:
    static ServiceMenu &Construation()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new ServiceMenu();
        }

        return *_selfObj;
    }
    static ServiceMenu *_selfObj;

    ~ServiceMenu();

    //初始化
    void init();

signals:
    void foucsChange();

protected:
    void paintEvent(QPaintEvent *e);

private slots:
    void _nibpButtonSlot();
    void _upgradeButtonSlot();
    void _versionButtonSlot();
    void _tempButtonSlot();
    void _errorLogButtonSlot();
    void _returnButtonSlot();

private slots:
    // 返回到菜单列表项。
    void _returnMenuList(SubMenu *m);
    // 更新标题
    void _titleChanged(void);
    // 子菜单页聚焦
    void _itemClicked(void);
    //滚动区域变化
    void _changeScrollValue(int value);

private:
    ServiceMenu();

    static const int _borderWidth = 8;

    QLabel *_title;                      //标题
    IListWidget *_itemList;              //列表
    QStackedWidget *_subMenu;            //子界面
    QScrollArea *_scorllArea;            //滚动区域

    IButton *_nibpButton;
    IButton *_upgradeButton;
    IButton *_versionButton;
    IButton *_tempButton;
    IButton *_errorLogButton;
    IButton *_returnButton;
};
#define serviceMenu (ServiceMenu::Construation())
