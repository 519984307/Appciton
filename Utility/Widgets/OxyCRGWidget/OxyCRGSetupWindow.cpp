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
#include <QLabel>
#include "LanguageManager.h"
#include <QGridLayout>
#include "OxyCRGSymbol.h"
#include "ConfigManager.h"
#include <QGroupBox>
#include "Button.h"

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
        : waveTypeIndex(0),
          trend1(NULL),
          trend2(NULL),
          wave(NULL),
          hrLow(NULL),
          hrHigh(NULL),
          rrLow(NULL),
          rrHigh(NULL),
          spo2Low(NULL),
          spo2High(NULL),
          co2Low(NULL),
          co2High(NULL)
    {
    }

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
    : Dialog(),
      d_ptr(new OxyCRGSetupWindowPrivate)
{
    setWindowTitle(trs("OxyCRGWidgetSetup"));

    QGroupBox *groupBox = new QGroupBox(trs("TrendWaveSelect"));
    QGridLayout *layout = new QGridLayout();
    layout->setMargin(10);
    QHBoxLayout *hlsub;

    int comboIndex = 0;
    int layoutIndex = 0;
    // trend1
    QLabel *label = new QLabel(trs("Trend1"));
    layout->addWidget(label, layoutIndex, 0);
    ComboBox *combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs(OxyCRGSymbol::convert(TREND_HR_PR))
                    << trs(OxyCRGSymbol::convert(TREND_RR))
                    );
    layout->addWidget(combo, layoutIndex, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    d_ptr->trend1 = combo;
    layoutIndex++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));

    // trend2
    label = new QLabel(trs("Trend2"));
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
                    << trs(OxyCRGSymbol::convert(OXYCRG_WAVE_RESP))
                    << trs(OxyCRGSymbol::convert(OXYCRG_WAVE_CO2))
                    );
    layout->addWidget(combo, layoutIndex, 1);
    combo->setProperty("Item", qVariantFromValue(comboIndex));
    d_ptr->wave = combo;
    layoutIndex++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));

    groupBox->setLayout(layout);

    QVBoxLayout *vl = new QVBoxLayout();
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
    combo->setMinimumWidth(100);
    d_ptr->hrLow = combo;
    layoutIndexTwo++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));

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
    d_ptr->hrHigh = combo;
    layoutIndexTwo++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));

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
    combo->setMinimumWidth(100);
    d_ptr->rrLow = combo;
    layoutIndexThree++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));

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
    d_ptr->rrHigh = combo;
    layoutIndexThree++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
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
    combo->setMinimumWidth(100);
    d_ptr->spo2Low = combo;
    layoutIndexFour++;
    comboIndex++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));

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
    btn->setMinimumWidth(100);
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
    d_ptr->co2High = combo;
    layoutIndexFive++;
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboUpdated(int)));
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

int OxyCRGSetupWindow::getWaveTypeIndex() const
{
    return d_ptr->waveTypeIndex;
}

int OxyCRGSetupWindow::getHRLow() const
{
    return d_ptr->hrLow->currentText().toInt();
}

int OxyCRGSetupWindow::getHRHigh() const
{
    return d_ptr->hrHigh->currentText().toInt();
}

int OxyCRGSetupWindow::getSPO2Low() const
{
    return d_ptr->spo2Low->currentText().toInt();
}

int OxyCRGSetupWindow::getSPO2High() const
{
    return d_ptr->spo2High->text().toInt();
}

int OxyCRGSetupWindow::getCO2Low() const
{
    return d_ptr->co2Low->text().toInt();
}

int OxyCRGSetupWindow::getCO2High() const
{
    return d_ptr->co2High->currentText().toInt();
}

int OxyCRGSetupWindow::getRRLow() const
{
    return d_ptr->rrLow->currentText().toInt();
}

int OxyCRGSetupWindow::getRRHigh() const
{
    return d_ptr->rrHigh->currentText().toInt();
}

void OxyCRGSetupWindow::showEvent(QShowEvent *ev)
{
    Dialog::showEvent(ev);
    int index = 0;
    currentConfig.getNumValue("OxyCRG|Trend1", index);
    d_ptr->trend1->setCurrentIndex(index);

    index = 0;
    currentConfig.getNumValue("OxyCRG|Wave", index);
    d_ptr->wave->setCurrentIndex(index);
    d_ptr->waveTypeIndex = index;

    index = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRLow", index);
    d_ptr->hrLow->setCurrentIndex(index);

    index = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|HRHigh", index);
    d_ptr->hrHigh->setCurrentIndex(index);

    index = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRLow", index);
    d_ptr->rrLow->setCurrentIndex(index);

    index = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|RRHigh", index);
    d_ptr->rrHigh->setCurrentIndex(index);
    if (index == 0)
    {
        d_ptr->rrLow->setEnabled(false);
        d_ptr->rrLow->setCurrentIndex(0);
    }

    index = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|SPO2Low", index);
    d_ptr->spo2Low->setCurrentIndex(index);

    index = 0;
    currentConfig.getNumValue("OxyCRG|Ruler|CO2High", index);
    d_ptr->co2High->setCurrentIndex(index);
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
        d_ptr->waveTypeIndex = index;
        currentConfig.setNumValue("OxyCRG|Wave", index);
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_HR_LOW:
    {
        currentConfig.setNumValue("OxyCRG|Ruler|HRLow", index);
     }
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_HR_HIGH:
    {
        currentConfig.setNumValue("OxyCRG|Ruler|HRHigh", index);
     }
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_SPO2_LOW:
    {
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2Low", index);
     }
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_CO2_HIGH:
     {
        currentConfig.setNumValue("OxyCRG|Ruler|CO2High", index);
      }
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_RR_LOW:
    {
        currentConfig.setNumValue("OxyCRG|Ruler|RRLow", index);
    }
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_RR_HIGH:
    {
        currentConfig.setNumValue("OxyCRG|Ruler|RRHigh", index);
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
     }
        break;
    };
}


