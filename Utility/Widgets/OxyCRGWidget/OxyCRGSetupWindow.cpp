/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/8/20
 **/
#include "OxyCRGSetupWindow.h"
#include "ComboBox.h"
#include <QMap>
#include <QLabel>
#include "LanguageManager.h"
#include <QGridLayout>
#include "OxyCRGSymbol.h"
#include "ConfigManager.h"
#include <QGroupBox>
#include "Button.h"
#include "WindowManager.h"
#include <QLineEdit>

class OxyCRGSetupWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_TREND_ONE = 0,
        ITEM_CBO_WAVE_SELECT,
        ITEM_CBO_HR_LOW,
        ITEM_CBO_HR_HIGH,
        ITEM_CBO_RR_LOW,
        ITEM_CBO_RR_HIGH,
        ITEM_CBO_SPO2_LOW,
        ITEM_CBO_CO2_HIGH,
        ITEM_CBO_MAX,
    };


    OxyCRGSetupWindowPrivate()
        : waveTypeIndex(0)
    {
        for (int i = 0; i < 8; i++)
        {
            rulerValue.data[i] = 0;
        }
        rulerValue.s.spo2High = 100;
        rulerValue.s.co2Low = 0;
        rulerUpdated.hrUpdatedStatus = false;
        rulerUpdated.spo2UpdatedStatus = false;
        rulerUpdated.co2UpdatedStatus = false;
        rulerUpdated.rrUpdatedStatus = false;
    }
    union RulerValue
        {
            int data[8];
            struct RulerValueStruct
            {
                int hrLow;
                int hrHigh;
                int spo2Low;
                int spo2High;
                int co2Low;
                int co2High;
                int rrLow;
                int rrHigh;
            } s;
        } rulerValue;

    struct RulerUpdated
    {
        bool hrUpdatedStatus;
        bool spo2UpdatedStatus;
        bool co2UpdatedStatus;
        bool rrUpdatedStatus;
    } rulerUpdated;

    int waveTypeIndex;

    ComboBox *trend1;
    Button * trend2;
    ComboBox *wave;
    ComboBox *hrLow;
    ComboBox *hrHigh;
    ComboBox *rrLow;
    ComboBox *rrHigh;
    ComboBox *spo2Low;
    Button   *spo2High;
    Button   *co2Low;
    ComboBox *co2High;
};

