/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/16
 **/

#include "ConfigEditEcgMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "ECGSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "ECGParam.h"
#include "IConfig.h"
#include "Button.h"

class ConfigEditECGMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ALARM_SOURCE = 0,
        ITEM_CBO_ECG1_WAVE,
        ITEM_CBO_ECG2_WAVE,
        ITEM_CBO_ECG1_GAIN,
        ITEM_CBO_ECG2_GAIN,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_HTBT_VOL,
        ITEM_CBO_LEAD_MODE,
        ITEM_CBO_NOTCH_FILTER,

        ITEM_CBO_MAX,
    };

    explicit ConfigEditECGMenuContentPrivate(Config * const config)
        :config(config)
    {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, QLabel *> comboLabels;
    Config *const config;
};

void ConfigEditECGMenuContentPrivate::loadOptions()
{
    for (unsigned i = 0; i < ITEM_CBO_MAX; i++)
    {
        MenuItem item = (MenuItem)i;
        combos[item]->blockSignals(true);
    }

    int index = 0;

    config->getNumValue("ECG|AlarmSource", index);
    combos[ITEM_CBO_ALARM_SOURCE]->setCurrentIndex(index);

    int leadmode = 0;
    config->getNumValue("ECG|LeadMode", leadmode);
    combos[ITEM_CBO_LEAD_MODE]->clear();
    for (int i = 0; i < ECG_LEAD_MODE_NR; i++)
    {
        combos[ITEM_CBO_LEAD_MODE]->addItem(trs(ECGSymbol::convert((ECGLeadMode)i)));
    }
    combos[ITEM_CBO_LEAD_MODE]->setCurrentIndex(leadmode);

    combos[ITEM_CBO_ECG1_WAVE]->clear();
    combos[ITEM_CBO_ECG2_WAVE]->clear();
    for (int i = 0; i < ECG_LEAD_NR; i++)
    {
        if ((leadmode == ECG_LEAD_MODE_5 && i > ECG_LEAD_V1)
                || (leadmode == ECG_LEAD_MODE_3 && i > ECG_LEAD_III))
        {
            break;
        }

        combos[ITEM_CBO_ECG1_WAVE]->addItem(trs(ECGSymbol::convert((ECGLead)i, ECG_CONVENTION_AAMI)));
        combos[ITEM_CBO_ECG2_WAVE]->addItem(trs(ECGSymbol::convert((ECGLead)i, ECG_CONVENTION_AAMI)));
    }

    config->getNumValue("ECG|Ecg1Wave", index);
    if (leadmode == ECG_LEAD_MODE_12)
    {
        combos[ITEM_CBO_ECG1_WAVE]->setCurrentIndex(index);
        config->getNumValue("ECG|Ecg2Wave", index);
        combos[ITEM_CBO_ECG2_WAVE]->setCurrentIndex(index);
        combos[ITEM_CBO_ECG2_WAVE]->setVisible(true);
        comboLabels[ITEM_CBO_ECG2_WAVE]->setVisible(true);
    }
    else if (leadmode == ECG_LEAD_MODE_5)
    {
        int wave2index = 0;
        config->getNumValue("ECG|Ecg2Wave", wave2index);
        if (index > ECG_LEAD_V1)
        {
            index = ECG_LEAD_I;
            if (index == wave2index)
            {
                index = ECG_LEAD_II;
            }
        }

        if (wave2index > ECG_LEAD_V1)
        {
            wave2index = ECG_LEAD_II;
        }

        combos[ITEM_CBO_ECG2_WAVE]->setVisible(true);
        comboLabels[ITEM_CBO_ECG2_WAVE]->setVisible(true);
        combos[ITEM_CBO_ECG1_WAVE]->setCurrentIndex(index);
        combos[ITEM_CBO_ECG2_WAVE]->setCurrentIndex(wave2index);
    }
    else if (leadmode == ECG_LEAD_MODE_3)
    {
        if (index > ECG_LEAD_III)
        {
            index = ECG_LEAD_II;
        }
        combos[ITEM_CBO_ECG1_WAVE]->setCurrentIndex(index);
        combos[ITEM_CBO_ECG2_WAVE]->setVisible(false);
        comboLabels[ITEM_CBO_ECG2_WAVE]->setVisible(false);
    }

    combos[ITEM_CBO_ECG1_GAIN]->clear();
    combos[ITEM_CBO_ECG2_GAIN]->clear();
    for (int i = 0; i < ECG_GAIN_NR; i++)
    {
        combos[ITEM_CBO_ECG1_GAIN]->addItem(trs(ECGSymbol::convert((ECGGain)i)));
        combos[ITEM_CBO_ECG2_GAIN]->addItem(trs(ECGSymbol::convert((ECGGain)i)));
    }

    config->getNumValue("ECG|Ecg1Gain", index);
    combos[ITEM_CBO_ECG1_GAIN]->setCurrentIndex(index);
    config->getNumValue("ECG|Ecg2Gain", index);
    combos[ITEM_CBO_ECG2_GAIN]->setCurrentIndex(index);

    if (leadmode > ECG_LEAD_MODE_3)
    {
        combos[ITEM_CBO_ECG2_GAIN]->setVisible(true);
        comboLabels[ITEM_CBO_ECG2_GAIN]->setVisible(true);
    }
    else
    {
        combos[ITEM_CBO_ECG2_GAIN]->setVisible(false);
        comboLabels[ITEM_CBO_ECG2_GAIN]->setVisible(false);
    }

    config->getNumValue("ECG|SweepSpeed", index);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(index);

    config->getNumValue("ECG|Filter", index);
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(index);

    config->getNumValue("ECG|QRSVolume", index);
    combos[ITEM_CBO_HTBT_VOL]->setCurrentIndex(index);

    config->getNumValue("ECG|NotchFilter", index);
    combos[ITEM_CBO_NOTCH_FILTER]->setCurrentIndex(index);

    for (unsigned i = 0; i < ITEM_CBO_MAX; i++)
    {
        MenuItem item = (MenuItem)i;
        combos[item]->blockSignals(false);
    }
}

