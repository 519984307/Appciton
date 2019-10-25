/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/16
 **/

#include "UserMaintainGeneralMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "Button.h"
#include "KeyInputPanel.h"
#include "PatientManager.h"
#include "WindowManager.h"
#include "MessageBox.h"

class UserMaintainGeneralMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_MONITOR_NAME,
        ITEM_BTN_DEPARTMENT,
        ITEM_BTN_BED_NUMBER,
        ITEM_BTN_MODIFY_PASSWORD,
        ITEM_CBO_CHANGE_BEDNUMBER_RIGHT,
        ITEM_CBO_LANGUAGE
    };

    UserMaintainGeneralMenuContentPrivate() {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
};

void UserMaintainGeneralMenuContentPrivate::loadOptions()
{
    QString str;
    systemConfig.getStrValue("General|MonitorName", str);
    buttons[ITEM_BTN_MONITOR_NAME]->setText(str);

    systemConfig.getStrValue("General|Department", str);
    buttons[ITEM_BTN_DEPARTMENT]->setText(str);

    systemConfig.getStrValue("General|BedNumber", str);
    buttons[ITEM_BTN_BED_NUMBER]->setText(str);

    systemConfig.getStrValue("General|UserMaintainPassword", str);
    buttons[ITEM_BTN_MODIFY_PASSWORD]->setText(str);

    int index = 0;
    systemConfig.getNumValue("General|ChangeBedNumberRight", index);
    combos[ITEM_CBO_CHANGE_BEDNUMBER_RIGHT]->setCurrentIndex(index);

    combos[ITEM_CBO_LANGUAGE]->blockSignals(true);
    systemConfig.getNumAttr("General|Language" , "NextOption", index);
    combos[ITEM_CBO_LANGUAGE]->setCurrentIndex(index);
    combos[ITEM_CBO_LANGUAGE]->blockSignals(false);
}

UserMaintainGeneralMenuContent::UserMaintainGeneralMenuContent()
    : MenuContent(trs("UserMaintainGeneralMenu"), trs("UserMaintainGeneralMenuDesc")),
      d_ptr(new UserMaintainGeneralMenuContentPrivate)
{
}

UserMaintainGeneralMenuContent::~UserMaintainGeneralMenuContent()
{
    delete d_ptr;
}

void UserMaintainGeneralMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void UserMaintainGeneralMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    Button *button;
    int itemID;

    // monitor name
    label = new QLabel(trs("EditMonitorName"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MONITOR_NAME);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MONITOR_NAME, button);

    // department
    label = new QLabel(trs("Department"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_DEPARTMENT);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_DEPARTMENT, button);

    // bed Number
    label = new QLabel(trs("BedNumber"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_BED_NUMBER);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_BED_NUMBER, button);

    // modify password
    label = new QLabel(trs("ModifyPassword"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MODIFY_PASSWORD);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->buttons.insert(UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MODIFY_PASSWORD, button);

    // change bed number right
    label = new QLabel(trs("ChangeBedNumberRight"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Protected")
                       << trs("Open"));
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_CHANGE_BEDNUMBER_RIGHT);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_CHANGE_BEDNUMBER_RIGHT, comboBox);

    // language setup
    label = new QLabel(trs("Language"));
    layout->addWidget(label, d_ptr->combos.count() + d_ptr->buttons.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("English")
                       << trs("Chinese"));
    itemID = static_cast<int>(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_LANGUAGE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    d_ptr->combos.insert(UserMaintainGeneralMenuContentPrivate::ITEM_CBO_LANGUAGE, comboBox);

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count(), 1);
}

void UserMaintainGeneralMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        UserMaintainGeneralMenuContentPrivate::MenuItem item
                = (UserMaintainGeneralMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case UserMaintainGeneralMenuContentPrivate::ITEM_CBO_CHANGE_BEDNUMBER_RIGHT:
            systemConfig.setNumValue("General|ChangeBedNumberRight", index);
            break;
        case UserMaintainGeneralMenuContentPrivate::ITEM_CBO_LANGUAGE:
        {
            systemConfig.setNumAttr("General|Language" , "NextOption" , index);
            MessageBox msg(trs("Prompt"), trs("ChangeLanguageTip"), false, true);
            windowManager.showWindow(&msg, WindowManager::ShowBehaviorNoAutoClose
                                     | WindowManager::ShowBehaviorModal);
            break;
        }
        default:
            break;
        }
    }
}

static bool checkPasswordValue(const QString &passwordValue)
{
        if (passwordValue.isEmpty())
        {
            return false;
        }
        int len = passwordValue.length();
        if (len != 8)
        {
            return false;
        }
        return true;
}


void UserMaintainGeneralMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        UserMaintainGeneralMenuContentPrivate::MenuItem item
                = (UserMaintainGeneralMenuContentPrivate::MenuItem)button->property("Item").toInt();
        switch (item)
        {
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MONITOR_NAME:
        {
            KeyInputPanel idPanel;
            idPanel.setWindowTitle(trs("EditMonitorName"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(MAX_DEVICE_ID_LEN - 1);

            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);

            windowManager.showWindow(&idPanel, WindowManager::ShowBehaviorModal |
                                     WindowManager::ShowBehaviorNoAutoClose);
            if (QDialog::Accepted == idPanel.result())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|MonitorName", text);
                }
            }
            break;
        }
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_DEPARTMENT:
        {
            KeyInputPanel idPanel;
            idPanel.setWindowTitle(trs("Department"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(11);
            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);
            windowManager.showWindow(&idPanel, WindowManager::ShowBehaviorModal |
                                     WindowManager::ShowBehaviorNoAutoClose);
            if (QDialog::Accepted == idPanel.result())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|Department", text);
                }
            }
            break;
        }
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_BED_NUMBER:
        {
            KeyInputPanel idPanel;
            idPanel.setWindowTitle(trs("BedNumber"));
            idPanel.setInitString(button->text());
            idPanel.setMaxInputLength(11);
            QString regKeyStr("[a-zA-Z]|[0-9]|_");
            idPanel.setBtnEnable(regKeyStr);

            windowManager.showWindow(&idPanel, WindowManager::ShowBehaviorModal |
                                     WindowManager::ShowBehaviorNoAutoClose);
            if (QDialog::Accepted == idPanel.result())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    patientManager.setBedNum(text);
                }
            }
            break;
        }
        case UserMaintainGeneralMenuContentPrivate::ITEM_BTN_MODIFY_PASSWORD:
        {
            // 调用数字键盘
            KeyInputPanel idPanel(KeyInputPanel::KEY_TYPE_NUMBER);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            idPanel.setBtnEnable(rec);
            // 设置键盘标题
            idPanel.setWindowTitle(trs("ModifyPassword"));
            // 最大输入长度
            idPanel.setMaxInputLength(8);
            // 固定为数字键盘
            idPanel.setKeytypeSwitchEnable(false);
            idPanel.setSymbolEnable(false);
            idPanel.setSpaceEnable(false);
            // 设置初始字符串 placeholder模式
            QString lastPassword = QString("%1:%2").arg(trs("OldPassword")).arg(button->text());
            idPanel.setInitString(lastPassword, true);

            idPanel.setCheckValueHook(checkPasswordValue);
            QString invaildHint = QString(trs("Input8DigitsPassword"));
            idPanel.setInvalidHint(invaildHint);

            windowManager.showWindow(&idPanel, WindowManager::ShowBehaviorModal |
                                     WindowManager::ShowBehaviorNoAutoClose);
            if (QDialog::Accepted == idPanel.result())
            {
                QString oldStr = button->text();
                QString text = idPanel.getStrValue();

                if (oldStr != text)
                {
                    button->setText(text);
                    systemConfig.setStrValue("General|UserMaintainPassword", text);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}
