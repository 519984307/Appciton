/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/17
 **/

#include "OthersMaintainMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include "IBPSymbol.h"
#include "IConfig.h"
#include "IBPParam.h"
#include "NumberInput.h"
#include "IMessageBox.h"
#include "Button.h"
#include "NurseCallSettingWindow.h"

class OthersMaintainMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_LINE,
        ITEM_CBO_ECG_STANDARD,
        ITEM_CBO_FREQUENCY_NOTCH,
        ITEM_CBO_PARAM_SWITCH_PREM,
        ITEM_CBO_CONFIG_SET,
    };

    OthersMaintainMenuContentPrivate()
        : NurseCallingBtn(NULL)
    {
        combos.clear();
    }

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;

    Button *NurseCallingBtn;
};

void OthersMaintainMenuContentPrivate::loadOptions()
{
    QString tmpStr;
    systemConfig.getStrValue("Others|WaveLine", tmpStr);
    if (tmpStr.toInt() >= combos[ITEM_CBO_WAVE_LINE]->count())
    {
        combos[ITEM_CBO_WAVE_LINE]->setCurrentIndex(0);
    }
    else
    {
        combos[ITEM_CBO_WAVE_LINE]->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|ECGStandard", tmpStr);
    if (tmpStr.toInt() >= combos[ITEM_CBO_ECG_STANDARD]->count())
    {
        combos[ITEM_CBO_ECG_STANDARD]->setCurrentIndex(0);
    }
    else
    {
        combos[ITEM_CBO_ECG_STANDARD]->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|FrequencyNotch", tmpStr);
    if (tmpStr.toInt() >= combos[ITEM_CBO_FREQUENCY_NOTCH]->count())
    {
        combos[ITEM_CBO_FREQUENCY_NOTCH]->setCurrentIndex(0);
    }
    else
    {
        combos[ITEM_CBO_FREQUENCY_NOTCH]->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|ParaSwitchPrem", tmpStr);
    if (tmpStr.toInt() >= combos[ITEM_CBO_PARAM_SWITCH_PREM]->count())
    {
        combos[ITEM_CBO_PARAM_SWITCH_PREM]->setCurrentIndex(0);
    }
    else
    {
        combos[ITEM_CBO_PARAM_SWITCH_PREM]->setCurrentIndex(tmpStr.toInt());
    }
    tmpStr.clear();

    systemConfig.getStrValue("Others|ConfImpactItemSettings", tmpStr);
    if (tmpStr.toInt() >= combos[ITEM_CBO_CONFIG_SET]->count())
    {
        combos[ITEM_CBO_CONFIG_SET]->setCurrentIndex(0);
    }
    else
    {
        combos[ITEM_CBO_CONFIG_SET]->setCurrentIndex(tmpStr.toInt());
    }
}

OthersMaintainMenuContent::OthersMaintainMenuContent()
    : MenuContent(trs("OthersMaintainMenu"), trs("OthersMaintainMenuDesc")),
      d_ptr(new OthersMaintainMenuContentPrivate)
{
}

OthersMaintainMenuContent::~OthersMaintainMenuContent()
{
    delete d_ptr;
}

void OthersMaintainMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void OthersMaintainMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // waveline setup
    label = new QLabel(trs("WaveLine"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Fat")
                       << trs("Med")
                       << trs("Thin")
                      );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_WAVE_LINE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_WAVE_LINE, comboBox);

    // ecg Standard
    label = new QLabel(trs("ECGStandard"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << "AAMI"
                       << "IEC"
                      );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_ECG_STANDARD);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_ECG_STANDARD, comboBox);

    // frequency Notch
    label = new QLabel(trs("FrequencyNotch"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("50 Hz")
                       << trs("60 Hz")
                      );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_FREQUENCY_NOTCH);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_FREQUENCY_NOTCH, comboBox);

    // param Switch Prem
    label = new QLabel(trs("paraSwitchPrem"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Unprotected")
                       << trs("Protected")
                      );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_PARAM_SWITCH_PREM);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_PARAM_SWITCH_PREM, comboBox);

    // config Impact Item Settings
    label = new QLabel(trs("ConfImpactItemSettings"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Close")
                       << trs("ParaSwitch")
                      );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_CONFIG_SET);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_CONFIG_SET, comboBox);

    // nurse Call Setting
    label = new QLabel(trs("NurseCallSetting"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    Button *button = new Button(trs("NurseCallSetting"));
    d_ptr->NurseCallingBtn = button;
    button->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(button, d_ptr->combos.count(), 1);
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void OthersMaintainMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        OthersMaintainMenuContentPrivate::MenuItem item
            = (OthersMaintainMenuContentPrivate::MenuItem)box->property("Item").toInt();
        QString string;
        switch (item)
        {
        case OthersMaintainMenuContentPrivate::ITEM_CBO_WAVE_LINE:
            string = "WaveLine";
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_ECG_STANDARD:
            string = "ECGStandard";
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_FREQUENCY_NOTCH:
            string = "FrequencyNotch";
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_PARAM_SWITCH_PREM:
            string = "ParaSwitchPrem";
            break;
        default:
            break;
        }
        systemConfig.setNumValue(QString("Others|%1").arg(string), index);
    }
}

void OthersMaintainMenuContent::onBtnReleased()
{
    NurseCallSettingWindow nurseCall;
    nurseCall.exec();
}
