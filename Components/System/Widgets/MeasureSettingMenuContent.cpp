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
#include "Button.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "LanguageManager.h"
#include "MeasureSettingWindow.h"
#include "SystemManager.h"
#include "WindowManager.h"

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
    };
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
    btn->setProperty("Item", qVariantFromValue(item));
    item++;

    // resp
    btn = new Button(QString("%1 >>").arg(trs("RESPSetting")));
    hl = new QHBoxLayout;
    hl->addStretch(1);
    hl->addWidget(btn, 1);
    vlayout->addLayout(hl);
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    btn->setProperty("Item", qVariantFromValue(item));
    item++;

    // temp
    btn = new Button(QString("%1 >>").arg(trs("TEMPSetting")));
    hl = new QHBoxLayout;
    hl->addStretch(1);
    hl->addWidget(btn, 1);
    vlayout->addLayout(hl);
    btn->setButtonStyle(Button::ButtonTextOnly);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    btn->setProperty("Item", qVariantFromValue(item));
    item++;

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
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
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
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
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
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
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
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
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
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
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
        btn->setProperty("Item", qVariantFromValue(item));
    }
    vlayout->addStretch();
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
        p->popup(trs("NIBPMenu"));
        break;
    }

    p->popup(strName, QVariant(), WindowManager::ShowBehaviorHideOthers);
}

