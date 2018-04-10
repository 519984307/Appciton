#pragma once
#include "MenuWidget.h"
#include "IButton.h"
#include <QList>

#define TITLE_H (30)           //标题高度
#define HELP_BUTTON_H (30)     //帮助按钮高度
#define ITEM_H (30)            //子项高度

class QLabel;
class QVBoxLayout;
class IWidget;
class QStackedWidget;
class FactoryWindowManager : public MenuWidget
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

private slots:
    void _testButtonSlot();
    void _dataRecordButtonlot();
    void _systemInfoButtonSlot();
    void _tempCalibateButtonSlot();
    void _returnButtonSlot();

private:
    //构造函数
    FactoryWindowManager();

    static const int _borderWidth = 8;

    enum
    {
        FACTORY_TEST,
        FACTORY_DATA_RECORD,
        FACTORY_SYSTEM_INFO,
        FACTORY_TEMP,
        FACTORY_NR
    };

    IButton *_testButton;
    IButton *_dataRecordButton;
    IButton *_systemInfoButton;
    IButton *_tempCalibateButton;
    IButton *_returnButton;

private:
    int _submenuWidth;                  //子菜单宽度
    int _submenuHeight;                 //子菜单高度
};

#define factoryWindowManager (FactoryWindowManager::construction())
#define deleteFactoryWindowManager() (delete FactoryWindowManager::_selfObj)


