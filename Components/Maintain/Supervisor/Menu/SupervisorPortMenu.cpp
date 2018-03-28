#include <QTimer>
#include "LanguageManager.h"
#include "FontManager.h"
#include "LabelButton.h"
#include "SupervisorPortMenu.h"
#include "SupervisorConfigManager.h"
#include "IMessageBox.h"
#include "IThread.h"
#include "IComboList.h"
#include "USBManager.h"
#include "SupervisorMenuManager.h"

SupervisorPortMenu *SupervisorPortMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SupervisorPortMenu::SupervisorPortMenu() : SubMenu(trs("SupervisorExportImport"))
{
    setDesc(trs("SupervisorExportImport"));

    startLayout();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SupervisorPortMenu::layoutExec()
{
    int submenuW = supervisorMenuManager.getSubmenuWidth();
    int submenuH = supervisorMenuManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;

    _info = new QLabel(trs("WarningNoUSB"), this);
    _info->setFont(fontManager.textFont(15));
    mainLayout->addWidget(_info, 0, Qt::AlignRight);

    _exportConfigBtn = new LButtonEx();
    _exportConfigBtn->setText(trs("SupervisorExport"));
    _exportConfigBtn->setFont(fontManager.textFont(fontSize));
    _exportConfigBtn->setFixedSize(btnWidth, ITEM_H);
    connect(_exportConfigBtn, SIGNAL(realReleased()), this,SLOT(_exConfigBtnSlot()));
    mainLayout->addWidget(_exportConfigBtn, 0, Qt::AlignRight);


    _importConfigBtn = new LButtonEx();
    _importConfigBtn->setText(trs("SupervisorImport"));
    _importConfigBtn->setFont(fontManager.textFont(fontSize));
    _importConfigBtn->setFixedSize(btnWidth, ITEM_H);
    connect(_importConfigBtn, SIGNAL(realReleased()), this,SLOT(_imConfigBtnSlot()));
    mainLayout->addWidget(_importConfigBtn, 0, Qt::AlignRight);

    _type = PORTTYPE_NR;
    _USBCheckTimer = new QTimer();
    _USBCheckTimer->setInterval(500);
    connect(_USBCheckTimer, SIGNAL(timeout()), this, SLOT(_USBCheckTimeout()));
}

/**************************************************************************************************
 * 功能:初始化按钮信息
 *************************************************************************************************/
void SupervisorPortMenu::readyShow()
{
    _exportConfigBtn->setText(trs("SupervisorExport"));
    _importConfigBtn->setText(trs("SupervisorImport"));
}

/**************************************************************************************************
 * 功能:导出配置槽函数
 *************************************************************************************************/
void SupervisorPortMenu::_exConfigBtnSlot()
{
    if (_type != PORTTYPE_NR)
    {
        return;
    }

    if (!usbManager.isUSBExist())
    {
        //警告没插U盘
        IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        _type = EXPORT_CONFIG;
        _exportConfigBtn->setText(trs("SupervisorExportingPleaseWait"));
        _exportConfigBtn->repaint();
        if (supervisorConfigManager.exportConfig())
        {
            IMessageBox messageBox(trs("Prompt"), trs("PromptExportOK"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
        }

        _exportConfigBtn->setText(trs("SupervisorExport"));
    }

    _type = PORTTYPE_NR;
}

/**************************************************************************************************
 * 功能:导入配置槽函数
 *************************************************************************************************/
void SupervisorPortMenu::_imConfigBtnSlot()
{
    if (_type != PORTTYPE_NR)
    {
        return;
    }

    //检查U盘
    if (!usbManager.isUSBExist())
    {
        //警告没插U盘
        IMessageBox messageBox(trs("Warn"), trs("WarningNoUSB"), QStringList(trs("EnglishYESChineseSURE")));
        messageBox.exec();
    }
    else
    {
        _type = IMPORT_CONFIG;
        _importConfigBtn->setText(trs("SupervisorImportingPleaseWait"));
        _importConfigBtn->repaint();
        if (supervisorConfigManager.importConfig())
        {
            //提示导入配置成功
            IMessageBox messageBox(trs("Prompt"), trs("PromptImportOK"), QStringList(trs("EnglishYESChineseSURE")));
            messageBox.exec();
        }

        _importConfigBtn->setText(trs("SupervisorImport"));
    }

    _type = PORTTYPE_NR;
}

/**************************************************************************************************
 * U盘检测
 *************************************************************************************************/
void SupervisorPortMenu::_USBCheckTimeout()
{
    //检查U盘
    if (!usbManager.isUSBExist())
    {
        _info->show();
    }
    else
    {
        _info->hide();
    }
}

/**************************************************************************************************
 * 功能:析构函数
 *************************************************************************************************/
SupervisorPortMenu::~SupervisorPortMenu()
{
    if (NULL != _USBCheckTimer)
    {
        delete _USBCheckTimer;
    }
}
