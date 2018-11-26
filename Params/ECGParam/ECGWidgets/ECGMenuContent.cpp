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
#include "LayoutManager.h"
#include "ECGDupParam.h"
#include "AlarmLimitWindow.h"
#include "SPO2Param.h"

#define PRINT_WAVE_NUM (3)

class ECGMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_HRPR_SOURCE = 1,
        ITEM_CBO_LEAD_MODE,
        ITEM_CBO_ECG1,
        ITEM_CBO_ECG2,
        ITEM_CBO_ECG_GAIN,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_NOTCH_FITER,
        ITEM_CBO_PACER_MARK,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_QRS_TONE,
    };

    ECGMenuContentPrivate()
        : selfLearnBtn(NULL)
        , arrhythmiaBtn(NULL)
        , sTSwitchBtn(NULL)
        , ecg1Label(NULL)
        , ecg2Label(NULL)
    {}

    // load settings
    void loadOptions();

    /**
     * @brief updatePrintWaveIds
     */
    void updatePrintWaveIds();

    QMap<MenuItem, ComboBox *> combos;
    Button *selfLearnBtn;
    Button *arrhythmiaBtn;
    Button *sTSwitchBtn;
    QStringList ecgWaveforms;
    QStringList ecgWaveformTitles;
    QLabel *ecg1Label;
    QLabel *ecg2Label;
    QList<int> ecgWaveIdList;
};

