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
#include "MainMenuWindow.h"
#include "Button.h"

class RESPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_SWEEP_SPEED = 1,
        ITEM_CBO_ZOOM,
        ITEM_CBO_LEAD,
        ITEM_CBO_AUTO_ACTIVATION,
    };

    RESPMenuContentPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
};

void RESPMenuContentPrivate::loadOptions()
{
    // sweep speed
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(respParam.getSweepSpeed());

    // zoom
    combos[ITEM_CBO_ZOOM]->setCurrentIndex(respParam.getZoom());

    // lead
    combos[ITEM_CBO_LEAD]->setCurrentIndex(respParam.getCalcLead());

    // activation
    combos[ITEM_CBO_AUTO_ACTIVATION]->setCurrentIndex(respParam.getRespMonitoring());
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

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // sweep speed
    label = new QLabel(trs("RESPSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_6_25))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_12_5))
                       << trs(RESPSymbol::convert(RESP_SWEEP_SPEED_25_0))
                      );
    itemID = static_cast<int>(RESPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // zoom
    label = new QLabel(trs("RESPZoom"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();

    for (int i = 0; i < RESP_ZOOM_NR; ++i)
    {
        comboBox->addItem(trs(RESPSymbol::convert(
                                  static_cast<RESPZoom>(i)
                              )));
    }
    itemID = static_cast<int>(RESPMenuContentPrivate::ITEM_CBO_ZOOM);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate::ITEM_CBO_ZOOM, comboBox);

    // Lead
    label = new QLabel(trs("RESPLead"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(RESPSymbol::convert(RESP_LEAD_I))
                       << trs(RESPSymbol::convert(RESP_LEAD_II))
                      );
    itemID = static_cast<int>(RESPMenuContentPrivate::ITEM_CBO_LEAD);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate::ITEM_CBO_LEAD, comboBox);

    // auto activation
    label = new QLabel(trs("RESPAutoActivation"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Disable")
                       << trs("Enable")
                      );
    itemID = static_cast<int>(RESPMenuContentPrivate::ITEM_CBO_AUTO_ACTIVATION);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(RESPMenuContentPrivate::ITEM_CBO_AUTO_ACTIVATION, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count() + 1, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 2, 1);
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
        case RESPMenuContentPrivate::ITEM_CBO_ZOOM:
            respParam.setZoom(static_cast<RESPZoom>(index));
            break;
        case RESPMenuContentPrivate::ITEM_CBO_LEAD:
            respParam.setCalcLead(static_cast<RESPLead>(index));
            break;
        case RESPMenuContentPrivate::ITEM_CBO_AUTO_ACTIVATION:
            respParam.setRespMonitoring(index);
            break;
        default:
            break;
        }
    }
}

void RESPMenuContent::onAlarmBtnReleased()
{
    Button *btn = qobject_cast<Button*>(sender());
    if (!btn)
    {
        return;
    }
    MainMenuWindow *w = MainMenuWindow::getInstance();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_RR_BR, true);
    w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
}
