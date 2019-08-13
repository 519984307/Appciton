/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by WeiJuan Zhu <zhuweijuan@blmed.cn>, 2019/8/12
 **/

#include "SPO2SeniorMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "SPO2Param.h"
#include "ConfigManager.h"

enum MenuItem
{
    ITEM_CBO_SPHB_PRECISION_MODE,
    ITEM_CBO_SPHB_VESSEL_MODE,
    ITEM_CBO_SPHB_AVERAGING_MODE,
    ITEM_CBO_PVI_AVERAGING_MODE,
    ITEM_CBO_NR
};

class SPO2SeniorMenuContentPrivate
{
public:
    SPO2SeniorMenuContentPrivate(){}
    ~SPO2SeniorMenuContentPrivate(){}

    QMap<MenuItem, ComboBox *> cbos;
    void loadOption();
};

SPO2SeniorMenuContent::SPO2SeniorMenuContent()
    : MenuContent(trs("SPO2SeniorMenu"), trs("SPO2SeniorMenuDesc")),
      d_ptr(new SPO2SeniorMenuContentPrivate())
{
}

SPO2SeniorMenuContent::~SPO2SeniorMenuContent()
{
    delete d_ptr;
}

void SPO2SeniorMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    // sphb precision mode
    QLabel *label = new QLabel();
    label->setText(trs("SpHbPrecisionMode"));
    layout->addWidget(label, d_ptr->cbos.count(), 0);
    ComboBox *combo = new ComboBox();
    combo->addItems(QStringList()
                   << trs(SPO2Symbol::convert(PRECISION_NEAREST_0_1))
                   << trs(SPO2Symbol::convert(PRECISION_NEAREST_0_5))
                   << trs(SPO2Symbol::convert(PRECISION_WHOLE_NUMBER)));
    layout->addWidget(combo, d_ptr->cbos.count(), 1);
    int item = static_cast<int>(ITEM_CBO_SPHB_PRECISION_MODE);
    combo->setProperty("Item", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    d_ptr->cbos.insert(ITEM_CBO_SPHB_PRECISION_MODE, combo);

    // sphb vessel mode
    label = new QLabel();
    label->setText(trs("SpHbVesselMode"));
    layout->addWidget(label, d_ptr->cbos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(SPO2Symbol::convert(BLOOD_VESSEL_ARTERIAL))
                    << trs(SPO2Symbol::convert(BLOOD_VESSEL_VENOUS)));
    layout->addWidget(combo, d_ptr->cbos.count(), 1);
    item = static_cast<int>(ITEM_CBO_SPHB_VESSEL_MODE);
    combo->setProperty("Item", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    d_ptr->cbos.insert(ITEM_CBO_SPHB_VESSEL_MODE, combo);

    // sphb averaging mode
    label = new QLabel();
    label->setText(trs("SpHbAveragingMode"));
    layout->addWidget(label, d_ptr->cbos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_LONG))
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_MED))
                    << trs(SPO2Symbol::convert(SPHB_AVERAGING_MODE_SHORT)));
    layout->addWidget(combo, d_ptr->cbos.count(), 1);
    item = static_cast<int>(ITEM_CBO_SPHB_AVERAGING_MODE);
    combo->setProperty("Item", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    d_ptr->cbos.insert(ITEM_CBO_SPHB_AVERAGING_MODE, combo);

    // pvi averaging mode
    label = new QLabel();
    label->setText(trs("PviAveragingMode"));
    layout->addWidget(label, d_ptr->cbos.count(), 0);
    combo = new ComboBox();
    combo->addItems(QStringList()
                    << trs(SPO2Symbol::convert(AVERAGING_MODE_NORMAL))
                    << trs(SPO2Symbol::convert(AVERAGING_MODE_FAST)));
    layout->addWidget(combo, d_ptr->cbos.count(), 1);
    item = static_cast<int>(ITEM_CBO_PVI_AVERAGING_MODE);
    combo->setProperty("Item", qVariantFromValue(item));
    connect(combo, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    d_ptr->cbos.insert(ITEM_CBO_PVI_AVERAGING_MODE, combo);

    layout->setRowStretch(d_ptr->cbos.count(), 1);
}

void SPO2SeniorMenuContent::readyShow()
{
    d_ptr->loadOption();
}

void SPO2SeniorMenuContent::onComboIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    MenuItem item = static_cast<MenuItem>(combo->property("Item").toInt());
    switch (item)
    {
    case ITEM_CBO_SPHB_PRECISION_MODE:
        spo2Param.setSpHbPrecision(static_cast<SpHbPrecisionMode>(index));
        break;
    case ITEM_CBO_SPHB_VESSEL_MODE:
        spo2Param.setSpHbBloodVessel(static_cast<SpHbBloodVesselMode>(index));
        break;
    case ITEM_CBO_SPHB_AVERAGING_MODE:
        spo2Param.setSpHbAveragingMode(static_cast<SpHbAveragingMode>(index));
        break;
    case ITEM_CBO_PVI_AVERAGING_MODE:
        spo2Param.setPviAveragingMode(static_cast<AveragingMode>(index));
        break;
    default:
        break;
    }
}

void SPO2SeniorMenuContentPrivate::loadOption()
{
    int index = static_cast<int>(spo2Param.getSpHbPrecision());
    cbos[ITEM_CBO_SPHB_PRECISION_MODE]->setCurrentIndex(index);

    index = static_cast<int>(spo2Param.getSpHbBloodVessel());
    cbos[ITEM_CBO_SPHB_VESSEL_MODE]->setCurrentIndex(index);

    index = static_cast<int>(spo2Param.getSpHbAveragingMode());
    cbos[ITEM_CBO_SPHB_AVERAGING_MODE]->setCurrentIndex(index);

    index = static_cast<int>(spo2Param.getPviAveragingMode());
    cbos[ITEM_CBO_PVI_AVERAGING_MODE]->setCurrentIndex(index);
}
