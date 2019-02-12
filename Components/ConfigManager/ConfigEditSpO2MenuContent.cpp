/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#include "ConfigEditSpO2MenuContent.h"
#include <QGridLayout>
#include <QLabel>
#include "LanguageManager.h"
#include <QMap>
#include "ComboBox.h"
#include <QGridLayout>
#include "SPO2Symbol.h"
#include "SPO2Define.h"
#include "ConfigManager.h"
#include "Button.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "SPO2Param.h"

class ConfigEditSpO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_SPO2_SENS = 0,
        ITEM_CBO_SMART_PTONE,
        ITEM_CBO_WAVE_VEL,
        ITEM_CBO_MODULE_CONTROL,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditSpO2MenuContentPrivate(Config *const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
};

ConfigEditSpO2MenuContentPrivate
::ConfigEditSpO2MenuContentPrivate(Config *const config)
    : config(config)
{
    combos.clear();
}

ConfigEditSpO2MenuContent::ConfigEditSpO2MenuContent(Config *const config):
    MenuContent(trs("SPO2Menu"),
                trs("SPO2MenuDesc")),
    d_ptr(new ConfigEditSpO2MenuContentPrivate(config))
{
}

ConfigEditSpO2MenuContent::~ConfigEditSpO2MenuContent()
{
    delete d_ptr;
}

void ConfigEditSpO2MenuContentPrivate::loadOptions()
{
    QStringList strList = QStringList()
                          << "Sensitivity"
                          << "SmartPluseTone"
                          << "SweepSpeed"
                          << "ModuleControl";

    for (int i = 0; i < strList.count(); i++)
    {
        int index = 0;
        MenuItem item = static_cast<MenuItem>(i);
        config->getNumValue(QString("SPO2|%1").arg(strList.at(i)), index);
        combos[item]->setCurrentIndex(index);
    }
}

void ConfigEditSpO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditSpO2MenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditSpO2MenuContentPrivate
                      ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }
}

void ConfigEditSpO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // sensitivity
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    SPO2ModuleType moduleType = spo2Param.getModuleType();
    if (moduleType == MODULE_MASIMO_SPO2 || moduleType == MODULE_RAINBOW_SPO2)
    {
        for (int i = SPO2_MASIMO_SENS_MAX; i < SPO2_MASIMO_SENS_NR; i++)
        {
            comboBox->addItem(trs(SPO2Symbol::convert(static_cast<SensitivityMode>(i))));
        }
    }
    else if (moduleType != MODULE_SPO2_NR)
    {
        for (int i = SPO2_SENS_LOW; i < SPO2_SENS_NR; i++)
        {
            comboBox->addItem(trs(SPO2Symbol::convert(static_cast<SPO2Sensitive>(i))));
        }
    }
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate
                         ::ITEM_CBO_SPO2_SENS, comboBox);
    itemID = ConfigEditSpO2MenuContentPrivate
             ::ITEM_CBO_SPO2_SENS;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // smart pluse tone
    label = new QLabel(trs("SmartPluseTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_OFF))
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_ON)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate
                         ::ITEM_CBO_SMART_PTONE, comboBox);
    itemID = ConfigEditSpO2MenuContentPrivate
             ::ITEM_CBO_SMART_PTONE;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));


    // Wave velocity
    label = new QLabel(trs("WaveVelocity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_WAVE_VELOCITY_62D5))
                       << trs(SPO2Symbol::convert(SPO2_WAVE_VELOCITY_125))
                       << trs(SPO2Symbol::convert(SPO2_WAVE_VELOCITY_250)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate
                         ::ITEM_CBO_WAVE_VEL, comboBox);
    itemID = ConfigEditSpO2MenuContentPrivate
             ::ITEM_CBO_WAVE_VEL;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // module control
    label = new QLabel(trs("ModuleControl"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_MODULE_DISABLE))
                       << trs(SPO2Symbol::convert(SPO2_MODULE_ENABLE)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate
                         ::ITEM_CBO_MODULE_CONTROL, comboBox);
    itemID = ConfigEditSpO2MenuContentPrivate
             ::ITEM_CBO_MODULE_CONTROL;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    layout->addWidget(btn, d_ptr->combos.count(), 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));

    layout->setRowStretch(d_ptr->combos.count() + 1, 1);
}

void ConfigEditSpO2MenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;
    switch (indexType)
    {
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_MODULE_CONTROL:
        str = "ModuleControl";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_PTONE:
        str = "SmartPluseTone";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SPO2_SENS:
        str = "Sensitivity";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_VEL:
        str = "SweepSpeed";
        break;
    default :
        qdebug("Invalid combo id.");
        break;
    }
    d_ptr->config->setNumValue(QString("SPO2|%1").arg(str), index);
}

void ConfigEditSpO2MenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_SPO2, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

















