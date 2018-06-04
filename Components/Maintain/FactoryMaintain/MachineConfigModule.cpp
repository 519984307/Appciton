#include "MachineConfigModule.h"
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

MachineConfigModule *MachineConfigModule::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
MachineConfigModule::MachineConfigModule() : SubMenu(trs("MachineConfigModuleMenu"))
{
    QString stringTemp[]={"IBPModule", "CO2Module", "ECG12LeadsModule",
                          "PrinterModule", "HDMIModule", "WifiModule",
                          "ScreenModule", "NurseCallingModule", "AnalogOutputModule",
                          "SYNCDefibrillationModule", "COModule", "NULL"};

    for(int i=MODULE_TYPE_IBP; i<MODULE_TYPE_NR; i++)
    {
        _stringTypeName[i] = stringTemp[i];
    }

    for(int i=MODULE_TYPE_IBP; i<MODULE_TYPE_NR; i++)
    {
        if(i!=MODULE_TYPE_CO2 && i!=MODULE_TYPE_CO)
        {
            _stringItems[i].append(trs("Disable"));
            _stringItems[i].append(trs("Enable"));
        }
    }

    _stringItems[MODULE_TYPE_CO2].append(trs("Disable"));
    _stringItems[MODULE_TYPE_CO2].append(trs("PHASEIN"));

    _stringItems[MODULE_TYPE_CO].append(trs("Disable"));
    _stringItems[MODULE_TYPE_CO].append(trs("M601"));

    for(int i=MODULE_TYPE_IBP; i<MODULE_TYPE_NR; i++)
    {
        _icomboList[i] = new IComboList(trs(QString("%1").arg(_stringTypeName[i])));
        _icomboList[i]->combolist->addItems(_stringItems[i]);
        connect(_icomboList[i]->combolist, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboListConfigChanged(int)));
        _icomboList[i]->combolist->setProperty("comboListID",qVariantFromValue(i));
    }

    setDesc(trs("MachineConfigModuleMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void MachineConfigModule::readyShow()
{
    int tmpValue;

    for(int i=MODULE_TYPE_IBP; i<MODULE_TYPE_NR; i++)
    {
        tmpValue = 0;
        systemConfig.getNumValue(QString("MachineConfigModule|%1").arg(_stringTypeName[i]), tmpValue);
        if(tmpValue<0)
        {
            _icomboList[i]->combolist->setCurrentIndex(0);
        }
        else
        {
            _icomboList[i]->combolist->setCurrentIndex(tmpValue);
        }
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void MachineConfigModule::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    for(int i=MODULE_TYPE_IBP; i<MODULE_TYPE_NR; i++)
    {
        _icomboList[i]->setFont(fontManager.textFont(fontSize));
        _icomboList[i]->label->setFixedSize(labelWidth, ITEM_H);
        _icomboList[i]->combolist->setFixedSize(btnWidth, ITEM_H);
        mainLayout->addWidget(_icomboList[i],0,Qt::AlignRight);
        mainLayout->addStretch();
    }
}

void MachineConfigModule::onComboListConfigChanged(int index)
{
    ComboList *combo = qobject_cast<ComboList *>(sender());
    if(!combo)
    {
        qdebug("Invalid signal sender.");
        return;
    }

    int comboId = combo->property("comboListID").toInt();

    if(comboId>=MODULE_TYPE_IBP && comboId< MODULE_TYPE_NR)
    {
        systemConfig.setNumValue(QString("MachineConfigModule|%1").arg(_stringTypeName[comboId]),
                                  index);
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
MachineConfigModule::~MachineConfigModule()
{

}
