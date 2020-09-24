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
#include <QLabel>
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QGridLayout>
#include <QList>
#include "ECGSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "ECGParam.h"
#include "IConfig.h"
#include "ConfigManager.h"
#include "ArrhythmiaMenuWindow.h"
#include "WindowManager.h"
#include <QTimer>
#include "MainMenuWindow.h"
#include "LayoutManager.h"
#include "ECGDupParam.h"
#include "AlarmLimitWindow.h"
#include "SPO2Param.h"
#include "IBPParam.h"
#include "NightModeManager.h"
#include <QTimerEvent>

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
        :
    #ifndef HIDE_ECG_ARRHYTHMIA_FUNCTION
         arrhythmiaBtn(NULL),
    #endif
         ecg1Label(NULL),
         ecg2Label(NULL),
         leadModeTimerId(-1)
    {}

    // load settings
    void loadOptions();

    /**
     * @brief setCboBlockSignal 设置下拉框信号阻塞
     * @param flag
     */
    void setCboBlockSignal(bool flag);

    /**
     * @brief updatePrintWaveIds
     */
    void updatePrintWaveIds();

    /**
     * @brief getCurGainIndex 获取ECG增益在下拉框中的当前索引
     * @param gain  ECG增益
     * @return   下拉框中的当前索引
     */
    int getCurGainIndex(ECGGain gain);

    QMap<MenuItem, ComboBox *> combos;
#ifndef HIDE_ECG_ARRHYTHMIA_FUNCTION
    Button *arrhythmiaBtn;
#endif
    QStringList ecgWaveforms;
    QStringList ecgWaveformTitles;
    QLabel *ecg1Label;
    QLabel *ecg2Label;
    QList<int> ecgWaveIdList;
    int leadModeTimerId;
};

