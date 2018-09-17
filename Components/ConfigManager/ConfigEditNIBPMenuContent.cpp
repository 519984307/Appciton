/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/17
 **/
#include "ConfigEditNIBPMenuContent.h"
#include <QMap>
#include <QLabel>
#include "LanguageManager.h"
#include "ComboBox.h"
#include "NIBPSymbol.h"
#include "NIBPDefine.h"
#include <QGridLayout>
#include "PatientManager.h"
#include "Button.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "ConfigManager.h"

class ConfigEditNIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MEASURE_MODE = 0,
        ITEM_CBO_INTERVAL_TIME,
        ITEM_CBO_INIT_CUFF,
        ITEM_CBO_MAX
    };

    explicit ConfigEditNIBPMenuContentPrivate(Config *const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();
    /**
     * @brief setInitPressure
     * @param index
     */
    void setInitPressure(int index);

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
};

ConfigEditNIBPMenuContentPrivate
    ::ConfigEditNIBPMenuContentPrivate(Config *const config)
    :config(config)
{
    combos.clear();
}

ConfigEditNIBPMenuContent::ConfigEditNIBPMenuContent(Config *const config):
    MenuContent(trs("NIBPMenu"),
                trs("NIBPMenuDesc")),
    d_ptr(new ConfigEditNIBPMenuContentPrivate(config))
{
}

ConfigEditNIBPMenuContent::~ConfigEditNIBPMenuContent()
{
    delete d_ptr;
}

void ConfigEditNIBPMenuContentPrivate::loadOptions()
{
    int index;
    index = 0;
    config->getNumValue("NIBP|NIBPMeasureMode", index);
    combos[ITEM_CBO_MEASURE_MODE]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("NIBP|IntervalTime", index);
    combos[ITEM_CBO_INTERVAL_TIME]->setCurrentIndex(index);

    int unit = UNIT_MMHG;
    config->getNumValue("Local|NIBPUnit", unit);
    combos[ITEM_CBO_INIT_CUFF]->blockSignals(true);
    combos[ITEM_CBO_INIT_CUFF]->clear();
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
            combos[ITEM_CBO_INIT_CUFF]->addItem(str);
        }
        config->getNumValue("NIBP|AdultInitialCuffInflation", index);
        combos[ITEM_CBO_INIT_CUFF]->setCurrentIndex(index);
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
            combos[ITEM_CBO_INIT_CUFF]->addItem(str);
        }
        config->getNumValue("NIBP|PedInitialCuffInflation", index);
        combos[ITEM_CBO_INIT_CUFF]->setCurrentIndex(index);
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
            combos[ITEM_CBO_INIT_CUFF]->addItem(str);
        }
        config->getNumValue("NIBP|NeoInitialCuffInflation", index);
        combos[ITEM_CBO_INIT_CUFF]->setCurrentIndex(index);
    }
    combos[ITEM_CBO_INIT_CUFF]->blockSignals(false);
}

void ConfigEditNIBPMenuContentPrivate::setInitPressure(int index)
{
    PatientType type = patientManager.getType();
    QString path;
    if (type == PATIENT_TYPE_ADULT)
    {
        path = "NIBP|AdultInitialCuffInflation";
    }
    else if (type == PATIENT_TYPE_PED)
    {
        path = "NIBP|PedInitialCuffInflation";
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        path = "NIBP|NeoInitialCuffInflation";
    }

    config->setNumValue(path, index);
}

void ConfigEditNIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToShow = !configManager.isOnlyShown();

    for (int i = 0; i < ConfigEditNIBPMenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditNIBPMenuContentPrivate
                ::MenuItem(i)]->setEnabled(isOnlyToShow);
    }
}

void ConfigEditNIBPMenuContent::onComboIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    if (!combo)
    {
        return;
    }
    int indexType = combo->property("Item").toInt();
    switch (indexType)
    {

    case ConfigEditNIBPMenuContentPrivate::ITEM_CBO_INIT_CUFF:
        d_ptr->setInitPressure(index);
        break;
    case ConfigEditNIBPMenuContentPrivate::ITEM_CBO_INTERVAL_TIME:
        d_ptr->config->setNumValue("NIBP|IntervalTime", index);
        break;
    case ConfigEditNIBPMenuContentPrivate::ITEM_CBO_MEASURE_MODE:
        d_ptr->config->setNumValue("NIBP|NIBPMeasureMode", index);
        break;
    default:
        break;
    }
}

void ConfigEditNIBPMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_NIBP_SYS, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

void ConfigEditNIBPMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // measure mode
    label = new QLabel(trs("NIBPMeasureMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << NIBPSymbol::convert(NIBP_MODE_MANUAL)
                       << NIBPSymbol::convert(NIBP_MODE_AUTO));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditNIBPMenuContentPrivate
                         ::ITEM_CBO_MEASURE_MODE, comboBox);
    itemID = ConfigEditNIBPMenuContentPrivate::ITEM_CBO_MEASURE_MODE;
    comboBox->setProperty("Item", itemID);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));

    // interval time
    label = new QLabel(trs("NIBPIntervalTime"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_2_5)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_5)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_10)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_15)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_20)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_30)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_45)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_60)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_90)
                       << NIBPSymbol::convert(NIBP_AUTO_INTERVAL_120));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditNIBPMenuContentPrivate
                         ::ITEM_CBO_INTERVAL_TIME, comboBox);
    itemID = ConfigEditNIBPMenuContentPrivate::ITEM_CBO_INIT_CUFF;
    comboBox->setProperty("Item", itemID);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));

    // Initial cuff
    label = new QLabel(trs("NIBPInitialCuff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << NIBPSymbol::convert(NIBP_MODE_MANUAL)
                       << NIBPSymbol::convert(NIBP_MODE_AUTO));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditNIBPMenuContentPrivate
                         ::ITEM_CBO_INIT_CUFF, comboBox);
    itemID = ConfigEditNIBPMenuContentPrivate::ITEM_CBO_INIT_CUFF;
    comboBox->setProperty("Item", itemID);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));


    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}





















































