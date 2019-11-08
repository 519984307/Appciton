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
#include "Framework/Language/LanguageManager.h"
#include "Button.h"
#include "ComboBox.h"
#include <QGridLayout>
#include "CO2Symbol.h"
#include "CO2Param.h"
#include "ConfigManager.h"
#include "KeyInputPanel.h"
#include "MessageBox.h"
#include "MainMenuWindow.h"
#include "RESPParam.h"
#include "IConfig.h"
#include "AlarmLimitWindow.h"
#include "SpinBox.h"

class CO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED = 0,
        ITEM_CBO_WAVE_RULER,
        ITEM_CBO_APNEA_TIME,
        ITEM_CBO_WORK_MODE,

        ITEM_BTN_O2_COMPEN,
        ITEM_BTN_N2O_COMPEN,
        ITEM_BTN_ZERO_CALIB
    };

    CO2MenuContentPrivate()
        : o2Spb(NULL), n2oSpb(NULL), o2Lbl(NULL), n2oLbl(NULL)
    {}

    /**
     * @brief loadOptions  // load settings
     */
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> btns;
    SpinBox *o2Spb;
    SpinBox *n2oSpb;
    QLabel *o2Lbl;
    QLabel *n2oLbl;
};

CO2MenuContent::CO2MenuContent():
    MenuContent(trs("CO2Menu") , trs("CO2MenuDesc")),
    d_ptr(new CO2MenuContentPrivate)
{
    connect(&co2Param, SIGNAL(updateZeroSta(bool)), this, SLOT(disableZero(bool)));
    connect(&co2Param, SIGNAL(updateCompensation(CO2Compensation, bool)), this,
            SLOT(onUpdateCompensation(CO2Compensation, bool)));
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

    // 波形标尺140.08
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
    combos[ITEM_CBO_WAVE_RULER]->setCurrentIndex(co2Param.getDisplayZoom());

    // 气体补偿。
    o2Spb->blockSignals(true);
    o2Spb->setEnabled(co2Param.getCompensationEnabled(CO2_COMPEN_O2));
    o2Spb->setValue(co2Param.getCompensation(CO2_COMPEN_O2));
    o2Spb->blockSignals(false);
    n2oSpb->blockSignals(true);
    o2Spb->setEnabled(co2Param.getCompensationEnabled(CO2_COMPEN_N2O));
    n2oSpb->setValue(co2Param.getCompensation(CO2_COMPEN_N2O));
    n2oSpb->blockSignals(false);

    ApneaAlarmTime index = co2Param.getApneaTime();

    combos[ITEM_CBO_APNEA_TIME]->setCurrentIndex(index);

    combos[ITEM_CBO_WORK_MODE]->blockSignals(true);
    combos[ITEM_CBO_WORK_MODE]->setCurrentIndex(static_cast<int>(co2Param.getCO2Switch()));
    combos[ITEM_CBO_WORK_MODE]->blockSignals(false);

    if (co2Param.isConnected())
    {
        btns[ITEM_BTN_ZERO_CALIB]->setEnabled(!co2Param.getDisableZeroStatus());
        combos[ITEM_CBO_WORK_MODE]->setEnabled(true);
    }
    else
    {
        btns[ITEM_BTN_ZERO_CALIB]->setEnabled(false);
        combos[ITEM_CBO_WORK_MODE]->setEnabled(false);
    }

    QString co2Str;
    machineConfig.getStrValue("CO2", co2Str);
    if (co2Str == "BLM_CO2")
    {
        o2Spb->setVisible(false);
        n2oSpb->setVisible(false);
        o2Lbl->setVisible(false);
        n2oLbl->setVisible(false);
    }
    else
    {
        o2Spb->setVisible(true);
        n2oSpb->setVisible(true);
        o2Lbl->setVisible(true);
        n2oLbl->setVisible(true);
    }
}

