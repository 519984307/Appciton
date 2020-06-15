/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/10
 **/

#include "COMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include "COSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "COParam.h"
#include "IConfig.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"
#include "MainMenuWindow.h"
#include "AlarmLimitWindow.h"
#include "WindowManager.h"
#include "COMeasureWindow.h"

class COMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CO_CONST = 1,
        ITEM_CBO_TI_SOURCE,
        ITEM_BTN_MANUAL_TI,
        ITEM_BTN_INJECTATE_VOLUME,
        ITEM_BTN_CO_MEASURE,
    };

    COMenuContentPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
};

void COMenuContentPrivate::loadOptions()
{
    short coef = coParam.getCatheterCoeff();
    buttons[ITEM_BTN_CO_CONST]->setText(QString::number(coef * 1.0 / 1000, 'f', 3));

    combos[ITEM_CBO_TI_SOURCE]->blockSignals(true);
    combos[ITEM_CBO_TI_SOURCE]->setCurrentIndex(coParam.getTiSource());
    combos[ITEM_CBO_TI_SOURCE]->blockSignals(false);
    if (combos[ITEM_CBO_TI_SOURCE]->currentIndex() == CO_TI_SOURCE_AUTO)
    {
        buttons[ITEM_BTN_MANUAL_TI]->setEnabled(false);
    }
    else
    {
        buttons[ITEM_BTN_MANUAL_TI]->setEnabled(true);
    }
    short ti = coParam.getManualTi();
    QString text = QString::number(ti * 1.0 / 10, 'f', 1);
    buttons[ITEM_BTN_MANUAL_TI]->setText(text);

    buttons[ITEM_BTN_INJECTATE_VOLUME]->setText(QString::number(coParam.getInjectionVolume()));
}

COMenuContent::COMenuContent()
    : MenuContent(trs("COMenu"), trs("COMenuDesc")),
      d_ptr(new COMenuContentPrivate)
{
}

COMenuContent::~COMenuContent()
{
    delete d_ptr;
}

void COMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void COMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    Button *button;
    QLabel *label;
    int itemID;
    int count = 0;

    // computation constant
    label = new QLabel(trs("ComputationConst"));
    layout->addWidget(label, count, 0);
    button = new Button();
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_BTN_CO_CONST);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_BTN_CO_CONST, button);
    count++;

    // TI source
    label = new QLabel(trs("InjectateTempSource"));
    layout->addWidget(label, count, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(COSymbol::convert(CO_TI_SOURCE_AUTO))
                       << trs(COSymbol::convert(CO_TI_SOURCE_MANUAL)));
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_TI_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, count, 1);
    d_ptr->combos.insert(COMenuContentPrivate::ITEM_CBO_TI_SOURCE, comboBox);
    count++;

    // manual TI Temperature
    label = new QLabel(QString("%1 (%2)").arg(trs("InjectateTemp")).arg(trs("celsius")));
    layout->addWidget(label, count, 0);
    button = new Button("20");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_BTN_MANUAL_TI);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_BTN_MANUAL_TI, button);
    count++;

    // injection volume
    label = new QLabel(QString("%1 (ml)").arg(trs("InjectateVolume")));
    layout->addWidget(label, count, 0);
    button = new Button("10");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_BTN_INJECTATE_VOLUME);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_BTN_INJECTATE_VOLUME, button);
    count++;

    button = new Button(trs("COMeasure"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_BTN_CO_MEASURE);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_BTN_CO_MEASURE, button);
    count++;

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, count, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));
    count++;

    layout->setRowStretch(count, 1);
}

void COMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        int itemID = box->property("Item").toInt();
        COMenuContentPrivate::MenuItem item = static_cast<COMenuContentPrivate::MenuItem>(itemID);
        switch (item)
        {
        case COMenuContentPrivate::ITEM_CBO_TI_SOURCE:
        {
            COTiSource tiSrc = static_cast<COTiSource>(index);
            if (tiSrc == CO_TI_SOURCE_MANUAL)
            {
                d_ptr->buttons.value(COMenuContentPrivate::ITEM_BTN_MANUAL_TI)->setEnabled(true);
            }
            else if (tiSrc == CO_TI_SOURCE_AUTO)
            {
                d_ptr->buttons.value(COMenuContentPrivate::ITEM_BTN_MANUAL_TI)->setEnabled(false);
            }
            int temp = d_ptr->buttons[COMenuContentPrivate::ITEM_BTN_MANUAL_TI]->text().toFloat() * 10;
            coParam.setTiSource(tiSrc, temp);
            break;
        }
        default:
            break;
        }
    }
}

void COMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        int itemID = button->property("Item").toInt();
        COMenuContentPrivate::MenuItem item = static_cast<COMenuContentPrivate::MenuItem>(itemID);
        switch (item) {
        case COMenuContentPrivate::ITEM_BTN_CO_CONST:
        {
            // 调用数字键盘
            KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER, true);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            numberPad.setBtnEnable(rec);
            // 设置键盘标题
            numberPad.setWindowTitle(trs("ComputationConst"));
            // 最大输入长度
            numberPad.setMaxInputLength(5);
            // 固定为数字键盘
            numberPad.setKeytypeSwitchEnable(false);
            numberPad.setSymbolEnable(false);
            // 设置初始字符串 placeholder模式
            numberPad.setInitString(button->text(), true);

            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                float value = text.toFloat(&ok);
                u_int16_t actualValue = value * 1000;
                if (ok)
                {
                    if (actualValue >= 1 && actualValue <= 999)
                    {
                        button->setText(text);
                        coParam.setCatheterCoeff(actualValue);
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"),
                                              trs("InvalidInput") + " 0.001-0.999 ",
                                              QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case COMenuContentPrivate::ITEM_BTN_MANUAL_TI:
        {
             // 调用数字键盘
            KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER, true);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            numberPad.setBtnEnable(rec);
            // 设置键盘标题
            numberPad.setWindowTitle(trs("InjectateTemp"));
            // 最大输入长度
            numberPad.setMaxInputLength(4);
            // 固定为数字键盘
            numberPad.setKeytypeSwitchEnable(false);
            numberPad.setSymbolEnable(false);
            // 设置初始字符串 placeholder模式
            numberPad.setInitString(button->text(), true);

            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                float value = text.toFloat(&ok);
                u_int16_t actualValue = value * 10;
                if (ok)
                {
                    if (actualValue <= 270)
                    {
                        button->setText(text);
                        coParam.setTiSource(CO_TI_SOURCE_MANUAL, actualValue);
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"),
                                              trs("InvalidInput") + " 0.0-27.0 ",
                                              QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case COMenuContentPrivate::ITEM_BTN_INJECTATE_VOLUME:
        {
            // 调用数字键盘
            KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER, false);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            numberPad.setBtnEnable(rec);
            numberPad.setSpaceEnable(false);
            // 设置键盘标题
            numberPad.setWindowTitle(trs("InjectateVolume"));
            // 最大输入长度
            numberPad.setMaxInputLength(3);
            // 固定为数字键盘
            numberPad.setKeytypeSwitchEnable(false);
            numberPad.setSymbolEnable(false);
            // 设置初始字符串 placeholder模式
            numberPad.setInitString(button->text(), true);

            if (numberPad.exec())
            {
                QString text = numberPad.getStrValue();
                bool ok = false;
                unsigned int value = text.toInt(&ok);
                if (ok)
                {
                    if (value >= 1 && value <= 200)
                    {
                        button->setText(text);
                        coParam.setInjectionVolume((unsigned char)value);
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"),
                                              trs("InvalidInput") + " 1-200 ",
                                              QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case COMenuContentPrivate::ITEM_BTN_CO_MEASURE:
        {
            COMeasureWindow *w = coParam.getMeasureWindow();
            windowManager.showWindow(w, WindowManager::ShowBehaviorCloseOthers
                                     | WindowManager::ShowBehaviorModal
                                     | WindowManager::ShowBehaviorNoAutoClose);
            break;
        }
        default:
            break;
        }
    }
}

void COMenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_CO_TB, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}
