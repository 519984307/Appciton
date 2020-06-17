/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
 **/

#include "ConfigEditCOMenuContent.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include "COSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "COParam.h"
#include "ConfigManager.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"

class ConfigEditCOMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_CO_CONST = 0,
        ITEM_CBO_TI_SOURCE,
        ITEM_BTN_MANUAL_TI,
        ITEM_BTN_INJECTATE_VOLUME,
    };

    explicit ConfigEditCOMenuContentPrivate(Config *const config)
          : config(config)
    {
    }

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
    Config * const config;
};

void ConfigEditCOMenuContentPrivate::loadOptions()
{
    buttons[ITEM_BTN_CO_CONST]->blockSignals(true);
    buttons[ITEM_BTN_MANUAL_TI]->blockSignals(true);
    buttons[ITEM_BTN_INJECTATE_VOLUME]->blockSignals(true);
    combos[ITEM_CBO_TI_SOURCE]->blockSignals(true);

    int number = 0;
    config->getNumValue("CO|ComputationConst", number);
    buttons[ITEM_BTN_CO_CONST]->setText(QString::number(number * 1.0 / 1000, 'f', 3));
    number = 0;
    config->getNumValue("CO|TISource", number);
    combos[ITEM_CBO_TI_SOURCE]->setCurrentIndex(number);
    number = 0;
    if (combos[ITEM_CBO_TI_SOURCE]->currentIndex() == CO_TI_SOURCE_AUTO)
    {
        buttons[ITEM_BTN_MANUAL_TI]->setEnabled(false);
    }
    else
    {
        buttons[ITEM_BTN_MANUAL_TI]->setEnabled(true);
    }

    number = 20;
    config->getNumValue("CO|InjectateTemp", number);
    buttons[ITEM_BTN_MANUAL_TI]->setText(QString::number(number * 1.0 / 10, 'f', 1));
    number = 0;
    config->getNumValue("CO|InjectateVolume", number);
    buttons[ITEM_BTN_INJECTATE_VOLUME]->setText(QString::number(number));

    buttons[ITEM_BTN_CO_CONST]->blockSignals(false);
    buttons[ITEM_BTN_MANUAL_TI]->blockSignals(false);
    buttons[ITEM_BTN_INJECTATE_VOLUME]->blockSignals(false);
    combos[ITEM_CBO_TI_SOURCE]->blockSignals(false);
}

ConfigEditCOMenuContent::ConfigEditCOMenuContent(Config * const config)
    : MenuContent(trs("COMenu"), trs("COMenuDesc")),
      d_ptr(new ConfigEditCOMenuContentPrivate(config))
{
}

ConfigEditCOMenuContent::~ConfigEditCOMenuContent()
{
    delete d_ptr;
}

void ConfigEditCOMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();
    d_ptr->combos[ConfigEditCOMenuContentPrivate::
            ITEM_CBO_TI_SOURCE]->setEnabled(!isOnlyToRead);
    d_ptr->buttons[ConfigEditCOMenuContentPrivate::
            ITEM_BTN_CO_CONST]->setEnabled(!isOnlyToRead);
    d_ptr->buttons[ConfigEditCOMenuContentPrivate::
            ITEM_BTN_MANUAL_TI]->setEnabled(!isOnlyToRead);
    d_ptr->buttons[ConfigEditCOMenuContentPrivate::
            ITEM_BTN_INJECTATE_VOLUME]->setEnabled(!isOnlyToRead);
}

void ConfigEditCOMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    Button *button;
    QLabel *label;
    int itemID;
    int count = 0;

    // Computation Constant
    label = new QLabel(trs("ComputationConst"));
    layout->addWidget(label, count, 0);
    button = new Button("0.542");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_BTN_CO_CONST);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_BTN_CO_CONST, button);
    count++;

    // temp source
    label = new QLabel(trs("InjectateTempSource"));
    layout->addWidget(label, count, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(COSymbol::convert(CO_TI_SOURCE_AUTO))
                       << trs(COSymbol::convert(CO_TI_SOURCE_MANUAL)));
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_CBO_TI_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, count, 1);
    d_ptr->combos.insert(ConfigEditCOMenuContentPrivate::ITEM_CBO_TI_SOURCE, comboBox);
    count++;

    // injection temp
    label = new QLabel(QString("%1 (%2)").arg(trs("InjectateTemp")).arg(trs("celsius")));
    layout->addWidget(label, count, 0);
    button = new Button("20");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_BTN_MANUAL_TI);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_BTN_MANUAL_TI, button);
    count++;

    // injection volumn
    label = new QLabel(QString("%1 (ml)").arg(trs("InjectateVolume")));
    layout->addWidget(label, count, 0);
    button = new Button("10");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(ConfigEditCOMenuContentPrivate::ITEM_BTN_INJECTATE_VOLUME);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, count, 1);
    d_ptr->buttons.insert(ConfigEditCOMenuContentPrivate::ITEM_BTN_INJECTATE_VOLUME, button);
    count++;

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));
    count++;

    layout->setRowStretch(count, 1);
}

void ConfigEditCOMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        int itemID = box->property("Item").toInt();
        ConfigEditCOMenuContentPrivate::MenuItem item;
        item = static_cast<ConfigEditCOMenuContentPrivate::MenuItem>(itemID);
        switch (item)
        {
        case ConfigEditCOMenuContentPrivate::ITEM_CBO_TI_SOURCE:
        {
            if (index == static_cast<int>(CO_TI_SOURCE_MANUAL))
            {
                d_ptr->buttons.value(ConfigEditCOMenuContentPrivate::ITEM_BTN_MANUAL_TI)->setEnabled(true);
            }
            else if (index == static_cast<int>(CO_TI_SOURCE_AUTO))
            {
                d_ptr->buttons.value(ConfigEditCOMenuContentPrivate::ITEM_BTN_MANUAL_TI)->setEnabled(false);
            }
            int temp = d_ptr->buttons[ConfigEditCOMenuContentPrivate::ITEM_BTN_MANUAL_TI]->text().toFloat() * 10;
            d_ptr->config->setNumValue("CO|InjectateTemp", (unsigned)temp);
            d_ptr->config->setNumValue("CO|TISource", index);
            break;
        }
        default:
            break;
        }
    }
}

void ConfigEditCOMenuContent::onButtonReleased()
{
    Button *button = qobject_cast<Button *>(sender());
    if (button)
    {
        int itemID = button->property("Item").toInt();;
        ConfigEditCOMenuContentPrivate::MenuItem item;
        item = static_cast<ConfigEditCOMenuContentPrivate::MenuItem>(itemID);
        switch (item)
        {
        case ConfigEditCOMenuContentPrivate::ITEM_BTN_CO_CONST:
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
                int16_t actualValue = value * 1000;
                if (ok)
                {
                    if (actualValue >= 1 && actualValue <= 999)
                    {
                        button->setText(text);
                        d_ptr->config->setNumValue("CO|ComputationConst", static_cast<int>(actualValue));
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"),
                                              trs("InvalidInput") + "0.001-0.999",
                                              QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ConfigEditCOMenuContentPrivate::ITEM_BTN_MANUAL_TI:
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
                int16_t actualValue = value * 10;
                if (ok)
                {
                    if (actualValue <= 270)
                    {
                        button->setText(text);
                        d_ptr->config->setNumValue("CO|TISource", static_cast<int>(CO_TI_SOURCE_MANUAL));
                        d_ptr->config->setNumValue("CO|InjectateTemp", static_cast<int>(actualValue));
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"),
                                              trs("InvalidInput") + "0.0-27.0",
                                              QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        case ConfigEditCOMenuContentPrivate::ITEM_BTN_INJECTATE_VOLUME:
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
                        d_ptr->config->setNumValue("CO|InjectateVolume", static_cast<int>(value));
                    }
                    else
                    {
                        MessageBox messageBox(trs("Prompt"),
                                              trs("InvalidInput") + "1-200",
                                              QStringList(trs("EnglishYESChineseSURE")));
                        messageBox.exec();
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

void ConfigEditCOMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_CO_CO, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}
