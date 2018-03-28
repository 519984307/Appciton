#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedWidget>
#include "FactoryWindowManager.h"
#include "IConfig.h"
#include "IWidget.h"
#include "IButton.h"
#include "LanguageManager.h"
#include "FontManager.h"
#include "SystemManager.h"
#include "Debug.h"
#include "FactoryDataRecord.h"
#include "FactoryTempMenu.h"
#include "FactoryTestMenu.h"
#include "FactorySystemInfoMenu.h"

FactoryWindowManager *FactoryWindowManager::_selfObj = NULL;

/**************************************************************************************************
* 功能: 构造函数
*************************************************************************************************/
#if defined(Q_WS_QWS)
FactoryWindowManager::FactoryWindowManager() : QWidget(NULL, Qt::FramelessWindowHint)
#else
FactoryWindowManager::FactoryWindowManager() : QWidget()
#endif
{
    //计算子菜单大小
    _caclSubmenuSize();

    //启动布局
    _doLayoutStyle();
}

/***************************************************************************************************
 * 功能：初始化菜单
 * 参数:
 *      mode:进入配置的模式
 **************************************************************************************************/
void FactoryWindowManager::initMenu(void)
{
    _subMenu->setVisible(false);
    for (int i = 0; i < FACTORY_TEMP; ++i)
    {
        pushbutton[i]->setVisible(true);
    }
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        pushbutton[FACTORY_TEMP]->setVisible(true);
    }
    else
    {
        pushbutton[FACTORY_TEMP]->setVisible(false);
    }
    pushbutton[0]->setFocus();
}


void FactoryWindowManager::upgradeMenu(void)
{
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        pushbutton[FACTORY_TEMP]->setVisible(true);
    }
    else
    {
        pushbutton[FACTORY_TEMP]->setVisible(false);
    }
}

/***************************************************************************************************
 * 功能：添加子菜单
 * 参数:
 *      win:待添加的菜单
 **************************************************************************************************/
void FactoryWindowManager::addSubMenu(QWidget *win)
{
    if (NULL != win)
    {
        _subMenu->addWidget(win);
    }
}

/***************************************************************************************************
 * 功能：获取菜单区域的Rect。
 * 返回：rect对象。
 **************************************************************************************************/
QRect FactoryWindowManager::getMenuArea(void)
{
    QRect r = _subMenu->geometry();
    r.adjust(x(), y(), x(), y());
    return r;
}

/**************************************************************************************************
* 功能: 启动界面布局
*************************************************************************************************/
void FactoryWindowManager::_doLayoutStyle()
{
    QPalette p;
    p.setColor(QPalette::Background, Qt::black);
    setPalette(p);

    int fontSize = fontManager.getFontSize(6);
    _windowTitleLabel = new QLabel(trs("FactorySystem"));
    _windowTitleLabel->setFont(fontManager.textFont(fontSize));
    _windowTitleLabel->setFixedHeight(40);
    _windowTitleLabel->setAlignment(Qt::AlignCenter);
    p.setColor(QPalette::Foreground, Qt::white);
    _windowTitleLabel->setPalette(p);

    pushbutton[FACTORY_TEST] = new PButton(FACTORY_TEST);
    pushbutton[FACTORY_TEST]->setText(trs("FactoryTest"));
    pushbutton[FACTORY_TEST]->setPicture(QPixmap("/usr/local/nPM/icons/Test.png"));

    pushbutton[FACTORY_DATA_RECORD] = new PButton(FACTORY_DATA_RECORD);
    pushbutton[FACTORY_DATA_RECORD]->setText(trs("DataRecord"));
    pushbutton[FACTORY_DATA_RECORD]->setPicture(QPixmap("/usr/local/nPM/icons/Record.png"));

    pushbutton[FACTORY_SYSTEM_INFO] = new PButton(FACTORY_SYSTEM_INFO);
    pushbutton[FACTORY_SYSTEM_INFO]->setText(trs("SystemInfo"));
    pushbutton[FACTORY_SYSTEM_INFO]->setPicture(QPixmap("/usr/local/nPM/icons/SystemInfo.png"));

    pushbutton[FACTORY_TEMP] = new PButton(FACTORY_TEMP);
    pushbutton[FACTORY_TEMP]->setText(trs("Temp"));
    pushbutton[FACTORY_TEMP]->setPicture(QPixmap("/usr/local/nPM/icons/TEMPCalibrate.png"));


    QVBoxLayout *thirdRowLayout0 = new QVBoxLayout();
    thirdRowLayout0->setSpacing(20);
    thirdRowLayout0->setAlignment(Qt::AlignTop);
    thirdRowLayout0->addWidget(pushbutton[FACTORY_TEST]);

    QVBoxLayout *thirdRowLayout1 = new QVBoxLayout();
    thirdRowLayout1->setSpacing(20);
    thirdRowLayout1->setAlignment(Qt::AlignTop);
    thirdRowLayout1->addWidget(pushbutton[FACTORY_DATA_RECORD]);
    thirdRowLayout1->addWidget(pushbutton[FACTORY_SYSTEM_INFO]);
    thirdRowLayout1->addWidget(pushbutton[FACTORY_TEMP]);

    //添加信号-槽函数
    for (int i = 0; i < FACTORY_NR; ++i)
    {
        pushbutton[i]->setFixedSize(145, 90);
        pushbutton[i]->setFont(fontManager.textFont(16));
        pushbutton[i]->setVisible(false);
        connect(pushbutton[i], SIGNAL(released(int)), this, SLOT(_numBtnSlot(int)));
    }

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignLeft);
    hLayout->setContentsMargins(_borderWidth, 0, _borderWidth, _borderWidth);
    hLayout->setSpacing(5);
    hLayout->addLayout(thirdRowLayout0);
    hLayout->addLayout(thirdRowLayout1);

    _subMenu = new QStackedWidget();

    _mainLayout = new QVBoxLayout();
    _mainLayout->setMargin(10);
    _mainLayout->setSpacing(20);
    _mainLayout->addWidget(_windowTitleLabel);
    _mainLayout->addWidget(_subMenu, 0, Qt::AlignCenter);

