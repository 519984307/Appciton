#include "MonitorInfo.h"
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

MonitorInfo *MonitorInfo::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
MonitorInfo::MonitorInfo() : SubMenu(trs("MonitorInfoMenu"))
{
    QString stringTemp[]={
        "CumulativeWorkingTime", "TemperatureInsideCase", "BatteryLevel",
        "BatteryVoltage", "MachineType", "MACAddress", "ScreenResolationSize",
        "ElectronicSerialNumber", "NULL"
    };
    for(unsigned int i=CUMULATIVE_WORKING_TIME; i<TYPE_NR && i < sizeof(stringTemp); i++)
    {
        _stringTypeName[i] = stringTemp[i];
    }

    for(int i=CUMULATIVE_WORKING_TIME; i<TYPE_NR; i++)
    {
        _labeledLabel[i] = new LabeledLabel(trs(QString("%1%2").arg(trs(_stringTypeName[i])).arg(":")),"NULL");
    }
    _setSerialNumber = new LabelButton("");
    _setSerialNumber->button->setText(trs("%1 >>").arg(trs("SetElectronicSerialNumber")));
    connect(_setSerialNumber->button, SIGNAL(released()), this, SLOT(_setSerialNumberSlot()));
    setDesc(trs("MonitorInfoMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void MonitorInfo::readyShow()
{
    QString tmpStr;

    for(int i=CUMULATIVE_WORKING_TIME; i<TYPE_NR; i++)
    {
        tmpStr.clear();
        systemConfig.getStrValue(QString("MonitorInfo|%1").arg(_stringTypeName[i]), tmpStr);
        if(tmpStr.isEmpty())
        {
            _labeledLabel[i]->setText(trs("NULL"));
        }
        else
        {
            _labeledLabel[i]->setText(trs(tmpStr));
        }
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void MonitorInfo::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    for(int i=CUMULATIVE_WORKING_TIME; i<TYPE_NR; i++)
    {
        _labeledLabel[i]->setFont(fontManager.textFont(fontSize));
        _labeledLabel[i]->setSize(labelWidth, ITEM_H);
        mainLayout->addWidget(_labeledLabel[i],0,Qt::AlignRight);
        mainLayout->addStretch();
    }
    _setSerialNumber->label->setFixedSize(labelWidth, ITEM_H);
    _setSerialNumber->button->setFixedSize(btnWidth, ITEM_H);
    _setSerialNumber->button->setFont(fontManager.textFont(fontSize));
    mainLayout->addWidget(_setSerialNumber,0,Qt::AlignRight);
    mainLayout->addStretch();
}

void MonitorInfo::_setSerialNumberSlot()
{
    KeyBoardPanel setSerialNumber;
    int fontSize = fontManager.getFontSize(1);
    setSerialNumber.setMaxInputLength(16);
    setSerialNumber.setTitleBarFont(fontManager.textFont(fontSize));
    setSerialNumber.setTitleBarText(trs("SetElectronicSerialNumber"));
    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    setSerialNumber.setBtnEnable(regKeyStr);

    if(setSerialNumber.exec())
    {
        QString serialNum = setSerialNumber.getStrValue();
        systemConfig.setStrValue("MonitorInfo|ElectronicSerialNumber", serialNum);
        _labeledLabel[ELECTRONIC_SERIAL_NUMBER]->setText(trs(serialNum));
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
MonitorInfo::~MonitorInfo()
{

}
