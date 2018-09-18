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
#include "LanguageManager.h"
#include "MeasureSettingWindow.h"
#include "SystemManager.h"

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
    int maxWidth = 200;

    // ecg
    btn = new Button(trs("ECGSetting"));
    btn->setMinimumWidth(maxWidth);
    btn->setButtonStyle(Button::ButtonTextOnly);
    vlayout->addWidget(btn, 1, Qt::AlignRight);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    btn->setProperty("Item", qVariantFromValue(item));
    item++;

    // resp
    btn = new Button(trs("RESPSetting"));
    btn->setMinimumWidth(maxWidth);
    btn->setButtonStyle(Button::ButtonTextOnly);
    vlayout->addWidget(btn, Qt::AlignRight);
    vlayout->addWidget(btn, 1, Qt::AlignRight);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    btn->setProperty("Item", qVariantFromValue(item));
    item++;

    // temp
    btn = new Button(trs("TEMPSetting"));
    btn->setMinimumWidth(maxWidth);
    btn->setButtonStyle(Button::ButtonTextOnly);
    vlayout->addWidget(btn, 1, Qt::AlignRight);
    connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
    btn->setProperty("Item", qVariantFromValue(item));
    item++;

    // ag
    if (systemManager.isSupport(CONFIG_AG))
    {
        btn = new Button(trs("AGSetting"));
        btn->setMinimumWidth(maxWidth);
        btn->setButtonStyle(Button::ButtonTextOnly);
        vlayout->addWidget(btn, 1, Qt::AlignRight);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
    }

    // ibp
    if (systemManager.isSupport(CONFIG_IBP))
    {
        btn = new Button(trs("IBPSetting"));
        btn->setMinimumWidth(maxWidth);
        btn->setButtonStyle(Button::ButtonTextOnly);
        vlayout->addWidget(btn, 1, Qt::AlignRight);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
    }

    // co
    if (systemManager.isSupport(CONFIG_CO))
    {
        btn = new Button(trs("COSetting"));
        btn->setMinimumWidth(maxWidth);
        btn->setButtonStyle(Button::ButtonTextOnly);
        vlayout->addWidget(btn, 1, Qt::AlignRight);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
    }

    // spo2
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        btn = new Button(trs("SPO2Setting"));
        btn->setMinimumWidth(maxWidth);
        btn->setButtonStyle(Button::ButtonTextOnly);
        vlayout->addWidget(btn, 1, Qt::AlignRight);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
    }

    // nibp
    if (systemManager.isSupport(CONFIG_NIBP))
    {
        btn = new Button(trs("NIBPSetting"));
        btn->setMinimumWidth(maxWidth);
        btn->setButtonStyle(Button::ButtonTextOnly);
        vlayout->addWidget(btn, 1, Qt::AlignRight);
        connect(btn, SIGNAL(released()), this, SLOT(onBtnReleasd()));
        btn->setProperty("Item", qVariantFromValue(item));
        item++;
    }

    // CO2
    if (systemManager.isSupport(CONFIG_CO2))
    {
        btn = new Button(trs("CO2Setting"));
        btn->setMinimumWidth(maxWidth);
        btn->setButtonStyle(Button::ButtonTextOnly);
        vlayout->addWidget(btn, 1, Qt::AlignRight);
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

    switch (item)
    {
    case MeasureSettingMenuContentPrivate::ITEM_BTN_ECG:
        p->popup(trs("ECGMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_RESP:
        p->popup(trs("RESPMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_TEMP:
        p->popup(trs("TEMPMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_AG:
        p->popup(trs("AGMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_IBP:
        p->popup(trs("IBPMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_CO:
        p->popup(trs("COMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_SPO2:
        p->popup(trs("SPO2Menu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_NIBP:
        p->popup(trs("NIBPMenu"));
        break;
    case MeasureSettingMenuContentPrivate::ITEM_BTN_CO2:
        p->popup(trs("CO2Menu"));
        break;
    }
}

