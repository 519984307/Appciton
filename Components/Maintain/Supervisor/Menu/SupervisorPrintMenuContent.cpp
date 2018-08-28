/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#include "SupervisorPrintMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"

class SupervisorPrintMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PRINT_ECG_RHYTHM,
        ITEM_CBO_PRINT_PATIENT_ALARM,
        ITEM_CBO_PRINT_PHY_ALARM,
        ITEM_CBO_PRINT_CODEMARKER,
        ITEM_CBO_PRINT_NIBP
    };

    SupervisorPrintMenuContentPrivate() {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
};

void SupervisorPrintMenuContentPrivate::loadOptions()
{
    int index = 0;
    currentConfig.getNumValue("Print|PresentingECG", index);
    combos[ITEM_CBO_PRINT_ECG_RHYTHM]->setCurrentIndex(index);

    currentConfig.getNumValue("Print|CheckPatient", index);
    combos[ITEM_CBO_PRINT_PATIENT_ALARM]->setCurrentIndex(index);

    currentConfig.getNumValue("Print|PhysiologicalAlarm", index);
    combos[ITEM_CBO_PRINT_PHY_ALARM]->setCurrentIndex(index);

    currentConfig.getNumValue("Print|CoderMarker", index);
    combos[ITEM_CBO_PRINT_CODEMARKER]->setCurrentIndex(index);

    currentConfig.getNumValue("Print|NIBPReading", index);
    combos[ITEM_CBO_PRINT_NIBP]->setCurrentIndex(index);
}

SupervisorPrintMenuContent::SupervisorPrintMenuContent()
    : MenuContent(trs("SupervisorPrintMenu"), trs("SupervisorPrintMenuDesc")),
      d_ptr(new SupervisorPrintMenuContentPrivate)
{
}

SupervisorPrintMenuContent::~SupervisorPrintMenuContent()
{
    delete d_ptr;
}

void SupervisorPrintMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void SupervisorPrintMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // ecg rhythm
    label = new QLabel(trs("ECGRhythm"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_ECG_RHYTHM);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_ECG_RHYTHM, comboBox);

    // check patient
    label = new QLabel(trs("CheckPatient"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_PATIENT_ALARM);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_PATIENT_ALARM, comboBox);

    // phy alarm
    label = new QLabel(trs("PhyAlarm"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_PHY_ALARM);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_PHY_ALARM, comboBox);

    // code marker
    label = new QLabel(trs("CodeMarker"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_CODEMARKER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_CODEMARKER, comboBox);

    // NIBP Reading
    label = new QLabel(trs("NIBPReading"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_NIBP);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_NIBP, comboBox);

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void SupervisorPrintMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        SupervisorPrintMenuContentPrivate::MenuItem item
                = (SupervisorPrintMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_ECG_RHYTHM:
            currentConfig.setNumValue("Print|PresentingECG", index);
            break;
        case SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_PATIENT_ALARM:
            currentConfig.setNumValue("Print|CheckPatient", index);
            break;
        case SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_PHY_ALARM:
            currentConfig.setNumValue("Print|PhysiologicalAlarm", index);
            break;
        case SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_CODEMARKER:
            currentConfig.setNumValue("Print|CoderMarker", index);
            break;
        case SupervisorPrintMenuContentPrivate::ITEM_CBO_PRINT_NIBP:
            currentConfig.setNumValue("Print|NIBPReading", index);
            break;
        default:
            break;
        }
    }
}
