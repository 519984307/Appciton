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
#include "ConfigManager.h"
#include "Button.h"

class ConfigEditECGMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_HRPR_SOURCE = 0,
        ITEM_CBO_ECG1_WAVE,
        ITEM_CBO_ECG2_WAVE,
        ITEM_CBO_ECG1_GAIN,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_HTBT_VOL,
        ITEM_CBO_LEAD_MODE,
        ITEM_CBO_NOTCH_FILTER,
        ITEM_CBO_PACER_MARK,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditECGMenuContentPrivate(Config *const config)
        : config(config),
          sTSwitchBtn(NULL),
          hrLabel(NULL)
    {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, QLabel *> comboLabels;
    Config *const config;
    Button *sTSwitchBtn;
    QLabel * hrLabel;  // 仅查看该菜单时,设置该label为可聚焦方式，便于旋转飞梭查看视图
};

void ConfigEditECGMenuContentPrivate::loadOptions()
{
    for (unsigned i = 0; i < ITEM_CBO_MAX; i++)
    {
        MenuItem item = static_cast<MenuItem>(i);
        combos[item]->blockSignals(true);
    }

    // 加载hr/pr来源
    int index = 0;
    config->getNumValue("ECG|HRSource", index);
    int sourceType = ecgParam.getHrSourceTypeFromId(static_cast<ParamID>(index));
    int cboIndex = 0;
    int itemCount = 0;
    combos[ITEM_CBO_HRPR_SOURCE]->clear();
    for (int i = HR_SOURCE_ECG; i < HR_SOURCE_NR; ++i)
    {
        if (i == HR_SOURCE_SPO2 && !systemManager.isSupport(PARAM_SPO2))
        {
            continue;
        }
        if (i == HR_SOURCE_IBP && !systemManager.isSupport(PARAM_IBP))
        {
            continue;
        }

        if (i == sourceType)
        {
            cboIndex = itemCount;
        }
        itemCount++;
        combos[ITEM_CBO_HRPR_SOURCE]->addItem(trs(ECGSymbol::convert(static_cast<HRSourceType>(i))));
    }

    if (cboIndex > combos[ITEM_CBO_HRPR_SOURCE]->count())
    {
        cboIndex = 0;
        index = ecgParam.getIdFromHrSourceType(static_cast<HRSourceType>(cboIndex));
        currentConfig.setNumValue("ECG|HRSource", index);
    }
    combos[ITEM_CBO_HRPR_SOURCE]->setCurrentIndex(cboIndex);


    int leadmode = 0;
    config->getNumValue("ECG|LeadMode", leadmode);
    combos[ITEM_CBO_LEAD_MODE]->clear();
    for (int i = 0; i < ECG_LEAD_MODE_NR; i++)
    {
#ifdef HIDE_ECG_12_LEAD_FUNCTION
        if (i == ECG_LEAD_MODE_12)
        {
            continue;
        }
#endif
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

        combos[ITEM_CBO_ECG1_WAVE]->addItem(trs(ECGSymbol::convert((ECGLead)i, ecgParam.getLeadNameConvention())));
        combos[ITEM_CBO_ECG2_WAVE]->addItem(trs(ECGSymbol::convert((ECGLead)i, ecgParam.getLeadNameConvention())));
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
    for (int i = 0; i < ECG_GAIN_NR; i++)
    {
        combos[ITEM_CBO_ECG1_GAIN]->addItem(trs(ECGSymbol::convert(static_cast<ECGGain>(i))));
    }

    QString leadName = "ECG";
    leadName += ECGSymbol::convert(static_cast<ECGLead>(index), ECG_CONVENTION_AAMI);
    leadName += "WaveWidget";

    config->getNumValue(QString("ECG|Gain|%1").arg(leadName), index);
    combos[ITEM_CBO_ECG1_GAIN]->setCurrentIndex(index);

    config->getNumValue("ECG|SweepSpeed", index);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(index);

    int filterMode =  0;
    config->getNumValue("ECG|FilterMode", filterMode);
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(filterMode);

    config->getNumValue("ECG|QRSVolume", index);
    combos[ITEM_CBO_HTBT_VOL]->setCurrentIndex(index);

    config->getNumValue("ECG|NotchFilter", index);
    combos[ITEM_CBO_NOTCH_FILTER]->clear();
    switch (filterMode)
    {
    case ECG_FILTERMODE_MONITOR:
    case ECG_FILTERMODE_SURGERY:
        combos[ITEM_CBO_NOTCH_FILTER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
        combos[ITEM_CBO_NOTCH_FILTER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
        combos[ITEM_CBO_NOTCH_FILTER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ)), ECG_NOTCH_50_AND_60HZ);
        break;
    case ECG_FILTERMODE_DIAGNOSTIC:
    case ECG_FILTERMODE_ST:
        combos[ITEM_CBO_NOTCH_FILTER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF)), ECG_NOTCH_OFF);
        combos[ITEM_CBO_NOTCH_FILTER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
        combos[ITEM_CBO_NOTCH_FILTER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
        break;
    default:
        break;
    }
    int notchFilterIndex = combos[ITEM_CBO_NOTCH_FILTER]->findText(trs(ECGSymbol::convert((ECGNotchFilter)index)));
    if (notchFilterIndex > -1)
    {
        combos[ITEM_CBO_NOTCH_FILTER]->setCurrentIndex(notchFilterIndex);
    }
    else
    {
        combos[ITEM_CBO_NOTCH_FILTER]->setCurrentIndex(0);
    }
    for (unsigned i = 0; i < ITEM_CBO_MAX; i++)
    {
        MenuItem item = static_cast<MenuItem>(i);
        combos[item]->blockSignals(false);
    }

    config->getNumValue("ECG|PacerMaker", index);
    combos[ITEM_CBO_PACER_MARK]->setCurrentIndex(index);
}

ConfigEditECGMenuContent::ConfigEditECGMenuContent(Config *const config)
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
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditECGMenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditECGMenuContentPrivate
                      ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }
