/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/5
 **/

#include "ECGMenuContent.h"
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
#include "ArrhythmiaMenuWindow.h"
#include "WindowManager.h"
#include <QTimer>
#include "MainMenuWindow.h"

class ECGMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_LEAD_MODE = 1,
        ITEM_CBO_ECG,
        ITEM_CBO_ECG_GAIN,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_NOTCH_FITER,
        ITEM_CBO_PACER_MARK,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_QRS_TONE,
    };

    ECGMenuContentPrivate()
        : selfLearnBtn(NULL), arrhythmiaBtn(NULL),
          sTSwitchBtn(NULL)
    {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    Button *selfLearnBtn;
    Button *arrhythmiaBtn;
    Button *sTSwitchBtn;
    QStringList ecgWaveforms;
    QStringList ecgWaveformTitles;
};

void ECGMenuContentPrivate::loadOptions()
{
    ECGLeadMode leadMode = ecgParam.getLeadMode();
    combos[ITEM_CBO_LEAD_MODE]->setCurrentIndex(leadMode);

    combos[ITEM_CBO_ECG]->blockSignals(true);
    ecgParam.getAvailableWaveforms(ecgWaveforms, ecgWaveformTitles, true);
    combos[ITEM_CBO_ECG]->clear();
    combos[ITEM_CBO_ECG]->addItems(ecgWaveformTitles);
    QString ecgTopWaveform = ecgParam.getCalcLeadWaveformName();
    int index = ecgWaveforms.indexOf(ecgTopWaveform);
    if (index)
    {
        combos[ITEM_CBO_ECG_GAIN]->blockSignals(true);
        ECGGain gain = ecgParam.getGain(static_cast<ECGLead>(index));
        combos[ITEM_CBO_ECG_GAIN]->setCurrentIndex(gain);
        combos[ITEM_CBO_ECG_GAIN]->blockSignals(false);
        combos[ITEM_CBO_ECG]->setCurrentIndex(index);
    }
    combos[ITEM_CBO_ECG]->blockSignals(false);

    ECGFilterMode filterMode = ecgParam.getFilterMode();
    ECGNotchFilter notchFilter = ecgParam.getNotchFilter();
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(ecgParam.getFilterMode());

    combos[ITEM_CBO_NOTCH_FITER]->clear();
    switch (filterMode)
    {
    case ECG_FILTERMODE_MONITOR:
    case ECG_FILTERMODE_SURGERY:
        combos[ITEM_CBO_NOTCH_FITER]->
        addItems(QStringList()
                 << trs(ECGSymbol::convert(ECG_NOTCH_OFF1))
                 << trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ))
                );
        if (notchFilter == ECG_NOTCH_50_AND_60HZ)
        {
            combos[ITEM_CBO_NOTCH_FITER]->setCurrentIndex(1);
        }
        else
        {
            combos[ITEM_CBO_NOTCH_FITER]->setCurrentIndex(notchFilter);
        }
        break;
    case ECG_FILTERMODE_DIAGNOSTIC:
    case ECG_FILTERMODE_ST:
        combos[ITEM_CBO_NOTCH_FITER]->
        addItems(QStringList()
                 << trs(ECGSymbol::convert(ECG_NOTCH_OFF1))
                 << trs(ECGSymbol::convert(ECG_NOTCH_50HZ))
                 << trs(ECGSymbol::convert(ECG_NOTCH_60HZ))
                );
        combos[ITEM_CBO_NOTCH_FITER]->setCurrentIndex(notchFilter);
        break;
    default:
        break;
    }

    combos[ITEM_CBO_PACER_MARK]->setCurrentIndex(ecgParam.getPacermaker());

    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ecgParam.getSweepSpeed());

    combos[ITEM_CBO_QRS_TONE]->setCurrentIndex(ecgParam.getQRSToneVolume());
}

ECGMenuContent::ECGMenuContent()
    : MenuContent(trs("ECGMenu"), trs("ECGMenuDesc")),
      d_ptr(new ECGMenuContentPrivate)
{
}

ECGMenuContent::~ECGMenuContent()
{
    delete d_ptr;
}

void ECGMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ECGMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // lead mode
    label = new QLabel(trs("ECGLeadMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_3))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_5))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_12))
                      );
    itemID = ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE, comboBox);

    // ECG
    label = new QLabel("ECG");
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = ECGMenuContentPrivate::ITEM_CBO_ECG;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_ECG, comboBox);

    // ECG 增益
    label = new QLabel(trs("ECGGain"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < ECG_GAIN_NR; i++)
    {
        comboBox->addItem(ECGSymbol::convert(static_cast<ECGGain>(i)));
    }
    itemID  = ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN, comboBox);

    // filter
    label = new QLabel(trs("FilterMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_ST))
                      );
    itemID = ECGMenuContentPrivate::ITEM_CBO_FILTER_MODE;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

    // notch
    label = new QLabel(trs("NotchFilter"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER, comboBox);

    // paceMark
    label = new QLabel(trs("ECGPaceMarker"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_PACE_OFF))
                       << trs(ECGSymbol::convert(ECG_PACE_ON))
                      );
    itemID = ECGMenuContentPrivate::ITEM_CBO_PACER_MARK;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_PACER_MARK, comboBox);

    // sweep speed
    label = new QLabel(trs("ECGSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_125))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_250))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_500))
                      );
    itemID = ECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // qrs tone
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        label = new QLabel(trs("ECGQRSPRToneVolume"));
    }
    else
    {
        label = new QLabel(trs("ECGQRSToneVolume"));
    }
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItem(trs("Off"));
    for (int i = SoundManager::VOLUME_LEV_1; i <= SoundManager::VOLUME_LEV_MAX; i++)
    {
        comboBox->addItem(QString::number(i));
    }
    itemID = ECGMenuContentPrivate::ITEM_CBO_QRS_TONE;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_QRS_TONE, comboBox);

    // self learn
    d_ptr->selfLearnBtn = new Button(trs("SelfLearn"));
    d_ptr->selfLearnBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->selfLearnBtn, d_ptr->combos.count(), 1);
    connect(d_ptr->selfLearnBtn, SIGNAL(released()), this, SLOT(selfLearnBtnReleased()));

    // 心律失常
    d_ptr->arrhythmiaBtn = new Button(trs("Arrhythmia"));
    d_ptr->arrhythmiaBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->arrhythmiaBtn, d_ptr->combos.count() + 1, 1);
    connect(d_ptr->arrhythmiaBtn, SIGNAL(released()), this, SLOT(arrhythmiaBtnReleased()));

    // ST 段开关
    d_ptr->sTSwitchBtn = new Button;
    d_ptr->sTSwitchBtn->setText(QString("%1 >>").arg(trs("STAnalysize")));
    d_ptr->sTSwitchBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->sTSwitchBtn, d_ptr->combos.count() + 2, 1);
    connect(d_ptr->sTSwitchBtn, SIGNAL(released()), this, SLOT(onSTSwitchBtnReleased()));

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count() + 3, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 4, 1);
}

void ECGMenuContent::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        ECGMenuContentPrivate::MenuItem item
            = (ECGMenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE:
            ecgParam.setLeadMode(static_cast<ECGLeadMode>(index));
            d_ptr->loadOptions();
            break;
        case ECGMenuContentPrivate::ITEM_CBO_ECG:
        {
            QString waveName = ecgParam.getCalcLeadWaveformName();
            ecgParam.setCalcLead(d_ptr->ecgWaveforms[index]);
            ecgParam.setLeadMode3DisplayLead(d_ptr->ecgWaveforms[index]);
            windowManager.replaceWaveform(waveName, d_ptr->ecgWaveforms[index]);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
            ECGGain gain = ecgParam.getGain(static_cast<ECGLead>(index));
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(gain);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);
            break;
        }
        case ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN:
        {
            ECGLead ecg = static_cast<ECGLead>(d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG]->currentIndex());
            ecgParam.setGain(static_cast<ECGGain>(index), ecg);
            break;
        }
        case ECGMenuContentPrivate::ITEM_CBO_FILTER_MODE:
        {
            ecgParam.setFilterMode(index);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->clear();
            switch (index)
            {
            case ECG_FILTERMODE_MONITOR:
            case ECG_FILTERMODE_SURGERY:
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                addItems(QStringList()
                         << trs(ECGSymbol::convert(ECG_NOTCH_OFF1))
                         << trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ))
                        );
                break;
            case ECG_FILTERMODE_DIAGNOSTIC:
            case ECG_FILTERMODE_ST:
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                addItems(QStringList()
                         << trs(ECGSymbol::convert(ECG_NOTCH_OFF1))
                         << trs(ECGSymbol::convert(ECG_NOTCH_50HZ))
                         << trs(ECGSymbol::convert(ECG_NOTCH_60HZ))
                        );
                break;
            default:
                break;
            }
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
            setCurrentIndex(ECG_NOTCH_OFF1);
            break;
        }
        case ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER:
        {
            ECGFilterMode filterMode = ecgParam.getFilterMode();
            if (filterMode == ECG_FILTERMODE_MONITOR || filterMode == ECG_FILTERMODE_SURGERY)
            {
                if (index == 1)
                {
                    index = ECG_NOTCH_50_AND_60HZ;
                }
            }
            ecgParam.setNotchFilter(static_cast<ECGNotchFilter>(index));
            break;
        }
        case ECGMenuContentPrivate::ITEM_CBO_PACER_MARK:
            ecgParam.setPacermaker(static_cast<ECGPaceMode>(index));
            break;
        case ECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            ecgParam.setSweepSpeed(static_cast<ECGSweepSpeed>(index));
            break;
        case ECGMenuContentPrivate::ITEM_CBO_QRS_TONE:
            ecgParam.setQRSToneVolume(static_cast<SoundManager::VolumeLevel>(index));
            break;
        default:
            break;
        }
    }
}

void ECGMenuContent::arrhythmiaBtnReleased()
{
    ArrhythmiaMenuWindow *instance = ArrhythmiaMenuWindow::getInstance();
    windowManager.showWindow(instance, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorHideOthers);
}

void ECGMenuContent::selfLearnBtnReleased()
{
    ecgParam.setSelfLearn(1);
}

void ECGMenuContent::onSTSwitchBtnReleased()
{
}

void ECGMenuContent::onAlarmBtnReleased()
{
    MainMenuWindow *w = MainMenuWindow::getInstance();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_HR_PR, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

