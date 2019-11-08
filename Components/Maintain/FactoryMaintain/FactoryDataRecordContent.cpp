/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/20
 **/
#include "FactoryDataRecordContent.h"
#include <QGridLayout>
#include <QMap>
#include <QLabel>
#include "IConfig.h"
#include "ComboBox.h"
#include "SystemManager.h"
#include "RawDataCollector.h"
#include "Framework/Language/LanguageManager.h"
#include "ECGParam.h"
#include "NIBPParam.h"
#include "SPO2Param.h"

class FactoryDataRecordContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ECG = 0,
        ITEM_CBO_SPO2,
        ITEM_CBO_NIBP,
        ITEM_CBO_CO2,
        ITEM_CBO_TEMP
    };

    FactoryDataRecordContentPrivate();

    void loadOptions();
    QMap <MenuItem, ComboBox *> combos;
    QMap <MenuItem, QLabel *> labs;
};

FactoryDataRecordContentPrivate::FactoryDataRecordContentPrivate()
{
    combos.clear();
}

void FactoryDataRecordContentPrivate::loadOptions()
{
    int value = 0;
    QString str;
    machineConfig.getStrValue("SPO2", str);
    if (systemManager.isSupport(CONFIG_SPO2) && str == "BLM_S5")
    {
        combos[ITEM_CBO_SPO2]->show();
        labs[ITEM_CBO_SPO2]->show();
    }
    else
    {
        combos[ITEM_CBO_SPO2]->hide();
        labs[ITEM_CBO_SPO2]->hide();
    }

    machineConfig.getStrValue("NIBP", str);
    if (systemManager.isSupport(CONFIG_NIBP) && str == "BLM_N5")
    {
        combos[ITEM_CBO_NIBP]->show();
        labs[ITEM_CBO_NIBP]->show();
    }
    else
    {
        combos[ITEM_CBO_NIBP]->hide();
        labs[ITEM_CBO_NIBP]->hide();
    }

    machineConfig.getStrValue("CO2", str);
    if (systemManager.isSupport(CONFIG_CO2) && str == "BLM_CO2")
    {
        value = 0;
        machineConfig.getNumValue("Record|CO2", value);
        combos[ITEM_CBO_CO2]->setCurrentIndex(value);
        combos[ITEM_CBO_CO2]->show();
        labs[ITEM_CBO_CO2]->show();
    }
    else
    {
        combos[ITEM_CBO_CO2]->hide();
        labs[ITEM_CBO_CO2]->hide();
    }

    machineConfig.getStrValue("TEMP", str);
    if (systemManager.isSupport(CONFIG_TEMP) && str == "BLM_T5")
    {
        value = 0;
        machineConfig.getNumValue("Record|TEMP", value);
        combos[ITEM_CBO_TEMP]->setCurrentIndex(value);
        combos[ITEM_CBO_TEMP]->show();
        labs[ITEM_CBO_TEMP]->show();
    }
    else
    {
        combos[ITEM_CBO_TEMP]->hide();
        labs[ITEM_CBO_TEMP]->hide();
    }
}

FactoryDataRecordContent::FactoryDataRecordContent()
    : MenuContent(trs("FactoryDataRecord"),
                  trs("FactoryDataRecordDesc")),
      d_ptr(new FactoryDataRecordContentPrivate)
{
}

FactoryDataRecordContent::~FactoryDataRecordContent()
{
    delete d_ptr;
}

void FactoryDataRecordContent::readyShow()
{
    d_ptr->loadOptions();
}

void FactoryDataRecordContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);
    this->setFocusPolicy(Qt::NoFocus);

    QLabel *label;
    ComboBox *combo;
    int itemId;

    label = new QLabel(trs("ECG"));
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_ECG, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_ECG, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_ECG;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("SPO2"));
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_SPO2, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_SPO2, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_SPO2;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("NIBP"));
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_NIBP, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_NIBP, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_NIBP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel("CO2");
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_CO2, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_CO2, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_CO2;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel("TEMP");
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_TEMP, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_TEMP, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_TEMP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void FactoryDataRecordContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;

    switch (indexType)
    {
    case FactoryDataRecordContentPrivate::ITEM_CBO_ECG:
        str = "ECG";
        ecgParam.setRawDataOnOff(index);
        rawDataCollector.setCollectStatus(RawDataCollector::ECG_DATA, index);
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_SPO2:
        str = "SPO2";
        spo2Param.enableRawDataSend(index);
        rawDataCollector.setCollectStatus(RawDataCollector::SPO2_DATA, index);
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_NIBP:
        str = "NIBP";
        nibpParam.enableRawDataSend(index);
        rawDataCollector.setCollectStatus(RawDataCollector::NIBP_DATA, index);
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_CO2:
        str = "CO2";
        rawDataCollector.setCollectStatus(RawDataCollector::CO2_DATA, index);
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_TEMP:
        str = "TEMP";
        rawDataCollector.setCollectStatus(RawDataCollector::TEMP_DATA, index);
        break;
    }
    machineConfig.setNumValue(QString("Record|%1").arg(str), index);
    machineConfig.saveToDisk();
}