void ECGMenuContentPrivate::loadOptions()
{
    int index = 0;

    currentConfig.getNumValue("ECG|HRSource", index);
    combos[ITEM_CBO_HRPR_SOURCE]->setCurrentIndex(index);

    ECGLeadMode leadMode = ecgParam.getLeadMode();
    combos[ITEM_CBO_LEAD_MODE]->blockSignals(true);
    combos[ITEM_CBO_LEAD_MODE]->setCurrentIndex(leadMode);
    combos[ITEM_CBO_LEAD_MODE]->blockSignals(false);

    combos[ITEM_CBO_ECG1]->blockSignals(true);
    combos[ITEM_CBO_ECG2]->blockSignals(true);
    ecgParam.getAvailableWaveforms(ecgWaveforms, ecgWaveformTitles, true);
    combos[ITEM_CBO_ECG1]->clear();
    combos[ITEM_CBO_ECG2]->clear();
    combos[ITEM_CBO_ECG1]->addItems(ecgWaveformTitles);
    combos[ITEM_CBO_ECG2]->addItems(ecgWaveformTitles);
    int index1 = 0;
    int index2 = 0;
    currentConfig.getNumValue("ECG|Ecg2Wave", index2);
    currentConfig.getNumValue("ECG|Ecg1Wave", index1);
    // 读取的ecg1wave、ecg2wave如果item相同，则复位为0、1.
    if (index2 == index1)
    {
        index1 = 0;
        index2 = 1;
        currentConfig.setNumValue("ECG|Ecg1Wave", index1);
        currentConfig.setNumValue("ECG|Ecg2Wave", index2);
    }

    // 更新初始状态ecg1,2波形id
    ecgWaveIdList.clear();
    ECGLead ecgLead = static_cast<ECGLead>(index1);
    WaveformID ecgwaveID = ecgParam.leadToWaveID(ecgLead);
    ecgWaveIdList.append(ecgwaveID);

    ecgLead = static_cast<ECGLead>(index2);
    ecgwaveID = ecgParam.leadToWaveID(ecgLead);
    ecgWaveIdList.append(ecgwaveID);

    // 保证导联的item与ecg1wave的item值一致
    int lead = ecgParam.getCalcLead();
    if (index1 != lead)
    {
        ecgParam.setCalcLead(static_cast<ECGLead>(index1));
    }
    QString ecgTopWaveform = ecgParam.getCalcLeadWaveformName();
    index = ecgWaveforms.indexOf(ecgTopWaveform);
    if (index >= 0)
    {
        combos[ITEM_CBO_ECG_GAIN]->blockSignals(true);
        ECGGain gain = ecgParam.getGain(static_cast<ECGLead>(index));
        combos[ITEM_CBO_ECG_GAIN]->setCurrentIndex(gain);
        combos[ITEM_CBO_ECG_GAIN]->blockSignals(false);
        combos[ITEM_CBO_ECG1]->setCurrentIndex(index);
    }
    combos[ITEM_CBO_ECG1]->blockSignals(false);

    combos[ITEM_CBO_ECG2]->setCurrentIndex(index2);
    combos[ITEM_CBO_ECG2]->blockSignals(false);

    ECGFilterMode filterMode = ecgParam.getFilterMode();

    // demo 模式下心电滤波模式固定为诊断，不可更改
    WorkMode workMode = systemManager.getCurWorkMode();
    if (workMode == WORK_MODE_DEMO)
    {
        if (filterMode != ECG_FILTERMODE_DIAGNOSTIC)
        {
            filterMode = ECG_FILTERMODE_DIAGNOSTIC;
            ecgParam.setFilterMode(filterMode);
        }
        combos[ITEM_CBO_FILTER_MODE]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_FILTER_MODE]->setEnabled(true);
    }

    // demo模式,12导界面下心电增益改为不可调
    UserFaceType faceType =  layoutManager.getUFaceType();
    if (workMode == WORK_MODE_DEMO
            && faceType == UFACE_MONITOR_ECG_FULLSCREEN)
    {
        combos[ITEM_CBO_ECG_GAIN]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_ECG_GAIN]->setEnabled(true);
    }

    ECGNotchFilter notchFilter = ecgParam.getNotchFilter();
    combos[ITEM_CBO_FILTER_MODE]->blockSignals(true);
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(filterMode);
    combos[ITEM_CBO_FILTER_MODE]->blockSignals(false);

    combos[ITEM_CBO_NOTCH_FITER]->clear();
    switch (filterMode)
    {
    case ECG_FILTERMODE_MONITOR:
    case ECG_FILTERMODE_SURGERY:
        combos[ITEM_CBO_NOTCH_FITER]->
        addItems(QStringList()
                 << trs(ECGSymbol::convert(ECG_NOTCH_OFF))
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
                 << trs(ECGSymbol::convert(ECG_NOTCH_OFF))
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

    int volSPO2 = spo2Param.getBeatVol();
    int volECG = ecgParam.getQRSToneVolume();
    if (volSPO2 != volECG)
    {
        // 保持脉搏音与心跳音同步
        spo2Param.setBeatVol(static_cast<SoundManager::VolumeLevel>(volECG));
    }

    combos[ITEM_CBO_QRS_TONE]->setCurrentIndex(volECG);

    bool isHide = true;

    // 增加判断是否为全屏12导界面，如果是，则显示一个ECG item
    if (leadMode == ECG_LEAD_MODE_3
            || faceType == UFACE_MONITOR_ECG_FULLSCREEN)
    {
        isHide = true;
        ecg1Label->setText(trs("ECG"));
    }
    else
    {
        isHide = false;
        ecg1Label->setText(trs("ECG1"));
    }
    ecg2Label->setVisible(!isHide);
    combos[ITEM_CBO_ECG2]->setVisible(!isHide);
}

void ECGMenuContentPrivate::updatePrintWaveIds()
{
    // ecg1
    int cboItem = 0;
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        int waveId = WAVE_NONE;
        systemConfig.getNumValue(path, waveId);
        // 旧的打印波形与ecg当前保存的波形一致时
        if (waveId == ecgWaveIdList.at(0))
        {
            int index = 0;
            currentConfig.getNumValue("ECG|Ecg1Wave", index);
            ECGLead ecgLead = static_cast<ECGLead>(index);
            WaveformID ecgwaveID = ecgParam.leadToWaveID(ecgLead);
            // 替换波形
            if (ecgwaveID != waveId)
            {
                systemConfig.setNumValue(path, static_cast<int>(ecgwaveID));
            }
            cboItem = i;
            break;
        }
    }

    // ecg2
    ECGLeadMode leadMode = ecgParam.getLeadMode();
    for (int i = 0; i < PRINT_WAVE_NUM; i++)
    {
        // 剔除ecg1对应的选择item
        if (cboItem == i)
        {
            continue;
        }
        QString path;
        path = QString("Print|SelectWave%1").arg(i + 1);
        int waveId = WAVE_NONE;
        systemConfig.getNumValue(path, waveId);
        // 旧的打印波形与ecg当前保存的波形一致时
        if (waveId == ecgWaveIdList.at(1))
        {
            int index = 0;
            currentConfig.getNumValue("ECG|Ecg2Wave", index);
            ECGLead ecgLead = static_cast<ECGLead>(index);
            WaveformID ecgwaveID = ecgParam.leadToWaveID(ecgLead);
            // 替换波形
            if (leadMode == ECG_LEAD_MODE_5
                    || leadMode == ECG_LEAD_MODE_12)
            {
                if (ecgwaveID != waveId)
                {
                    systemConfig.setNumValue(path, static_cast<int>(ecgwaveID));
                }
            }
            else
            {
                systemConfig.setNumValue(path, static_cast<int>(WAVE_NONE));
            }
            break;
        }
    }
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

    // hr/pr 来源
    label = new QLabel(trs("HR_PRSource"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(HR_SOURCE_ECG))
                       << trs(ECGSymbol::convert(HR_SOURCE_SPO2))
                       << trs(ECGSymbol::convert(HR_SOURCE_IBP))
                       << trs(ECGSymbol::convert(HR_SOURCE_AUTO))
                      );
    itemID = ECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE, comboBox);

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
    label = new QLabel("ECG1");
    d_ptr->ecg1Label = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = ECGMenuContentPrivate::ITEM_CBO_ECG1;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_ECG1, comboBox);

    // ECG2
    label = new QLabel("ECG2");
    d_ptr->ecg2Label = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = ECGMenuContentPrivate::ITEM_CBO_ECG2;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_ECG2, comboBox);

    // ECG 增益
    label = new QLabel(trs("ECGGain"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < ECG_GAIN_NR; i++)
    {
        comboBox->addItem(trs(ECGSymbol::convert(static_cast<ECGGain>(i))));
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
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
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
    label = new QLabel(trs("ECGQRSToneVolume"));
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
        case ECGMenuContentPrivate::ITEM_CBO_HRPR_SOURCE:
        {
            HRSourceType sourceType = static_cast<HRSourceType>(index);
            ecgDupParam.setHrSource(sourceType);

            // 切换类型时手动更新hr/pr值，避免上次pr/hr值为无效值
            short originalHR = ecgDupParam.getHR(true);
            PRSourceType prSourceType = ecgDupParam.getCurPRSource();
            // 增加自动模式下优先更新hr功能
            if (sourceType == HR_SOURCE_ECG
                    || sourceType == HR_SOURCE_AUTO)
            {
                ecgDupParam.updateHR(originalHR);
            }
            else if (sourceType == HR_SOURCE_SPO2
                     || sourceType == HR_SOURCE_IBP)
            {
                ecgDupParam.updatePR(0, prSourceType, false);
            }
        }
        break;
        case ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE:
        {
            ECGLeadMode mode = static_cast<ECGLeadMode>(index);
            ecgParam.setLeadMode(mode);
            if (mode == ECG_LEAD_MODE_3 && layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN)
            {
                // no ecg full screen in 3 lead mode
                layoutManager.setUFaceType(UFACE_MONITOR_STANDARD);
            }
            int lastECGIndex = d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->currentIndex();
            d_ptr->loadOptions();
            index = d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->currentIndex();
            if (lastECGIndex == index)
            {
                break;
            }

            // fall through, because the calculate lead changed
        }
        case ECGMenuContentPrivate::ITEM_CBO_ECG1:
        {
            int waveIndex = 0;
            currentConfig.getNumValue("ECG|Ecg2Wave", waveIndex);
            // 如果ecg2与ecg1选择相同的item时
            if (waveIndex == index)
            {
                int calLead = ecgParam.getCalcLead();
                // 在导联模式先于ecg1波形索引发生改变的情况下,如果此时的计算导联与旧ecg2的波形索引一致
                // 那么预设ecg2的波形索引向后加1,避免与计算导联选择相同
                if (waveIndex == calLead)
                {
                    waveIndex = calLead + 1;

                    if (waveIndex >= ECG_LEAD_NR)
                    {
                        waveIndex = ECG_LEAD_I;
                    }
                }
                else
                {
                    waveIndex =  calLead;
                }

                // 更新ecg2的item选择
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG2]->blockSignals(true);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG2]->setCurrentIndex(waveIndex);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG2]->blockSignals(false);
                currentConfig.setNumValue("ECG|Ecg2Wave", waveIndex);
            }

            ecgParam.setCalcLead(d_ptr->ecgWaveforms[index]);
            ecgParam.setLeadMode3DisplayLead(d_ptr->ecgWaveforms[index]);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
            ECGGain gain = ecgParam.getGain(static_cast<ECGLead>(index));
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(gain);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);
            currentConfig.setNumValue("ECG|Ecg1Wave", index);
            layoutManager.updateLayout();
            // 需要在布局更新后调用更新参数接口
            ecgParam.updateWaveWidgetStatus();
            break;
        }
        case ECGMenuContentPrivate::ITEM_CBO_ECG2:
        {
            ECGLead lead = static_cast<ECGLead>(index);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
            ECGGain gain = ecgParam.getGain(lead);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(gain);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);

            bool isUpdateWaveStatus = false;
            // 如果ecg2与ecg1选择相同的item时
            if (lead == ecgParam.getCalcLead())
            {
                // 更新最新导联
                int waveIndex = 0;
                currentConfig.getNumValue("ECG|Ecg2Wave", waveIndex);
                currentConfig.setNumValue("ECG|Ecg1Wave", waveIndex);
                lead = static_cast<ECGLead>(waveIndex);
                ecgParam.setCalcLead(d_ptr->ecgWaveforms[waveIndex]);
                ecgParam.setLeadMode3DisplayLead(d_ptr->ecgWaveforms[waveIndex]);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
                ECGGain gain = ecgParam.getGain(lead);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(gain);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);

                // 更新ecg1的item选择
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->blockSignals(true);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->setCurrentIndex(waveIndex);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->blockSignals(false);
                isUpdateWaveStatus = true;
            }
            currentConfig.setNumValue("ECG|Ecg2Wave", index);
            layoutManager.updateLayout();
            if (isUpdateWaveStatus)
            {
                // 需要在布局更新后调用更新参数接口
                ecgParam.updateWaveWidgetStatus();
            }
            break;
        }

        case ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN:
        {
            ECGLead ecg = static_cast<ECGLead>(d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->currentIndex());
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
                         << trs(ECGSymbol::convert(ECG_NOTCH_OFF))
                         << trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ))
                        );
                break;
            case ECG_FILTERMODE_DIAGNOSTIC:
            case ECG_FILTERMODE_ST:
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                addItems(QStringList()
                         << trs(ECGSymbol::convert(ECG_NOTCH_OFF))
                         << trs(ECGSymbol::convert(ECG_NOTCH_50HZ))
                         << trs(ECGSymbol::convert(ECG_NOTCH_60HZ))
                        );
                break;
            default:
                break;
            }
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
            setCurrentIndex(ECG_NOTCH_OFF);
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

        if (item == ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE
                || item == ECGMenuContentPrivate::ITEM_CBO_ECG1
                || item == ECGMenuContentPrivate::ITEM_CBO_ECG2)
        {
            d_ptr->updatePrintWaveIds();
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
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_HR_PR, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}

void ECGMenuContent::onPopupListItemFocusChanged(int volume)
{
    ComboBox * w = qobject_cast<ComboBox*>(sender());
    if (w == d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_QRS_TONE])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.heartBeatTone();
    }
}

