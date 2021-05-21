/**
 ** This file is part of the nPM project.
 ** Copyright(C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/9/18
 **/

#include "MeasureSettingMenuContent.h"
#include "Framework/UI/Button.h"
#include "Framework/Language/LanguageManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "MeasureSettingWindow.h"
#include "SystemManager.h"
#include "WindowManager.h"
#include "PatientManager.h"
#include "IConfig.h"

class MeasureSettingMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_BTN_ECG = 0,
        ITEM_BTN_RESP,
        ITEM_BTN_TEMP,
        ITEM_BTN_AG,
        ITEM_BTN_IBP,
        ITEM_BTN_CO,
        ITEM_BTN_SPO2,
        ITEM_BTN_NIBP,
        ITEM_BTN_CO2,
        ITEM_BTN_O2,
        ITEM_BTN_APNEA_STIMULATION,
    };

    MeasureSettingMenuContentPrivate()
        : apneaStimulationBtn(NULL)
    {}

    void loadOptions();

    Button *apneaStimulationBtn;
};



MeasureSettingMenuContent::MeasureSettingMenuContent()
                         : MenuContent(trs("MeasureSettingMenu"),
                                       trs("MeasureSettingMenuDesc")),
                           d_ptr(new MeasureSettingMenuContentPrivate)
{
}


MeasureSettingMenuContent::~MeasureSettingMenuContent()
{
    delete d_ptr;
}

void MeasureSettingMenuContent::layoutExec()
{
    if (layout())
    {
        return;
    }

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->setMargin(10);

    Button *btn;
    int item = 0;
    QHBoxLayout *hl;

    // ecg
    btn = new Button(QString("%1 >>").arg(trs("ECGSetting")));
    hl = new QHBoxLayout;
    hl->addStretch(1);
    hl->addWidget(btn, 1);
    vlayout->addLayout(hl);
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    item = MeasureSettingMenuContentPrivate::ITEM_BTN_ECG;
    btn->setProperty("Item", qVariantFromValue(item));

    // resp
    if (systemManager.isSupport(CONFIG_RESP))
    {
    btn = new Button(QString("%1 >>").arg(trs("RESPSetting")));
    hl = new QHBoxLayout;
    hl->addStretch(1);
    hl->addWidget(btn, 1);
    vlayout->addLayout(hl);
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    item = MeasureSettingMenuContentPrivate::ITEM_BTN_RESP;
    btn->setProperty("Item", qVariantFromValue(item));
    }

    // temp
    if (systemManager.isSupport(CONFIG_TEMP))
    {
    btn = new Button(QString("%1 >>").arg(trs("TEMPSetting")));
    hl = new QHBoxLayout;
    hl->addStretch(1);
    hl->addWidget(btn, 1);
    vlayout->addLayout(hl);
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    item = MeasureSettingMenuContentPrivate::ITEM_BTN_TEMP;
    btn->setProperty("Item", qVariantFromValue(item));
    }

    // ag
    if (systemManager.isSupport(CONFIG_AG))
    {
        btn = new Button(QString("%1 >>").arg(trs("AGSetting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_AG;
        btn->setProperty("Item", qVariantFromValue(item));
    }

    // ibp
    if (systemManager.isSupport(CONFIG_IBP))
    {
        btn = new Button(QString("%1 >>").arg(trs("IBPSetting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_IBP;
        btn->setProperty("Item", qVariantFromValue(item));
    }

    // co
    if (systemManager.isSupport(CONFIG_CO))
    {
        btn = new Button(QString("%1 >>").arg(trs("COSetting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_CO;
        btn->setProperty("Item", qVariantFromValue(item));
    }

    // spo2
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        btn = new Button(QString("%1 >>").arg(trs("SPO2Setting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_SPO2;
        btn->setProperty("Item", qVariantFromValue(item));
    }

    // nibp
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        btn = new Button(QString("%1 >>").arg(trs("NIBPSetting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_NIBP;
        btn->setProperty("Item", qVariantFromValue(item));
    }

    // CO2
    if (systemManager.isSupport(CONFIG_CO2))
    {
        btn = new Button(QString("%1 >>").arg(trs("CO2Setting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_CO2;
        btn->setProperty("Item", qVariantFromValue(item));
    }

#ifdef ENABLE_O2_APNEASTIMULATION
    // O2
    if (systemManager.isSupport(CONFIG_O2))
    {
        btn = new Button(QString("%1 >>").arg(trs("O2Setting")));
        hl = new QHBoxLayout;
        hl->addStretch(1);
        hl->addWidget(btn, 1);
        vlayout->addLayout(hl);
        btn->setButtonStyle(Button::ButtonTextOnly);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        item = MeasureSettingMenuContentPrivate::ITEM_BTN_O2;
        btn->setProperty("Item", qVariantFromValue(item));
    }

    // DAVID要求不打开O2参数模块，也可以支持新生儿窒息唤醒功能
    btn = new Button(QString("%1 >>").arg(trs("ApneaStimulationSetup")));
    hl = new QHBoxLayout;
    hl->addStretch(1);
    hl->addWidget(btn, 1);
    vlayout->addLayout(hl);
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    item = MeasureSettingMenuContentPrivate::ITEM_BTN_APNEA_STIMULATION;
    btn->setProperty("Item", qVariantFromValue(item));
    d_ptr->apneaStimulationBtn = btn;
#endif
    vlayout->addStretch();
}

void MeasureSettingMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void MeasureSettingMenuContent::onBtnReleasd()
{
    Button *btn = qobject_cast<Button*>(sender());
    int item = btn->property("Item").toInt();
    MeasureSettingWindow *p = MeasureSettingWindow::getInstance();

    if (item < 0)
    {
        return;
    }
    QString strName = "";
    switch (item)
    {
    case MeasureSettingMenuContentPrivate::ITEM_BTN_ECG:
        strName = trs("ECGMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_RESP:
        strName = trs("RESPMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_TEMP:
        strName = trs("TEMPMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_AG:
        strName = trs("AGMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_IBP:
        strName = trs("IBPMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_CO:
        strName = trs("COMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_SPO2:
        strName = trs("SPO2Menu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_NIBP:
        strName = trs("NIBPMenu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_CO2:
        strName = trs("CO2Menu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_O2:
        strName = trs("O2Menu");
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_APNEA_STIMULATION:
        strName = trs("ApneaStimulationMenu");
        break;
    }

    p->popup(strName, QVariant(), WindowManager::ShowBehaviorHideOthers);
}


void MeasureSettingMenuContentPrivate::loadOptions()
{
#ifdef ENABLE_O2_APNEASTIMULATION
    // DAVID要求不打开O2参数模块，也可以支持新生儿窒息唤醒功能
    if (patientManager.getType() == PATIENT_TYPE_NEO)
    {
        apneaStimulationBtn->setVisible(true);
    }
    else
    {
        apneaStimulationBtn->setVisible(false);
    }
#endif
}