OxyCRGSetupWindow::OxyCRGSetupWindow()
    : Window(),
      d_ptr(new OxyCRGSetupWindowPrivate)
{
    setWindowTitle(trs("OxyCRGWidgetSetup"));

    QGroupBox *groupBox = new QGroupBox(trs("TrendWaveSelect"));
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    int configIndex;
    QHBoxLayout *hlsub;

    int comboIndex = 0;
    int layoutIndex = 0;
    // trend1
    QLabel *label = new QLabel(trs("TRend1"));
    layout->addWidget(label, layoutIndex, 0);
    ComboBox *combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(TREND_HR_PR))
                    << trs(OxyCRGSymbol::convert(TREND_RR))
                    );
    layout->addWidget(combo, layoutIndex, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->trend1 = combo;
    layoutIndex++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Trend1", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);

    // trend2
    label = new QLabel(trs("TRend2"));
    layout->addWidget(label, layoutIndex, 0);
    Button *btn = new Button();
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setText(OxyCRGSymbol::convert(TREND_SPO2));
    btn->setEnabled(false);
    layout->addWidget(btn, layoutIndex, 1);
    d_ptr->trend2 = btn;
    layoutIndex++;

    // wave
    label = new QLabel(trs("Wave"));
    layout->addWidget(label, layoutIndex, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(OxyCRG_Trend_RESP))
                    << trs(OxyCRGSymbol::convert(OxyCRG_Trend_CO2))
                    );
    layout->addWidget(combo, layoutIndex, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->wave = combo;
    layoutIndex++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Wave", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);

    groupBox->setLayout(layout);

    QVBoxLayout *vl = new QVBoxLayout(this);
    QVBoxLayout *vlSub = new QVBoxLayout;
    vl->addWidget(groupBox);

    // ruler adjust
    groupBox = new QGroupBox(trs("RulerAdjust"));
    layout = new QGridLayout;
    QHBoxLayout *hl = new QHBoxLayout;
    int layoutIndexTwo = 0;

    // hrlow
    hlsub = new QHBoxLayout;
    label = new QLabel;
    hlsub->addWidget(label);
    label = new QLabel(trs("HR"));
    hlsub->addWidget(label);
    label = new QLabel;
    hlsub->addWidget(label);
    vlSub->addLayout(hlsub, Qt::AlignBottom);

    label = new QLabel(trs("Low"));
    layout->addWidget(label, layoutIndexTwo, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(HR_LOW_0))
                    << trs(OxyCRGSymbol::convert(HR_LOW_40))
                    << trs(OxyCRGSymbol::convert(HR_LOW_80))
                    << trs(OxyCRGSymbol::convert(HR_LOW_120))
                    );
    layout->addWidget(combo, layoutIndexTwo, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->hrLow = combo;
    layoutIndexTwo++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);

    // hr high
    label = new QLabel(trs("High"));
    layout->addWidget(label, layoutIndexTwo, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(HR_HIGH_160))
                    << trs(OxyCRGSymbol::convert(HR_HIGH_200))
                    << trs(OxyCRGSymbol::convert(HR_HIGH_240))
                    << trs(OxyCRGSymbol::convert(HR_HIGH_280))
                    << trs(OxyCRGSymbol::convert(HR_HIGH_320))
                    << trs(OxyCRGSymbol::convert(HR_HIGH_360))
                    );
    layout->addWidget(combo, layoutIndexTwo, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->hrHigh = combo;
    layoutIndexTwo++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);

    vlSub->addLayout(layout);
    hl->addLayout(vlSub);


    // rrlow
    vlSub = new QVBoxLayout;
    int layoutIndexThree = 0;
    layout = new QGridLayout;
    hlsub = new QHBoxLayout;
    label = new QLabel;
    hlsub->addWidget(label);
    label = new QLabel(trs("RR"));
    hlsub->addWidget(label);
    label = new QLabel;
    hlsub->addWidget(label);
    vlSub->addLayout(hlsub, Qt::AlignBottom);

    label = new QLabel(trs("Low"));
    layout->addWidget(label, layoutIndexThree, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(RR_LOW_0))
                    << trs(OxyCRGSymbol::convert(RR_LOW_40))
                    );
    layout->addWidget(combo, layoutIndexThree, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->rrLow = combo;
    layoutIndexThree++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRLow", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);

    // hr high
    label = new QLabel(trs("High"));
    layout->addWidget(label, layoutIndexThree, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(RR_HIGH_40))
                    << trs(OxyCRGSymbol::convert(RR_HIGH_80))
                    << trs(OxyCRGSymbol::convert(RR_HIGH_120))
                    << trs(OxyCRGSymbol::convert(RR_HIGH_160))
                    );
    layout->addWidget(combo, layoutIndexThree, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->rrHigh = combo;
    layoutIndexThree++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRHigh", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);
    if (configIndex == 0)
    {
        d_ptr->rrLow->setEnabled(false);
        d_ptr->rrLow->setCurrentIndex(0);
    }
    vlSub->addLayout(layout);
    hl->addLayout(vlSub);

    // spo2low
    vlSub = new QVBoxLayout;
    int layoutIndexFour = 0;
    layout = new QGridLayout;
    hlsub = new QHBoxLayout;
    label = new QLabel;
    hlsub->addWidget(label);
    label = new QLabel(trs("SPO2"));
    hlsub->addWidget(label);
    label = new QLabel;
    hlsub->addWidget(label);
    vlSub->addLayout(hlsub, Qt::AlignBottom);


    label = new QLabel(trs("Low"));
    layout->addWidget(label, layoutIndexFour, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(SPO2_LOW_60))
                    << trs(OxyCRGSymbol::convert(SPO2_LOW_68))
                    << trs(OxyCRGSymbol::convert(SPO2_LOW_76))
                    << trs(OxyCRGSymbol::convert(SPO2_LOW_84))
                    << trs(OxyCRGSymbol::convert(SPO2_LOW_92))
                    );
    layout->addWidget(combo, layoutIndexFour, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->spo2Low = combo;
    layoutIndexFour++;
    comboIndex++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|SPO2Low", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);

    // spo2 high
    label = new QLabel(trs("High"));
    layout->addWidget(label, layoutIndexFour, 0);
    btn = new Button();
    btn->setText(trs(OxyCRGSymbol::convert(SPO2_HIGH_100)));
    btn->setButtonStyle(Button::ButtonTextOnly);
    btn->setEnabled(false);
    layout->addWidget(btn, layoutIndexFour, 1);
    d_ptr->spo2High = btn;
    layoutIndexFour++;
    vlSub->addLayout(layout);
    hl->addLayout(vlSub);

    // co2low
    vlSub = new QVBoxLayout;
    int layoutIndexFive = 0;
    layout = new QGridLayout;

    hlsub = new QHBoxLayout;
    label = new QLabel;
    hlsub->addWidget(label);
    label = new QLabel(trs("CO2"));
    hlsub->addWidget(label);
    label = new QLabel;
    hlsub->addWidget(label);
    vlSub->addLayout(hlsub, Qt::AlignBottom);

    label = new QLabel(trs("Low"));
    layout->addWidget(label, layoutIndexFive, 0);
    btn = new Button();
    btn->setText(trs(OxyCRGSymbol::convert(CO2_LOW_0)));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, layoutIndexFive, 1);
    btn->setEnabled(false);
    d_ptr->co2Low = btn;
    layoutIndexFive++;

    // co2 high
    label = new QLabel(trs("High"));
    layout->addWidget(label, layoutIndexFive, 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_15))
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_20))
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_25))
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_40))
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_50))
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_60))
                    << trs(OxyCRGSymbol::convert(CO2_HIGH_80))
                    );
    layout->addWidget(combo, layoutIndexFive, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
    d_ptr->co2High = combo;
    layoutIndexFive++;
    configIndex = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|CO2High", configIndex);
    combo->blockSignals(true);
    combo->setCurrentIndex(configIndex);
    combo->blockSignals(false);
    vlSub->addLayout(layout);
    hl->addLayout(vlSub);

    groupBox->setLayout(hl);
    vl->addWidget(groupBox);

    setWindowLayout(vl);
}

