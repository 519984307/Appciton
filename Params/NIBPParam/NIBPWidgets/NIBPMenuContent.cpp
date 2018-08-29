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

class NIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MEASURE_MODE = 0,
        ITEM_CBO_AUTO_INTERVAL = 1,
        ITEM_CBO_INITIAL_CUFF = 2,

        ITEM_BTN_START_STAT = 0,
    };

    NIBPMenuContentPrivate() {}
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
};


NIBPMenuContent::NIBPMenuContent():
    MenuContent(trs("NIBPMenu"), trs("NIBPMenuDesc")),
    d_ptr(new NIBPMenuContentPrivate)
{
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

    ComboBox *comboBox = NULL;
    QLabel *label;
    int itemID;

    // measure mode
    label = new QLabel(trs("NIBPMeasureMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(NIBPSymbol::convert(NIBP_MODE_MANUAL))
                       << trs(NIBPSymbol::convert(NIBP_MODE_AUTO))
                      );
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
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_120))
                       << trs(NIBPSymbol::convert(NIBP_AUTO_INTERVAL_NR))
                      );
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_AUTO_INTERVAL);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_AUTO_INTERVAL, comboBox);

    // initial cuff
    label = new QLabel(trs("NIBPInitialCuff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_CBO_INITIAL_CUFF);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this,
            SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(NIBPMenuContentPrivate::ITEM_CBO_INITIAL_CUFF, comboBox);

    Button *button;
    int row = d_ptr->combos.count();

    // start stat
    label = new QLabel(trs("Stat"));
    layout->addWidget(label, row + d_ptr->btns.count(), 0);
    button = new Button();
    button->setText(trs("StatStart"));
    button->setButtonStyle(Button::ButtonTextOnly);
    itemID = static_cast<int>(NIBPMenuContentPrivate::ITEM_BTN_START_STAT);
    button->setProperty("Btn", qVariantFromValue(itemID));
    connect(button, SIGNAL(released()), this,
            SLOT(onBtnReleasedChanged()));
    layout->addWidget(button, row + d_ptr->btns.count(), 1);
    d_ptr->btns.insert(NIBPMenuContentPrivate::ITEM_BTN_START_STAT, button);

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

    int unit = UNIT_MMHG;
    currentConfig.getNumValue("Local|NIBPUnit", unit);
    combos[ITEM_CBO_INITIAL_CUFF]->blockSignals(true);
    combos[ITEM_CBO_INITIAL_CUFF]->clear();
    PatientType type = patientManager.getType();
    if (type == PATIENT_TYPE_ADULT)
    {
        for (unsigned i = 0; i < NIBP_ADULT_INITIAL_CUFF_NR; ++i)
        {
            QString str = NIBPSymbol::convert((NIBPAdultInitialCuff)i);
            if (unit == UNIT_KPA)
            {
                str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
            }
            str = str + " " + Unit::getSymbol((UnitType)unit);
            combos[ITEM_CBO_INITIAL_CUFF]->addItem(str);
        }
        currentConfig.getNumValue("NIBP|AdultInitialCuffInflation", index);
        combos[ITEM_CBO_INITIAL_CUFF]->setCurrentIndex(index);
    }
    else if (type == PATIENT_TYPE_PED)
    {
        for (unsigned i = 0; i < NIBP_PREDIATRIC_INITIAL_CUFF_NR; ++i)
        {
            QString str = NIBPSymbol::convert((NIBPPrediatrictInitialCuff)i);
            if (unit == UNIT_KPA)
            {
                str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
            }
            str = str + " " + Unit::getSymbol((UnitType)unit);
            combos[ITEM_CBO_INITIAL_CUFF]->addItem(str);
        }
        currentConfig.getNumValue("NIBP|PedInitialCuffInflation", index);
        combos[ITEM_CBO_INITIAL_CUFF]->setCurrentIndex(index);
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        for (unsigned i = 0; i < NIBP_NEONATAL_INITIAL_CUFF_NR; ++i)
        {
            QString str = NIBPSymbol::convert((NIBPNeonatalInitialCuff)i);
            if (unit == UNIT_KPA)
            {
                str = Unit::convert((UnitType)unit, UNIT_MMHG, str.toInt());
            }
            str = str + " " + Unit::getSymbol((UnitType)unit);
            combos[ITEM_CBO_INITIAL_CUFF]->addItem(str);
        }
        currentConfig.getNumValue("NIBP|NeoInitialCuffInflation", index);
        combos[ITEM_CBO_INITIAL_CUFF]->setCurrentIndex(index);
    }
    combos[ITEM_CBO_INITIAL_CUFF]->blockSignals(false);

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
}

void NIBPMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_NIBP_SYS, true);
    if (!w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void NIBPMenuContent::onComboBoxIndexChanged(int index)
{
    static int i = 0;
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
    case NIBPMenuContentPrivate::ITEM_CBO_INITIAL_CUFF:
        nibpParam.setInitPressure(index);
        i++;
        qDebug() << "NIBPMenuContent::onComboBoxIndexChanged(int index)----" << i << "---" << index << endl;
        break;
    }
}
