/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/

#include "CO2MenuContent.h"
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
#include "MainMenuWindow.h"

class CO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED = 0,
        ITEM_CBO_WAVE_RULER,
        ITEM_CBO_FICO2_DISPLAY,

        ITEM_BTN_O2_COMPEN = 0,
        ITEM_BTN_N2O_COMPEN,
        ITEM_BTN_ZERO_CALIB,
    };

    CO2MenuContentPrivate() {}

    /**
     * @brief loadOptions  // load settings
     */
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> btns;
};

CO2MenuContent::CO2MenuContent():
    MenuContent(trs("CO2Menu") , trs("CO2MenuDesc")),
    d_ptr(new CO2MenuContentPrivate)
{
}

CO2MenuContent::~CO2MenuContent()
{
    delete d_ptr;
}

void CO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void CO2MenuContentPrivate::loadOptions()
{
    // 波形速度。
    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(co2Param.getSweepSpeed());

    combos[ITEM_CBO_WAVE_RULER]->setCurrentIndex(co2Param.getDisplayZoom());

    // 气体补偿。
    btns[ITEM_BTN_O2_COMPEN]->setText(QString::number(co2Param.getCompensation(CO2_COMPEN_O2)));
    btns[ITEM_BTN_N2O_COMPEN]->setText(QString::number(co2Param.getCompensation(CO2_COMPEN_N2O)));

    // 显示控制。
    combos[ITEM_CBO_FICO2_DISPLAY]->setCurrentIndex(co2Param.getFICO2Display());
}

void CO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int indexType = combos->property("Item").toInt();

    switch (indexType)
    {
    case CO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
        co2Param.setSweepSpeed((CO2SweepSpeed)index);
        break;
    case CO2MenuContentPrivate::ITEM_CBO_WAVE_RULER:
        co2Param.setDisplayZoom(static_cast<CO2DisplayZoom>(index));
        break;
    case CO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY:
        co2Param.setFiCO2Display((CO2FICO2Display)index);
        break;
    };
}

void CO2MenuContent::onBtnReleasedChanged()
{
    Button *button = qobject_cast<Button *>(sender());
    int index = button->property("Btn").toInt();

    KeyInputPanel numberInput;
    unsigned num = 1000;
    switch (index)
    {
    case CO2MenuContentPrivate::ITEM_BTN_O2_COMPEN:
        numberInput.setWindowTitle(trs("O2Compensation"));
        numberInput.setMaxInputLength(3);
        numberInput.setInitString(button->text());
        if (numberInput.exec())
        {
            QString strValue = numberInput.getStrValue();
            num = strValue.toShort();
            if (num <= 100)
            {
                co2Param.setCompensation(CO2_COMPEN_O2, num);
                button->setText(strValue);
            }
            else
            {
                MessageBox messageBox(trs("O2Compensation"), trs("InvalidInput") + " 0-100 ", QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
        break;
    case CO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN:
        numberInput.setWindowTitle(trs("N2OCompensation"));
        numberInput.setMaxInputLength(3);
        numberInput.setInitString(button->text());
        if (numberInput.exec())
        {
            QString strValue = numberInput.getStrValue();
            num = strValue.toShort();
            if (num <= 100)
            {
                co2Param.setCompensation(CO2_COMPEN_N2O, num);
                button->setText(strValue);
            }
            else
            {
                MessageBox messageBox(trs("N2OCompensation"), trs("InvalidInput") + " 0-100 ",
                                       QStringList(trs("EnglishYESChineseSURE")));
                messageBox.exec();
            }
        }
        break;
    case CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB:
        co2Param.zeroCalibration();
        break;
    default:
        break;
    }
}

void CO2MenuContent::onAlarmBtnReleased()
{
    MainMenuWindow *w = MainMenuWindow::getInstance();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_ETCO2, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void CO2MenuContent::layoutExec()
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
                      );
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = CO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // co2 ruler
    label = new QLabel(trs("Ruler"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    int maxZoom = CO2_DISPLAY_ZOOM_NR;
    float zoomArray[CO2_DISPLAY_ZOOM_NR] = {4.0, 8.0, 12.0, 20.0};
    QString str;
    UnitType unit = co2Param.getUnit();
    for (int i = 0; i < maxZoom; i++)
    {
        str.clear();
        if (unit == UNIT_KPA)
        {
            float tempVal = Unit::convert(UNIT_KPA, UNIT_PERCENT, zoomArray[i]).toFloat();
            str = QString("0.0~%1").arg(QString::number(tempVal, 'f', 1));
        }
        else if (unit == UNIT_MMHG)
        {
            str = "0~";
            int tempVal = Unit::convert(UNIT_MMHG, UNIT_PERCENT, zoomArray[i]).toInt();
            tempVal = (tempVal + 5) / 10 * 10;
            str += QString::number(tempVal);
        }
        else
        {
            str = QString("0.0~%1").arg(QString::number(zoomArray[i], 'f', 1));
        }
        str += " ";
        str += Unit::localeSymbol(unit);
        comboBox->addItem(str);
    }
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = static_cast<int>(CO2MenuContentPrivate::ITEM_CBO_WAVE_RULER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_WAVE_RULER, comboBox);

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
    itemID = CO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_FICO2_DISPLAY, comboBox);

    int row = d_ptr->combos.count();

    // o2 compensation
    label = new QLabel(trs("O2Compensation"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setText(QString(80));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    itemID = CO2MenuContentPrivate::ITEM_BTN_O2_COMPEN;
    button->setProperty("Btn", itemID);
    d_ptr->btns.insert(CO2MenuContentPrivate::ITEM_BTN_O2_COMPEN, button);

    // n2o compensation
    label = new QLabel(trs("N2OCompensation"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button("20");
    button->setButtonStyle(Button::ButtonTextOnly);
//    button->setText("20");
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    itemID = CO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN;
    button->setProperty("Btn", itemID);
    d_ptr->btns.insert(CO2MenuContentPrivate::ITEM_BTN_N2O_COMPEN, button);

    // zero calibration
    label = new QLabel(trs("CO2ZeroCalib"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setText(trs("CO2ZeroStart"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    itemID = CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB;
    button->setProperty("Btn", itemID);
    d_ptr->btns.insert(CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB, button);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, (row + d_ptr->btns.count()), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch((row + d_ptr->btns.count() + 1), 1);
}