OxyCRGSetupWindow::~OxyCRGSetupWindow()
{
    delete d_ptr;
}

void OxyCRGSetupWindow::setWaveTypeIndex(int index)
{
    if (index >= 0 && index < 2)
    {
        d_ptr->waveTypeIndex = index;
    }
}

int OxyCRGSetupWindow::getWaveTypeIndex() const
{
    return d_ptr->waveTypeIndex;
}

int OxyCRGSetupWindow::getHRLow(bool &status) const
{
    status = d_ptr->rulerUpdated.hrUpdatedStatus;
    return d_ptr->rulerValue.s.hrLow;
}

int OxyCRGSetupWindow::getHRHigh(bool &status) const
{
    status = d_ptr->rulerUpdated.hrUpdatedStatus;
    return d_ptr->rulerValue.s.hrHigh;
}

int OxyCRGSetupWindow::getSPO2Low(bool &status) const
{
    status = d_ptr->rulerUpdated.spo2UpdatedStatus;
    return d_ptr->rulerValue.s.spo2Low;
}

int OxyCRGSetupWindow::getSPO2High(bool &status) const
{
    status = d_ptr->rulerUpdated.spo2UpdatedStatus;
    return d_ptr->rulerValue.s.spo2High;
}

int OxyCRGSetupWindow::getCO2Low(bool &status) const
{
    status = d_ptr->rulerUpdated.co2UpdatedStatus;
    return d_ptr->rulerValue.s.co2Low;
}

int OxyCRGSetupWindow::getCO2High(bool &status) const
{
    status = d_ptr->rulerUpdated.co2UpdatedStatus;
    return d_ptr->rulerValue.s.co2High;
}

int OxyCRGSetupWindow::getRRLow(bool &status) const
{
    status = d_ptr->rulerUpdated.rrUpdatedStatus;
    return d_ptr->rulerValue.s.rrLow;
}

int OxyCRGSetupWindow::getRRHigh(bool &status) const
{
    status = d_ptr->rulerUpdated.rrUpdatedStatus;
    return d_ptr->rulerValue.s.rrHigh;
}

void OxyCRGSetupWindow::showEvent(QShowEvent *ev)
{
    Window::showEvent(ev);
    d_ptr->hrLow->setMinimumWidth(100);
    d_ptr->rrLow->setMinimumWidth(100);
    d_ptr->spo2Low->setMinimumWidth(100);
    d_ptr->co2Low->setMinimumWidth(100);
}

void OxyCRGSetupWindow::onComboUpdated(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    int indexType = combo->property("Item").toInt();

    switch (indexType)
    {
    case OxyCRGSetupWindowPrivate::ITEM_CBO_TREND_ONE:
        currentConfig.setNumValue("OxyCRG|Trend1", index);
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_WAVE_SELECT:
        setWaveTypeIndex(index);
        currentConfig.setNumValue("OxyCRG|Wave", index);
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_HR_LOW:
        currentConfig.setNumValue("OxyCRG|Ruler|HRLow", index);
        d_ptr->rulerValue.s.hrLow =
            combo->currentText().toInt();
        d_ptr->rulerUpdated.hrUpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_HR_HIGH:
        currentConfig.setNumValue("OxyCRG|Ruler|HRHigh", index);
        d_ptr->rulerValue.s.hrHigh =
            combo->currentText().toInt();
        d_ptr->rulerUpdated.hrUpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_SPO2_LOW:
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2Low", index);
        d_ptr->rulerValue.s.spo2Low = combo->currentText().toInt();
        d_ptr->rulerUpdated.spo2UpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_CO2_HIGH:
        currentConfig.setNumValue("OxyCRG|Ruler|CO2High", index);
        d_ptr->rulerValue.s.co2High = combo->currentText().toInt();
        d_ptr->rulerUpdated.co2UpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_RR_LOW:
        currentConfig.setNumValue("OxyCRG|Ruler|RRLow", index);
        d_ptr->rulerValue.s.rrLow =
            combo->currentText().toInt();
        d_ptr->rulerUpdated.rrUpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_RR_HIGH:
        currentConfig.setNumValue("OxyCRG|Ruler|RRHigh", index);
        d_ptr->rulerValue.s.rrHigh =
            combo->currentText().toInt();
        d_ptr->rulerUpdated.rrUpdatedStatus = true;
        d_ptr->rrLow->setEnabled(true);
        if (index != 0)
        {
            break;
        }
        d_ptr->rrLow->setEnabled(false);
        d_ptr->rrLow->blockSignals(true);
        d_ptr->rrLow->setCurrentIndex(0);
        d_ptr->rrLow->blockSignals(false);
        currentConfig.setNumValue("OxyCRG|Ruler|RRLow", 0);
        break;
    };
}