void ECGMenuContentPrivate::loadOptions()
{
    int sourceType = 0;;
    setCboBlockSignal(true);
    combos[ITEM_CBO_HRPR_SOURCE]->clear();
    currentConfig.getNumValue("ECG|HRSource", sourceType);
    int cboIndex = 0;
    int itemCount = 0;
    for (int i = HR_SOURCE_ECG; i < HR_SOURCE_NR; ++i)
    {
        if (i == HR_SOURCE_SPO2 && !systemManager.isSupport(PARAM_SPO2))
        {
            continue;
        }
        // ibp disconnected or not supported
        if (i == HR_SOURCE_IBP && (!systemManager.isSupport(PARAM_IBP) || !ibpParam.isConnected()))
        {
            continue;
        }

        if (i == sourceType)
        {
            cboIndex = itemCount;   // 选中对应item
        }
        itemCount++;
        combos[ITEM_CBO_HRPR_SOURCE]->addItem(trs(ECGSymbol::convert(static_cast<HRSourceType>(i))));
    }

    if (cboIndex > combos[ITEM_CBO_HRPR_SOURCE]->count())
    {
        cboIndex = 0;
        currentConfig.setNumValue("ECG|HRSource", cboIndex);
    }
    combos[ITEM_CBO_HRPR_SOURCE]->setCurrentIndex(cboIndex);


    ECGLeadMode leadMode = ecgParam.getLeadMode();
    combos[ITEM_CBO_LEAD_MODE]->setCurrentIndex(leadMode);

    ecgParam.getAvailableWaveforms(&ecgWaveforms, &ecgWaveformTitles, true);
    combos[ITEM_CBO_ECG1]->clear();
    combos[ITEM_CBO_ECG2]->clear();
    combos[ITEM_CBO_ECG1]->addItems(ecgWaveformTitles);
    combos[ITEM_CBO_ECG2]->addItems(ecgWaveformTitles);
    int index1 = 0;
    int index2 = 0;
    currentConfig.getNumValue("ECG|Ecg2Wave", index2);
    currentConfig.getNumValue("ECG|Ecg1Wave", index1);

    // 保证当前的ecg1、2的索引值小于当前ecg波形的数量
    int items = combos[ITEM_CBO_ECG1]->count();
    if (index1 >= items)
    {
        index1 = ecgParam.getCalcLead();
        if (index1 >= items)
        {
            index1 = ECG_LEAD_I;
        }
        currentConfig.setNumValue("ECG|Ecg1Wave", index1);
    }
    // 非3导联模式下，改变ecg2索引
    if (leadMode != ECG_LEAD_MODE_3)
    {
        if (index2 >= items)
        {
            index2 = ECG_LEAD_I;
            if (index2 == index1)
            {
                index2 += 1;
            }
            currentConfig.setNumValue("ECG|Ecg2Wave", index2);
        }
    }

    // 读取的ecg1wave、ecg2wave如果item相同，则复位为0、1.
    if (leadMode != ECG_LEAD_MODE_3 && index2 == index1)
    {
        index1 = 1;
        index2 = 0;
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
    int index = ecgWaveforms.indexOf(ecgTopWaveform);
    if (index >= 0)
    {
        ECGGain gain = ecgParam.getGain(static_cast<ECGLead>(index));
#ifdef HIDE_ECG_GAIN_X0125_AND_X4
        /*
         * 根据DV IEC测试反馈问题，
         * 在ECG增益为X4时，会出现波形截顶情况；以及在ECG增益为X0.125时，波形振幅太小，基本为直线；
         * 所以DV要求不显示X0.125，X4 ECG增益
         */
        combos[ITEM_CBO_ECG_GAIN]->clear();
        for (int i = ECG_GAIN_X025; i < ECG_GAIN_X40; i++)
        {
            /* 增加ECG增益选项: X0.25, X0.5, X1, X2 */
            combos[ITEM_CBO_ECG_GAIN]->addItem(trs(ECGSymbol::convert(static_cast<ECGGain>(i))),
                                               qVariantFromValue(i));
        }
        /* 增加ECG增益选项: AUTO */
        combos[ITEM_CBO_ECG_GAIN]->addItem(trs(ECGSymbol::convert(ECG_GAIN_AUTO)),
                                           qVariantFromValue(static_cast<int>(ECG_GAIN_AUTO)));
#else
        for (int i = ECG_GAIN_X0125; i < ECG_GAIN_NR; i++)
        {
            combos[ITEM_CBO_ECG_GAIN]->addItem(trs(ECGSymbol::convert(static_cast<ECGGain>(i))),
                                               qVariantFromValue(i));
        }
#endif

        combos[ITEM_CBO_ECG_GAIN]->setCurrentIndex(getCurGainIndex(gain));
        combos[ITEM_CBO_ECG1]->setCurrentIndex(index);
    }

    combos[ITEM_CBO_ECG2]->setCurrentIndex(index2);

    ECGFilterMode filterMode = ecgParam.getFilterMode();

    // demo 模式下心电滤波模式固定为诊断，不可更改
    WorkMode workMode = systemManager.getCurWorkMode();
    if (workMode == WORK_MODE_DEMO)
    {
        combos[ITEM_CBO_FILTER_MODE]->setEnabled(false);
        combos[ITEM_CBO_NOTCH_FITER]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_FILTER_MODE]->setEnabled(true);
        combos[ITEM_CBO_NOTCH_FITER]->setEnabled(true);
    }

    // demo模式,12导界面下心电增益改为不可调
    UserFaceType faceType =  layoutManager.getUFaceType();
    if (workMode == WORK_MODE_DEMO)
    {
        combos[ITEM_CBO_ECG_GAIN]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_ECG_GAIN]->setEnabled(true);
    }

    ECGNotchFilter notchFilter = ecgParam.getNotchFilter();
    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(filterMode);

    combos[ITEM_CBO_NOTCH_FITER]->clear();
    switch (filterMode)
    {
    case ECG_FILTERMODE_MONITOR:
    case ECG_FILTERMODE_SURGERY:
#ifdef ECG_MONITOR_NOTIFY_FILTER_OFF
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF)), ECG_NOTCH_OFF);
#endif
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ)), ECG_NOTCH_50_AND_60HZ);
        break;
    case ECG_FILTERMODE_DIAGNOSTIC:
    case ECG_FILTERMODE_ST:
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF)), ECG_NOTCH_OFF);
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
        combos[ITEM_CBO_NOTCH_FITER]->addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
        break;
    default:
        break;
    }
    int notchFilterIndex = combos[ITEM_CBO_NOTCH_FITER]->findText(trs(ECGSymbol::convert(notchFilter)));
    if (notchFilterIndex > -1)
    {
        combos[ITEM_CBO_NOTCH_FITER]->setCurrentIndex(notchFilterIndex);
    }
    else
    {
        combos[ITEM_CBO_NOTCH_FITER]->setCurrentIndex(0);
    }

    combos[ITEM_CBO_PACER_MARK]->setCurrentIndex(ecgParam.getPacermaker());

    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ecgParam.getSweepSpeed());

    currentConfig.getNumValue("ECG|QRSVolume", index);
    combos[ITEM_CBO_QRS_TONE]->setCurrentIndex(index);
    if (nightModeManager.nightMode())
    {
        combos[ITEM_CBO_QRS_TONE]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_QRS_TONE]->setEnabled(true);
    }

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

    setCboBlockSignal(false);
}