void CO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int indexType = combos->property("Item").toInt();

    switch (indexType)
    {
    case CO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
        co2Param.setSweepSpeed(static_cast<CO2SweepSpeed>(index));
        break;
    case CO2MenuContentPrivate::ITEM_CBO_WAVE_RULER:
        co2Param.setDisplayZoom(static_cast<CO2DisplayZoom>(index));
        break;
    case CO2MenuContentPrivate::ITEM_CBO_APNEA_TIME:
        currentConfig.setNumValue("CO2|ApneaTime", index);
        co2Param.setApneaTime(static_cast<ApneaAlarmTime>(index));
        // 联动resp的窒息时间
        currentConfig.setNumValue("RESP|ApneaDelay", index);
        respParam.setApneaTime(static_cast<ApneaAlarmTime>(index));
        break;
    case CO2MenuContentPrivate::ITEM_CBO_WORK_MODE:
        co2Param.setModuleWorkMode(static_cast<CO2WorkMode>(index + 1));
        break;
    default:
        break;
    }
}

void CO2MenuContent::onBtnReleasedChanged()
{
    Button *button = qobject_cast<Button *>(sender());
    int index = button->property("Btn").toInt();

    switch (index)
    {
    case CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB:
        co2Param.zeroCalibration();
        break;
    default:
        break;
    }
}

void CO2MenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_ETCO2, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}

void CO2MenuContent::o2CompenSpinboxReleased(int value, int scale)
{
    co2Param.setCompensation(CO2_COMPEN_O2, value * scale);
}

void CO2MenuContent::n2oCompenSpinboxReleased(int value, int scale)
{
    co2Param.setCompensation(CO2_COMPEN_N2O, value * scale);
}

void CO2MenuContent::disableZero(bool sta)
{
    d_ptr->btns[CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB]->setEnabled(!sta);
}

void CO2MenuContent::onUpdateCompensation(CO2Compensation gas, bool enable)
{
    if (gas == CO2_COMPEN_O2)
    {
        d_ptr->o2Spb->setEnabled(enable);
    }
    else if (gas == CO2_COMPEN_N2O)
    {
        d_ptr->n2oSpb->setEnabled(enable);
    }
}

void CO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    ComboBox *comboBox;
    Button *button;
    QLabel *label;
    SpinBox *spinBox;
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
    itemID = CO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // co2 ruler
    label = new QLabel(trs("Ruler"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = static_cast<int>(CO2MenuContentPrivate::ITEM_CBO_WAVE_RULER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_WAVE_RULER, comboBox);

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
    itemID = CO2MenuContentPrivate::ITEM_CBO_APNEA_TIME;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_APNEA_TIME, comboBox);



    // CO2 work mode
    label = new QLabel(trs("CO2WorkMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("CO2Standby")
                       << trs("CO2Measure"));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = CO2MenuContentPrivate::ITEM_CBO_WORK_MODE;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(CO2MenuContentPrivate::ITEM_CBO_WORK_MODE, comboBox);

    int row = d_ptr->combos.count();

    // zero calibration
    label = new QLabel(trs("CO2ZeroCalib"));
    layout->addWidget(label, row, 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    button->setText(trs("CO2ZeroStart"));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    itemID = CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB;
    button->setProperty("Btn", itemID);
    d_ptr->btns.insert(CO2MenuContentPrivate::ITEM_BTN_ZERO_CALIB, button);

    row = row + d_ptr->btns.count();

    // o2 compensation
    label = new QLabel(trs("O2Compensation"));
    layout->addWidget(label, row, 0);
    spinBox = new SpinBox();
    spinBox->setRange(0, 100);
    spinBox->setStep(1);
    layout->addWidget(spinBox, row, 1);
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(o2CompenSpinboxReleased(int, int)));
    d_ptr->o2Spb = spinBox;
    d_ptr->o2Lbl = label;


    // n2o compensation
    label = new QLabel(trs("N2OCompensation"));
    layout->addWidget(label, row + 1, 0);
    spinBox = new SpinBox();
    spinBox->setRange(0, 100);
    spinBox->setStep(1);
    layout->addWidget(spinBox, row + 1, 1);
    connect(spinBox, SIGNAL(valueChange(int, int)), this, SLOT(n2oCompenSpinboxReleased(int, int)));
    d_ptr->n2oSpb = spinBox;
    d_ptr->n2oLbl = label;

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, (row + 2), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(row + 3, 1);
}

