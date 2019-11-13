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
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QMap>
#include "ConfigManager.h"
#include "SystemManager.h"

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
        ITEM_CBO_IBP_COLOR,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditDisplayMenuContentPrivate(QStringList colorList, Config * const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    /**
     * @brief paramStaInit  参数状态初始化
     */
    void paramStaInit();

    QMap <MenuItem, ComboBox *> combos;
    QStringList colorList;
    Config * const config;
    bool paramSupportMap[ITEM_CBO_MAX];
};

ConfigEditDisplayMenuContentPrivate
    ::ConfigEditDisplayMenuContentPrivate(QStringList colorList, Config * const config)
    : colorList(colorList),
      config(config)
{
    combos.clear();

    paramStaInit();
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
                          << "AGColor"
                          << "IBPColor";
    QString color;
    bool isOnlyToRead = configManager.isReadOnly();
    for (int i = 0; i < strList.count(); i++)
    {
        color.clear();
        MenuItem item = static_cast<MenuItem>(i);
        if (paramSupportMap[item] == false)
        {
            continue;
        }
        config->getStrValue(QString("Display|%1").arg(strList.at(i)), color);
        combos[item]->setCurrentIndex(colorList.indexOf(color));
        combos[item]->setEnabled(!isOnlyToRead);
    }
}

void ConfigEditDisplayMenuContentPrivate::paramStaInit()
{
    if (systemManager.isSupport(CONFIG_TEMP))
    {
        paramSupportMap[ITEM_CBO_TEMP_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_TEMP_COLOR] = false;
    }

    if (systemManager.isSupport(CONFIG_AG))
    {
        paramSupportMap[ITEM_CBO_AG_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_AG_COLOR] = false;
    }

    if (systemManager.isSupport(CONFIG_RESP))
    {
        paramSupportMap[ITEM_CBO_RESP_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_RESP_COLOR] = false;
    }

    if (systemManager.isSupport(CONFIG_NIBP))
    {
        paramSupportMap[ITEM_CBO_NIBP_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_NIBP_COLOR] = false;
    }

    if (systemManager.isSupport(CONFIG_CO2))
    {
        paramSupportMap[ITEM_CBO_CO2_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_CO2_COLOR] = false;
    }

    if (systemManager.isSupport(CONFIG_SPO2))
    {
        paramSupportMap[ITEM_CBO_SPO2_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_SPO2_COLOR] = false;
    }

    if (systemManager.isSupport(CONFIG_IBP))
    {
        paramSupportMap[ITEM_CBO_IBP_COLOR] = true;
    }
    else
    {
        paramSupportMap[ITEM_CBO_IBP_COLOR] = false;
    }

    paramSupportMap[ITEM_CBO_ECG_COLOR] = true;
}

void ConfigEditDisplayMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditDisplayMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;
    QStringList trColorList;
    for (int i = 0; i < d_ptr->colorList.count(); i++)
    {
        trColorList.append(trs(d_ptr->colorList.at(i)));
    }

    // ecg color
    label = new QLabel(trs("ECG"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(trColorList);
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                         ::ITEM_CBO_ECG_COLOR, comboBox);
    itemID = ConfigEditDisplayMenuContentPrivate
             ::ITEM_CBO_ECG_COLOR;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    ConfigEditDisplayMenuContentPrivate::MenuItem id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_SPO2_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // spo2 color
        label = new QLabel(trs("SPO2"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_SPO2_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                 ::ITEM_CBO_SPO2_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

    id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_NIBP_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // nibp color
        label = new QLabel(trs("NIBP"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_NIBP_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                 ::ITEM_CBO_NIBP_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

    id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_CO2_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // CO2 color
        label = new QLabel(trs("CO2"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_CO2_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                 ::ITEM_CBO_CO2_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

    id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_RESP_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // RESP color
        label = new QLabel(trs("RESP"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_RESP_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                 ::ITEM_CBO_RESP_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

    id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_TEMP_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // TEMP color
        label = new QLabel(trs("TEMP"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_TEMP_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                 ::ITEM_CBO_TEMP_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

    id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_AG_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // AG color
        label = new QLabel(trs("AG"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_AG_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                 ::ITEM_CBO_AG_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox , SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

    id = ConfigEditDisplayMenuContentPrivate::ITEM_CBO_IBP_COLOR;
    if (d_ptr->paramSupportMap[id])
    {
        // IBP color
        label = new QLabel(trs("IBP"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox;
        comboBox->addItems(trColorList);
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditDisplayMenuContentPrivate
                             ::ITEM_CBO_IBP_COLOR, comboBox);
        itemID = ConfigEditDisplayMenuContentPrivate
                ::ITEM_CBO_IBP_COLOR;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    }

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
    case ConfigEditDisplayMenuContentPrivate::ITEM_CBO_IBP_COLOR:
        str = "IBPColor";
        break;
    }
    d_ptr->config->setStrValue(QString("Display|%1").arg(str), d_ptr->colorList.at(index));
}

















