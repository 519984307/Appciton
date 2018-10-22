/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/9
 **/

#include "RESPMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include <ComboBox.h>
#include <QGridLayout>
#include "RESPParam.h"
#include <QMap>
#include "ConfigManager.h"
#include "SystemManager.h"
#include "Button.h"
#include "MainMenuWindow.h"
#include "RESPSymbol.h"
#include "CO2Param.h"
#include "IConfig.h"
#include "RESPDupParam.h"

class RESPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_APNEA_DELAY = 0,
        ITEM_CBO_BREATH_LEAD,
        ITEM_CBO_RR_SOURCE,
        ITEM_CBO_WAVE_GAIN,
        ITEM_CBO_SWEEP_SPEED,
    };

    RESPMenuContentPrivate()
                : brRRSouce(NULL)
    {}

    // load settings
    void loadOptions();

    QLabel *brRRSouce;

    QMap<MenuItem, ComboBox *> combos;
};

void RESPMenuContentPrivate::loadOptions()
{
    // sweep speed
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(respParam.getSweepSpeed());

    // apnea delay
    combos[ITEM_CBO_APNEA_DELAY]->setCurrentIndex(respParam.getApneaTime());

    // rr source
    int index = 0;
    if (respDupParam.isAutoBrSourceEnabled())
    {
        index = BR_RR_AUTO;
    }
    else if (respDupParam.getBrSource() == RESPDupParam::BR_SOURCE_CO2)
    {
        index = BR_RR_SOURCE_CO2;
    }
    else
    {
        index = BR_RR_SOURCE_RESP;
    }
    combos[ITEM_CBO_RR_SOURCE]->setCurrentIndex(index);
    if (respDupParam.getParamSourceType() == RESPDupParam::BR)
    {
        brRRSouce->setText(trs("BRSource"));
    }
    else
    {
        brRRSouce->setText(trs("RRSource"));
    }

    // lead
    combos[ITEM_CBO_BREATH_LEAD]->setCurrentIndex(respParam.getCalcLead());

    // wave gain
    combos[ITEM_CBO_WAVE_GAIN]->setCurrentIndex(respParam.getZoom());
}

RESPMenuContent::RESPMenuContent()
    : MenuContent(trs("RESPMenu"), trs("RESPMenuDesc")),
      d_ptr(new RESPMenuContentPrivate())
{
}

RESPMenuContent::~RESPMenuContent()
{
    delete d_ptr;
}

void RESPMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void RESPMenuContent::layoutExec()
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
                       << trs(RESPSymbol::convert(RESP_APNEA_TIME_OFF))
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
    d_ptr->combos.insert(RESPMenuContentPrivate
                         ::ITEM_CBO_APNEA_DELAY, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    itemID = RESPMenuContentPrivate::ITEM_CBO_APNEA_DELAY;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // breath lead
    label = new QLabel(trs("BreathLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_LEAD_II))
                       << trs(RESPSymbol::convert(RESP_LEAD_I)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate
                         ::ITEM_CBO_BREATH_LEAD, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    itemID = RESPMenuContentPrivate::ITEM_CBO_BREATH_LEAD;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // rr source
    label = new QLabel(trs("RRSource"));
    d_ptr->brRRSouce = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(BR_RR_AUTO))
                       << trs(RESPSymbol::convert(BR_RR_SOURCE_CO2))
                       << trs(RESPSymbol::convert(BR_RR_SOURCE_RESP)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate
                         ::ITEM_CBO_RR_SOURCE, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    itemID = RESPMenuContentPrivate::ITEM_CBO_RR_SOURCE;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // resp wave gain
    label = new QLabel(trs("RESPWaveGain"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                   << trs(RESPSymbol::convert(RESP_ZOOM_X100))
                   << trs(RESPSymbol::convert(RESP_ZOOM_X200))
                   << trs(RESPSymbol::convert(RESP_ZOOM_X300))
                   << trs(RESPSymbol::convert(RESP_ZOOM_X400))
                   << trs(RESPSymbol::convert(RESP_ZOOM_X500)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate
                         ::ITEM_CBO_WAVE_GAIN, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    itemID = RESPMenuContentPrivate::ITEM_CBO_WAVE_GAIN;
    comboBox->setProperty("Item", qVariantFromValue(itemID));

    // sweep speed
    label = new QLabel(trs("RESPSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_6_25))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_12_5))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_25_0)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate
                         ::ITEM_CBO_SWEEP_SPEED, comboBox);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    itemID = RESPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED;
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

void RESPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        RESPMenuContentPrivate::MenuItem item
            = static_cast<RESPMenuContentPrivate::MenuItem>(
                  box->property("Item").toInt());
        switch (item)
        {
        case RESPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            respParam.setSweepSpeed(static_cast<RESPSweepSpeed>(index));
            currentConfig.setNumValue("RESP|SweepSpeed", index);
            break;
        case RESPMenuContentPrivate::ITEM_CBO_APNEA_DELAY:
            currentConfig.setNumValue("Alarm|ApneaTime", index);
            respParam.setApneaTime((ApneaAlarmTime)index);
            co2Param.setApneaTime((ApneaAlarmTime)index);
            break;
        case RESPMenuContentPrivate::ITEM_CBO_RR_SOURCE:
        {
            if (index == 0)
            {
                respDupParam.setAutoBrSourceStatue(true);
                break;
            }
            else
            {
                respDupParam.setAutoBrSourceStatue(false);
            }
            if (BR_RR_SOURCE_CO2 == index)
            {
                respDupParam.setBrSource(RESPDupParam::BR_SOURCE_CO2);
            }
            else if (BR_RR_SOURCE_RESP == index)
            {
                respDupParam.setBrSource(RESPDupParam::BR_SOURCE_RESP);
            }
        }
            break;
        case RESPMenuContentPrivate::ITEM_CBO_WAVE_GAIN:
            respParam.setZoom(static_cast<RESPZoom>(index));
            break;
        case RESPMenuContentPrivate::ITEM_CBO_BREATH_LEAD:
            respParam.setCalcLead(static_cast<RESPLead>(index));
            break;
        default:
            break;
        }
    }
}

void RESPMenuContent::onAlarmBtnReleased()
{
    MainMenuWindow *w = MainMenuWindow::getInstance();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_RR_BR, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}
