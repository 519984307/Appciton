#include "UserMaintainGeneralSet.h"
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

UserMaintainGeneralSet *UserMaintainGeneralSet::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
UserMaintainGeneralSet::UserMaintainGeneralSet() : SubMenu(trs("UserMaintainGeneralMenu"))
{
    setDesc(trs("UserMaintainGeneralMenuDesc"));

    startLayout();
}

/**************************************************************************************************
 * 显示。
 *************************************************************************************************/
void UserMaintainGeneralSet::readyShow()
{
    QString tmpStr;
    systemConfig.getStrValue("General|MonitorName", tmpStr);
    _monitorName->setValue(tmpStr);

    tmpStr.clear();
    systemConfig.getStrValue("General|UserMaintainPassword", tmpStr);
    _modifyPassword->setValue(tmpStr);

    tmpStr.clear();
    systemConfig.getStrValue("General|Department", tmpStr);
    _department->setValue(tmpStr);

    tmpStr.clear();
    systemConfig.getStrValue("General|BedNumber", tmpStr);
    _bedNumber->setValue(tmpStr);

    int value = 0;
    systemConfig.getNumValue("General|ChangeBedNumberRight", value);
    _changeBedNumberRight->setCurrentIndex(value);


    systemConfig.getNumValue("General|Language", value);
    _language->setCurrentIndex(value);

}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void UserMaintainGeneralSet::layoutExec()
{
    int submenuW = userMaintainManager.getSubmenuWidth();
    int submenuH = userMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    //monitor Name
    _monitorName = new LabelButton(trs("Monitor Name"));
    _monitorName->setFont(fontManager.textFont(fontSize));
    _monitorName->label->setFixedSize(labelWidth, ITEM_H);
    _monitorName->button->setFixedSize(btnWidth, ITEM_H);
    connect(_monitorName->button, SIGNAL(released(int)), this, SLOT(_editMonitorName()));
    mainLayout->addWidget(_monitorName);

    //Department
    _department = new LabelButton(trs("Department"));
    _department->label->setFixedSize(labelWidth, ITEM_H);
    _department->button->setFixedSize(btnWidth, ITEM_H);
    _department->setFont(fontManager.textFont(fontSize));
    connect(_department->button, SIGNAL(released(int)), this, SLOT(_editDepartment()));
    mainLayout->addWidget(_department);

    //BedNumber
    _bedNumber = new LabelButton(trs("BedNumber"));
    _bedNumber->label->setFixedSize(labelWidth, ITEM_H);
    _bedNumber->button->setFixedSize(btnWidth, ITEM_H);
    _bedNumber->setFont(fontManager.textFont(fontSize));
    connect(_bedNumber->button, SIGNAL(released(int)), this, SLOT(_editBedNumber()));
    mainLayout->addWidget(_bedNumber);

    //Modify Password
    _modifyPassword = new LabelButton(trs("Modify Password"));
    _modifyPassword->label->setFixedSize(labelWidth, ITEM_H);
    _modifyPassword->button->setFixedSize(btnWidth, ITEM_H);
    _modifyPassword->setFont(fontManager.textFont(fontSize));
    connect(_modifyPassword->button, SIGNAL(released(int)), this, SLOT(_passwordReleased()));
    mainLayout->addWidget(_modifyPassword);

    //Change Bed Number Right
    _changeBedNumberRight = new IComboList(trs("Change Bed Number Right"));
    _changeBedNumberRight->label->setFixedSize(labelWidth, ITEM_H);
    _changeBedNumberRight->combolist->setFixedSize(btnWidth, ITEM_H);
    _changeBedNumberRight->setFont(fontManager.textFont(fontSize));
    _changeBedNumberRight->addItem(trs("Protected"));
    _changeBedNumberRight->addItem(trs("Open"));
    connect(_changeBedNumberRight->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_setChangeBedNumberRight(int)));
    mainLayout->addWidget(_changeBedNumberRight);


    //Language setup
    _language = new IComboList(trs("Language"));
    _language->label->setFixedSize(labelWidth, ITEM_H);
    _language->combolist->setFixedSize(btnWidth, ITEM_H);
    _language->setFont(fontManager.textFont(fontSize));
    _language->addItem(trs("English"));
    _language->addItem(trs("Chinese"));
    connect(_language->combolist, SIGNAL(currentIndexChanged(int)),
            this, SLOT(_languageSetup(int)));
    mainLayout->addWidget(_language);
}

/**************************************************************************************************
 * MonitorName槽函数。
 *************************************************************************************************/
void UserMaintainGeneralSet::_editMonitorName()
{
    KeyBoardPanel idPanel;
    idPanel.setTitleBarText(trs("Edit Monitor Name"));
    idPanel.setInitString(_monitorName->button->text());
    idPanel.setMaxInputLength(MAX_DEVICE_ID_LEN - 1);

    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    idPanel.setBtnEnable(regKeyStr);

    if (1 == idPanel.exec())
    {
        QString oldStr = _monitorName->button->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            _monitorName->setValue(text);
            systemConfig.setStrValue("General|MonitorName", text);
        }
    }
}

/**************************************************************************************************
 * UserMaintainPassword槽函数。
 *************************************************************************************************/
void UserMaintainGeneralSet::_passwordReleased()
{
    KeyBoardPanel idPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    idPanel.setTitleBarText(trs("Modify Password"));
    idPanel.setInitString(_modifyPassword->button->text());
    idPanel.setMaxInputLength(11);
    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    idPanel.setBtnEnable(regKeyStr);

    if (1 == idPanel.exec())
    {
        QString oldStr = _modifyPassword->button->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            _modifyPassword->setValue(text);
            systemConfig.setStrValue("General|UserMaintainPassword", text);
        }
    }
}
/**************************************************************************************************
 * Department槽函数。
 *************************************************************************************************/
void UserMaintainGeneralSet::_editDepartment()
{
    KeyBoardPanel idPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    idPanel.setTitleBarText(trs("Department"));
    idPanel.setInitString(_department->button->text());
    idPanel.setMaxInputLength(11);
    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    idPanel.setBtnEnable(regKeyStr);

    if (1 == idPanel.exec())
    {
        QString oldStr = _department->button->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            _department->setValue(text);
            systemConfig.setStrValue("General|Department", text);
        }
    }
}

/**************************************************************************************************
 * Bed Number槽函数。
 *************************************************************************************************/
void UserMaintainGeneralSet::_editBedNumber()
{
    KeyBoardPanel idPanel(KeyBoardPanel::KEY_TYPE_NUMBER);
    idPanel.setTitleBarText(trs("Bed Number"));
    idPanel.setInitString(_bedNumber->button->text());
    idPanel.setMaxInputLength(11);
    QString regKeyStr("[a-zA-Z]|[0-9]|_");
    idPanel.setBtnEnable(regKeyStr);

    if (1 == idPanel.exec())
    {
        QString oldStr = _bedNumber->button->text();
        QString text = idPanel.getStrValue();

        if (oldStr != text)
        {
            _bedNumber->setValue(text);
            systemConfig.setStrValue("General|BedNumber", text);
        }
    }
}
/**************************************************************************************************
 * Change Bed Number Right。
 *************************************************************************************************/
void UserMaintainGeneralSet::_setChangeBedNumberRight(int index)
{
    systemConfig.setNumValue("General|ChangeBedNumberRight", index);
}

/**************************************************************************************************
 * Language setup。
 *************************************************************************************************/
void UserMaintainGeneralSet::_languageSetup(int index)
{
    systemConfig.setNumValue("General|Language", index);
}


/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
UserMaintainGeneralSet::~UserMaintainGeneralSet()
{

}