//    _mainLayout->addStretch();
    _mainLayout->addLayout(hLayout);
    _mainLayout->addStretch();


    setLayout(_mainLayout);
}

/**************************************************************************************************
* 功能: 初始化子菜单
*************************************************************************************************/
void FactoryWindowManager::_caclSubmenuSize()
{
    int screenWidth = 0;
    int screenHeight = 0;
    machineConfig.getNumValue("ScreenWidth", screenWidth);
    machineConfig.getNumValue("ScreenHeight", screenHeight);
    setFixedSize(screenWidth, screenHeight);

//    //子菜单的宽度为整个屏幕的80%
//    _submenuWidth = screenWidth * 8 / 10;

//    //子菜单的高度为屏幕的高度减去标题和帮助按钮的高度
//    _submenuHeight = screenHeight - TITLE_H - HELP_BUTTON_H - 30;

    //子菜单的宽度
    _submenuWidth = screenWidth - 20;

    //子菜单的高度
    _submenuHeight = screenHeight - 20;
}

void FactoryWindowManager::_enterFactoryTestMenu()
{
    QRect r = geometry();
    factoryTestMenu.autoShow(r.x() + 10, r.y() + 10);
    factoryTestMenu.readyShow();
}

void FactoryWindowManager::_enterFactoryDataRecord()
{
    QRect r = geometry();
    factoryDataRecord.autoShow(r.x() + 10, r.y() + 10);
    factoryDataRecord.readyShow();
}

void FactoryWindowManager::_enterFactorySystemInfoMenu()
{
    QRect r = geometry();
    factorySystemInfoMenu.autoShow(r.x() + 10, r.y() + 10);
    factorySystemInfoMenu.readyShow();
}

void FactoryWindowManager::_enterFactoryTempMenu()
{
    QRect r = geometry();
    factoryTempMenu.autoShow(r.x() + 10, r.y() + 10);
    factoryTempMenu.readyShow();
}

void FactoryWindowManager::_numBtnSlot(int index)
{
    switch(index)
    {
    case FACTORY_TEST:
        _enterFactoryTestMenu();
        break;
    case FACTORY_DATA_RECORD:
        _enterFactoryDataRecord();
        break;
    case FACTORY_SYSTEM_INFO:
        _enterFactorySystemInfoMenu();
        break;
    case FACTORY_TEMP:
        _enterFactoryTempMenu();
        break;
    default:
        break;
    }
}

/**************************************************************************************************
* 功能: 析构函数
*************************************************************************************************/
FactoryWindowManager::~FactoryWindowManager()
{

}



