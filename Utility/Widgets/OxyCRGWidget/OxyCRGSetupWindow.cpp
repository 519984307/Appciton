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

class OxyCRGSetupWindowPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_TREND_ONE = 0,
        ITEM_CBO_TREND_TWO,
        ITEM_CBO_WAVE_SELECT,
        ITEM_CBO_HR_LOW,
        ITEM_CBO_HR_HIGH,
        ITEM_CBO_SPO2_LOW,
        ITEM_CBO_SPO2_HIGH,
        ITEM_CBO_CO2_LOW,
        ITEM_CBO_CO2_HIGH,
        ITEM_CBO_RR_LOW,
        ITEM_CBO_RR_HIGH,
        ITEM_CBO_MAX,
    };


    OxyCRGSetupWindowPrivate()
        : waveTypeIndex(0)
    {
        for (int i = 0; i < 8; i++)
        {
            rulerValue.data[i] = 0;
        }
        rulerUpdated.hrUpdatedStatus = false;
        rulerUpdated.spo2UpdatedStatus = false;
        rulerUpdated.co2UpdatedStatus = false;
        rulerUpdated.rrUpdatedStatus = false;
        combos.clear();
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
    QMap<int, ComboBox *> combos;
};

OxyCRGSetupWindow::OxyCRGSetupWindow()
    : Window(),
      d_ptr(new OxyCRGSetupWindowPrivate)
{
    setWindowTitle(trs("OxyCRGWidgetSetup"));

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QStringList nameList(QStringList()
                         << "TRend1"
                         << "Trend2"
                         << "Wave"
                         << "HRLow"
                         << "HRHigh"
                         << "SPO2Low"
                         << "SPO2High"
                         << "CO2Low"
                         << "CO2High"
                         << "RRLow"
                         << "RRHigh"
                        );

    QLabel *label;
    ComboBox *combo;
    int itemId = OxyCRGSetupWindowPrivate::ITEM_CBO_MAX;
    QStringList comboItemList[itemId];

    for (int i = 0; i < itemId / 2; i++)
    {
        label = new QLabel(trs(nameList.at(i)));
        layout->addWidget(label, i, 0);

        combo = new ComboBox;
        layout->addWidget(combo, i, 1);
        connect(combo, SIGNAL(currentIndexChanged(int)), this,
                SLOT(onComboUpdated(int)));

        combo->setProperty("Item", qVariantFromValue(i));
        d_ptr->combos[i] = combo;
    }
    for (int i = itemId / 2; i < itemId; i++)
    {
        label = new QLabel(trs(nameList.at(i)));

        layout->addWidget(label, i - itemId / 2, 3);

        combo = new ComboBox;
        layout->addWidget(combo, i - itemId / 2, 4);
        connect(combo, SIGNAL(currentIndexChanged(int)), this,
                SLOT(onComboUpdated(int)));

        combo->setProperty("Item", qVariantFromValue(i));
        d_ptr->combos[i] = combo;
    }
    layout->setColumnMinimumWidth(2, 100);
    setWindowLayout(layout);
    layout->setRowStretch(itemId, 1);

    for (int i = 0; i < TRENDONE_NR; i++)
    {
        comboItemList[0].append(
            trs(OxyCRGSymbol::convert(TrendTypeOne(i))));
    }

    for (int i = 0; i < TRENDTWO_NR; i++)
    {
        comboItemList[1].append(
            trs(OxyCRGSymbol::convert(TrendTypeTwo(i))));
    }

    for (int i = 0; i < OxyCRG_Trend_NR; i++)
    {
        comboItemList[2].append(
            trs(OxyCRGSymbol::convert(OxyCRGTrend(i))));
    }
    for (int i = 0; i < HR_LOW_NR; i++)
    {
        comboItemList[3].append(
            trs(OxyCRGSymbol::convert(HRLowTypes(i))));
    }

    for (int i = 0; i < HR_HIGH_NR; i++)
    {
        comboItemList[4].append(
            trs(OxyCRGSymbol::convert(HRHighTypes(i))));
    }

    for (int i = 0; i < SPO2_LOW_NR; i++)
    {
        comboItemList[5].append(
            trs(OxyCRGSymbol::convert(SPO2LowTypes(i))));
    }

    for (int i = 0; i < SPO2_HIGH_NR; i++)
    {
        comboItemList[6].append(
            trs(OxyCRGSymbol::convert(SPO2HighTypes(i))));
    }

    for (int i = 0; i < CO2_LOW_NR; i++)
    {
        comboItemList[7].append(
            trs(OxyCRGSymbol::convert(CO2LowTypes(i))));
    }

    for (int i = 0; i < CO2_HIGH_NR; i++)
    {
        comboItemList[8].append(
            trs(OxyCRGSymbol::convert(CO2HighTypes(i))));
    }

    for (int i = 0; i < RR_LOW_NR; i++)
    {
        comboItemList[9].append(
            trs(OxyCRGSymbol::convert(RRLowTypes(i))));
    }

    for (int i = 0; i < RR_HIGH_NR; i++)
    {
        comboItemList[10].append(
            trs(OxyCRGSymbol::convert(RRHighTypes(i))));
    }

    for (int i = 0; i < itemId; i++)
    {
        d_ptr->combos[i]->blockSignals(true);
        d_ptr->combos[i]->addItems(comboItemList[i]);
        int indexSelect = 0;
        QString str;
        if (i < 3)
        {
            str = "OxyCRG|%1";
        }
        else
        {
            str = "OxyCRG|Ruler|%1";
        }
        currentConfig.getNumValue(QString(str)
                                  .arg(nameList.at(i)), indexSelect);

        d_ptr->combos[i]->setCurrentIndex(indexSelect);
        d_ptr->combos[i]->blockSignals(false);

        if (i == OxyCRGSetupWindowPrivate::ITEM_CBO_RR_HIGH)
        {
            if (indexSelect == 0)
            {
                d_ptr->combos[i-1]->setEnabled(false);
                d_ptr->combos[i-1]->setCurrentIndex(0);
            }
        }
    }
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
    d_ptr->combos[5]->setFixedWidth(d_ptr->combos[0]->width());
}

