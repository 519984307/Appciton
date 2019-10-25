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
#include "AlarmLimitWindow.h"

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

    /**
     * @brief blockItemSignal 阻塞信号
     * @param flag
     */
    void blockItemSignal(bool flag);
};

void RESPMenuContentPrivate::loadOptions()
{
    // sweep speed
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(respParam.getSweepSpeed());

    // apnea delay
    combos[ITEM_CBO_APNEA_DELAY]->setCurrentIndex(respParam.getApneaTime());

    // rr source
    combos[ITEM_CBO_RR_SOURCE]->clear();
    if (co2Param.isConnected())
    {
        combos[ITEM_CBO_RR_SOURCE]->addItems(QStringList()
                                             << trs(RESPSymbol::convert(BR_RR_AUTO))
                                             << trs(RESPSymbol::convert(BR_RR_SOURCE_ECG))
                                             << trs(RESPSymbol::convert(BR_RR_SOURCE_CO2)));
    }
    else
    {
        combos[ITEM_CBO_RR_SOURCE]->addItems(QStringList()
                                             << trs(RESPSymbol::convert(BR_RR_AUTO))
                                             << trs(RESPSymbol::convert(BR_RR_SOURCE_ECG)));
    }

    int index = 0;
    currentConfig.getNumValue("RESP|BrSource", index);
    if (index == BR_RR_SOURCE_CO2 && co2Param.isConnected() == false)
    {
        index = 0;
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
    if (systemManager.getCurWorkMode() == WORK_MODE_DEMO)
    {
        // demo模式不允许设置增益
        combos[ITEM_CBO_WAVE_GAIN]->setEnabled(false);
    }
    else
    {
        combos[ITEM_CBO_WAVE_GAIN]->setEnabled(true);
    }

    combos[ITEM_CBO_WAVE_GAIN]->setCurrentIndex(respParam.getZoom());
}

void RESPMenuContentPrivate::blockItemSignal(bool flag)
{
    for (int i = ITEM_CBO_APNEA_DELAY; i <= ITEM_CBO_SWEEP_SPEED; ++i)
    {
        MenuItem index = static_cast<MenuItem>(i);
        combos[index]->blockSignals(flag);
    }
}

RESPMenuContent::RESPMenuContent()
    : MenuContent(trs("RESPMenu"), trs("RESPMenuDesc")),
      d_ptr(new RESPMenuContentPrivate())
{
    connect(&co2Param, SIGNAL(connectStatusUpdated(bool)), this, SLOT(updateBRSource()));
}

RESPMenuContent::~RESPMenuContent()
{
    delete d_ptr;
}

void RESPMenuContent::readyShow()
{
    d_ptr->blockItemSignal(true);
    d_ptr->loadOptions();
    d_ptr->blockItemSignal(false);
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
                       << trs(RESPSymbol::convert(RESP_LEAD_I))
                       << trs(RESPSymbol::convert(RESP_LEAD_AUTO)));
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
                       << trs(RESPSymbol::convert(RESP_ZOOM_X025))
                       << trs(RESPSymbol::convert(RESP_ZOOM_X050))
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
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_25_0))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_50_0)));
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
            currentConfig.setNumValue("RESP|ApneaDelay", index);
            respParam.setApneaTime(static_cast<ApneaAlarmTime>(index));
            // 联动CO2的窒息时间
            currentConfig.setNumValue("CO2|ApneaTime", index);
            co2Param.setApneaTime(static_cast<ApneaAlarmTime>(index));
            break;
        case RESPMenuContentPrivate::ITEM_CBO_RR_SOURCE:
            respDupParam.setRRSource(static_cast<BRRRSourceType>(index));
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
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_RR_BR, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}

void RESPMenuContent::updateBRSource()
{
    d_ptr->blockItemSignal(true);
    d_ptr->loadOptions();
    d_ptr->blockItemSignal(false);
}
