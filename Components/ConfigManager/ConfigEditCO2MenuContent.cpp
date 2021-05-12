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
#include "Framework/UI/Button.h"
#include "Framework/UI/SpinBox.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include "CO2Symbol.h"
#include "CO2Param.h"
#include "ConfigManager.h"
#include "RESPParam.h"

class ConfigEditCO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED = 0,
        ITEM_CBO_WAVE_RULER,
        ITEM_CBO_APNEA_TIME,

        ITEM_SPIN_O2_COMPEN = 0,
        ITEM_SPIN_N2O_COMPEN,
    };

    explicit ConfigEditCO2MenuContentPrivate(Config *const config)
        : config(config)
    {}

    /**
     * @brief loadOptions  // load settings
     */
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, SpinBox *> spins;
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
            ::ITEM_CBO_APNEA_TIME]->setEnabled(!isOnlyToRead);
    d_ptr->spins[ConfigEditCO2MenuContentPrivate
            ::ITEM_SPIN_O2_COMPEN]->setEnabled(!isOnlyToRead);
    d_ptr->spins[ConfigEditCO2MenuContentPrivate
            ::ITEM_SPIN_N2O_COMPEN]->setEnabled(!isOnlyToRead);
    d_ptr->combos[ConfigEditCO2MenuContentPrivate
            ::ITEM_CBO_WAVE_RULER]->setEnabled(!isOnlyToRead);
}

void ConfigEditCO2MenuContentPrivate::loadOptions()
{
    int index = 0;

    // 波形速度。
    config->getNumValue("CO2|SweepSpeed", index);
    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(index);

    // 气体补偿。
    index = 0;  // init the 'index'
    config->getNumValue("CO2|O2Compensation", index);
    spins[ITEM_SPIN_O2_COMPEN]->setValue(index);

    index = 0;  // init the 'index'
    config->getNumValue("CO2|N2OCompensation", index);
    spins[ITEM_SPIN_N2O_COMPEN]->setValue(index);

    // 显示控制。
    index = 0;  // init the 'index'
    config->getNumValue("CO2|ApneaTime", index);
    combos[ITEM_CBO_APNEA_TIME]->setCurrentIndex(index);

    // co2标尺
    combos[ITEM_CBO_WAVE_RULER]->blockSignals(true);
    combos[ITEM_CBO_WAVE_RULER]->clear();
    int maxZoom = CO2_DISPLAY_ZOOM_NR;
    float zoomArray[CO2_DISPLAY_ZOOM_NR] = {4.0, 8.0, 12.0, 20.0};
    QString str;
    UnitType unit = co2Param.getUnit();
    for (int i = 0; i < maxZoom; i++)
    {
        str.clear();
        if (unit == UNIT_MMHG)
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
        str += trs(Unit::getSymbol(unit));
        combos[ITEM_CBO_WAVE_RULER]->addItem(str);
    }
    combos[ITEM_CBO_WAVE_RULER]->blockSignals(false);
    index = 0;  // init the 'index'
    config->getNumValue("CO2|DisplayZoom", index);
    combos[ITEM_CBO_WAVE_RULER]->setCurrentIndex(index);
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
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_RULER:
        d_ptr->config->setNumValue("CO2|DisplayZoom", index);
        break;
    case ConfigEditCO2MenuContentPrivate::ITEM_CBO_APNEA_TIME:
        d_ptr->config->setNumValue("CO2|ApneaTime", index);
        // 联动resp窒息延迟
        d_ptr->config->setNumValue("RESP|ApneaDelay", index);
        break;
    };
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

void ConfigEditCO2MenuContent::o2CompenSpinboxReleased(int value, int scale)
{
    d_ptr->config->setNumValue("CO2|O2Compensation", value * scale);
}

void ConfigEditCO2MenuContent::n2oCompenSpinboxReleased(int value, int scale)
{
    d_ptr->config->setNumValue("CO2|N2OCompensation", value * scale);
}

void ConfigEditCO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // wave speed
    label = new QLabel(trs("CO2SweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    /*
     * disable 50mm/s for CO2 wave, because the CO2 wave sample rate is too low
     * and the scan gap is too large when in 50mm/s
     */
    comboBox->addItems(QStringList()
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_62_5)
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_125)
                       << CO2Symbol::convert(CO2_SWEEP_SPEED_250));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // co2 ruler
    label = new QLabel(trs("Ruler"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = static_cast<int>(ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_RULER);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_WAVE_RULER, comboBox);

    // apnea time
    label = new QLabel(trs("ApneaDelay"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_20_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_25_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_30_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_35_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_40_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_45_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_50_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_55_SEC))
                       << trs(CO2Symbol::convert(CO2_APNEA_TIME_60_SEC)));

    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditCO2MenuContentPrivate::ITEM_CBO_APNEA_TIME;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditCO2MenuContentPrivate::ITEM_CBO_APNEA_TIME, comboBox);

    int row = d_ptr->combos.count();

    // o2 compensation
    label = new QLabel(trs("O2Compensation"));
    layout->addWidget(label, row, 0);
    SpinBox *spinBox = new SpinBox();
    spinBox->setRange(0, 100);
    spinBox->setStep(1);
    layout->addWidget(spinBox, row, 1);
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(o2CompenSpinboxReleased(int, int)));
    d_ptr->spins.insert(ConfigEditCO2MenuContentPrivate::ITEM_SPIN_O2_COMPEN, spinBox);


    // n2o compensation
    label = new QLabel(trs("N2OCompensation"));
    layout->addWidget(label, row + 1, 0);
    spinBox = new SpinBox();
    spinBox->setRange(0, 100);
    spinBox->setStep(1);
    layout->addWidget(spinBox, row + 1, 1);
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(n2oCompenSpinboxReleased(int, int)));
    d_ptr->spins.insert(ConfigEditCO2MenuContentPrivate::ITEM_SPIN_N2O_COMPEN, spinBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, (row + d_ptr->spins.count()), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch((row + d_ptr->spins.count() + 1), 1);
}

