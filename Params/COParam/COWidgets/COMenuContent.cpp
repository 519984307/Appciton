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

class COMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_CO_RATIO = 1,
        ITEM_CBO_INJECT_TEMP_SOURCE,
        ITEM_CBO_INJECTION_TEMP,
        ITEM_CBO_INJECTION_VOLUMN,
        ITEM_CBO_MEASURE_CONTROL,
    };

    COMenuContentPrivate() : measureSta(CO_MEASURE_STOP){}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> buttons;
    COMeasureCtrl measureSta;              // 测量状态
};

void COMenuContentPrivate::loadOptions()
{
    buttons[ITEM_CBO_CO_RATIO]->setText(QString::number(static_cast<double>(coParam.getCORatio() * 1.0 / 1000)));
    combos[ITEM_CBO_INJECT_TEMP_SOURCE]->setCurrentIndex(coParam.getTempSource());
    if (combos[ITEM_CBO_INJECT_TEMP_SOURCE]->currentIndex() == CO_TI_SOURCE_AUTO)
    {
        buttons[ITEM_CBO_INJECTION_TEMP]->setEnabled(false);
    }
    else
    {
        buttons[ITEM_CBO_INJECTION_TEMP]->setEnabled(true);
    }
    QString text = QString::number(static_cast<double>(coParam.getInjectionTemp() * 1.0 / 10));
    buttons[ITEM_CBO_INJECTION_TEMP]->setText(text);
    buttons[ITEM_CBO_INJECTION_VOLUMN]->setText(QString::number(coParam.getInjectionVolumn()));
    buttons[ITEM_CBO_MEASURE_CONTROL]->setText(trs(COSymbol::convert(coParam.getMeasureCtrl())));
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

    // Ratio
    label = new QLabel(trs("CORatio"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("0.542");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_CO_RATIO);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_CO_RATIO, button);

    // temp source
    label = new QLabel(trs("InjectionTempSource"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(COSymbol::convert(CO_TI_SOURCE_AUTO))
                       << trs(COSymbol::convert(CO_TI_SOURCE_MANUAL)));
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->combos.insert(COMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE, comboBox);

    // injection temp
    label = new QLabel(trs("InjectionTemp"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("20");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP, button);

    // injection volumn
    label = new QLabel(trs("InjectionVolumn"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button("10");
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN, button);

    // measure contrl
    label = new QLabel(trs("MeasureControl"));
    layout->addWidget(label, d_ptr->buttons.count() + d_ptr->combos.count(), 0);
    button = new Button(trs("COStart"));
    button ->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(COMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL);
    button->setProperty("Item", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onButtonReleased()));
    layout->addWidget(button, d_ptr->buttons.count() + d_ptr->combos.count(), 1);
    d_ptr->buttons.insert(COMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL, button);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count() + d_ptr->buttons.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count() + 1, 1);
}

void COMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        COMenuContentPrivate::MenuItem item
                = (COMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case COMenuContentPrivate::ITEM_CBO_INJECT_TEMP_SOURCE:
        {
            if (index == static_cast<int>(CO_TI_SOURCE_MANUAL))
            {
                d_ptr->buttons.value(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP)->setEnabled(true);
            }
            else if (index == static_cast<int>(CO_TI_SOURCE_AUTO))
            {
                d_ptr->buttons.value(COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP)->setEnabled(false);
            }
            int temp = d_ptr->buttons[COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP]->text().toFloat() * 10;
            coParam.setTempSource((COTiSource)index, temp);
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
        COMenuContentPrivate::MenuItem item
                 = (COMenuContentPrivate::MenuItem) button->property("Item").toInt();
        switch (item) {
        case COMenuContentPrivate::ITEM_CBO_CO_RATIO:
        {
            // 调用数字键盘
            KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER, true);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            numberPad.setBtnEnable(rec);
            // 设置键盘标题
            numberPad.setWindowTitle(trs("CORatio"));
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
                        coParam.setCORatio(actualValue);
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
        case COMenuContentPrivate::ITEM_CBO_INJECTION_TEMP:
        {
             // 调用数字键盘
            KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER, true);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            numberPad.setBtnEnable(rec);
            // 设置键盘标题
            numberPad.setWindowTitle(trs("InjectionTemp"));
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
                        coParam.setTempSource(CO_TI_SOURCE_MANUAL, actualValue);
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
        case COMenuContentPrivate::ITEM_CBO_INJECTION_VOLUMN:
        {
            // 调用数字键盘
            KeyInputPanel numberPad(KeyInputPanel::KEY_TYPE_NUMBER, true);
            // 使能键盘的有效字符
            QString rec("[0-9]");
            numberPad.setBtnEnable(rec);
            // 设置键盘标题
            numberPad.setWindowTitle(trs("InjectionVolumn"));
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
                        coParam.setInjectionVolumn((unsigned char)value);
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
        case COMenuContentPrivate::ITEM_CBO_MEASURE_CONTROL:
        {
            coParam.measureCtrl(d_ptr->measureSta);
            if (d_ptr->measureSta == CO_MEASURE_STOP)
            {
                button->setText(trs("COEnd"));
                d_ptr->measureSta = CO_MEASURE_START;
            }
            else if (d_ptr->measureSta == CO_MEASURE_START)
            {
                button->setText(trs("COStart"));
                d_ptr->measureSta = CO_MEASURE_STOP;
            }
            break;
        }
        default:
            break;
        }
    }
}

void COMenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_CO_CO, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}