ConfigEditECGMenuContent::ConfigEditECGMenuContent(Config * const config)
    : MenuContent(trs("ECGMenu"), trs("ECGMenuDesc")),
      d_ptr(new ConfigEditECGMenuContentPrivate(config))
{
}

ConfigEditECGMenuContent::~ConfigEditECGMenuContent()
{
    delete d_ptr;
}

void ConfigEditECGMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditECGMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // alarm source
    label = new QLabel(trs("AlarmSource"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ALARM_SOURCE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_ALARM_SRC_HR))
                       << trs(ECGSymbol::convert(ECG_ALARM_SRC_PR))
                       << trs(ECGSymbol::convert(ECG_ALARM_SRC_AUTO)));
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_ALARM_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ALARM_SOURCE, comboBox);

    // ecg1 wave
    label = new QLabel(trs("Ecg1Wave"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_WAVE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_WAVE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_WAVE, comboBox);

    // ecg2 wave
    label = new QLabel(trs("Ecg2Wave"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_WAVE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_WAVE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_WAVE, comboBox);

    // ecg1 gain
    label = new QLabel(trs("Ecg1Gain"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN, comboBox);

    // ecg2 gain
    label = new QLabel(trs("Ecg2Gain"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_GAIN,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_GAIN);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_GAIN, comboBox);

    // sweep speed
    label = new QLabel(trs("SweepSpeed"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_125))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_250))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_500))
                      );
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // filter
    label = new QLabel(trs("Filter"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_ST))
                      );
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

    // heart beat volume
    label = new QLabel(trs("QRSVolume"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << QString::number(SoundManager::VOLUME_LEV_0)
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5));
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL, comboBox);

    // lead mode
    label = new QLabel(trs("ECGLeadMode"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_LEAD_MODE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_LEAD_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_LEAD_MODE, comboBox);

    // notch
    label = new QLabel(trs("NotchFilter"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_NOTCH_OFF1))
                       << trs(ECGSymbol::convert(ECG_NOTCH_50HZ))
                       << trs(ECGSymbol::convert(ECG_NOTCH_60HZ))
                       << trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ))
                      );
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void ConfigEditECGMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    static int iiii = 0;
    iiii++;
    if (box)
    {
        ConfigEditECGMenuContentPrivate::MenuItem item
            = (ConfigEditECGMenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_LEAD_MODE:
            d_ptr->config->setNumValue("ECG|LeadMode", index);
            d_ptr->loadOptions();
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE:
            d_ptr->config->setNumValue("ECG|Filter", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER:
            d_ptr->config->setNumValue("ECG|NotchFilter", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ALARM_SOURCE:
            d_ptr->config->setNumValue("ECG|AlarmSource", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_WAVE:
            d_ptr->config->setNumValue("ECG|Ecg1Wave", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_WAVE:
            d_ptr->config->setNumValue("ECG|Ecg2Wave", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN:
            d_ptr->config->setNumValue("ECG|Ecg1Gain", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_GAIN:
            d_ptr->config->setNumValue("ECG|Ecg2Gain", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL:
            d_ptr->config->setNumValue("ECG|QRSVolume", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            d_ptr->config->setNumValue("ECG|SweepSpeed", index);
            break;
        default:
            break;
        }
    }
}

void ConfigEditECGMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_HR_PR, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}


