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
#include "Button.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "ConfigManager.h"
#include "RESPDupParam.h"

class ConfigEditRespMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_APNEA_DELAY = 0,
        ITEM_CBO_BREATH_LEAD,
        ITEM_CBO_RR_SOURCE,
        ITEM_CBO_RESP_GAIN,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditRespMenuContentPrivate(Config * const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
    QLabel *brRRSouce;
};

ConfigEditRespMenuContentPrivate
    ::ConfigEditRespMenuContentPrivate(Config *const config)
    : config(config), brRRSouce(NULL)
{
    combos.clear();
}

ConfigEditRespMenuContent::ConfigEditRespMenuContent(Config *const config):
    MenuContent(trs("RESPMenu"),
                trs("RESPMenuDesc")),
    d_ptr(new ConfigEditRespMenuContentPrivate(config))
{
}

ConfigEditRespMenuContent::~ConfigEditRespMenuContent()
{
    delete d_ptr;
}

void ConfigEditRespMenuContentPrivate::loadOptions()
{
    int index = 0;
    config->getNumValue("RESP|ApneaDelay", index);
    combos[ITEM_CBO_APNEA_DELAY]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("RESP|BreathLead", index);
    combos[ITEM_CBO_BREATH_LEAD]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("RESP|BrSource", index);
    combos[ITEM_CBO_RR_SOURCE]->setCurrentIndex(index);
    if (respDupParam.getParamSourceType() == RESPDupParam::BR)
    {
        brRRSouce->setText(trs("BRSource"));
    }
    else
    {
        brRRSouce->setText(trs("RRSource"));
    }
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
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditRespMenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditRespMenuContentPrivate
                ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }
}

void ConfigEditRespMenuContent::onComboIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    int indexType = combo->property("Item").toInt();
    switch (indexType)
    {
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_APNEA_DELAY:
        d_ptr->config->setNumValue("RESP|ApneaDelay", index);
        // 联动co2的窒息延迟
        d_ptr->config->setNumValue("CO2|ApneaTime", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_BREATH_LEAD:
        d_ptr->config->setNumValue("RESP|BreathLead", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_RR_SOURCE:
        d_ptr->config->setNumValue("RESP|BrSource", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_RESP_GAIN:
        d_ptr->config->setNumValue("RESP|Gain", index);
        break;
    case ConfigEditRespMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
        d_ptr->config->setNumValue("RESP|SweepSpeed", index);
        break;
    default:
        qdebug("Invalid combo id.");
        break;
    }
}

void ConfigEditRespMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_RR_BR, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void ConfigEditRespMenuContent::layoutExec()
{
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
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_20_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_25_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_30_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_35_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_40_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_45_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_50_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_55_SEC))
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_60_SEC)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_APNEA_DELAY, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_APNEA_DELAY;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // breath lead
    label = new QLabel(trs("BreathLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_LEAD_II))
                       << trs(RESPSymbol::convert(RESP_LEAD_I))
                       << trs(RESPSymbol::convert(RESP_LEAD_AUTO)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_BREATH_LEAD, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_BREATH_LEAD;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // rr source
    label = new QLabel(trs("RRSource"));
    d_ptr->brRRSouce = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(BR_RR_AUTO))
                       << trs(RESPSymbol::convert(BR_RR_SOURCE_ECG))
                       << trs(RESPSymbol::convert(BR_RR_SOURCE_CO2)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate::ITEM_CBO_RR_SOURCE, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_RR_SOURCE;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // gain
    label = new QLabel(trs("RESPWaveGain"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_ZOOM_X025))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X050))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X100))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X200))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X300))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X400))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X500)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_RESP_GAIN, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_RESP_GAIN;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // sweep speed
    label = new QLabel(trs("SweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_6_25))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_12_5))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_25_0))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_50_0)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditRespMenuContentPrivate
                         ::ITEM_CBO_SWEEP_SPEED, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));
    itemID = ConfigEditRespMenuContentPrivate::ITEM_CBO_SWEEP_SPEED;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}






