void ECGMenuContentPrivate::setCboBlockSignal(bool flag)
{
    for (int i = ITEM_CBO_HRPR_SOURCE; i <=  ITEM_CBO_QRS_TONE; ++i)
    {
        MenuItem itemId = static_cast<MenuItem>(i);
        combos[itemId]->blockSignals(flag);
    }
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

int ECGMenuContentPrivate::getCurGainIndex(ECGGain gain)
{
#ifdef HIDE_ECG_GAIN_X0125_AND_X4
    /*
     * 根据DV IEC测试反馈问题，
     * 在ECG增益为X4时，会出现波形截顶情况；以及在ECG增益为X0.125时，波形振幅太小，基本为直线；
     * 获取不显示X0.125，X4 ECG增益后各增益对应的索引。
     */
    if (gain == ECG_GAIN_AUTO)
    {
        return combos[ITEM_CBO_ECG_GAIN]->count() - 1;
    }

    return gain - ECG_GAIN_X025;
#else
    return gain;
#endif
}

ECGMenuContent::ECGMenuContent()
    : MenuContent(trs("ECGMenu"), trs("ECGMenuDesc")),
      d_ptr(new ECGMenuContentPrivate)
{
    connect(&ibpParam, SIGNAL(ibpConnectStatusUpdated(bool)), this, SLOT(updateHRPRSource()));
}

ECGMenuContent::~ECGMenuContent()
{
    delete d_ptr;
}

void ECGMenuContent::readyShow()
{
    d_ptr->loadOptions();
    d_ptr->leadModeTimerId = startTimer(500);
}

void ECGMenuContent::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e)
    if (d_ptr->leadModeTimerId != -1)
    {
        killTimer(d_ptr->leadModeTimerId);
        d_ptr->leadModeTimerId = -1;
    }
}

void ECGMenuContent::timerEvent(QTimerEvent *e)
{
    if (d_ptr->leadModeTimerId == e->timerId())
    {
        ECGLeadMode leadMode = ecgParam.getLeadMode();
        d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE]->setCurrentIndex(leadMode);
    }
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
#ifndef HIDE_ECG_12_LEAD_FUNCTION
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_3))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_5))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_12)));
#else
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_3))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_5)));
#endif
    itemID = ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE;
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE, comboBox);

    // ECG
    label = new QLabel(trs("ECG1"));
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
    label = new QLabel(trs("ECG2"));
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
#ifndef  HIDE_ECG_ST_PVCS_SUBPARAM
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_ST)));
#else
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC)));
#endif
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
    label = new QLabel(trs("ECGPaceDetection"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_PACE_OFF))
                       << trs(ECGSymbol::convert(ECG_PACE_ON)));
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
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_625))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_125))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_250))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_500)));
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
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_QRS_TONE, comboBox);

