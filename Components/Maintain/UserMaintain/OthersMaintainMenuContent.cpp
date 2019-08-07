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
#include "IMessageBox.h"
#include "Button.h"
#include "NurseCallSetWindow.h"
#include "ECGParam.h"
#include "CO2Param.h"
#include "RESPParam.h"
#include "O2ParamInterface.h"

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
        ITEM_CBO_CO2_WAVE_MODE,
        ITEM_CBO_RESP_WAVE_MODE,
        ITEM_CBO_APNEA_AWAKE,
        ITEM_BTN_NURSE_CALL,
    };

    OthersMaintainMenuContentPrivate()
        : nurseCallBtn(NULL)
    {
        combos.clear();
    }

    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;

    Button *nurseCallBtn;
};

void OthersMaintainMenuContentPrivate::loadOptions()
{
    QString tmpStr;
#ifndef HIDE_OTHER_MAINTAIN_ITEMS
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
#endif

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

#ifndef HIDE_OTHER_MAINTAIN_ITEMS
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
#endif

    int mode = co2Param.getSweepMode();
    combos[ITEM_CBO_CO2_WAVE_MODE]->setCurrentIndex(mode);

    mode = respParam.getSweepMode();
    combos[ITEM_CBO_RESP_WAVE_MODE]->setCurrentIndex(mode);

#ifdef ENABLE_O2_APNEASTIMULATION
    bool sta = false;
    systemConfig.getNumValue("PrimaryCfg|O2|ApneaAwake", sta);
    combos[ITEM_CBO_APNEA_AWAKE]->setCurrentIndex(sta);
#endif
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
#ifndef HIDE_OTHER_MAINTAIN_ITEMS
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
#endif
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

#ifndef HIDE_OTHER_MAINTAIN_ITEMS
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
#endif

    // CO2波形模式
    label = new QLabel(trs("CO2WaveMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("CO2Curve")
                       << trs("CO2Filled")
                       );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_CO2_WAVE_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_CO2_WAVE_MODE, comboBox);

    // resp波形模式
    label = new QLabel(trs("RESPWaveMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("RESPCurve")
                       << trs("RESPFilled")
                       );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_RESP_WAVE_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_RESP_WAVE_MODE, comboBox);

#ifdef ENABLE_O2_APNEASTIMULATION
    // 窒息唤醒
    label = new QLabel(trs("ApneaStimulation"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On")
                       );
    itemID = static_cast<int>(OthersMaintainMenuContentPrivate::ITEM_CBO_APNEA_AWAKE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(OthersMaintainMenuContentPrivate::ITEM_CBO_APNEA_AWAKE, comboBox);
#endif
#ifndef HIDE_NURSE_CALL_FUNCTION
     d_ptr->nurseCallBtn = new Button(QString("%1%2").
                                      arg(trs("NurseCallSetup")).
                                      arg(" >>"));
     d_ptr->nurseCallBtn->setButtonStyle(Button::ButtonTextOnly);
     layout->addWidget(d_ptr->nurseCallBtn, d_ptr->combos.count() + 1, 1);
     connect(d_ptr->nurseCallBtn, SIGNAL(released()), this, SLOT(onBtnReleased()));


#endif
     layout->setRowStretch(d_ptr->combos.count() + 2, 1);
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
            systemConfig.setNumValue(QString("Others|%1").arg(string), index);
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_ECG_STANDARD:
            ecgParam.updateECGStandard(index);
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_FREQUENCY_NOTCH:
            string = "FrequencyNotch";
            systemConfig.setNumValue(QString("Others|%1").arg(string), index);
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_PARAM_SWITCH_PREM:
            string = "ParaSwitchPrem";
            systemConfig.setNumValue(QString("Others|%1").arg(string), index);
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_CO2_WAVE_MODE:
            co2Param.setSweepMode(static_cast<CO2SweepMode>(index));
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_RESP_WAVE_MODE:
            respParam.setSweepMode(static_cast<RESPSweepMode>(index));
            break;
        case OthersMaintainMenuContentPrivate::ITEM_CBO_APNEA_AWAKE:
        {
            O2ParamInterface *o2Param = O2ParamInterface::getO2ParamInterface();
            if (o2Param)
            {
                o2Param->setApneaAwakeStatus(index);
            }
            break;
        }
        default:
            break;
        }
    }
}

void OthersMaintainMenuContent::onBtnReleased()
{
    NurseCallSetWindow w;
    windowManager.showWindow(&w,
                             WindowManager::ShowBehaviorNoAutoClose
                             | WindowManager::ShowBehaviorModal);
}
