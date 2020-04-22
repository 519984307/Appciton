/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/11
 **/

#include "SPO2MenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QLabel>
#include <QGridLayout>
#include <QList>
#include "SPO2Symbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "SPO2Param.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "MainMenuWindow.h"
#include "AlarmLimitWindow.h"
#include "ECGParam.h"
#include "NightModeManager.h"
#include "CCHDWindow.h"
#include "PatientManager.h"
#include "WindowManager.h"

class SPO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED,
        ITEM_CBO_AVERAGE_TIME,
        ITEM_CBO_SENSITIVITY,
        ITEM_CBO_FAST_SAT,
        ITEM_CBO_SMART_TONE,
        ITEM_CBO_PULSE_VOL,
        ITEM_CBO_NIBP_SAME_SIDE,
        ITEM_CBO_SIGNAL_IQ,
        ITEM_CBO_SPO2_LINE_FREQ,
        ITEM_CBO_SPO2_SENSOR,
        ITEM_CBO_SPHB_PRECISION_MODE,
        ITEM_CBO_SPHB_VESSEL_MODE,
        ITEM_CBO_SPHB_AVERAGING_MODE,
        ITEM_CBO_SPHB_UNIT,
        ITEM_CBO_PVI_AVERAGING_MODE
    };

    SPO2MenuContentPrivate()
        : cchdBtn(NULL),
          moduleType(MODULE_BLM_S5)
    {}

    /**
     * @brief setCboBlockSignalsStatus  设置cobomo信号锁住状态
     * @param isBlocked  true--锁住  false--解开
     */
    void setCboBlockSignalsStatus(bool isBlocked);

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, QLabel *> seniorParamLbl;
    Button *cchdBtn;

    SPO2ModuleType moduleType;
};

void SPO2MenuContentPrivate::setCboBlockSignalsStatus(bool isBlocked)
{
    for (int i = ITEM_CBO_WAVE_SPEED; i <= ITEM_CBO_PVI_AVERAGING_MODE; i++)
    {
        MenuItem item = static_cast<MenuItem>(i);
        if (combos[item])
        {
            combos[item]->blockSignals(isBlocked);
        }
    }
}

