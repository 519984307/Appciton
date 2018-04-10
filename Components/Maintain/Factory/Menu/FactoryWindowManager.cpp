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
#include "MenuManager.h"
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
FactoryWindowManager::FactoryWindowManager() : MenuWidget("FactorySystem")
#else
FactoryWindowManager::FactoryWindowManager() : MenuWidget("FactorySystem")
#endif
{
    _submenuWidth = menuManager.getSubmenuWidth();
    _submenuHeight = menuManager.getSubmenuHeight();

    int fontSize = fontManager.getFontSize(3);

    _testButton = new IButton("_testButton");
    _testButton->setFont(fontManager.textFont(fontSize));
    _testButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_testButton, SIGNAL(realReleased()), this,
            SLOT(_testButtonSlot()));

    _dataRecordButton = new IButton("_dataRecordButton");
    _dataRecordButton->setFont(fontManager.textFont(fontSize));
    _dataRecordButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_dataRecordButton, SIGNAL(realReleased()), this,
            SLOT(_dataRecordButtonlot()));

    _systemInfoButton = new IButton("_systemInfoButton");
    _systemInfoButton->setFont(fontManager.textFont(fontSize));
    _systemInfoButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_systemInfoButton, SIGNAL(realReleased()), this,
            SLOT(_systemInfoButtonSlot()));

    _tempCalibateButton = new IButton("_tempCalibateButton");
    _tempCalibateButton->setFont(fontManager.textFont(fontSize));
    _tempCalibateButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_tempCalibateButton, SIGNAL(realReleased()), this,
            SLOT(_tempCalibateButtonSlot()));

    _returnButton = new IButton("_returnButton");
    _returnButton->setFont(fontManager.textFont(fontSize));
    _returnButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_returnButton, SIGNAL(realReleased()), this,
            SLOT(_returnButtonSlot()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout = new QVBoxLayout();
    vLayout->setMargin(10);
    vLayout->setSpacing(20);

    vLayout->addStretch();
    vLayout->addWidget(_testButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_dataRecordButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_systemInfoButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_tempCalibateButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_returnButton, 0, Qt::AlignCenter);
    vLayout->addStretch();

    mainLayout->addLayout(vLayout);
}

/***************************************************************************************************
 * 功能：初始化菜单
 * 参数:
 *      mode:进入配置的模式
 **************************************************************************************************/
void FactoryWindowManager::initMenu(void)
{
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        _tempCalibateButton->setVisible(true);
    }
    else
    {
        _tempCalibateButton->setVisible(false);
    }
}


void FactoryWindowManager::upgradeMenu(void)
{
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        _tempCalibateButton->setVisible(true);
    }
    else
    {
        _tempCalibateButton->setVisible(false);
    }
}

void FactoryWindowManager::_testButtonSlot()
{
    menuManager.popup(&factoryTestMenu);
}

void FactoryWindowManager::_dataRecordButtonlot()
{
    menuManager.popup(&factoryDataRecord);
}

void FactoryWindowManager::_systemInfoButtonSlot()
{
    menuManager.popup(&factorySystemInfoMenu);
}

void FactoryWindowManager::_tempCalibateButtonSlot()
{
    menuManager.popup(&factoryTempMenu);
}

void FactoryWindowManager::_returnButtonSlot()
{
    menuManager.returnPrevious();
}

/**************************************************************************************************
* 功能: 析构函数
*************************************************************************************************/
FactoryWindowManager::~FactoryWindowManager()
{

}



