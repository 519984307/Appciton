#include <QVBoxLayout>
#include <QLabel>
#include "FontManager.h"
#include "LanguageManager.h"
#include "LabelButton.h"
#include "IComboList.h"
#include "FactorySystemInfoMenu.h"
#include "IConfig.h"
#include "KeyBoardPanel.h"
#include "Debug.h"
#include "IButton.h"
#include "FactoryWindowManager.h"
#include "FactoryMaintainManager.h"

FactorySystemInfoMenu *FactorySystemInfoMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactorySystemInfoMenu::FactorySystemInfoMenu() : SubMenu(trs("SystemInfoSetting"))
{

    setDesc(trs("SystemInfoSettingDesc"));
    startLayout();
}

void FactorySystemInfoMenu::layoutExec()
{
    int submenuW = factoryMaintainManager.getSubmenuWidth();
    int submenuH = factoryMaintainManager.getSubmenuHeight();
    setMenuSize(submenuW, submenuH);

    int itemW = submenuW - ICOMBOLIST_SPACE;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;
    int labelWidth = itemW - btnWidth;

    _serialNum = new LabelButton(trs("SerialNum"));
    _serialNum->setFont(fontManager.textFont(fontSize));
    _serialNum->label->setFixedSize(labelWidth, ITEM_H);
    _serialNum->label->setAlignment(Qt::AlignRight);
    _serialNum->button->setFixedSize(btnWidth, ITEM_H);
    connect(_serialNum->button, SIGNAL(realReleased()), this, SLOT(_serialNumReleased()));
    mainLayout->addWidget(_serialNum,0,Qt::AlignRight);
    mainLayout->addStretch();
}
/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactorySystemInfoMenu::readyShow()
{
    QString str;
    machineConfig.getStrValue("SerialNumber", str);
    _serialNum->setValue(str);
    _serialNum->setFocus();
}

/**************************************************************************************************
 * 序列号槽函数。
 *************************************************************************************************/
void FactorySystemInfoMenu::_serialNumReleased()
{
    QString text;
    int maxLen = 11;

    KeyBoardPanel panel;
    panel.setMaxInputLength(maxLen);
    panel.setInitString(_serialNum->button->text());
    panel.setTitleBarText(trs("SerialNum"));
    panel.setSpaceEnable(false);
    panel.setBtnEnable("[a-zA-Z0-9]");

    if (1 == panel.exec())
    {
        QString oldStr = _serialNum->button->text();
        text = panel.getStrValue();

        if (oldStr != text && !text.isNull())
        {
            _serialNum->setValue(text);
            machineConfig.setStrValue("SerialNumber", text);
            machineConfig.save();
            machineConfig.saveToDisk();
        }
    }
}

/**************************************************************************************************
 * 析构。
 *************************************************************************************************/
FactorySystemInfoMenu::~FactorySystemInfoMenu()
{

}
