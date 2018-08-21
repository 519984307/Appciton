/**
** This file is part of the nPM project.
** Copyright (C) Better Life Medical Technology Co., Ltd.
** All Rights Reserved.
** Unauthorized copying of this file, via any medium is strictly prohibited
** Proprietary and confidential
**
** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
**/
#include "ConfigEditCO2MenuContent.h"
#include <QMap>
#include <QLabel>
#include "LanguageManager.h"
#include "Button.h"
#include "ComboBox.h"
#include <QGridLayout>
#include "CO2Symbol.h"
#include "CO2Param.h"
#include "ConfigManager.h"
#include "NumberInput.h"
#include "MessageBox.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditCO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED = 0,
        ITEM_CBO_FICO2_DISPLAY,

        ITEM_BTN_O2_COMPEN = 0,
        ITEM_BTN_N2O_COMPEN,
    };

    explicit ConfigEditCO2MenuContentPrivate(ConfigEditCO2MenuContent * const q_ptr)
        :q_ptr(q_ptr)
    {}

    /**
     * @brief loadOptions  // load settings
     */
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> btns;
    ConfigEditCO2MenuContent * const q_ptr;
};

ConfigEditCO2MenuContent::ConfigEditCO2MenuContent():
    MenuContent(trs("CO2Menu"), trs("CO2MenuDesc")),
    d_ptr(new ConfigEditCO2MenuContentPrivate(this))
{
}

ConfigEditCO2MenuContent::~ConfigEditCO2MenuContent()
{
    delete d_ptr;
}

void ConfigEditCO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
}


void ConfigEditCO2MenuContentPrivate::loadOptions()
{
    ConfigEditMenuWindow *w = qobject_cast<ConfigEditMenuWindow *>(q_ptr->getMenuWindow());
    Config *config = w->getCurrentEditConfig();
    int index = 0;

    // 波形速度。
    config->getNumValue("CO2|SweepSpeed", index);
    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(index);

    // 气体补偿。
    config->getNumValue("CO2|Compensation|O2", index);
    btns[ITEM_BTN_O2_COMPEN]->setText(QString::number(index));
    config->getNumValue("CO2|Compensation|NO2", index);
    btns[ITEM_BTN_N2O_COMPEN]->setText(QString::number(index));

    // 显示控制。
    config->getNumValue("CO2|FICO2Display", index);
    combos[ITEM_CBO_FICO2_DISPLAY]->setCurrentIndex(index);
}

void ConfigEditCO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int indexType = combos->property("Item").toInt();
    ConfigEditMenuWindow *w = qobject_cast<ConfigEditMenuWindow *>(this->getMenuWindow());
    Config *config = w->getCurrentEditConfig();
    switch (indexType)
    {
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
        config->setNumValue("CO2|SweepSpeed", index);
        break;
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY:
        config->setNumValue("CO2|FICO2Display", index);
        break;
    };
}

void ConfigEditCO2MenuContent::onBtnReleasedChanged()
{
    Button *button = qobject_cast<Button *>(sender());
    int index = button->property("Btn").toInt();

    ConfigEditMenuWindow * w = qobject_cast<ConfigEditMenuWindow *>(this->getMenuWindow());
    Config *config = w->getCurrentEditConfig();
    NumberInput numberInput;
    unsigned num = 1000;
    switch (index)
    {
    case ConfigEditCO2MenuContentPrivate::ITEM_BTN_O2_COMPEN:
        numberInput.setTitleBarText(trs("O2Compensation"));
        numberInput.setMaxInputLength(3);
        numberInput.setInitString(button->text());
        if (numberInput.exec())
        {
            QString strValue = numberInput.getStrValue();
            num = strValue.toShort();
            if (num <= 100)
            {
                config->setNumValue("CO2|Compensation|O2", num);
                button->setText(strValue);
            }
            else
            {
                MessageBox messageBox(trs("O2Compensation"), trs("InvalidInput") + "0-100", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
        break;
    case ConfigEditCO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN:
        numberInput.setTitleBarText(trs("N2OCompensation"));
        numberInput.setMaxInputLength(3);
        numberInput.setInitString(button->text());
        if (numberInput.exec())
        {
            QString strValue = numberInput.getStrValue();
            num = strValue.toShort();
            if (num <= 100)
            {
                config->setNumValue("CO2|Compensation|NO2", num);
                button->setText(strValue);
            }
            else
            {
                MessageBox messageBox(trs("N2OCompensation"), trs("InvalidInput") + "0-100",
                                       QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
        break;
    default:
        break;
    }
}

void ConfigEditCO2MenuContent::layoutExec()
{
    if (layout())
    {
        // already install layout
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    ComboBox *comboBox;
    Button *button;
    QLabel *label;
    int itemID;

    // wave speed
    label = new QLabel(trs("CO2SweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_62_5)
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_125)
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_250)
                      );
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // fico2 display
    label = new QLabel(trs("CO2FiCO2Display"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << CO2Symbol::convert(CO2_FICO2_DISPLAY_OFF)
                       << CO2Symbol::convert(CO2_FICO2_DISPLAY_ON)
                      );
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY, comboBox);

    int row = d_ptr->combos.count();

    // o2 compensation
    label = new QLabel(trs("O2Compensation"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setText(QString(80));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_BTN_O2_COMPEN;
    button->setProperty("Btn", itemID);
    d_ptr->btns.insert(ConfigEditCO2MenuContentPrivate::ITEM_BTN_O2_COMPEN, button);

    // n2o compensation
    label = new QLabel(trs("N2OCompensation"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button("20");
    button->setButtonStyle(Button::ButtonTextOnly);
//    button->setText("20");
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN;
    button->setProperty("Btn", itemID);
    d_ptr->btns.insert(ConfigEditCO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN, button);

    layout->setRowStretch((row + d_ptr->btns.count()), 1);
}

