/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/11
 **/

#include "DisplayMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "SoundManager.h"
#include "SystemManager.h"
#include "IConfig.h"

class DisplayMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ECG_COLOR,
        ITEM_CBO_SPO2_COLOR,
        ITEM_CBO_NIBP_COLOR,
        ITEM_CBO_CO2_COLOR,
        ITEM_CBO_RESP_COLOR,
        ITEM_CBO_TEMP_COLOR,
        ITEM_CBO_PARAM_COLOR,
        ITEM_CBO_NIGHT_MODE
    };

    DisplayMenuContentPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    QStringList colorList;
};

void DisplayMenuContentPrivate::loadOptions()
{
    QString color;
    currentConfig.getStrValue("Display|ECGColor", color);
    combos[ITEM_CBO_ECG_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|SPO2Color", color);
    combos[ITEM_CBO_SPO2_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|NIBPColor", color);
    combos[ITEM_CBO_NIBP_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|CO2Color", color);
    combos[ITEM_CBO_CO2_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|RESPColor", color);
    combos[ITEM_CBO_RESP_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|TEMPColor", color);
    combos[ITEM_CBO_TEMP_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|ParamColor", color);
    combos[ITEM_CBO_PARAM_COLOR]->setCurrentIndex(colorList.indexOf(color));

    currentConfig.getStrValue("Display|NightMode", color);
    combos[ITEM_CBO_NIGHT_MODE]->setCurrentIndex(colorList.indexOf(color));
}

DisplayMenuContent::DisplayMenuContent()
    : MenuContent(trs("SupervisorDisplayMenu"), trs("SupervisorDisplayMenuDesc")),
      d_ptr(new DisplayMenuContentPrivate)
{
    QString color;
    currentConfig.getStrValue("Display|AllColors", color);
    d_ptr->colorList = color.split(',', QString::KeepEmptyParts);
}

DisplayMenuContent::~DisplayMenuContent()
{
    delete d_ptr;
}

void DisplayMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void DisplayMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // ECG
    label = new QLabel(trs("ECG"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_ECG_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_ECG_COLOR, comboBox);

    // SPO2
    label = new QLabel(trs("SPO2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_SPO2_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_SPO2_COLOR, comboBox);

    // NIBP
    label = new QLabel(trs("NIBP"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_NIBP_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_NIBP_COLOR, comboBox);

    // CO2
    label = new QLabel(trs("CO2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_CO2_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_CO2_COLOR, comboBox);

    // RESP
    label = new QLabel(trs("RESP"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_RESP_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_RESP_COLOR, comboBox);

    // TEMP
    label = new QLabel(trs("TEMP"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_TEMP_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_TEMP_COLOR, comboBox);

    // parameter color
    label = new QLabel(trs("ParamColor"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_PARAM_COLOR);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_PARAM_COLOR, comboBox);

    // night mode color
    label = new QLabel(trs("NightMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    for (int i = 0; i < d_ptr->colorList.count(); i ++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    itemID = static_cast<int>(DisplayMenuContentPrivate::ITEM_CBO_NIGHT_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(DisplayMenuContentPrivate::ITEM_CBO_NIGHT_MODE, comboBox);

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void DisplayMenuContent::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    QString strPath;
    if (box)
    {
        DisplayMenuContentPrivate::MenuItem item
                = (DisplayMenuContentPrivate::MenuItem)box->property("Item").toInt();
        switch (item) {
        case DisplayMenuContentPrivate::ITEM_CBO_ECG_COLOR:
            strPath = "Display|ECGColor";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_SPO2_COLOR:
            strPath = "Display|SPO2Color";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_NIBP_COLOR:
            strPath = "Display|NIBPColor";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_CO2_COLOR:
            strPath = "Display|CO2Color";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_RESP_COLOR:
            strPath = "Display|RESPColor";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_TEMP_COLOR:
            strPath = "Display|TEMPColor";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_PARAM_COLOR:
            strPath = "Display|ParamColor";
            break;
        case DisplayMenuContentPrivate::ITEM_CBO_NIGHT_MODE:
            strPath = "Display|NightMode";
            break;
        }
        currentConfig.setStrValue(strPath, d_ptr->colorList.at(index));
    }
}

