#pragma once
#include "MenuGroup.h"
#include "NIBPProviderIFace.h"
#include "NIBPState.h"
#include "NIBPParamService.h"
#include "IMessageBox.h"

#define InvStr() ("---")

class QLabel;
class IListWidget;
class QStackedWidget;
class SubMenu;
class QScrollArea;
class IButton;
class NIBPRepair : public MenuGroup
{
    Q_OBJECT

public:
    static NIBPRepair &Construation()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new NIBPRepair();
        }

        return *_selfObj;
    }
    static NIBPRepair *_selfObj;

    ~NIBPRepair();

    // 弹出菜单。
    void popup(int x, int y);

    //添加子界面
    void addSubmenu(SubMenu *subMenu);

    //初始化
    void init();

    //进入模式的应答
    void unPacket(bool flag);

    //获取子菜单高度
    int getSubmenuHeight() {return _submenuHeight;}

    //获取子菜单宽度
    int getSubmenuWidth() {return _submenuWidth;}

    // 返回到菜单列表项。
    void returnMenu(void);

    void messageBox(void);

    void warnShow(bool enable);

    // 设置焦点顺序
    void setFocusOrder(bool flag);

    // 获取维护模式是否错误
    bool getRepairError(void);

signals:
    void foucsChange();

protected:
    void paintEvent(QPaintEvent *e);
    //按键事件
    void keyPressEvent(QKeyEvent *event);

private slots:
    // 返回到菜单列表项。
    void _returnMenuList();
    // 更新标题
    void _titleChanged(void);
    // 子菜单页聚焦
    void _itemClicked(void);
    //关闭窗口
    void _closeMenu(void);
    // 页面更改。
    void _changePage(QListWidgetItem *current, QListWidgetItem *previous);
    //滚动区域变化
    void _changeScrollValue(int value);
    // 关闭按钮获得焦点
    void _closeBtnSetFoucs();

private:
    NIBPRepair();

    int _listWidth;                      //列表宽度
    int _submenuWidth;                   //子菜单宽度
    int _submenuHeight;                  //高度

    static const int _titleBarHeight = 29;
    static const int _listItemHeight = 26;
    static const int _borderWidth = 4;

    QLabel *_title;                      //标题
    IButton *_closeBtn;
    QLabel *_warn;                       //警告
    IListWidget *_itemList;              //列表
    QStackedWidget *_subMenu;            //子界面
    QScrollArea *_scorllArea;            //滚动区域
    IButton *_return;

    IMessageBox *messageBoxWait;
    IMessageBox *messageBoxError;

    bool _replyFlag;                     //进入维护模式标志
    bool _repairError;                   //维护模式错误
};
#define nibprepair (NIBPRepair::Construation())
