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

FactorySystemInfoMenu *FactorySystemInfoMenu::_selfObj = NULL;

/**************************************************************************************************
 * 构造。
 *************************************************************************************************/
FactorySystemInfoMenu::FactorySystemInfoMenu() : MenuWidget(trs("SystemInfoSetting"))
{
    int submenuW = factoryWindowManager.getSubmenuWidth();
    int submenuH = factoryWindowManager.getSubmenuHeight();
    setFixedSize(submenuW, submenuH);

    int itemW = submenuW - 200;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;

    _serialNum = new LabelButton(trs("SerialNum"));
    _serialNum->setFont(fontManager.textFont(fontSize));
    _serialNum->label->setFixedSize(btnWidth, ITEM_H);
    _serialNum->label->setAlignment(Qt::AlignCenter);
    _serialNum->button->setFixedSize(btnWidth, ITEM_H);
    connect(_serialNum->button, SIGNAL(realReleased()), this, SLOT(_serialNumReleased()));

    QVBoxLayout *labelLayout = new QVBoxLayout();
    labelLayout->setContentsMargins(50, 0, 50, 0);
    labelLayout->setSpacing(10);
    labelLayout->setAlignment(Qt::AlignTop);
    labelLayout->addWidget(_serialNum);

    labelLayout->addStretch();

    labelLayout->setSpacing(10);

    mainLayout->addLayout(labelLayout);
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
