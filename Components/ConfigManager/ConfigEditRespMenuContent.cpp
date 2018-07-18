/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/17
 **/
#include "ConfigEditRespMenuContent.h"
#include <QGridLayout>
#include <QLabel>
#include "ComboBox.h"
#include "LanguageManager.h"
#include <QMap>
#include "RESPDefine.h"
#include "RESPSymbol.h"
#include "ConfigEditMenuWindow.h"

class ConfigEditRespMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_APNEA_DELAY = 0,
        ITEM_CBO_BREATH_LEAD,
        ITEM_CBO_RESP_GAIN,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_MAX,
    };

    ConfigEditRespMenuContentPrivate();
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
};

ConfigEditRespMenuContentPrivate::ConfigEditRespMenuContentPrivate()
{
    combos.clear();
}

ConfigEditRespMenuContent::ConfigEditRespMenuContent():
    MenuContent(trs("RespMenu"),
                trs("RespMenuDesc")),
    d_ptr(new ConfigEditRespMenuContentPrivate)
{
}

ConfigEditRespMenuContent::~ConfigEditRespMenuContent()
{
    delete d_ptr;
}

void ConfigEditRespMenuContentPrivate::loadOptions()
{
    Config *config = ConfigEditMenuWindow
                     ::getInstance()->getCurrentEditConfig();
    int index = 0;
    config->getNumValue("RESP|ApneaDelay", index);
    combos[ITEM_CBO_APNEA_DELAY]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("RESP|BreathLead", index);
    combos[ITEM_CBO_BREATH_LEAD]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("RESP|Gain", index);
    combos[ITEM_CBO_RESP_GAIN]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("RESP|SweepSpeed", index);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(index);
}

void ConfigEditRespMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ConfigEditRespMenuContent::onComboIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    int indexType = combo->property("Item").toInt();

    Config *config = ConfigEditMenuWindow
                     ::getInstance()->getCurrentEditConfig();
    switch (indexType)
    {
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_APNEA_DELAY:
        config->setNumValue("RESP|ApneaDelay", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_BREATH_LEAD:
        config->setNumValue("RESP|BreathLead", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_RESP_GAIN:
        config->setNumValue("RESP|Gain", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
        config->setNumValue("RESP|SweepSpeed", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}

void ConfigEditRespMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // apena delay
    label = new QLabel(trs("ApneaDelay"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << QString::number(RESP_APNEA_TIME_20_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_25_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_30_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_35_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_40_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_45_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_50_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_55_SEC * 5 + 15)
                       << QString::number(RESP_APNEA_TIME_60_SEC * 5 + 15));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_APNEA_DELAY, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged()));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_APNEA_DELAY;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // breath lead
    label = new QLabel(trs("BreathLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_LEAD_II))
                       << trs(RESPSymbol::convert(RESP_LEAD_I)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_BREATH_LEAD, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged()));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_BREATH_LEAD;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // gain
    label = new QLabel(trs("RespGain"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_ZOOM_X025))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X050))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X100))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X300))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X400))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X500)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_RESP_GAIN, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged()));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_RESP_GAIN;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // sweep speed
    label = new QLabel(trs("SweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_6_25))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_12_5))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_25_0)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_SWEEP_SPEED, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged()));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_SWEEP_SPEED;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    layout->setRowStretch(d_ptr->combos.count(), 1);
}






















