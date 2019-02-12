/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by luoyuchun <luoyuchun@blmed.cn>, 2018/7/13
 **/


#include "NIBPMenuContent.h"
#include <QMap>
#include "ComboBox.h"
#include "Button.h"
#include <QGridLayout>
#include <QLabel>
#include "NIBPSymbol.h"
#include "LanguageManager.h"
#include "NIBPParam.h"
#include "ConfigManager.h"
#include "NIBPMonitorStateDefine.h"
#include "MainMenuWindow.h"
#include "AlarmLimitWindow.h"
#include "SpinBox.h"
#include "IConfig.h"

class NIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MEASURE_MODE = 0,
        ITEM_CBO_AUTO_INTERVAL = 1,
        ITEM_CBO_INITIAL_CUFF = 2,
        ITEM_CBO_COMPLETE_TONE = 3,

        ITEM_BTN_START_STAT = 0,
        ITEM_BTN_ADDITION_MEASURE = 1
    };

    NIBPMenuContentPrivate() : initCuffSpb(NULL) {}
    /**
     * @brief loadOptions  //load settings
     */
    void loadOptions(void);
    /**
     * @brief statBtnShow
     */
    void statBtnShow(void);

    QMap<MenuItem, ComboBox *> combos;
    QMap<MenuItem, Button *> btns;
    SpinBox *initCuffSpb;
};


NIBPMenuContent::NIBPMenuContent():
    MenuContent(trs("NIBPMenu"), trs("NIBPMenuDesc")),
    d_ptr(new NIBPMenuContentPrivate)
{
    connect(&nibpParam, SIGNAL(statBtnState(bool)), this, SLOT(onStatBtnStateChanged(bool)));
}

NIBPMenuContent::~NIBPMenuContent()
{
    delete d_ptr;
}

void NIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
}

void NIBPMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    QLabel *label;
    int itemID;

    // measure mode
    label = new QLabel(trs("NIBPMeasureMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    ComboBox *comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(NIBPSymbol::convert(NIBP_MODE_MANUAL))
                       << trs(NIBPSymbol::convert(NIBP_MODE_AUTO)));
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_MEASURE_MODE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_MEASURE_MODE, comboBox);

    // auto interval
    label = new QLabel(trs("NIBPAutoInterval"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_2_5))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_5))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_10))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_15))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_20))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_30))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_45))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_60))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_90))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_120)));
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_AUTO_INTERVAL);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_AUTO_INTERVAL, comboBox);

    // complete tone
    label = new QLabel(trs("NIBPCompleteTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << QString::number(SoundManager::VOLUME_LEV_1)
                       << QString::number(SoundManager::VOLUME_LEV_2)
                       << QString::number(SoundManager::VOLUME_LEV_3)
                       << QString::number(SoundManager::VOLUME_LEV_4)
                       << QString::number(SoundManager::VOLUME_LEV_5));
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_COMPLETE_TONE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    connect(comboBox, SIGNAL(itemFocusChanged(int)), this, SLOT(onCboItemFocusChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_COMPLETE_TONE, comboBox);

    // initial cuff
    label = new QLabel(trs("NIBPInitialCuff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->initCuffSpb = new SpinBox();
    d_ptr->initCuffSpb->setStep(10);
    connect(d_ptr->initCuffSpb, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxReleased(int, int)));
    QHBoxLayout *hLayout = new QHBoxLayout();
    label = new QLabel("mmHg");
    hLayout->addWidget(d_ptr->initCuffSpb);
    hLayout->addWidget(label);
    layout->addLayout(hLayout, d_ptr->combos.count(), 1);


    Button *button;
    int row = d_ptr->combos.count() + 1;

    // start stat
    label = new QLabel(trs("STAT"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button();
    button->setText(trs("STATSTART"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_BTN_START_STAT);
    button->setProperty("Btn", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this,
            SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    d_ptr->btns.insert(NIBPMenuContentPrivate::ITEM_BTN_START_STAT, button);

    // nibp auto addition measure
    label = new QLabel(trs("NIBPAdditionMeasure"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button();
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_BTN_ADDITION_MEASURE);
    button->setProperty("Btn", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this, SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    d_ptr->btns.insert(NIBPMenuContentPrivate::ITEM_BTN_ADDITION_MEASURE, button);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, row + d_ptr->btns.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch((row + d_ptr->btns.count() + 1), 1);
}

void NIBPMenuContentPrivate::loadOptions()
{
    int index = nibpParam.getSuperMeasurMode();
    combos[ITEM_CBO_MEASURE_MODE]->setCurrentIndex(index);
    // 时间
    combos[ITEM_CBO_AUTO_INTERVAL]->setCurrentIndex(nibpParam.getAutoInterval());

    PatientType type = patientManager.getType();
    int initVal;
    if (type == PATIENT_TYPE_ADULT)
    {
        initCuffSpb->setRange(120, 280);
    }
    else if (type == PATIENT_TYPE_PED)
    {
        initCuffSpb->setRange(80, 250);
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        initCuffSpb->setRange(60, 140);
    }
    currentConfig.getNumValue("NIBP|InitialCuffInflation", initVal);
    initCuffSpb->setValue(initVal);

    systemConfig.getNumValue("PrimaryCfg|NIBP|AutomaticRetry", index);
    if (index)
    {
        btns[ITEM_BTN_ADDITION_MEASURE]->setText(trs("On"));
    }
    else
    {
        btns[ITEM_BTN_ADDITION_MEASURE]->setText(trs("Off"));
    }

    systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", index);
    combos[ITEM_CBO_COMPLETE_TONE]->setCurrentIndex(index);
    statBtnShow();
}

void NIBPMenuContentPrivate::statBtnShow(void)
{
    if (nibpParam.isConnected())
    {
        btns[ITEM_BTN_START_STAT]->setEnabled(true);
    }
    else
    {
        btns[ITEM_BTN_START_STAT]->setEnabled(false);
    }

    if (nibpParam.isSTATMeasure())
    {
        btns[ITEM_BTN_START_STAT]->setText(trs("STATSTOP"));
    }
    else
    {
        btns[ITEM_BTN_START_STAT]->setText(trs("STATSTART"));
    }
}


void NIBPMenuContent::onBtnReleasedChanged()
{
    Button *btns = qobject_cast<Button *>(sender());
    int index = btns->property("Btn").toInt();
    switch (index)
    {
        case NIBPMenuContentPrivate::ITEM_BTN_START_STAT:
        {
            if (nibpParam.curStatusType() == NIBP_MONITOR_ERROR_STATE)
            {
                return;
            }
            // 退出STAT模式
            if (nibpParam.isSTATMeasure())
            {
                btns->setText(trs("STATSTART"));
                nibpCountdownTime.STATMeasureStop();
            }
            // 进入STAT模式
            else
            {
                btns->setText(trs("STATSTOP"));
            }
            nibpParam.setMeasurMode(NIBP_MODE_STAT);
            break;
        }
        case NIBPMenuContentPrivate::ITEM_BTN_ADDITION_MEASURE:
        {
            bool flag = nibpParam.isAdditionalMeasure();
            if (flag)
            {
                btns->setText(trs("Off"));
            }
            else
            {
                btns->setText(trs("On"));
            }
            nibpParam.setAdditionalMeasure(!flag);
            systemConfig.setNumValue("PrimaryCfg|NIBP|AutomaticRetry", static_cast<int>(!flag));
            break;
        }
        default:
            break;
    }
}

void NIBPMenuContent::onAlarmBtnReleased()
{
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_NIBP_SYS, true);
    AlarmLimitWindow w(subParamName);
    windowManager.showWindow(&w, WindowManager::ShowBehaviorModal);
}

void NIBPMenuContent::onSpinBoxReleased(int value, int scale)
{
    nibpParam.setInitPressure(value * scale);
    currentConfig.setNumValue("NIBP|InitialCuffInflation", value);
}

void NIBPMenuContent::onCboItemFocusChanged(int index)
{
    ComboBox *cbo = qobject_cast<ComboBox *>(sender());
    int indexType = cbo->property("Item").toInt();
    if (indexType == NIBPMenuContentPrivate::ITEM_CBO_COMPLETE_TONE)
    {
        SoundManager::VolumeLevel volume = static_cast<SoundManager::VolumeLevel>(index);
        nibpParam.setNIBPCompleteTone(volume);
        soundManager.nibpCompleteTone();
    }
}

void NIBPMenuContent::onStatBtnStateChanged(bool flag)
{
    if (!flag)
    {
        d_ptr->btns[NIBPMenuContentPrivate::ITEM_BTN_START_STAT]->setText(trs("STATSTART"));
    }
    else
    {
        d_ptr->btns[NIBPMenuContentPrivate::ITEM_BTN_START_STAT]->setText(trs("STATSTOP"));
    }
}

void NIBPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combos = qobject_cast<ComboBox *>(sender());
    int indexType = combos->property("Item").toInt();
    switch (indexType)
    {
    case NIBPMenuContentPrivate::ITEM_CBO_MEASURE_MODE:
        nibpParam.setMeasurMode((NIBPMode)index);
        break;
    case NIBPMenuContentPrivate::ITEM_CBO_AUTO_INTERVAL:
        nibpParam.setAutoInterval((NIBPAutoInterval)index);
        break;
    case NIBPMenuContentPrivate::ITEM_CBO_COMPLETE_TONE:
        systemConfig.setNumValue("PrimaryCfg|NIBP|CompleteTone", index);
    default:
        break;
    }
}