#ifndef HIDE_ECG_ARRHYTHMIA_FUNCTION
    // 心律失常
    d_ptr->arrhythmiaBtn = new Button(trs("Arrhythmia"));
    d_ptr->arrhythmiaBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->arrhythmiaBtn, d_ptr->combos.count() + 1, 1);
    connect(d_ptr->arrhythmiaBtn, SIGNAL(released()), this, SLOT(arrhythmiaBtnReleased()));
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
            HRSourceType sourceType = HR_SOURCE_NR;
            QString itemText = box->currentText();
            for (int i = HR_SOURCE_ECG; i < HR_SOURCE_NR; i++)
            {
                HRSourceType type = static_cast<HRSourceType>(i);
                if (itemText == trs(ECGSymbol::convert(type)))
                {
                    sourceType = type;
                    break;
                }
            }

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
        break;
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

                    ECGLeadMode leadMode = ecgParam.getLeadMode();
                    ECGLead leadLimit = ECG_LEAD_V6;
                    if (leadMode == ECG_LEAD_MODE_3)
                    {
                        leadLimit = ECG_LEAD_III;
                    }
                    else if (leadMode == ECG_LEAD_MODE_5)
                    {
                        leadLimit = ECG_LEAD_V1;
                    }
                    if (waveIndex > leadLimit)
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
            currentConfig.setNumValue("ECG|Ecg1Wave", index);
            layoutManager.updateLayout();
            // 需要在布局更新后调用更新参数接口
            ecgParam.updateWaveWidgetStatus();
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
            ECGGain gain = ecgParam.getGain(static_cast<ECGLead>(index));
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(d_ptr->getCurGainIndex(gain));
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);
            break;
        }
        case ECGMenuContentPrivate::ITEM_CBO_ECG2:
        {
            ECGLead lead = static_cast<ECGLead>(index);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
            ECGGain gain = ecgParam.getGain(lead);
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(d_ptr->getCurGainIndex(gain));
            d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);

            bool isUpdateWaveStatus = false;
            int preECG2Wave = 0;
            currentConfig.getNumValue("ECG|Ecg2Wave", preECG2Wave);
            // 如果ecg2与ecg1选择相同的item时
            if (lead == ecgParam.getCalcLead())
            {
                // 更新最新导联
                currentConfig.setNumValue("ECG|Ecg1Wave", preECG2Wave);
                lead = static_cast<ECGLead>(preECG2Wave);
                ecgParam.setCalcLead(d_ptr->ecgWaveforms[preECG2Wave]);
                ecgParam.setLeadMode3DisplayLead(d_ptr->ecgWaveforms[preECG2Wave]);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(true);
                ECGGain gain = ecgParam.getGain(lead);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->setCurrentIndex(d_ptr->getCurGainIndex(gain));
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG_GAIN]->blockSignals(false);

                // 更新ecg1的item选择
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->blockSignals(true);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->setCurrentIndex(preECG2Wave);
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
            QVariant itemData = (box->itemData(index));
            if (layoutManager.getUFaceType() == UFACE_MONITOR_ECG_FULLSCREEN)
            {
                ecgParam.setGain(static_cast<ECGGain>(itemData.toInt()));
            }
            else
            {
                ECGLead ecg = static_cast<ECGLead>(d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_ECG1]->currentIndex());
                ecgParam.setGain(static_cast<ECGGain>(itemData.toInt()), ecg);
            }
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
#ifdef ECG_MONITOR_NOTIFY_FILTER_OFF
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF)), ECG_NOTCH_OFF);
#endif
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ)), ECG_NOTCH_50_AND_60HZ);
                break;
            case ECG_FILTERMODE_DIAGNOSTIC:
            case ECG_FILTERMODE_ST:
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_OFF)), ECG_NOTCH_OFF);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_50HZ)), ECG_NOTCH_50HZ);
                d_ptr->combos[ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER]->
                        addItem(trs(ECGSymbol::convert(ECG_NOTCH_60HZ)), ECG_NOTCH_60HZ);
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
            ECGNotchFilter notch = static_cast<ECGNotchFilter>(d_ptr->combos[ECGMenuContentPrivate
                                                               ::ITEM_CBO_NOTCH_FITER]->itemData(index).toInt());
            ecgParam.setNotchFilter(notch);
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

#ifndef HIDE_ECG_ARRHYTHMIA_FUNCTION
void ECGMenuContent::arrhythmiaBtnReleased()
{
    ArrhythmiaMenuWindow *instance = ArrhythmiaMenuWindow::getInstance();
    windowManager.showWindow(instance, WindowManager::ShowBehaviorModal | WindowManager::ShowBehaviorHideOthers);
}
#endif

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
        /* play sound with the focus volume */
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.heartBeatTone();
        /* reset the sound volume */
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT ,
                               static_cast<SoundManager::VolumeLevel>(w->currentIndex()));
    }
}

void ECGMenuContent::updateHRPRSource()
{
    d_ptr->loadOptions();
}

