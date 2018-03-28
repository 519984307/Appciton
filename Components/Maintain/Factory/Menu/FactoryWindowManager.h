#pragma once
#include <QWidget>
#include <QPushButton>
#include <QList>

#define TITLE_H (30)           //标题高度
#define HELP_BUTTON_H (30)     //帮助按钮高度
#define ITEM_H (30)            //子项高度

class QLabel;
class QVBoxLayout;
class IWidget;
class PButton;
class QStackedWidget;
class FactoryWindowManager : public QWidget
{
    Q_OBJECT

public:
    //构造函数
    static FactoryWindowManager &construction()
    {
        if (NULL == _selfObj)
        {
            _selfObj = new FactoryWindowManager();
        }

        return *_selfObj;
    }
    static FactoryWindowManager *_selfObj;

    ~FactoryWindowManager();

    //初始化
    void initMenu(void);

    //跟新界面
    void upgradeMenu(void);

    //获取子菜单宽度
    int getSubmenuWidth() const {return _submenuWidth;}

    //获取子菜单高度
    int getSubmenuHeight() const {return _submenuHeight;}

    //添加子菜单
    void addSubMenu(QWidget *win);

    //获取菜单区域的Rect
    QRect getMenuArea(void);

private slots:
    //图标链接
    void _numBtnSlot(int index);

private:
    //构造函数
    FactoryWindowManager();

    //启动布局
    void _doLayoutStyle();

    //计算子菜单大小
    void _caclSubmenuSize();

    void _enterFactoryTestMenu();
    void _enterFactoryDataRecord();
    void _enterFactorySystemInfoMenu();
    void _enterFactoryTempMenu();

    QVBoxLayout *_mainLayout;           //主布局器
    QStackedWidget *_subMenu;           //子菜单
    QLabel *_windowTitleLabel;          //窗体名称

    static const int _borderWidth = 8;

    enum
    {
        FACTORY_TEST,
        FACTORY_DATA_RECORD,
        FACTORY_SYSTEM_INFO,
        FACTORY_TEMP,
        FACTORY_NR
    };

    PButton *pushbutton[FACTORY_NR];

private:
    int _submenuWidth;                  //子菜单宽度
    int _submenuHeight;                 //子菜单高度
};

#define factoryWindowManager (FactoryWindowManager::construction())
#define deleteFactoryWindowManager() (delete FactoryWindowManager::_selfObj)


