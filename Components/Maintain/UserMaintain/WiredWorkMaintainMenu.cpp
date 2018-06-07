#include "WiredWorkMaintainMenu.h"
#include <QVBoxLayout>
#include <QRegExp>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IButton.h"
#include "IComboList.h"
#include "SupervisorMenuManager.h"
#include "SupervisorConfigManager.h"
#include "PatientDefine.h"
#include "KeyBoardPanel.h"
#include "Debug.h"
#include "UserMaintainManager.h"
#include "Utility.h"

#define MAX_IP_STRING_LENGTH 15

WiredWorkMaintainMenu *WiredWorkMaintainMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
WiredWorkMaintainMenu::WiredWorkMaintainMenu() : SubMenu(trs("WiredWorkMaintainMenu")),
                                                 _addressType(NULL),
                                                 _ipAddress(NULL),
                                                 _subnetMask(NULL),
                                                 _gateWay(NULL)
{
    setDesc(trs("WiredWorkMaintainMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void WiredWorkMaintainMenu::readyShow()
{
    QString tmpStr;
    systemConfig.getStrValue("WiredWork|IpAddress", tmpStr);
    if(tmpStr.isNull())
    {
        _ipAddress->button->setText("0.0.0.0");
    }
    else
    {
        _ipAddress->button->setText(tmpStr);
    }
    tmpStr.clear();

    systemConfig.getStrValue("WiredWork|SubnetMask", tmpStr);
    if(tmpStr.isNull())
    {
        _subnetMask->button->setText("0.0.0.0");
    }
    else
    {
        _subnetMask->button->setText(tmpStr);
    }
    tmpStr.clear();

    systemConfig.getStrValue("WiredWork|GateWay", tmpStr);
    if(tmpStr.isNull())
    {
        _gateWay->button->setText("255.255.255.0");
    }
    else
    {
        _gateWay->button->setText(tmpStr);
    }
    tmpStr.clear();

    systemConfig.getStrValue("WiredWork|AddressType", tmpStr);
    if(tmpStr.toInt()>=_addressType->count())
    {
        _addressType->setCurrentIndex(0);
    }
    else
    {
        _addressType->setCurrentIndex(tmpStr.toInt());
        if(tmpStr.toInt())
        {
            _addressTypeSlot(tmpStr.toInt());
        }
    }
    tmpStr.clear();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void WiredWorkMaintainMenu::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //_addressType
    _addressType = new IComboList(trs("AddressType"));
    _addressType->setFont(fontManager.textFont(fontSize));
    _addressType->label->setFixedSize(labelWidth, ITEM_H);
    _addressType->combolist->setFixedSize(btnWidth, ITEM_H);
    _addressType->combolist->addItem(trs("manual"));
    _addressType->combolist->addItem(trs("DHCP"));
    mainLayout->addWidget(_addressType);

    QString string = trs("IpAddress");
    QString string2 = trs("SubnetMask");
    //_ipAddress
    _ipAddress = new LabelButton(trs("IpAddress"));
    _ipAddress->button->setFixedSize(btnWidth, ITEM_H);
    _ipAddress->label->setFixedSize(labelWidth, ITEM_H);
    _ipAddress->setFont(fontManager.textFont(fontSize));
    mainLayout->addWidget(_ipAddress,Qt::AlignRight);
    mainLayout->addStretch();

    //_subnetMask
    _subnetMask = new LabelButton(trs("SubnetMask"));
    _subnetMask->button->setFixedSize(btnWidth, ITEM_H);
    _subnetMask->label->setFixedSize(labelWidth, ITEM_H);
    _subnetMask->setFont(fontManager.textFont(fontSize));
    mainLayout->addWidget(_subnetMask,Qt::AlignRight);
    mainLayout->addStretch();

    //_gateWay
    _gateWay = new LabelButton(trs("GateWay"));
    _gateWay->button->setFixedSize(btnWidth, ITEM_H);
    _gateWay->label->setFixedSize(labelWidth, ITEM_H);
    _gateWay->setFont(fontManager.textFont(fontSize));
    mainLayout->addWidget(_gateWay,Qt::AlignRight);
    mainLayout->addStretch();

    connect(_ipAddress->button, SIGNAL(released(int)), this, SLOT(editIpAddress()));
    connect(_subnetMask->button, SIGNAL(released(int)), this, SLOT(editIpAddress()));
    connect(_gateWay->button, SIGNAL(released(int)), this, SLOT(editIpAddress()));
    connect(_addressType->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(_addressTypeSlot(int)));
}

/**************************************************************************************************
 * _addressTypeSlot槽函数。
 *************************************************************************************************/
void WiredWorkMaintainMenu::_addressTypeSlot(int index)
{
    systemConfig.setNumValue("WiredWork|AddressType", index);
    _ipAddress->setEnabled(!index);
    _subnetMask->setEnabled(!index);
    _gateWay->setEnabled(!index);
}

/***************************************************************************************************
 * check whether a ip string is valid
 **************************************************************************************************/
static bool isIpStrValid(const QString &ipStr)
{
    QRegExp reg("^(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|[1-9]\\d|1\\d\\d|2[0-4]\\d|25[0-5])$");
    return reg.exactMatch(ipStr);
}

/**************************************************************************************************
 * editIpAddress槽函数。
 *************************************************************************************************/
void WiredWorkMaintainMenu::editIpAddress()
{
    LButton *sen = qobject_cast<LButton*>(sender());
    QString string;
    if(!sen)
    {
        return;
    }

    KeyBoardPanel englishPanel;
    englishPanel.setMaxInputLength(MAX_IP_STRING_LENGTH);
    englishPanel.setInitString(sen->text());
    englishPanel.setSpaceEnable(false);
    englishPanel.setBtnEnable("[0-9.]");
    englishPanel.setCheckValueHook(isIpStrValid);

    if (sen == _ipAddress->button)
    {
        englishPanel.setTitleBarText(trs("IpAddress"));
        string = "IpAddress";
    }
    else if (sen == _gateWay->button)
    {
        englishPanel.setTitleBarText(trs("GateWay"));
        string = "GateWay";
    }
    else if (sen == _subnetMask->button)
    {
        englishPanel.setTitleBarText(trs("SubnetMask"));
        string = "SubnetMask";
    }

    if (englishPanel.exec())
    {
        QString text = englishPanel.getStrValue();
        systemConfig.setStrValue(QString("WiredWork|%1").arg(string), text);
        sen->setText(text);
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
WiredWorkMaintainMenu::~WiredWorkMaintainMenu()
{

}