void SPO2MenuContentPrivate::loadOptions()
{
    setCboBlockSignalsStatus(true);

    combos[ITEM_CBO_SENSITIVITY]->clear();
    if (moduleType == MODULE_MASIMO_SPO2
            || moduleType == MODULE_RAINBOW_SPO2)
    {
        for (int i = SPO2_MASIMO_SENS_MAX; i < SPO2_MASIMO_SENS_NR; i++)
        {
            combos[ITEM_CBO_SENSITIVITY]->addItem(trs(SPO2Symbol::convert(static_cast<SensitivityMode>(i))));
        }
    }
    else if (moduleType != MODULE_SPO2_NR)
    {
        for (int i = SPO2_SENS_LOW; i < SPO2_SENS_NR; i++)
        {
            combos[ITEM_CBO_SENSITIVITY]->addItem(trs(SPO2Symbol::convert(static_cast<SPO2Sensitive>(i))));
        }
    }

    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(spo2Param.getSweepSpeed());
    if (moduleType == MODULE_MASIMO_SPO2 || moduleType == MODULE_RAINBOW_SPO2)
    {
        combos[ITEM_CBO_AVERAGE_TIME]->setCurrentIndex(spo2Param.getAverageTime());
        combos[ITEM_CBO_FAST_SAT]->setCurrentIndex(spo2Param.getFastSat());
    }
    combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(spo2Param.getSensitivity());
    combos[ITEM_CBO_SMART_TONE]->setCurrentIndex(spo2Param.getSmartPulseTone());

    int volIndex;
    currentConfig.getNumValue("ECG|QRSVolume", volIndex);
    combos[ITEM_CBO_PULSE_VOL]->setCurrentIndex(volIndex);
    if (nightModeManager.nightMode())
    {
        combos[ITEM_CBO_PULSE_VOL]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_PULSE_VOL]->setEnabled(true);
    }

    combos[ITEM_CBO_NIBP_SAME_SIDE]->setCurrentIndex(spo2Param.isNibpSameSide());

    if (patientManager.getType() != PATIENT_TYPE_NEO)
    {
        cchdBtn->setVisible(false);
    }
    else
    {
        cchdBtn->setVisible(true);
    }

    int index = spo2Param.isShowSignalIQ() ? 1 : 0;
    combos[ITEM_CBO_SIGNAL_IQ]->setCurrentIndex(index);



    QString str;
    machineConfig.getStrValue("SPO2", str);
    if (str == "RAINBOW_SPO2")
    {
        combos[ITEM_CBO_SPO2_SENSOR]->setVisible(true);
        combos[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(true);
        combos[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(true);
        combos[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(true);
        combos[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(true);
        combos[ITEM_CBO_SPHB_UNIT]->setVisible(true);
        combos[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPO2_SENSOR]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPHB_UNIT]->setVisible(true);
        seniorParamLbl[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(true);

        currentConfig.getNumValue("SPO2|Sensor", index);
        combos[ITEM_CBO_SPO2_SENSOR]->setCurrentIndex(index);

        index = spo2Param.getLineFrequency() == SPO2_LINE_FREQ_50HZ ? 0 : 1;
        combos[ITEM_CBO_SPO2_LINE_FREQ]->setCurrentIndex(index);

        index = static_cast<int>(spo2Param.getSpHbPrecision());
        combos[ITEM_CBO_SPHB_PRECISION_MODE]->setCurrentIndex(index);

        index = static_cast<int>(spo2Param.getSpHbBloodVessel());
        combos[ITEM_CBO_SPHB_VESSEL_MODE]->setCurrentIndex(index);

        index = static_cast<int>(spo2Param.getSpHbAveragingMode());
        combos[ITEM_CBO_SPHB_AVERAGING_MODE]->setCurrentIndex(index);

        index = static_cast<int>(spo2Param.getSpHbUnit());
        combos[ITEM_CBO_SPHB_UNIT]->setCurrentIndex(index);

        index = static_cast<int>(spo2Param.getPviAveragingMode());
        combos[ITEM_CBO_PVI_AVERAGING_MODE]->setCurrentIndex(index);
    }
    else
    {
        combos[ITEM_CBO_SPO2_SENSOR]->setVisible(false);
        combos[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(false);
        combos[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(false);
        combos[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(false);
        combos[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(false);
        combos[ITEM_CBO_SPHB_UNIT]->setVisible(false);
        combos[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPO2_SENSOR]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_UNIT]->setVisible(false);
        seniorParamLbl[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(false);
    }

    setCboBlockSignalsStatus(false);
}

SPO2MenuContent::SPO2MenuContent()
    : MenuContent(trs("SPO2Menu"), trs("SPO2MenuDesc")),
      d_ptr(new SPO2MenuContentPrivate)
{
    // get spo2 module type
    d_ptr->moduleType = spo2Param.getModuleType();
}

SPO2MenuContent::~SPO2MenuContent()
{
    delete d_ptr;
}

void SPO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void SPO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // wave speed
    label = new QLabel(trs("SPO2SweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_62D5)
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_125)
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_250));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = SPO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // 平均时间
    if (d_ptr->moduleType == MODULE_MASIMO_SPO2 || d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        label = new QLabel(trs("AverageTime"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        for (int i = 0; i < SPO2_AVER_TIME_NR; i++)
        {
            comboBox->addItem(SPO2Symbol::convert((AverageTime)i));
        }
        itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME, comboBox);
    }

    // 灵敏度
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    SPO2ModuleType moduleType = spo2Param.getModuleType();
    if (moduleType == MODULE_MASIMO_SPO2 || moduleType == MODULE_RAINBOW_SPO2)
    {
        for (int i = SPO2_MASIMO_SENS_MAX; i < SPO2_MASIMO_SENS_NR; i++)
        {
            comboBox->addItem(trs(SPO2Symbol::convert(static_cast<SensitivityMode>(i))));
        }
    }
    else if (moduleType != MODULE_SPO2_NR)
    {
        for (int i = SPO2_SENS_LOW; i < SPO2_SENS_NR; i++)
        {
            comboBox->addItem(trs(SPO2Symbol::convert(static_cast<SPO2Sensitive>(i))));
        }
    }

    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SENSITIVITY);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);

    // 快速血氧
    if (d_ptr->moduleType == MODULE_MASIMO_SPO2 || d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        label = new QLabel(trs("FastSat"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << trs("Off")
                           << trs("On"));
        itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_FAST_SAT);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_FAST_SAT, comboBox);
    }

    // 智能脉搏音
    label = new QLabel(trs("SPO2SmartPulseTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_OFF))
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_ON)));
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SMART_TONE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SMART_TONE, comboBox);

    // 音量
    label = new QLabel(trs("BeatVol"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItem(trs("Off"));

    // 设置声音触发方式
    connect(comboBox, SIGNAL(itemFocusChanged(int)),
            this, SLOT(onPopupListItemFocusChanged(int)));

    for (int i = SoundManager::VOLUME_LEV_1; i <= SoundManager::VOLUME_LEV_MAX; i++)
    {
        comboBox->addItem(QString::number(i));
    }
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_PULSE_VOL);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_PULSE_VOL, comboBox);

    // NIBP同侧功能
    label = new QLabel(trs("NIBPSimul"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE, comboBox);

    // 是否显示Signal IQ
    label = new QLabel(trs("ShowSignalIQ"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SIGNAL_IQ);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SIGNAL_IQ, comboBox);

    // 血氧探头
    label = new QLabel(trs("SpO2Sensor"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_SPO2_SENSOR, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("M-LNCS")
                       << trs("R25")
                       << trs("R1 20L"));
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SPO2_SENSOR);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SPO2_SENSOR, comboBox);

    // Line Frequency
    label = new QLabel(trs("LineFrequency"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList() << "50HZ" << "60HZ");
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ, comboBox);

    // sphb precision mode
    label = new QLabel();
    label->setText(trs("SpHbPrecisionMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                   << trs(SPO2Symbol::convert(PRECISION_NEAREST_0_1))
                   << trs(SPO2Symbol::convert(PRECISION_NEAREST_0_5))
                   << trs(SPO2Symbol::convert(PRECISION_WHOLE_NUMBER)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    int item = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE, comboBox);

    // sphb vessel mode
    label = new QLabel();
    label->setText(trs("SpHbVesselMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(SPO2Symbol::convert(BLOOD_VESSEL_ARTERIAL))
                    << trs(SPO2Symbol::convert(BLOOD_VESSEL_VENOUS)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE, comboBox);

    // sphb averaging mode
    label = new QLabel();
    label->setText(trs("SpHbAveragingMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_LONG))
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_MED))
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_SHORT)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE, comboBox);

    // sphb Unit
    label = new QLabel();
    label->setText(trs("SpHbUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(Unit::getSymbol(UNIT_GDL))
                    << trs(Unit::getSymbol(UNIT_MMOL_L)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT, comboBox);

    // pvi averaging mode
    label = new QLabel();
    label->setText(trs("PviAveragingMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(SPO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(SPO2Symbol::convert(AVERAGING_MODE_NORMAL))
                    << trs(SPO2Symbol::convert(AVERAGING_MODE_FAST)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    // CCHD筛查
    d_ptr->cchdBtn = new Button(trs("CCHDCheck"));
    d_ptr->cchdBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->cchdBtn, d_ptr->combos.count() + 1, 1);
    connect(d_ptr->cchdBtn, SIGNAL(released()), this, SLOT(onCCHDCheckBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void SPO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        SPO2MenuContentPrivate::MenuItem item
            = (SPO2MenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case SPO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
            spo2Param.setSweepSpeed(index);
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME:
            spo2Param.setAverageTime((AverageTime)index);
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SENSITIVITY:
            spo2Param.setSensitivity(index);
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_FAST_SAT:
            spo2Param.setFastSat(static_cast<bool>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SMART_TONE:
            spo2Param.setSmartPulseTone((SPO2SMARTPLUSETONE)index);
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_PULSE_VOL:
            spo2Param.setBeatVol(static_cast<SoundManager::VolumeLevel>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE:
            spo2Param.setNibpSameSide(box->currentIndex());
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SIGNAL_IQ:
            spo2Param.showSignalIQ(static_cast<bool>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ:
            spo2Param.setLineFrequency(static_cast<SPO2LineFrequencyType>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SPO2_SENSOR:
            spo2Param.setSensor(static_cast<SPO2RainbowSensor>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE:
            spo2Param.setSpHbPrecision(static_cast<SpHbPrecisionMode>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE:
            spo2Param.setSpHbBloodVessel(static_cast<SpHbBloodVesselMode>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE:
            spo2Param.setSpHbAveragingMode(static_cast<SpHbAveragingMode>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT:
            spo2Param.setSpHbUnit(static_cast<SpHbUnitType>(index));
            break;
        case SPO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE:
            spo2Param.setPviAveragingMode(static_cast<AveragingMode>(index));
            break;
        default:
            break;
        }
    }
}

void SPO2MenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_SPO2, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}

void SPO2MenuContent::onPopupListItemFocusChanged(int volume)
{
    ComboBox *w = qobject_cast<ComboBox*>(sender());
    if (w == d_ptr->combos[SPO2MenuContentPrivate::ITEM_CBO_PULSE_VOL])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_PULSE , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.pulseTone();
    }
}

void SPO2MenuContent::onCCHDCheckBtnReleased()
{
    CCHDWindow *win = new CCHDWindow();
    win->exec();
}
