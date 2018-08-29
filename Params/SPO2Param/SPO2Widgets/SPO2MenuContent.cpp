/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/11
 **/

#include "SPO2MenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "SPO2Symbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "SPO2Param.h"
#include "IConfig.h"
#include "MainMenuWindow.h"
#include "Button.h"

class SPO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_SMART_TONE
    };

    SPO2MenuContentPrivate() {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
};

void SPO2MenuContentPrivate::loadOptions()
{
    combos[ITEM_CBO_SMART_TONE]->setCurrentIndex(spo2Param.getSmartPulseTone());
}

SPO2MenuContent::SPO2MenuContent()
    : MenuContent(trs("SPO2Menu"), trs("SPO2MenuDesc")),
      d_ptr(new SPO2MenuContentPrivate)
{
}

SPO2MenuContent::~SPO2MenuContent()
{
    delete d_ptr;
}

void SPO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void SPO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    label = new QLabel(trs("SPO2SmartPulseTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_ON))
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_OFF))
                      );
    itemID = static_cast<int>(SPO2MenuContentPrivate::ITEM_CBO_SMART_TONE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(SPO2MenuContentPrivate::ITEM_CBO_SMART_TONE, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void SPO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        SPO2MenuContentPrivate::MenuItem item
            = (SPO2MenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case SPO2MenuContentPrivate::ITEM_CBO_SMART_TONE:
            spo2Param.setSmartPulseTone((SPO2SMARTPLUSETONE)index);
            break;
        default:
            break;
        }
    }
}

void SPO2MenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_SPO2, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}
