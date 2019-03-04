/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#include "Supervisor12LMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "ConfigManager.h"
#include "ECGParam.h"
#include "PrintDefine.h"

class Supervisor12LMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_NOTCH_FILTER,
        ITEM_CBO_12L_FREQ,
        ITEM_CBO_PRINT_FORMAT,
        ITEM_CBO_TIME_WINDOW_2X6PDF,
        ITEM_CBO_AUTO_PRINT_12L,
        ITEM_CBO_PRINT_TOP_LEAD,
        ITEM_CBO_PRINT_MID_LEAD,
        ITEM_CBO_PRINT_BOT_LEAD
    };

    Supervisor12LMenuContentPrivate() {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
};

void Supervisor12LMenuContentPrivate::loadOptions()
{
    int index = 0;
    currentConfig.getNumValue("ECG12L|NotchFilter", index);
    combos[ITEM_CBO_NOTCH_FILTER]->setCurrentIndex(index);

    currentConfig.getNumValue("ECG12L|ECG12LeadBandwidth", index);
    combos[ITEM_CBO_12L_FREQ]->setCurrentIndex(index);

    currentConfig.getNumValue("ECG12L|PrintSnapshotFormat", index);
    combos[ITEM_CBO_PRINT_FORMAT]->setCurrentIndex(index);

    currentConfig.getNumValue("ECG12L|TimeIntervalFor2x6Report", index);
    combos[ITEM_CBO_PRINT_FORMAT]->setCurrentIndex(index);

    currentConfig.getNumValue("ECG12L|AutoPrinting12LReport", index);
    combos[ITEM_CBO_AUTO_PRINT_12L]->setCurrentIndex(index);

    int leadNameConvention = 0, displayMode = 0;
    currentConfig.getNumValue("ECG|ECGLeadConvention", leadNameConvention);
    currentConfig.getNumValue("ECG12L|DisplayFormat", displayMode);
    combos[ITEM_CBO_PRINT_TOP_LEAD]->blockSignals(true);
    combos[ITEM_CBO_PRINT_MID_LEAD]->blockSignals(true);
    combos[ITEM_CBO_PRINT_BOT_LEAD]->blockSignals(true);
    combos[ITEM_CBO_PRINT_TOP_LEAD]->clear();
    combos[ITEM_CBO_PRINT_MID_LEAD]->clear();
    combos[ITEM_CBO_PRINT_BOT_LEAD]->clear();
    for (int i = ECG_LEAD_I; i <= ECG_LEAD_V6; ++i)
    {
        combos[ITEM_CBO_PRINT_TOP_LEAD]->addItem(ECGSymbol::convert((ECGLead)i,
                (ECGLeadNameConvention)leadNameConvention, true, displayMode));

        combos[ITEM_CBO_PRINT_MID_LEAD]->addItem(ECGSymbol::convert((ECGLead)i,
                (ECGLeadNameConvention)leadNameConvention, true, displayMode));

        combos[ITEM_CBO_PRINT_BOT_LEAD]->addItem(ECGSymbol::convert((ECGLead)i,
                (ECGLeadNameConvention)leadNameConvention, true, displayMode));
    }
    combos[ITEM_CBO_PRINT_TOP_LEAD]->blockSignals(false);
    combos[ITEM_CBO_PRINT_MID_LEAD]->blockSignals(false);
    combos[ITEM_CBO_PRINT_BOT_LEAD]->blockSignals(false);

    currentConfig.getNumValue("ECG12L|RealtimePrintTopLead", index);
    combos[ITEM_CBO_PRINT_TOP_LEAD]->setCurrentIndex(index);

    currentConfig.getNumValue("ECG12L|RealtimePrintMiddleLead", index);
    combos[ITEM_CBO_PRINT_MID_LEAD]->setCurrentIndex(index);

    currentConfig.getNumValue("ECG12L|RealtimePrintBottomLead", index);
    combos[ITEM_CBO_PRINT_BOT_LEAD]->setCurrentIndex(index);
}

Supervisor12LMenuContent::Supervisor12LMenuContent()
    : MenuContent(trs("Supervisor12LMenu"), trs("Supervisor12LMenuDesc")),
      d_ptr(new Supervisor12LMenuContentPrivate)
{
}

Supervisor12LMenuContent::~Supervisor12LMenuContent()
{
    delete d_ptr;
}

void Supervisor12LMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void Supervisor12LMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // 工频
    label = new QLabel(trs("SupervisorNotchFilter"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_NOTCH_OFF))
                       << ECGSymbol::convert(ECG_NOTCH_50HZ)
                       << ECGSymbol::convert(ECG_NOTCH_60HZ)
                      );
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_NOTCH_FILTER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_NOTCH_FILTER, comboBox);

    // 12L带宽
    label = new QLabel(trs("Supervisor12lFrequency"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << ECGSymbol::convert(ECG_BANDWIDTH_0525_40HZ)
                       << ECGSymbol::convert(ECG_BANDWIDTH_0525_150HZ)
                      );
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_12L_FREQ);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_12L_FREQ, comboBox);

    // print snapshot format
    label = new QLabel(trs("Print12LSnapshotFormat"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << PrintSymbol::convert(PRINT_12LEAD_SNAPSHOT_STAND)
                       << PrintSymbol::convert(PRINT_12LEAD_SNAPSHOT_CABRELA)
                      );
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_FORMAT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_FORMAT, comboBox);

    // time window 2x6pdf
    label = new QLabel(trs("SupervisorTimeWindow2x6PDF"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << "0-5s"
                       << "2.5-7.5s"
                       << "5-10s"
                      );
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_TIME_WINDOW_2X6PDF);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_TIME_WINDOW_2X6PDF, comboBox);

    // auto print
    label = new QLabel(trs("SupervisorAutoPrint12L"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_AUTO_PRINT_12L);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_AUTO_PRINT_12L, comboBox);

    // top lead
    label = new QLabel(trs("RealTimePrintTopLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_TOP_LEAD);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_TOP_LEAD, comboBox);

    // middle lead
    label = new QLabel(trs("RealTimePrintMidLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_MID_LEAD);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_MID_LEAD, comboBox);

    // bottom lead
    label = new QLabel(trs("RealTimePrintBottomLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_BOT_LEAD);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_BOT_LEAD, comboBox);

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void Supervisor12LMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        Supervisor12LMenuContentPrivate::MenuItem item
            = (Supervisor12LMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case Supervisor12LMenuContentPrivate::ITEM_CBO_NOTCH_FILTER:
            currentConfig.setNumValue("ECG12L|NotchFilter", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_12L_FREQ:
            currentConfig.setNumValue("ECG12L|ECG12LeadBandwidth", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_FORMAT:
            currentConfig.setNumValue("ECG12L|PrintSnapshotFormat", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_TIME_WINDOW_2X6PDF:
            currentConfig.setNumValue("ECG12L|TimeIntervalFor2x6Report", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_AUTO_PRINT_12L:
            currentConfig.setNumValue("ECG12L|AutoPrinting12LReport", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_TOP_LEAD:
            currentConfig.setNumValue("ECG12L|RealtimePrintTopLead", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_MID_LEAD:
            currentConfig.setNumValue("ECG12L|RealtimePrintMiddleLead", index);
            break;
        case Supervisor12LMenuContentPrivate::ITEM_CBO_PRINT_BOT_LEAD:
            currentConfig.setNumValue("ECG12L|RealtimePrintBottomLead", index);
            break;
        default:
            break;
        }
    }
}
