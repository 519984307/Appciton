/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#include "ConfigEditSpO2MenuContent.h"
#include <QGridLayout>
#include <QLabel>
#include <QMap>
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include "SPO2Symbol.h"
#include "SPO2Define.h"
#include "ConfigManager.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "SPO2Param.h"
#include "Debug.h"
#include "SystemManager.h"

class ConfigEditSpO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED,
        ITEM_CBO_AVERAGE_TIME,
        ITEM_CBO_SENSITIVITY,
        ITEM_CBO_FAST_SAT,
        ITEM_CBO_SMART_TONE,
        ITEM_CBO_BEAT_VOL,
        ITEM_CBO_NIBP_SAME_SIDE,
        ITEM_CBO_SIGNAL_IQ,
        ITEM_CBO_SPO2_LINE_FREQ,
        ITEM_CBO_SPHB_PRECISION_MODE,
        ITEM_CBO_SPHB_VESSEL_MODE,
        ITEM_CBO_SPHB_AVERAGING_MODE,
        ITEM_CBO_SPHB_UNIT,
        ITEM_CBO_PVI_AVERAGING_MODE,
        ITEM_CBO_ALARM_AUDIO_DELAY,    // alarm audio delay
        ITEM_CBO_MAX,
    };

    explicit ConfigEditSpO2MenuContentPrivate(Config *const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    QMap<MenuItem, QLabel *> seniorParamLbl;
    Config *const config;
    SPO2ModuleType moduleType;

    QLabel *focusLable;  // 仅查看该菜单时,设置该label为可聚焦方式，便于旋转飞梭查看视图
};

ConfigEditSpO2MenuContentPrivate
::ConfigEditSpO2MenuContentPrivate(Config *const config)
    : config(config)
    , moduleType(MODULE_BLM_S5)
    , focusLable(NULL)
{
    combos.clear();
}

ConfigEditSpO2MenuContent::ConfigEditSpO2MenuContent(Config *const config):
    MenuContent(trs("SPO2Menu"),
                trs("SPO2MenuDesc")),
    d_ptr(new ConfigEditSpO2MenuContentPrivate(config))
{
    d_ptr->moduleType = spo2Param.getModuleType();
}

ConfigEditSpO2MenuContent::~ConfigEditSpO2MenuContent()
{
    delete d_ptr;
}

