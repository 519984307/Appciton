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
#include "KeyInputPanel.h"
#include "MessageBox.h"
#include "RESPParam.h"

class ConfigEditCO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED = 0,
        ITEM_CBO_FICO2_DISPLAY,
        ITEM_CBO_APNEA_TIME,

        ITEM_BTN_O2_COMPEN = 0,
        ITEM_BTN_N2O_COMPEN,
    };

    explicit ConfigEditCO2MenuContentPrivate(Config *const config)
        : config(config)
    {}

    /**
     * @brief loadOptions  // load settings
     */
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> btns;
    Config *const config;
};

ConfigEditCO2MenuContent::ConfigEditCO2MenuContent(Config *const config):
    MenuContent(trs("CO2Menu"), trs("CO2MenuDesc")),
    d_ptr(new ConfigEditCO2MenuContentPrivate(config))
{
}

ConfigEditCO2MenuContent::~ConfigEditCO2MenuContent()
{
    delete d_ptr;
}

void ConfigEditCO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();
    d_ptr->combos[ConfigEditCO2MenuContentPrivate
            ::ITEM_CBO_WAVE_SPEED]->setEnabled(!isOnlyToRead);
    d_ptr->combos[ConfigEditCO2MenuContentPrivate
            ::ITEM_CBO_FICO2_DISPLAY]->setEnabled(!isOnlyToRead);
    d_ptr->combos[ConfigEditCO2MenuContentPrivate
            ::ITEM_CBO_APNEA_TIME]->setEnabled(!isOnlyToRead);
    d_ptr->btns[ConfigEditCO2MenuContentPrivate
            ::ITEM_BTN_O2_COMPEN]->setEnabled(!isOnlyToRead);
    d_ptr->btns[ConfigEditCO2MenuContentPrivate
            ::ITEM_BTN_N2O_COMPEN]->setEnabled(!isOnlyToRead);
}


void ConfigEditCO2MenuContentPrivate::loadOptions()
{
    int index = 0;

    // 波形速度。
    config->getNumValue("CO2|SweepSpeed", index);
    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(index);

    // 气体补偿。
    config->getNumValue("CO2|O2Compensation", index);
    btns[ITEM_BTN_O2_COMPEN]->setText(QString::number(index));
    config->getNumValue("CO2|NO2Compensation", index);
    btns[ITEM_BTN_N2O_COMPEN]->setText(QString::number(index));

    // 显示控制。
    config->getNumValue("CO2|FICO2Display", index);
    combos[ITEM_CBO_FICO2_DISPLAY]->setCurrentIndex(index);

    config->getNumValue("Alarm|ApneaTime", index);
    combos[ITEM_CBO_APNEA_TIME]->setCurrentIndex(index);
}

void ConfigEditCO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int indexType = combos->property("Item").toInt();
    switch (indexType)
    {
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
        d_ptr->config->setNumValue("CO2|SweepSpeed", index);
        break;
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY:
        d_ptr->config->setNumValue("CO2|FICO2Display", index);
        break;
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_APNEA_TIME:
        d_ptr->config->setNumValue("Alarm|ApneaTime", index);
        break;
    };
}

void ConfigEditCO2MenuContent::onBtnReleasedChanged()
{
    Button *button = qobject_cast<Button *>(sender());
    int index = button->property("Btn").toInt();

    // 调用数字键盘
    KeyInputPanel numberInput(KeyInputPanel::KEY_TYPE_NUMBER);
    // 最大输入长度
    numberInput.setMaxInputLength(3);
    // 固定为数字键盘
    numberInput.setKeytypeSwitchEnable(false);
    numberInput.setSymbolEnable(false);
    numberInput.setSpaceEnable(false);
    // 设置初始字符串 placeholder模式
    numberInput.setInitString(button->text(), true);

    unsigned num = 1000;
    switch (index)
    {
    case ConfigEditCO2MenuContentPrivate::ITEM_BTN_O2_COMPEN:
        numberInput.setWindowTitle(trs("O2Compensation"));
        if (numberInput.exec())
        {
            QString strValue = numberInput.getStrValue();
            num = strValue.toShort();
            if (num <= 100)
            {
                d_ptr->config->setNumValue("CO2|O2Compensation", num);
                button->setText(QString::number(num));
            }
            else
            {
                MessageBox messageBox(trs("O2Compensation"), trs("InvalidInput") + "0-100", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
        break;
    case ConfigEditCO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN:
        numberInput.setWindowTitle(trs("N2OCompensation"));
        if (numberInput.exec())
        {
            QString strValue = numberInput.getStrValue();
            num = strValue.toShort();
            if (num <= 100)
            {
                d_ptr->config->setNumValue("CO2|N2OCompensation", num);
                button->setText(QString::number(num));
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

void ConfigEditCO2MenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_ETCO2, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void ConfigEditCO2MenuContent::layoutExec()
{
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
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_500)
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
                       << trs(CO2Symbol::convert(CO2_FICO2_DISPLAY_OFF))
                       << trs(CO2Symbol::convert(CO2_FICO2_DISPLAY_ON))
                      );
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY, comboBox);

    // apnea time
    label = new QLabel(trs("ApneaDelay"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_OFF))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_20_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_25_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_30_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_35_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_40_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_45_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_50_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_55_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_60_SEC))
                      );
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_CBO_APNEA_TIME;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_APNEA_TIME, comboBox);

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

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, (row + d_ptr->btns.count()), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch((row + d_ptr->btns.count() + 1), 1);
}

