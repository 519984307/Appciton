/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2019/3/26
 **/

#include "ApneaStimulationMenuContent.h"
#include "LanguageManager.h"
#include "ComboBox.h"
#include "SpinBox.h"
#include <QGridLayout>
#include <QLabel>
#include "ConfigManager.h"
#include "PatientManager.h"
#include "O2Param.h"
#include "RunningStatusBar.h"

class ApneaStimulationMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_SELFTEST,
        ITEM_SPB_VIBRATEINTENSITY,
        ITEM_CBO_RESP,
        ITEM_CBO_CO2,
        ITEM_SPB_HR,
        ITEM_SPB_SPO2,
    };
    ApneaStimulationMenuContentPrivate()
    {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, SpinBox *> spinbos;
};

ApneaStimulationMenuContent::ApneaStimulationMenuContent()
    : MenuContent(trs("ApneaStimulationMenu"),
                  trs("ApneaStimulationMenuDesc")),
      d_ptr(new ApneaStimulationMenuContentPrivate())
{
}

ApneaStimulationMenuContent::~ApneaStimulationMenuContent()
{
    delete d_ptr;
}

void ApneaStimulationMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *glayout = new QGridLayout(this);
    glayout->setMargin(10);
    glayout->setHorizontalSpacing(0);

    ComboBox *combo;
    SpinBox *spin;
    QLabel *label;
    int itemID = 0;
    int row = 0;

    // 自测
    label = new QLabel(trs("SelfTest"));
    glayout->addWidget(label, row, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    itemID = static_cast<int>(ApneaStimulationMenuContentPrivate::ITEM_CBO_SELFTEST);
    combo->setProperty("Item", qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(combo, row++, 1);
    d_ptr->combos.insert(ApneaStimulationMenuContentPrivate::ITEM_CBO_SELFTEST, combo);

    // 震动强度
    label = new QLabel(trs("VibrationIntensity"));
    glayout->addWidget(label, row, 0);
    spin = new SpinBox();
    spin->setRange(50, 100);
    spin->setStep(1);
    spin->setValue(50);
    itemID = static_cast<int>(ApneaStimulationMenuContentPrivate::ITEM_SPB_VIBRATEINTENSITY);
    spin->setProperty("Item", qVariantFromValue(itemID));
    connect(spin, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    glayout->addWidget(spin, row++, 1);
    d_ptr->spinbos.insert(ApneaStimulationMenuContentPrivate::ITEM_SPB_VIBRATEINTENSITY, spin);

    // resp
    label = new QLabel("RESP");
    glayout->addWidget(label, row, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    itemID = static_cast<int>(ApneaStimulationMenuContentPrivate::ITEM_CBO_RESP);
    combo->setProperty("Item", qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(combo, row++, 1);
    d_ptr->combos.insert(ApneaStimulationMenuContentPrivate::ITEM_CBO_RESP, combo);

    // CO2
    label = new QLabel("CO2");
    glayout->addWidget(label, row, 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs("Off")
                    << trs("On"));
    itemID = static_cast<int>(ApneaStimulationMenuContentPrivate::ITEM_CBO_CO2);
    combo->setProperty("Item", qVariantFromValue(itemID));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    glayout->addWidget(combo, row++, 1);
    d_ptr->combos.insert(ApneaStimulationMenuContentPrivate::ITEM_CBO_CO2, combo);

    // HR
    label = new QLabel("HR");
    glayout->addWidget(label, row, 0);
    spin = new SpinBox();
    spin->setRange(90, 110);
    spin->setStep(1);
    spin->setValue(100);
    itemID = static_cast<int>(ApneaStimulationMenuContentPrivate::ITEM_SPB_HR);
    spin->setProperty("Item", qVariantFromValue(itemID));
    connect(spin, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    glayout->addWidget(spin, row++, 1);
    d_ptr->spinbos.insert(ApneaStimulationMenuContentPrivate::ITEM_SPB_HR, spin);

    // SPO2
    label = new QLabel("SPO2");
    glayout->addWidget(label, row, 0);
    spin = new SpinBox();
    spin->setRange(80, 90);
    spin->setStep(1);
    spin->setValue(85);
    itemID = static_cast<int>(ApneaStimulationMenuContentPrivate::ITEM_SPB_SPO2);
    spin->setProperty("Item", qVariantFromValue(itemID));
    connect(spin, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxValueChanged(int, int)));
    glayout->addWidget(spin, row++, 1);
    d_ptr->spinbos.insert(ApneaStimulationMenuContentPrivate::ITEM_SPB_SPO2, spin);

    glayout->setRowStretch(row, 1);
}

void ApneaStimulationMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ApneaStimulationMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (combo)
    {
        ApneaStimulationMenuContentPrivate::MenuItem item
            = (ApneaStimulationMenuContentPrivate::MenuItem)combo->property("Item").toInt();
        switch (item)
        {
        case ApneaStimulationMenuContentPrivate::ITEM_CBO_SELFTEST:
            currentConfig.setNumValue("ApneaStimulation|SelfTest", index);
            o2Param.setVibrationReason(APNEASTIMULATION_REASON_SELFTEST, index);
            break;
        case ApneaStimulationMenuContentPrivate::ITEM_CBO_RESP:
            currentConfig.setNumValue("ApneaStimulation|RESP", index);
            break;
        case ApneaStimulationMenuContentPrivate::ITEM_CBO_CO2:
            currentConfig.setNumValue("ApneaStimulation|CO2", index);
            break;
        default:
            break;
        }
    }
}

void ApneaStimulationMenuContent::onSpinBoxValueChanged(int value, int scale)
{
    Q_UNUSED(scale)
    SpinBox *spinbox = qobject_cast<SpinBox *>(sender());
    if (spinbox)
    {
        ApneaStimulationMenuContentPrivate::MenuItem item
            = (ApneaStimulationMenuContentPrivate::MenuItem)spinbox->property("Item").toInt();
        switch (item)
        {
        case ApneaStimulationMenuContentPrivate::ITEM_SPB_HR:
            currentConfig.setNumValue("ApneaStimulation|HR", value);
            break;
        case ApneaStimulationMenuContentPrivate::ITEM_SPB_SPO2:
            currentConfig.setNumValue("ApneaStimulation|SPO2", value);
            break;
        case ApneaStimulationMenuContentPrivate::ITEM_SPB_VIBRATEINTENSITY:
            o2Param.vibrationIntensityControl(value);
            break;
        default:
            break;
        }
    }
}

void ApneaStimulationMenuContentPrivate::loadOptions()
{
    int index = 0;
    if (patientManager.getType() == PATIENT_TYPE_NEO)
    {
        currentConfig.getNumValue("ApneaStimulation|SelfTest", index);
        combos[ITEM_CBO_SELFTEST]->setCurrentIndex(index);
        combos[ITEM_CBO_SELFTEST]->setEnabled(true);

        index = 0;
        currentConfig.getNumValue("ApneaStimulation|RESP", index);
        combos[ITEM_CBO_RESP]->setCurrentIndex(index);
        combos[ITEM_CBO_RESP]->setEnabled(true);

        index = 0;
        currentConfig.getNumValue("ApneaStimulation|CO2", index);
        combos[ITEM_CBO_CO2]->setCurrentIndex(index);
        combos[ITEM_CBO_CO2]->setEnabled(true);

        index = 0;
        currentConfig.getNumValue("ApneaStimulation|HR", index);
        spinbos[ITEM_SPB_HR]->setValue(index);
        spinbos[ITEM_SPB_HR]->setEnabled(true);

        index = 0;
        currentConfig.getNumValue("ApneaStimulation|SPO2", index);
        spinbos[ITEM_SPB_SPO2]->setValue(index);
        spinbos[ITEM_SPB_SPO2]->setEnabled(true);
    }
    else
    {
        combos[ITEM_CBO_SELFTEST]->setCurrentIndex(index);
        combos[ITEM_CBO_SELFTEST]->setEnabled(false);

        combos[ITEM_CBO_RESP]->setCurrentIndex(index);
        combos[ITEM_CBO_RESP]->setEnabled(false);

        combos[ITEM_CBO_CO2]->setCurrentIndex(index);
        combos[ITEM_CBO_CO2]->setEnabled(false);

        index = 100;
        spinbos[ITEM_SPB_HR]->setValue(index);
        spinbos[ITEM_SPB_HR]->setEnabled(false);

        index = 85;
        spinbos[ITEM_SPB_SPO2]->setValue(index);
        spinbos[ITEM_SPB_SPO2]->setEnabled(false);
    }
}
