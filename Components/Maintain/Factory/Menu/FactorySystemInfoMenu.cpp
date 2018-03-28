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
FactorySystemInfoMenu::FactorySystemInfoMenu() : PopupWidget()
{
    setCloseBtnTxt("");
    setCloseBtnPic(QImage("/usr/local/nPM/icons/main.png"));
    setCloseBtnColor(Qt::transparent);

//    setDesc(trs("SystemInfoSetting"));

//    startLayout();
    layoutExec();
}

/**************************************************************************************************
 * 布局。
 *************************************************************************************************/
void FactorySystemInfoMenu::layoutExec()
{
    int submenuW = factoryWindowManager.getSubmenuWidth();
    int submenuH = factoryWindowManager.getSubmenuHeight();
    setFixedSize(submenuW, submenuH);

    setTitleBarText(trs("SystemInfoSetting"));

    int itemW = submenuW - 200;
    int fontSize = fontManager.getFontSize(1);
    int btnWidth = itemW / 2;

    _serialNum = new LabelButton(trs("SerialNum"));
    _serialNum->setFont(fontManager.textFont(fontSize));
    _serialNum->label->setFixedSize(btnWidth, ITEM_H);
    _serialNum->label->setAlignment(Qt::AlignCenter);
    _serialNum->button->setFixedSize(btnWidth, ITEM_H);
    connect(_serialNum->button, SIGNAL(realReleased()), this, SLOT(_serialNumReleased()));
    contentLayout->addWidget(_serialNum);

    contentLayout->addStretch();

    contentLayout->setSpacing(10);
}

void FactorySystemInfoMenu::keyPressEvent(QKeyEvent *e)
{
    switch (e->key())
    {
        case Qt::Key_Up:
        case Qt::Key_Left:
            focusNextPrevChild(false);
            return;
        case Qt::Key_Down:
        case Qt::Key_Right:
            focusNextChild();
            return;
        default:
            break;
    }

    PopupWidget::keyPressEvent(e);
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
