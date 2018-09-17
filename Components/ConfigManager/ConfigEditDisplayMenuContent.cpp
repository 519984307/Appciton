/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#include "ConfigEditDisplayMenuContent.h"
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include <QMap>
#include "ComboBox.h"
#include "ConfigManager.h"

class ConfigEditDisplayMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ECG_COLOR = 0,
        ITEM_CBO_SPO2_COLOR,
        ITEM_CBO_CO2_COLOR,
        ITEM_CBO_NIBP_COLOR,
        ITEM_CBO_RESP_COLOR,
        ITEM_CBO_TEMP_COLOR,
        ITEM_CBO_AG_COLOR,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditDisplayMenuContentPrivate(QStringList colorList, Config * const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    QStringList colorList;
    Config * const config;
};

ConfigEditDisplayMenuContentPrivate
    ::ConfigEditDisplayMenuContentPrivate(QStringList colorList, Config * const config)
    : colorList(colorList),
      config(config)
{
    combos.clear();
}

ConfigEditDisplayMenuContent::ConfigEditDisplayMenuContent(QStringList colorList , Config * const config):
    MenuContent(trs("ParameterColor"),
                trs("ParameterColorDesc")),
    d_ptr(new ConfigEditDisplayMenuContentPrivate(colorList, config))
{
}

ConfigEditDisplayMenuContent::~ConfigEditDisplayMenuContent()
{
    delete d_ptr;
}

void ConfigEditDisplayMenuContentPrivate::loadOptions()
{
    QStringList strList = QStringList()
                          << "ECGColor"
                          << "SPO2Color"
                          << "CO2Color"
                          << "NIBPColor"
                          << "RESPColor"
                          << "TEMPColor"
                          << "AGColor";
    QString color;
    for (int i = 0; i < strList.count(); i++)
    {
        color.clear();
        MenuItem item = static_cast<MenuItem>(i);
        config->getStrValue(QString("Display|%1").arg(strList.at(i)), color);
        combos[item]->setCurrentIndex(colorList.indexOf(color));
    }
}

void ConfigEditDisplayMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToShow = !configManager.isOnlyShown();
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_TEMP_COLOR]->setEnabled(isOnlyToShow);
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_AG_COLOR]->setEnabled(isOnlyToShow);
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_RESP_COLOR]->setEnabled(isOnlyToShow);
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_NIBP_COLOR]->setEnabled(isOnlyToShow);
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_CO2_COLOR]->setEnabled(isOnlyToShow);
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_SPO2_COLOR]->setEnabled(isOnlyToShow);
    d_ptr->combos[ConfigEditDisplayMenuContentPrivate
            ::ITEM_CBO_ECG_COLOR]->setEnabled(isOnlyToShow);
}

void ConfigEditDisplayMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // ecg color
    label = new QLabel(trs("ECG"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_ECG_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_ECG_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // spo2 color
    label = new QLabel(trs("SPO2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_SPO2_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_SPO2_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // nibp color
    label = new QLabel(trs("NIBP"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_NIBP_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_NIBP_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // CO2 color
    label = new QLabel(trs("CO2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_CO2_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_CO2_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // RESP color
    label = new QLabel(trs("RESP"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_RESP_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_RESP_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // TEMP color
    label = new QLabel(trs("TEMP"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_TEMP_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_TEMP_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // AG color
    label = new QLabel(trs("AG"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        comboBox->addItem(trs(d_ptr->colorList.at(i)));
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_AG_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_AG_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    layout->setRowStretch(d_ptr->combos.count(), 1);
}

void ConfigEditDisplayMenuContent::onComboBoxIndexChanged(int index)
{
    if (index < 0)
    {
        return;
    }
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;
    switch (indexType)
    {
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_AG_COLOR:
        str = "AGColor";
        break;
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_CO2_COLOR:
        str = "CO2Color";
        break;
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_ECG_COLOR:
        str = "ECGColor";
        break;
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_NIBP_COLOR:
        str = "NIBPColor";
        break;
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_RESP_COLOR:
        str = "RESPColor";
        break;
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_SPO2_COLOR:
        str = "SPO2Color";
        break;
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_TEMP_COLOR:
        str = "TEMPColor";
        break;
    }
    d_ptr->config->setStrValue(QString("Display|%1").arg(str), d_ptr->colorList.at(index));
}

















