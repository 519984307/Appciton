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

class FactoryDataRecordContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ECG = 0,
        ITEM_CBO_SPO2,
        ITEM_CBO_NIBP,
        ITEM_CBO_TEMP,
        ITEM_CBO_BATINFO,
        ITEM_CBO_PDCOMM,
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
    machineConfig.getNumValue("Record|ECG", value);
    combos[ITEM_CBO_ECG]->setCurrentIndex(value);

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        QString str;
        machineConfig.getStrValue("SPO2", str);
        if (str == "BLM_TS3")
        {
            machineConfig.getNumValue("Record|SPO2", value);
            combos[ITEM_CBO_SPO2]->setCurrentIndex(value);
            combos[ITEM_CBO_SPO2]->show();
            labs[ITEM_CBO_SPO2]->show();
        }
        else
        {
            combos[ITEM_CBO_SPO2]->hide();
            labs[ITEM_CBO_SPO2]->hide();
        }
    }
    else
    {
        combos[ITEM_CBO_SPO2]->hide();
        labs[ITEM_CBO_SPO2]->hide();
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        machineConfig.getNumValue("Record|NIBP", value);
        combos[ITEM_CBO_NIBP]->setCurrentIndex(value);
        combos[ITEM_CBO_NIBP]->show();
        labs[ITEM_CBO_NIBP]->show();
    }
    else
    {
        combos[ITEM_CBO_NIBP]->hide();
        labs[ITEM_CBO_NIBP]->hide();
    }

    if (systemManager.isSupport(CONFIG_TEMP))
    {
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

    machineConfig.getNumValue("Record|Battery", value);
    combos[ITEM_CBO_BATINFO]->setCurrentIndex(value);

    machineConfig.getNumValue("Record|PDCommLog", value);
    combos[ITEM_CBO_PDCOMM]->setCurrentIndex(value);
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
                    << trs("On")
                   );
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
                    << trs("On")
                   );
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
                    << trs("On")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_NIBP, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_NIBP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("TEMP"));
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_TEMP, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_TEMP, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_TEMP;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("BatteryInfo"));
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_BATINFO, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_BATINFO, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_BATINFO;
    combo->setProperty("Item", qVariantFromValue(itemId));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    label = new QLabel(trs("PDCommData"));
    d_ptr->labs.insert(FactoryDataRecordContentPrivate
                       ::ITEM_CBO_PDCOMM, label);
    layout->addWidget(label, d_ptr->combos.count(), 0);
    combo = new ComboBox;
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On")
                   );
    layout->addWidget(combo, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(FactoryDataRecordContentPrivate
                         ::ITEM_CBO_PDCOMM, combo);
    itemId = FactoryDataRecordContentPrivate::ITEM_CBO_PDCOMM;
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
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_SPO2:
        str = "SPO2";
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_BATINFO:
        str = "Battery";
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_NIBP:
        str = "NIBP";
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_TEMP:
        str = "TEMP";
        break;
    case FactoryDataRecordContentPrivate::ITEM_CBO_PDCOMM:
        str = "PDCommLog";
        break;
    }
    machineConfig.setNumValue(QString("Record|%1").arg(str), index);
    machineConfig.saveToDisk();
}
