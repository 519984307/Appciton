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
#include "UnitManager.h"
#include "SystemManager.h"
#include "SoundManager.h"
#include "NightModeManager.h"
#include "PatientManager.h"
#include "NIBPCountdownTime.h"

class NIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MEASURE_MODE = 0,
        ITEM_CBO_AUTO_INTERVAL = 1,
        ITEM_CBO_INITIAL_CUFF = 2,
        ITEM_CBO_COMPLETE_TONE = 3,
        ITEM_CBO_ADDITION_MEASURE = 4,
        ITEM_BTN_START_STAT = 0
    };

    NIBPMenuContentPrivate()
        : initCuffSpb(NULL)
        , initCuffUnitLbl(NULL)
        , curUnitType(UNIT_NONE)
        , lastType(PATIENT_TYPE_ADULT)
    {}
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
    QLabel *initCuffUnitLbl;
    QStringList initCuffStrs;
    UnitType curUnitType;
    PatientType lastType;
    QString moduleStr;
};


NIBPMenuContent::NIBPMenuContent():
    MenuContent(trs("NIBPMenu"), trs("NIBPMenuDesc")),
    d_ptr(new NIBPMenuContentPrivate)
{
    d_ptr->lastType = patientManager.getType();
    connect(&nibpParam, SIGNAL(statBtnState(bool)), this, SLOT(onStatBtnStateChanged(bool)));
    machineConfig.getStrValue("NIBP", d_ptr->moduleStr);
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
    Button *button;

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
                       << trs("On"));
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_COMPLETE_TONE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(itemFoucsIndexChanged(int)), this, SLOT(onPopupListItemFocusChanged(int)));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_COMPLETE_TONE, comboBox);

    // initial cuff
    QString name = trs("NIBPInitialPressure") + "(" + Unit::getSymbol(UNIT_MMHG) + ")";
    label = new QLabel(name);
    d_ptr->initCuffUnitLbl = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->initCuffSpb = new SpinBox();
    d_ptr->initCuffSpb->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
    connect(d_ptr->initCuffSpb, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxReleased(int)));
    layout->addWidget(d_ptr->initCuffSpb, d_ptr->combos.count(), 1);

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
    layout->addWidget(label, d_ptr->combos.count() + 2, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_ADDITION_MEASURE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count() + 2, 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_ADDITION_MEASURE, comboBox);

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, row + d_ptr->btns.count() + 1, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch((row + d_ptr->btns.count() + 1), 1);
}

void NIBPMenuContentPrivate::loadOptions()
{
    combos[ITEM_CBO_MEASURE_MODE]->blockSignals(true);
    int index = nibpParam.getSuperMeasurMode();
    combos[ITEM_CBO_MEASURE_MODE]->setCurrentIndex(index);
    combos[ITEM_CBO_MEASURE_MODE]->blockSignals(false);
    // 时间
    combos[ITEM_CBO_AUTO_INTERVAL]->setCurrentIndex(nibpParam.getAutoInterval());

    PatientType type = patientManager.getType();
    if (curUnitType != nibpParam.getUnit() || lastType != type)
    {
        // 判断是否需要重新加载字符串
        int start = 0, end = 0;
        if (type == PATIENT_TYPE_ADULT)
        {
            start = 80;
            end = 240;
        }
        else if (type == PATIENT_TYPE_PED)
        {
            start = 80;
            end = 200;
        }
        else if (type == PATIENT_TYPE_NEO)
        {
            start = 60;
            end = 140;
        }
        UnitType unit = nibpParam.getUnit();
        curUnitType = unit;
        UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
        initCuffStrs.clear();
        for (int i = start; i <= end; i += 10)
        {
            if (unit == defUnit)
            {
                // 当前单位为预定的单位时（mmgh）
                initCuffStrs.append(QString::number(i));
            }
            else
            {
                initCuffStrs.append(Unit::convert(unit, defUnit, i));
            }
        }
        initCuffSpb->setStringList(initCuffStrs);
        if (unit == defUnit)
        {
            initCuffUnitLbl->setText(trs("NIBPInitialPressure") + "(" + Unit::getSymbol(UNIT_MMHG) + ")");
        }
        else
        {
            initCuffUnitLbl->setText(trs("NIBPInitialPressure") + "(" + Unit::getSymbol(UNIT_KPA) + ")");
        }
    }

    int initVal = 0;
    if (moduleStr == "BLM_N5")
    {
        currentConfig.getNumValue("NIBP|InitialCuffInflation", initVal);
    }
    else
    {
        currentConfig.getNumValue("NIBP|SUNTECHInitialCuffInflation", initVal);
    }
    initCuffSpb->setValue(initVal);

    lastType = type;

    systemConfig.getNumValue("PrimaryCfg|NIBP|AutomaticRetry", index);
    combos[ITEM_CBO_ADDITION_MEASURE]->setCurrentIndex(index);

    if (nightModeManager.nightMode())
    {
        combos[ITEM_CBO_COMPLETE_TONE]->setEnabled(false);
    }
    else
    {
        systemConfig.getNumValue("PrimaryCfg|NIBP|CompleteTone", index);
        combos[ITEM_CBO_COMPLETE_TONE]->setCurrentIndex(index);
        combos[ITEM_CBO_COMPLETE_TONE]->setEnabled(true);
    }
    statBtnShow();
}

void NIBPMenuContentPrivate::statBtnShow(void)
{
    // demo 模式下失能STAT
    WorkMode workMode = systemManager.getCurWorkMode();
    if (nibpParam.isConnected() && workMode != WORK_MODE_DEMO)
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
            if (nibpParam.curStatusType() == NIBP_MONITOR_ERROR_STATE ||
                    systemManager.getCurWorkMode() == WORK_MODE_DEMO || nibpParam.isZeroSelfTestState())
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

void NIBPMenuContent::onSpinBoxReleased(int value)
{
    if (d_ptr->moduleStr == "BLM_N5")
    {
        currentConfig.setNumValue("NIBP|InitialCuffInflation", value);
    }
    else
    {
        currentConfig.setNumValue("NIBP|SUNTECHInitialCuffInflation", value);
    }
    UnitType curUnit = nibpParam.getUnit();
    UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
    if (curUnit != defUnit)
    {
        // 若单位不是mmgh时，要换算后才设置。
        value = Unit::convert(defUnit, curUnit, d_ptr->initCuffStrs.at(value).toFloat()).toInt();
    }
    else
    {
        value = d_ptr->initCuffStrs.at(value).toInt();
    }
    nibpParam.setInitPressure(value);
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

void NIBPMenuContent::onPopupListItemFocusChanged(int volume)
{
    if (volume)
    {
        soundManager.setNIBPCompleteTone(volume);
        soundManager.nibpCompleteTone();
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
        soundManager.setNIBPCompleteTone(index);
        break;
    case NIBPMenuContentPrivate::ITEM_CBO_ADDITION_MEASURE:
        systemConfig.setNumValue("PrimaryCfg|NIBP|AutomaticRetry", index);
        break;
    default:
        break;
    }
}
