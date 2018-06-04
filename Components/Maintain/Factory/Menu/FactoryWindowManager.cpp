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
FactoryWindowManager::FactoryWindowManager() : MenuWidget(trs("FactorySystem"))
#else
FactoryWindowManager::FactoryWindowManager() : MenuWidget(trs("FactorySystem"))
#endif
{
    _submenuWidth = menuManager.getSubmenuWidth();
    _submenuHeight = menuManager.getSubmenuHeight();
    setFixedSize(_submenuWidth, _submenuHeight);

    int fontSize = fontManager.getFontSize(3);

    _testButton = new IButton(trs("FactoryTest"));
    _testButton->setFont(fontManager.textFont(fontSize));
    _testButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_testButton, SIGNAL(realReleased()), this,
            SLOT(_testButtonSlot()));

    _dataRecordButton = new IButton(trs("DataRecord"));
    _dataRecordButton->setFont(fontManager.textFont(fontSize));
    _dataRecordButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_dataRecordButton, SIGNAL(realReleased()), this,
            SLOT(_dataRecordButtonlot()));

    _systemInfoButton = new IButton(trs("SystemInfo"));
    _systemInfoButton->setFont(fontManager.textFont(fontSize));
    _systemInfoButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_systemInfoButton, SIGNAL(realReleased()), this,
            SLOT(_systemInfoButtonSlot()));

    _tempCalibateButton = new IButton(trs("Temp"));
    _tempCalibateButton->setFont(fontManager.textFont(fontSize));
    _tempCalibateButton->setFixedSize(_submenuWidth / 2, 30);
    connect(_tempCalibateButton, SIGNAL(realReleased()), this,
            SLOT(_tempCalibateButtonSlot()));

    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout = new QVBoxLayout();
    vLayout->setMargin(10);
    vLayout->setSpacing(20);

    vLayout->addStretch();
    vLayout->addWidget(_testButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_dataRecordButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_systemInfoButton, 0, Qt::AlignCenter);
    vLayout->addWidget(_tempCalibateButton, 0, Qt::AlignCenter);
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
   //factoryTestMenu.popup();
}

void FactoryWindowManager::_dataRecordButtonlot()
{
    //factoryDataRecord.popup();
}

void FactoryWindowManager::_systemInfoButtonSlot()
{
    //factorySystemInfoMenu.popup();
}

void FactoryWindowManager::_tempCalibateButtonSlot()
{
    //factoryTempMenu.popup();
}

/**************************************************************************************************
* 功能: 析构函数
*************************************************************************************************/
FactoryWindowManager::~FactoryWindowManager()
{

}



