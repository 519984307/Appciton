#include "SoftWareVersion.h"
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

SoftWareVersion *SoftWareVersion::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
SoftWareVersion::SoftWareVersion() : SubMenu(trs("SoftWareVersionMenu"))
{
    QString stringTemp[]={
        "SystemSoftwareVersion", "PowerManagerSoftwareVersion", "Uboot",
        "Kernel", "KeyboardModule", "RecorderModule", "ECGAlgorithmType",
        "BootAndStandbyLogoVersion", "NULL"
    };
    for(unsigned int i=SOFT_TYPE_SYSTEM_SOFTWARE_VERSION; i<SOFT_TYPE_NR && i < sizeof(stringTemp); i++)
    {
        _stringTypeName[i] = stringTemp[i];
    }

    for(int i=SOFT_TYPE_SYSTEM_SOFTWARE_VERSION; i<SOFT_TYPE_NR; i++)
    {
        _labelType[i].typeName = new QLabel(trs(QString("%1%2").arg(trs(_stringTypeName[i])).arg(":")));
        _labelType[i].typeName->setAlignment(Qt::AlignRight);
        _labelType[i].typeInfo = new QLabel(trs("NULL"));
        _labelType[i].typeInfo->setAlignment(Qt::AlignHCenter);
    }

    setDesc(trs("UserMaintainGeneralMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void SoftWareVersion::readyShow()
{
    QString tmpStr;

    for(int i=SOFT_TYPE_SYSTEM_SOFTWARE_VERSION; i<SOFT_TYPE_NR; i++)
    {
        tmpStr.clear();
        systemConfig.getStrValue(QString("SoftWareVersion|%1").arg(_stringTypeName[i]), tmpStr);
        if(tmpStr.isEmpty())
        {
            _labelType[i].typeInfo->setText(trs("NULL"));
        }
        else
        {
             _labelType[i].typeInfo->setText(trs(tmpStr));
        }
    }
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void SoftWareVersion::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;


    for(int i=SOFT_TYPE_SYSTEM_SOFTWARE_VERSION; i<SOFT_TYPE_NR; i++)
    {
        QHBoxLayout *hlayout=new QHBoxLayout;

        _labelType[i].typeName->setFont(fontManager.textFont(fontSize));
        _labelType[i].typeName->setFixedSize(labelWidth, ITEM_H);
        hlayout->addWidget(_labelType[i].typeName);

        _labelType[i].typeInfo->setFont(fontManager.textFont(fontSize));
        _labelType[i].typeInfo->setFixedSize(labelWidth, ITEM_H);
        hlayout->addWidget(_labelType[i].typeInfo);

        mainLayout->addLayout(hlayout,Qt::AlignRight);
        mainLayout->addStretch();
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
SoftWareVersion::~SoftWareVersion()
{

}
