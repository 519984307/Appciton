/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by Bingyun Chen <chenbingyun@blmed.cn>, 2018/7/5
 **/

#include "ECGMenuContent.h"
#include "LanguageManager.h"
#include <QLabel>
#include "ComboBox.h"
#include <QGridLayout>
#include <QList>
#include "ECGSymbol.h"
#include "SoundManager.h"
#include "SystemManager.h"
#include "ECGParam.h"
#include "IConfig.h"
#include "Button.h"
#include "ArrhythmiaMenuWindow.h"
#include "WindowManager.h"

class ECGMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_LEAD_MODE = 1,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_NOTCH_FITER,
        ITEM_CBO_PACER_MARK,
        ITEM_CBO_12LPACER_MARK,
        ITEM_CBO_SELF_LEARN,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_QRS_TONE,
    };

    ECGMenuContentPrivate()
        : arrhythmiaBtn(NULL)
    {}

    // load settings
    void loadOptions();

    QMap<MenuItem, ComboBox *> combos;
    Button *arrhythmiaBtn;
};

void ECGMenuContentPrivate::loadOptions()
{
    combos[ITEM_CBO_LEAD_MODE]->setCurrentIndex(ecgParam.getLeadMode());

    combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(ecgParam.getFilterMode());

    combos[ITEM_CBO_NOTCH_FITER]->setCurrentIndex(ecgParam.getNotchFilter());

    combos[ITEM_CBO_PACER_MARK]->setCurrentIndex(ecgParam.getPacermaker());

    if (combos[ITEM_CBO_12LPACER_MARK])
    {
        combos[ITEM_CBO_12LPACER_MARK]->setCurrentIndex(ecgParam.get12LPacermaker());
    }

    combos[ITEM_CBO_SELF_LEARN]->setCurrentIndex(ecgParam.getSelfLearn());

    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(ecgParam.getSweepSpeed());

    combos[ITEM_CBO_QRS_TONE]->setCurrentIndex(ecgParam.getQRSToneVolume());
}

ECGMenuContent::ECGMenuContent()
    : MenuContent(trs("ECGMenu"), trs("ECGMenuDesc")),
      d_ptr(new ECGMenuContentPrivate)
{
}

ECGMenuContent::~ECGMenuContent()
{
    delete d_ptr;
}

void ECGMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void ECGMenuContent::layoutExec()
{
    if (layout())
    {
        // already install layout
        return;
    }

    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // lead mode
    label = new QLabel(trs("ECGLeadMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_3))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_5))
                       << trs(ECGSymbol::convert(ECG_LEAD_MODE_12))
                      );
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE, comboBox);

    // filter
    label = new QLabel(trs("FilterMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_MONITOR))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_DIAGNOSTIC))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_SURGERY))
                       << trs(ECGSymbol::convert(ECG_FILTERMODE_ST))
                      );
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_FILTER_MODE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

    // notch
    label = new QLabel(trs("NotchFilter"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_NOTCH_OFF1))
                       << trs(ECGSymbol::convert(ECG_NOTCH_50HZ))
                       << trs(ECGSymbol::convert(ECG_NOTCH_60HZ))
                       << trs(ECGSymbol::convert(ECG_NOTCH_50_AND_60HZ))
                      );
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER, comboBox);

    // paceMark
    label = new QLabel(trs("ECGPaceMarker"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_PACE_OFF))
                       << trs(ECGSymbol::convert(ECG_PACE_ON))
                      );
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_PACER_MARK);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_PACER_MARK, comboBox);

    // 12 lead pace mark
    int ecg12LeadEnable = 0;
    machineConfig.getNumValue("ECG12LEADEnable", ecg12LeadEnable);
    if (ecg12LeadEnable)
    {
        label = new QLabel(trs("ECG12LPaceMarker"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << trs(ECGSymbol::convert(ECG_PACE_OFF))
                           << trs(ECGSymbol::convert(ECG_PACE_ON))
                          );
        itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_12LPACER_MARK);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_12LPACER_MARK, comboBox);
    }
    else
    {
        d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_12LPACER_MARK, NULL);
    }

    // self learn
    label = new QLabel(trs("SelfLearn"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On")
                      );
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_SELF_LEARN);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_SELF_LEARN, comboBox);

    // sweep speed
    label = new QLabel(trs("ECGSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_125))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_250))
                       << trs(ECGSymbol::convert(ECG_SWEEP_SPEED_500))
                      );
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    // qrs tone
    if (systemManager.isSupport(CONFIG_SPO2))
    {
        label = new QLabel(trs("ECGQRSPRToneVolume"));
    }
    else
    {
        label = new QLabel(trs("ECGQRSToneVolume"));
    }
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItem(trs("Off"));
    for (int i = SoundManager::VOLUME_LEV_1; i <= SoundManager::VOLUME_LEV_MAX; i++)
    {
        comboBox->addItem(QString::number(i));
    }
    itemID = static_cast<int>(ECGMenuContentPrivate::ITEM_CBO_QRS_TONE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ECGMenuContentPrivate::ITEM_CBO_QRS_TONE, comboBox);

    d_ptr->arrhythmiaBtn = new Button(trs("Arrhythmia"));
    d_ptr->arrhythmiaBtn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(d_ptr->arrhythmiaBtn, d_ptr->combos.count(), 1);
    connect(d_ptr->arrhythmiaBtn, SIGNAL(released()), this, SLOT(arrhythmiaBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void ECGMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *box = qobject_cast<ComboBox *>(sender());
    if (box)
    {
        ECGMenuContentPrivate::MenuItem item
            = (ECGMenuContentPrivate::MenuItem) box->property("Item").toInt();
        switch (item)
        {
        case ECGMenuContentPrivate::ITEM_CBO_LEAD_MODE:
            ecgParam.setLeadMode((ECGLeadMode)index);
            d_ptr->loadOptions();
            break;
        case ECGMenuContentPrivate::ITEM_CBO_FILTER_MODE:
            ecgParam.setFilterMode(index);
            break;
        case ECGMenuContentPrivate::ITEM_CBO_NOTCH_FITER:
            ecgParam.setNotchFilter(index);
            break;
        case ECGMenuContentPrivate::ITEM_CBO_PACER_MARK:
            ecgParam.setPacermaker((ECGPaceMode)index);
            break;
        case ECGMenuContentPrivate::ITEM_CBO_12LPACER_MARK:
            ecgParam.set12LPacermaker((ECGPaceMode)index);
            break;
        case ECGMenuContentPrivate::ITEM_CBO_SELF_LEARN:
            ecgParam.setSelfLearn(index);
            break;
        case ECGMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
            ecgParam.setSweepSpeed((ECGSweepSpeed)index);
            break;
        case ECGMenuContentPrivate::ITEM_CBO_QRS_TONE:
            ecgParam.setQRSToneVolume(index);
            break;
        default:
            break;
        }
    }
}

void ECGMenuContent::arrhythmiaBtnReleased()
{
    ArrhythmiaMenuWindow *instance = ArrhythmiaMenuWindow::getInstance();
    windowManager.showWindow(instance, WindowManager::WINDOW_TYPE_MODAL);
}


