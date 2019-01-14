/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/18
 **/

#include "UnitSetupMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IConfig.h"
#include "Button.h"
#include "ConfigManager.h"
#include "UnitManager.h"
#include "TEMPParam.h"
#include "CO2Param.h"
#include "NIBPParam.h"

class UnitSetupMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_HEIGHT_UNIT,
        ITEM_CBO_WEIGHT_UNIT,
        ITEM_CBO_ST_UNIT,
        ITEM_CBO_PRESSURE_UNIT,
        ITEM_CBO_TEMP_UNIT,
        ITEM_CBO_CVP_UNIT,
        ITEM_CBO_ICP_UNIT,
        ITEM_CBO_CO2_UNIT,
        ITEM_CBO_MAX
    };

    UnitSetupMenuContentPrivate() {}

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
};

void UnitSetupMenuContentPrivate::loadOptions()
{
    int unit  = 0;
    int index = 0;
    systemConfig.getNumValue("Unit|HeightUnit", unit);
    if (unit == UNIT_CM)
    {
        index = 0;
    }
    else if (unit == UNIT_INCH)
    {
        index = 1;
    }
    combos[ITEM_CBO_HEIGHT_UNIT]->setCurrentIndex(index);

    systemConfig.getNumValue("Unit|WeightUnit", unit);
    if (unit == UNIT_KG)
    {
        index = 0;
    }
    else if (unit == UNIT_LB)
    {
        index = 1;
    }
    combos[ITEM_CBO_WEIGHT_UNIT]->setCurrentIndex(index);

    systemConfig.getNumValue("Unit|STUnit", unit);
    if (unit == UNIT_MV)
    {
        index = 0;
    }
    else if (unit == UNIT_MM)
    {
        index = 1;
    }
    combos[ITEM_CBO_ST_UNIT]->setCurrentIndex(index);

    systemConfig.getNumValue("Unit|PressureUnit", unit);
    if (unit == UNIT_MMHG)
    {
        index = 0;
    }
    else if (unit == UNIT_KPA)
    {
        index = 1;
    }
    combos[ITEM_CBO_PRESSURE_UNIT]->setCurrentIndex(index);

    systemConfig.getNumValue("Unit|TemperatureUnit", unit);
    if (unit == UNIT_TF)
    {
        index = 0;
    }
    else if (unit == UNIT_TC)
    {
        index = 1;
    }
    combos[ITEM_CBO_TEMP_UNIT]->setCurrentIndex(index);

    systemConfig.getNumValue("Unit|CVPUnit", unit);
    if (unit == UNIT_MMHG)
    {
        index = 0;
    }
    else if (unit == UNIT_KPA)
    {
        index = 1;
    }
    else if (unit == UNIT_CMH2O)
    {
        index = 2;
    }
    combos[ITEM_CBO_CVP_UNIT]->setCurrentIndex(index);

    systemConfig.getNumValue("Unit|ICPUnit", unit);
    if (unit == UNIT_MMHG)
    {
        index = 0;
    }
    else if (unit == UNIT_KPA)
    {
        index = 1;
    }
    else if (unit == UNIT_CMH2O)
    {
        index = 2;
    }
    combos[ITEM_CBO_ICP_UNIT]->setCurrentIndex(index);

    currentConfig.getNumValue("Unit|CO2Unit", unit);
    if (unit == UNIT_MMHG)
    {
        index = 0;
    }
    else if (unit == UNIT_KPA)
    {
        index = 1;
    }
    else if (unit == UNIT_PERCENT)
    {
        index = 2;
    }
    combos[ITEM_CBO_CO2_UNIT]->setCurrentIndex(index);

    bool isOnlyToRead = configManager.isReadOnly();
    QMap<MenuItem, ComboBox *>::iterator it =  combos.begin();
    for (; it != combos.end(); ++it)
    {
        it.value()->setEnabled(!isOnlyToRead);
    }
}

UnitSetupMenuContent::UnitSetupMenuContent()
    : MenuContent(trs("UnitSetup"), trs("UnitSetupDesc")),
      d_ptr(new UnitSetupMenuContentPrivate)
{
}

UnitSetupMenuContent::~UnitSetupMenuContent()
{
    delete d_ptr;
}

void UnitSetupMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();
    for (int i = 0; i < UnitSetupMenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[UnitSetupMenuContentPrivate
                ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }
}

void UnitSetupMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // height unit
    label = new QLabel(trs("HeightUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_CM)))
                       << trs(Unit::getSymbol(UnitType(UNIT_INCH)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_HEIGHT_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_HEIGHT_UNIT, comboBox);

    // weight unit
    label = new QLabel(trs("WeightUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_KG)))
                       << trs(Unit::getSymbol(UnitType(UNIT_LB)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_WEIGHT_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_WEIGHT_UNIT, comboBox);

    // ST unit
    label = new QLabel(trs("STUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_MV)))
                       << trs(Unit::getSymbol(UnitType(UNIT_MM)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_ST_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_ST_UNIT, comboBox);

    // Pressure unit
    label = new QLabel(trs("PressureUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_MMHG)))
                       << trs(Unit::getSymbol(UnitType(UNIT_KPA)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_PRESSURE_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_PRESSURE_UNIT, comboBox);

    // temp unit
    label = new QLabel(trs("TemperatureUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_TF)))
                       << trs(Unit::getSymbol(UnitType(UNIT_TC)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_TEMP_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_TEMP_UNIT, comboBox);

    // cvp unit
    label = new QLabel(trs("CVPUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_MMHG)))
                       << trs(Unit::getSymbol(UnitType(UNIT_KPA)))
                       << trs(Unit::getSymbol(UnitType(UNIT_CMH2O)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_CVP_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_CVP_UNIT, comboBox);

    // ICP unit
    label = new QLabel(trs("ICPUnit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_MMHG)))
                       << trs(Unit::getSymbol(UnitType(UNIT_KPA)))
                       << trs(Unit::getSymbol(UnitType(UNIT_CMH2O)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_ICP_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_ICP_UNIT, comboBox);

    // CO2 unit
    label = new QLabel(trs("CO2Unit"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(Unit::getSymbol(UnitType(UNIT_MMHG)))
                       << trs(Unit::getSymbol(UnitType(UNIT_KPA)))
                       << trs(Unit::getSymbol(UnitType(UNIT_PERCENT)))
                      );
    itemID = static_cast<int>(UnitSetupMenuContentPrivate::ITEM_CBO_CO2_UNIT);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(UnitSetupMenuContentPrivate::ITEM_CBO_CO2_UNIT, comboBox);

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void UnitSetupMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        UnitSetupMenuContentPrivate::MenuItem item
            = (UnitSetupMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item)
        {
        case UnitSetupMenuContentPrivate::ITEM_CBO_HEIGHT_UNIT:
        {
            if (index == 0)
            {
                index = UNIT_CM;
            }
            else if (index == 1)
            {
                index = UNIT_INCH;
            }
            systemConfig.setNumValue("Unit|HeightUnit", index);
            break;
        }
        case UnitSetupMenuContentPrivate::ITEM_CBO_WEIGHT_UNIT:
        {
            if (index == 0)
            {
                index = UNIT_KG;
            }
            else if (index == 1)
            {
                index = UNIT_LB;
            }
            systemConfig.setNumValue("Unit|WeightUnit", index);
            break;
        }
        case UnitSetupMenuContentPrivate::ITEM_CBO_ST_UNIT:
            if (index == 0)
            {
                index = UNIT_MV;
            }
            else if (index == 1)
            {
                index = UNIT_MM;
            }
            systemConfig.setNumValue("Unit|STUnit", index);
            break;
        case UnitSetupMenuContentPrivate::ITEM_CBO_PRESSURE_UNIT:
            if (index == 0)
            {
                index = UNIT_MMHG;
            }
            else if (index == 1)
            {
                index = UNIT_KPA;
            }
            else
            {
                index = UNIT_MMHG;
            }
            nibpParam.setUnit(static_cast<UnitType>(index));
            break;
        case UnitSetupMenuContentPrivate::ITEM_CBO_TEMP_UNIT:
            if (index == 0)
            {
                index = UNIT_TF;
            }
            else if (index == 1)
            {
                index = UNIT_TC;
            }
            else
            {
                index = UNIT_TF;
            }
            tempParam.setUnit(static_cast<UnitType>(index));
            break;
        case UnitSetupMenuContentPrivate::ITEM_CBO_CVP_UNIT:
            if (index == 0)
            {
                index = UNIT_MMHG;
            }
            else if (index == 1)
            {
                index = UNIT_KPA;
            }
            else if (index == 2)
            {
                index = UNIT_CMH2O;
            }
            else
            {
                index = UNIT_MMHG;
            }
            systemConfig.setNumValue("Unit|CVPUnit", index);
            break;
        case UnitSetupMenuContentPrivate::ITEM_CBO_ICP_UNIT:
            if (index == 0)
            {
                index = UNIT_MMHG;
            }
            else if (index == 1)
            {
                index = UNIT_KPA;
            }
            else if (index == 2)
            {
                index = UNIT_CMH2O;
            }
            else
            {
                index = UNIT_MMHG;
            }
            systemConfig.setNumValue("Unit|ICPUnit", index);
            break;
        case UnitSetupMenuContentPrivate::ITEM_CBO_CO2_UNIT:
            if (index == 0)
            {
                index = UNIT_MMHG;
            }
            else if (index == 1)
            {
                index = UNIT_KPA;
            }
            else if (index == 2)
            {
                index = UNIT_PERCENT;
            }
            else
            {
                index = UNIT_MMHG;
            }
            currentConfig.setNumValue("Local|CO2Unit", index);
            co2Param.updateUnit();
            break;
        default:
            break;
        }
    }
}