void OxyCRGSetupWindow::onComboUpdated(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    int indexType = combo->property("Item").toInt();
    int trendIndex = 0;

    switch (indexType)
    {
    case OxyCRGSetupWindowPrivate::ITEM_CBO_TREND_ONE:
        currentConfig.setNumValue("OxyCRG|Trend1", index);
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_TREND_TWO:
        currentConfig.setNumValue("OxyCRG|Trend2", index);
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_WAVE_SELECT:
        setWaveTypeIndex(index);
        currentConfig.setNumValue("OxyCRG|Wave", index);
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_HR_LOW:
        currentConfig.setNumValue("OxyCRG|Ruler|HRLow", trendIndex);
        d_ptr->rulerValue.s.hrLow =
            combo->currentText().toInt();
        d_ptr->rulerUpdated.hrUpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_HR_HIGH:
        currentConfig.setNumValue("OxyCRG|Ruler|HRHigh", trendIndex);
        d_ptr->rulerValue.s.hrHigh =
            combo->currentText().toInt();
        d_ptr->rulerUpdated.hrUpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_SPO2_LOW:
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2Low", index);
        d_ptr->rulerValue.s.spo2Low = combo->currentText().toInt();
        d_ptr->rulerUpdated.spo2UpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_SPO2_HIGH:
        currentConfig.setNumValue("OxyCRG|Ruler|SPO2High", index);
        d_ptr->rulerValue.s.spo2High = combo->currentText().toInt();
        d_ptr->rulerUpdated.spo2UpdatedStatus = true;
        break;
    case OxyCRGSetupWindowPrivate::ITEM_CBO_CO2_LOW:
        currentConfig.setNumValue("OxyCRG|Ruler|CO2Low", index);
        d_ptr->rulerValue.s.co2Low = combo->currentText().toInt();
        d_ptr->rulerUpdated.co2UpdatedStatus = true;
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
        d_ptr->combos[OxyCRGSetupWindowPrivate::
                ITEM_CBO_RR_LOW]->setEnabled(true);
        if (index != 0)
        {
            break;
        }
        d_ptr->combos[OxyCRGSetupWindowPrivate::
                ITEM_CBO_RR_LOW]->setEnabled(false);
        d_ptr->combos[OxyCRGSetupWindowPrivate::
                ITEM_CBO_RR_LOW]->blockSignals(true);
        d_ptr->combos[OxyCRGSetupWindowPrivate::
                ITEM_CBO_RR_LOW]->setCurrentIndex(0);
        d_ptr->combos[OxyCRGSetupWindowPrivate::
                ITEM_CBO_RR_LOW]->blockSignals(false);
        currentConfig.setNumValue("OxyCRG|Ruler|RRLow", 0);
        break;
    };
}