#ifndef HIDE_ECG_ST_PVCS_SUBPARAM
    d_ptr->sTSwitchBtn->setEnabled(!isOnlyToRead);
#endif
    if (isOnlyToRead)
    {
        d_ptr->hrLabel->setFocusPolicy(Qt::StrongFocus);
    }
    else
    {
        d_ptr->hrLabel->setFocusPolicy(Qt::NoFocus);
    }
}

void ConfigEditECGMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // alarm source
    label = new QLabel(trs("HR_PRSource"));
    d_ptr->hrLabel = label;
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE, comboBox);

    // ecg1 wave
    label = new QLabel(trs("ECG1"));
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
    label = new QLabel(trs("ECG2"));
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
    label = new QLabel(trs("ECGGain"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN, comboBox);

    // sweep speed
    label = new QLabel(trs("SweepSpeed"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_625))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_125))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_250))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_500)));
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // filter
    label = new QLabel(trs("FilterMode"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
                   #ifndef  HIDE_ECG_ST_PVCS_SUBPARAM
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_ST))
                   #endif
                       );
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

    // heart beat volume
    label = new QLabel(trs("ECGQRSToneVolume"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5));
    connect(comboBox, SIGNAL(itemFocusChanged(int)),
            this, SLOT(onPopupListItemFocusChanged(int)));
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
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
                       << trs(ECGSymbol::convert(ECG_NOTCH_OFF))
                       << trs(ECGSymbol::convert(ECG_NOTCH_50HZ))
                       << trs(ECGSymbol::convert(ECG_NOTCH_60HZ))
                       << trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ)));
    itemID = static_cast<int>(ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER, comboBox);

    // paceMark
    label = new QLabel(trs("ECGPaceDetection"));
    d_ptr->comboLabels.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_PACER_MARK,
                              label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_PACE_OFF))
                       << trs(ECGSymbol::convert(ECG_PACE_ON)));
    itemID = ConfigEditECGMenuContentPrivate::ITEM_CBO_PACER_MARK;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditECGMenuContentPrivate::ITEM_CBO_PACER_MARK, comboBox);

#ifndef  HIDE_ECG_ST_PVCS_SUBPARAM
    // ST 段开关
    d_ptr->sTSwitchBtn = new Button;
    d_ptr->sTSwitchBtn->setText(QString("%1 >>").arg(trs("STAnalysize")));
    d_ptr->sTSwitchBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->sTSwitchBtn, d_ptr->combos.count(), 1);
    connect(d_ptr->sTSwitchBtn, SIGNAL(released()), this, SLOT(onSTSwitchBtnReleased()));
#endif

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count() + 1, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 2, 1);
}

void ConfigEditECGMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
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
        {
            d_ptr->config->setNumValue("ECG|FilterMode", index);
            d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->clear();
            switch (index)
            {
            case ECG_FILTERMODE_MONITOR:
            case ECG_FILTERMODE_SURGERY:
                d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
                d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
                d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ)), ECG_NOTCH_50_AND_60HZ);
                break;
            case ECG_FILTERMODE_DIAGNOSTIC:
            case ECG_FILTERMODE_ST:
                d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF)), ECG_NOTCH_OFF);
                d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
                d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
                break;
            }
            d_ptr->combos[ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER]->
            setCurrentIndex(ECG_NOTCH_OFF);
        }
        break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_NOTCH_FILTER:
        {
            int notch =
                    d_ptr->combos[ConfigEditECGMenuContentPrivate
                                                ::ITEM_CBO_NOTCH_FILTER]->itemData(index).toInt();
            d_ptr->config->setNumValue("ECG|NotchFilter", notch);
            break;
        }
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE:
            d_ptr->config->setNumValue("ECG|HRSource", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_WAVE:
        {
            int waveIndex = 0;
            d_ptr->config->getNumValue("ECG|Ecg2Wave", waveIndex);
            // ecg1 与ecg2的选择重复时
            if (waveIndex == index)
            {
                waveIndex = 0;
                d_ptr->config->getNumValue("ECG|Ecg1Wave", waveIndex);
                d_ptr->config->setNumValue("ECG|Ecg2Wave", waveIndex);
                ConfigEditECGMenuContentPrivate::MenuItem menuItem =
                    ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_WAVE;
                d_ptr->combos[menuItem]->blockSignals(true);
                d_ptr->combos[menuItem]->setCurrentIndex(waveIndex);
                d_ptr->combos[menuItem]->blockSignals(false);
            }

            d_ptr->config->setNumValue("ECG|Ecg1Wave", index);

            // 加载对应的增益
            QString leadName = "ECG";
            leadName += ECGSymbol::convert(static_cast<ECGLead>(index), ECG_CONVENTION_AAMI);
            leadName += "WaveWidget";
            d_ptr->config->getNumValue(QString("ECG|Gain|%1").arg(leadName), index);
            ConfigEditECGMenuContentPrivate::MenuItem menuItem =
                ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN;
            d_ptr->combos[menuItem]->blockSignals(true);
            d_ptr->combos[menuItem]->setCurrentIndex(index);
            d_ptr->combos[menuItem]->blockSignals(false);
        }
        break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG2_WAVE:
        {
            int waveIndex = 0;
            d_ptr->config->getNumValue("ECG|Ecg1Wave", waveIndex);
            // ecg1 与ecg2的选择重复时
            if (waveIndex == index)
            {
                waveIndex = 0;
                d_ptr->config->getNumValue("ECG|Ecg2Wave", waveIndex);
                d_ptr->config->setNumValue("ECG|Ecg1Wave", waveIndex);
                ConfigEditECGMenuContentPrivate::MenuItem menuItem =
                    ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_WAVE;
                d_ptr->combos[menuItem]->blockSignals(true);
                d_ptr->combos[menuItem]->setCurrentIndex(waveIndex);
                d_ptr->combos[menuItem]->blockSignals(false);
            }

            d_ptr->config->setNumValue("ECG|Ecg2Wave", index);
        }
        break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_ECG1_GAIN:
        {
            int lead = 0;
            d_ptr->config->getNumValue("ECG|Ecg1Wave", lead);
            QString leadName = "ECG";
            leadName += ECGSymbol::convert(static_cast<ECGLead>(lead), ECG_CONVENTION_AAMI);
            leadName += "WaveWidget";
            d_ptr->config->setNumValue(QString("ECG|Gain|%1").arg(leadName), index);
        }
        break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_HTBT_VOL:
            d_ptr->config->setNumValue("ECG|QRSVolume", index);
            currentConfig.getNumValue("ECG|QRSVolume", index);
            soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT, static_cast<SoundManager::VolumeLevel>(index));
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            d_ptr->config->setNumValue("ECG|SweepSpeed", index);
            break;
        case ConfigEditECGMenuContentPrivate::ITEM_CBO_PACER_MARK:
            d_ptr->config->setNumValue("ECG|PacerMaker", index);
        default:
            break;
        }
    }
}

void ConfigEditECGMenuContent::onSTSwitchBtnReleased()
{
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

void ConfigEditECGMenuContent::onPopupListItemFocusChanged(int volume)
{
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.heartBeatTone();
}