void ConfigEditSpO2MenuContentPrivate::loadOptions()
{
    int index;

    // wave speed
    index = 0;
    config->getNumValue("SPO2|SweepSpeed", index);
    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(index);

    if (moduleType == MODULE_RAINBOW_SPO2)
    {
        // average time
        index = 0;
        config->getNumValue("SPO2|AverageTime", index);
        combos[ITEM_CBO_AVERAGE_TIME]->setCurrentIndex(index);

        // fast sat
        index = 0;
        config->getNumValue("SPO2|FastSat", index);
        combos[ITEM_CBO_FAST_SAT]->setCurrentIndex(index);
    }

    // Sensitivity
    index = 0;
    config->getNumValue("SPO2|Sensitivity", index);
    if (moduleType == MODULE_RAINBOW_SPO2)
    {
        if (index > SPO2_MASIMO_SENS_MAX)
        {
            index -= 1;
        }
    }
    combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(index);

    // Smart Pluse Tone
    index = 0;
    config->getNumValue("SPO2|SmartPluseTone", index);
    combos[ITEM_CBO_SMART_TONE]->setCurrentIndex(index);

    // Beat Volume
    index = 0;
    config->getNumValue("ECG|QRSVolume", index);
    combos[ITEM_CBO_BEAT_VOL]->setCurrentIndex(index);

    // NIBP Same Side
    index = 0;
    config->getNumValue("SPO2|NIBPSameSide", index);
    combos[ITEM_CBO_NIBP_SAME_SIDE]->setCurrentIndex(index);

    // signal IQ
    index = 0;
    config->getNumValue("SPO2|SignalIQ", index);
    combos[ITEM_CBO_SIGNAL_IQ]->setCurrentIndex(index);

    // Alarm audio delay
    index = 0;
    config->getNumValue("SPO2|AlarmDelay", index);
    combos[ITEM_CBO_ALARM_AUDIO_DELAY]->setCurrentIndex(index);

    if (moduleType == MODULE_RAINBOW_SPO2)
    {
        bool isNeoMachine = systemManager.isNeonateMachine();  // Neonate machine status
        combos[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(true);
        combos[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(!isNeoMachine);
        combos[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(!isNeoMachine);
        combos[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(!isNeoMachine);
        combos[ITEM_CBO_SPHB_UNIT]->setVisible(!isNeoMachine);
        combos[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(true);
        seniorParamLbl[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(!isNeoMachine);
        seniorParamLbl[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(!isNeoMachine);
        seniorParamLbl[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(!isNeoMachine);
        seniorParamLbl[ITEM_CBO_SPHB_UNIT]->setVisible(!isNeoMachine);
        seniorParamLbl[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(true);

        index = 0;
        config->getNumValue("SPO2|LineFrequency", index);
        combos[ITEM_CBO_SPO2_LINE_FREQ]->setCurrentIndex(index);

        index = 0;
        config->getNumValue("SPO2|SpHbPrecision", index);
        combos[ITEM_CBO_SPHB_PRECISION_MODE]->setCurrentIndex(index);

        index = 0;
        config->getNumValue("SPO2|SpHbBloodVessel", index);
        combos[ITEM_CBO_SPHB_VESSEL_MODE]->setCurrentIndex(index);

        index = 0;
        config->getNumValue("SPO2|SpHbAveragingMode", index);
        combos[ITEM_CBO_SPHB_AVERAGING_MODE]->setCurrentIndex(index);

        index = 0;
        config->getNumValue("SPO2|SpHbUnit", index);
        combos[ITEM_CBO_SPHB_UNIT]->setCurrentIndex(index);

        index = 0;
        config->getNumValue("SPO2|PviAveragingMode", index);
        combos[ITEM_CBO_PVI_AVERAGING_MODE]->setCurrentIndex(index);
    }
    else
    {
        combos[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(false);
        combos[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(false);
        combos[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(false);
        combos[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(false);
        combos[ITEM_CBO_SPHB_UNIT]->setVisible(false);
        combos[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPO2_LINE_FREQ]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_PRECISION_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_VESSEL_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_AVERAGING_MODE]->setVisible(false);
        seniorParamLbl[ITEM_CBO_SPHB_UNIT]->setVisible(false);
        seniorParamLbl[ITEM_CBO_PVI_AVERAGING_MODE]->setVisible(false);
    }
}

void ConfigEditSpO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditSpO2MenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        ConfigEditSpO2MenuContentPrivate::MenuItem item =
                static_cast<ConfigEditSpO2MenuContentPrivate::MenuItem>(i);
        if (d_ptr->combos[item])
        {
            d_ptr->combos[item]->setEnabled(!isOnlyToRead);
        }
    }

    // 模块类型不同时，参数设置菜单的长度也会有所改变;
    // 当参数设置菜单过长超过一屏时，加入可选择的聚焦点,
    // 便于使用飞梭查看所有菜单内部item
    switch (d_ptr->moduleType)
    {
        case MODULE_BLM_S5:
            break;
        case MODULE_RAINBOW_SPO2:
        {
            if (d_ptr->focusLable)
            {
                if (isOnlyToRead)
                {
                    d_ptr->focusLable->setFocusPolicy(Qt::StrongFocus);
                }
                else
                {
                    d_ptr->focusLable->setFocusPolicy(Qt::NoFocus);
                }
            }
        }
            break;
        default:
            break;
    }
}

void ConfigEditSpO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // wave speed
    label = new QLabel(trs("SPO2SweepSpeed"));
    d_ptr->focusLable = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_62D5)
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_125)
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_250));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // 平均时间
    if (d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        label = new QLabel(trs("AverageTime"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        for (int i = 0; i < SPO2_AVER_TIME_NR; i++)
        {
            comboBox->addItem(SPO2Symbol::convert((AverageTime)i));
        }
        itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME, comboBox);
    }

    // 灵敏度
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    if (d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        /*
         * According to the MX-5 Prev V&V communication protocol Checklist/Data Table 16.1,
         * ensure Maximum sensitivity is not allowed as a default. The device must use Normal
         * or APOD instead upon power cycle or reset.
         */
        for (int i = SPO2_MASIMO_SENS_NORMAL; i < SPO2_MASIMO_SENS_NR; i++)
        {
            comboBox->addItem(trs(SPO2Symbol::convert(static_cast<SensitivityMode>(i))));
        }
    }
    else if (d_ptr->moduleType != MODULE_SPO2_NR)
    {
        for (int i = SPO2_SENS_LOW; i < SPO2_SENS_NR; i++)
        {
            comboBox->addItem(trs(SPO2Symbol::convert(static_cast<SPO2Sensitive>(i))));
        }
    }

    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SENSITIVITY);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);

    // 快速血氧
    if (d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        label = new QLabel(trs("FastSat"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << trs("Off")
                           << trs("On"));
        itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_FAST_SAT);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_FAST_SAT, comboBox);
    }

    // 智能脉搏音
    label = new QLabel(trs("SPO2SmartPulseTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_OFF))
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_ON)));
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_TONE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_TONE, comboBox);

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
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL, comboBox);

    // NIBP同侧功能
    label = new QLabel(trs("NIBPSimul"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE, comboBox);

    // 是否显示Signal IQ
    label = new QLabel(trs("ShowSignalIQ"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SIGNAL_IQ);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SIGNAL_IQ, comboBox);

    // Line Frequency
    label = new QLabel(trs("LineFrequency"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList() << "50HZ" << "60HZ");
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ, comboBox);

    // sphb precision mode
    label = new QLabel();
    label->setText(trs("SpHbPrecisionMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                   << trs(SPO2Symbol::convert(PRECISION_NEAREST_0_1))
                   << trs(SPO2Symbol::convert(PRECISION_NEAREST_0_5))
                   << trs(SPO2Symbol::convert(PRECISION_WHOLE_NUMBER)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    int item = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE, comboBox);

    // sphb vessel mode
    label = new QLabel();
    label->setText(trs("SpHbVesselMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(SPO2Symbol::convert(BLOOD_VESSEL_ARTERIAL))
                    << trs(SPO2Symbol::convert(BLOOD_VESSEL_VENOUS)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE, comboBox);

    // sphb averaging mode
    label = new QLabel();
    label->setText(trs("SpHbAveragingMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_LONG))
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_MED))
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_SHORT)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE, comboBox);

    // sphb Unit
    label = new QLabel();
    label->setText(trs("SpHbUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(Unit::getSymbol(UNIT_GDL))
                    << trs(Unit::getSymbol(UNIT_MMOL_L)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT, comboBox);

    // pvi averaging mode
    label = new QLabel();
    label->setText(trs("PviAveragingMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                    << trs(SPO2Symbol::convert(AVERAGING_MODE_NORMAL))
                    << trs(SPO2Symbol::convert(AVERAGING_MODE_FAST)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    item = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE);
    comboBox->setProperty("Item", qVariantFromValue(item));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE, comboBox);

    // spo2 alarm delay
    label = new QLabel(trs("SPO2AlarmAudioDelay"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->seniorParamLbl.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_ALARM_AUDIO_DELAY, label);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList() << trs(SPO2Symbol::convert(SPO2_ALARM_AUDIO_DELAY_0S))
                                     << trs(SPO2Symbol::convert(SPO2_ALARM_AUDIO_DELAY_5S))
                                     << trs(SPO2Symbol::convert(SPO2_ALARM_AUDIO_DELAY_10S))
                                     << trs(SPO2Symbol::convert(SPO2_ALARM_AUDIO_DELAY_15S)));
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_ALARM_AUDIO_DELAY);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_ALARM_AUDIO_DELAY, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void ConfigEditSpO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;
    switch (indexType)
    {
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_TONE:
        str = "SmartPluseTone";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SENSITIVITY:
        if (d_ptr->moduleType == MODULE_RAINBOW_SPO2)
        {
            index += 1;
        }
        str = "Sensitivity";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
        str = "SweepSpeed";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME:
        str = "AverageTime";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_FAST_SAT:
        str = "FastSat";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL:
    {
        int vol = 0;
        currentConfig.getNumValue("ECG|QRSVolume", vol);
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(vol));
        d_ptr->config->setNumValue("ECG|QRSVolume", index);
    }
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE:
        str = "NIBPSameSide";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SIGNAL_IQ:
        str = "SignalIQ";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPO2_LINE_FREQ:
        str = "LineFrequency";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_PRECISION_MODE:
        str = "SpHbPrecision";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_VESSEL_MODE:
        str = "SpHbBloodVessel";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_AVERAGING_MODE:
        str = "SpHbAveragingMode";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPHB_UNIT:
        str = "SpHbUnit";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_PVI_AVERAGING_MODE:
        str = "PviAveragingMode";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_ALARM_AUDIO_DELAY:
        str = "AlarmDelay";
        break;
    default :
        qdebug("Invalid combo id.");
        break;
    }
    if (!str.isEmpty())
    {
        d_ptr->config->setNumValue(QString("SPO2|%1").arg(str), index);
    }
}

void ConfigEditSpO2MenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_SPO2, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void ConfigEditSpO2MenuContent::onPopupListItemFocusChanged(int volume)
{
    ComboBox *w = qobject_cast<ComboBox*>(sender());
    if (w == d_ptr->combos[ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.heartBeatTone();
    }
}
