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
#include "SpinBox.h"
#include "IConfig.h"
#include "NIBPParam.h"

class ConfigEditNIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_MEASURE_MODE = 0,
        ITEM_CBO_INTERVAL_TIME,
        ITEM_CBO_MAX
    };

    explicit ConfigEditNIBPMenuContentPrivate(Config *const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
    SpinBox *initCuffSpb;
    QStringList initCuffStrs;
    QLabel *initCuffUnitLbl;
};

ConfigEditNIBPMenuContentPrivate
    ::ConfigEditNIBPMenuContentPrivate(Config *const config)
    :config(config), initCuffSpb(NULL), initCuffUnitLbl(NULL)
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
    config->getNumValue("NIBP|MeasureMode", index);
    combos[ITEM_CBO_MEASURE_MODE]->setCurrentIndex(index);
    index = 0;
    config->getNumValue("NIBP|AutoInterval", index);
    combos[ITEM_CBO_INTERVAL_TIME]->setCurrentIndex(index);

    UnitType defUnit = paramInfo.getUnitOfSubParam(SUB_PARAM_NIBP_SYS);
    UnitType curUnit = nibpParam.getUnit();
    PatientType type = patientManager.getType();
    int initVal;
    int start = 0, end = 0;
    if (type == PATIENT_TYPE_ADULT)
    {
        start = 120;
        end = 280;
    }
    else if (type == PATIENT_TYPE_PED)
    {
        start = 80;
        end = 250;
    }
    else if (type == PATIENT_TYPE_NEO)
    {
        start = 60;
        end = 140;
    }
    initCuffStrs.clear();
    for (int i = start; i <= end; i += 10)
    {
        if (curUnit == defUnit)
        {
            initCuffStrs.append(QString::number(i));
        }
        else
        {
            initCuffStrs.append(Unit::convert(curUnit, defUnit, i));
        }
    }
    initCuffSpb->setStringList(initCuffStrs);

    if (curUnit == defUnit)
    {
        initCuffUnitLbl->setText(Unit::getSymbol(UNIT_MMHG));
    }
    else
    {
        initCuffUnitLbl->setText(Unit::getSymbol(UNIT_KPA));
    }

    config->getNumValue("NIBP|InitialCuffInflation", initVal);
    initCuffSpb->setValue(initVal);

    int unit = UNIT_MMHG;
    config->getNumValue("Local|NIBPUnit", unit);
}

void ConfigEditNIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditNIBPMenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditNIBPMenuContentPrivate
                ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }
    d_ptr->initCuffSpb->setEnabled(!isOnlyToRead);
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
    case ConfigEditNIBPMenuContentPrivate::ITEM_CBO_INTERVAL_TIME:
        d_ptr->config->setNumValue("NIBP|AutoInterval", index);
        break;
    case ConfigEditNIBPMenuContentPrivate::ITEM_CBO_MEASURE_MODE:
        d_ptr->config->setNumValue("NIBP|MeasureMode", index);
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

void ConfigEditNIBPMenuContent::onSpinBoxReleased(int value)
{
    d_ptr->config->setNumValue("NIBP|InitialCuffInflation", value);
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
                       << trs(NIBPSymbol::convert(NIBP_MODE_MANUAL))
                       << trs(NIBPSymbol::convert(NIBP_MODE_AUTO)));
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
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditNIBPMenuContentPrivate
                         ::ITEM_CBO_INTERVAL_TIME, comboBox);
    itemID = ConfigEditNIBPMenuContentPrivate::ITEM_CBO_INTERVAL_TIME;
    comboBox->setProperty("Item", itemID);
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboIndexChanged(int)));

     // initial cuff
    label = new QLabel(trs("NIBPInitialCuff"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    d_ptr->initCuffSpb = new SpinBox();
    d_ptr->initCuffSpb->setSpinBoxStyle(SpinBox::SPIN_BOX_STYLE_STRING);
    connect(d_ptr->initCuffSpb, SIGNAL(valueChange(int, int)), this, SLOT(onSpinBoxReleased(int)));
    QHBoxLayout *hLayout = new QHBoxLayout();
    d_ptr->initCuffUnitLbl = new QLabel("mmHg");
    hLayout->addWidget(d_ptr->initCuffSpb);
    hLayout->addWidget(d_ptr->initCuffUnitLbl);
    layout->addLayout(hLayout, d_ptr->combos.count(), 1);


    // 添加报警设置链接
    int count = d_ptr->combos.count() + 1;
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, count, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(count + 1, 1);
}
