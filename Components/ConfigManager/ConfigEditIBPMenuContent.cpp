/**
 ** This file is part of the nPM project.
 ** Copyright (C) Better Life Medical Technology Co., Ltd.
 ** All Rights Reserved.
 ** Unauthorized copying of this file, via any medium is strictly prohibited
 ** Proprietary and confidential
 **
 ** Written by ZhongHuan Duan duanzhonghuan@blmed.cn, 2018/7/18
 **/
#include "ConfigEditIBPMenuContent.h"
#include <QGridLayout>
#include <QLabel>
#include "Framework/UI/Button.h"
#include "Framework/UI/ComboBox.h"
#include "Framework/Language/LanguageManager.h"
#include <QMap>
#include "ConfigManager.h"
#include "IBPSymbol.h"
#include "ParamInfo.h"
#include "ParamDefine.h"
#include "Debug.h"

class ConfigEditIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_PRESSURE_NAME1 = 0,
        ITEM_CBO_PRESSURE_NAME2,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_IBP_SENS,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditIBPMenuContentPrivate(Config * const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
};

ConfigEditIBPMenuContentPrivate::ConfigEditIBPMenuContentPrivate(Config *const config)
    :config(config)
{
    combos.clear();
}

ConfigEditIBPMenuContent::ConfigEditIBPMenuContent(Config * const config):
    MenuContent(trs("IBPMenu"),
                trs("IBPMenuDesc")),
    d_ptr(new ConfigEditIBPMenuContentPrivate(config))
{
}

ConfigEditIBPMenuContent::~ConfigEditIBPMenuContent()
{
    delete d_ptr;
}

void ConfigEditIBPMenuContentPrivate::loadOptions()
{
    QStringList strList = QStringList()
                          << "ChannelPressureEntitle1"
                          << "ChannelPressureEntitle2"
                          << "IBPSweepSpeed"
                          << "FilterMode"
                          << "Sensitivity";
    for (int i = 0; i < strList.count(); i++)
    {
        int index = 0;
        MenuItem item = static_cast<MenuItem>(i);
        combos[item]->blockSignals(true);
        config->getNumValue(QString("IBP|%1").arg(strList.at(i)), index);
        combos[item]->setCurrentIndex(index);
        combos[item]->blockSignals(false);
    }
}

void ConfigEditIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditIBPMenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        d_ptr->combos[ConfigEditIBPMenuContentPrivate
                ::MenuItem(i)]->setEnabled(!isOnlyToRead);
    }
}

void ConfigEditIBPMenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);
    layout->setMargin(10);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // channel pressure entitle
    label = new QLabel(trs("ChannelPressureEntitle1"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_LABEL_ART))
                       << trs(IBPSymbol::convert(IBP_LABEL_PA))
                       << trs(IBPSymbol::convert(IBP_LABEL_CVP))
                       << trs(IBPSymbol::convert(IBP_LABEL_LAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_RAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_ICP))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP1))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP2)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_PRESSURE_NAME1, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_PRESSURE_NAME1;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // channel pressure entitle2
    label = new QLabel(trs("ChannelPressureEntitle2"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_LABEL_ART))
                       << trs(IBPSymbol::convert(IBP_LABEL_PA))
                       << trs(IBPSymbol::convert(IBP_LABEL_CVP))
                       << trs(IBPSymbol::convert(IBP_LABEL_LAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_RAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_ICP))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP1))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP2)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_PRESSURE_NAME2, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_PRESSURE_NAME2;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // Sweep Speed
    label = new QLabel(trs("IBPSweepSpeed"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_62_5))
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_125))
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_250))
                       << trs(IBPSymbol::convert(IBP_SWEEP_SPEED_500)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_SWEEP_SPEED, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_SWEEP_SPEED;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // Filter Mode
    label = new QLabel(trs("FilterMode"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_FILTER_MODE_0))
                       << trs(IBPSymbol::convert(IBP_FILTER_MODE_1)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_FILTER_MODE, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_FILTER_MODE;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // Sensitivity
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox;
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_HIGH))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_MID))
                       << trs(IBPSymbol::convert(IBP_SENSITIVITY_LOW)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate
                         ::ITEM_CBO_IBP_SENS, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate
             ::ITEM_CBO_IBP_SENS;
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

void ConfigEditIBPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;
    switch (indexType)
    {
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_PRESSURE_NAME1:
        str = "ChannelPressureEntitle1";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_PRESSURE_NAME2:
        str = "ChannelPressureEntitle2";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
        str = "IBPSweepSpeed";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_FILTER_MODE:
        str = "FilterMode";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_IBP_SENS:
        str = "Sensitivity";
        break;
    default :
        qdebug("Invalid combo id.");
        break;
    }
    d_ptr->config->setNumValue(QString("IBP|%1").arg(str), index);
}

void ConfigEditIBPMenuContent::onAlarmBtnReleased()
{
    MenuWindow *w = this->getMenuWindow();
    QString subParamName = paramInfo.getSubParamName(SUB_PARAM_ART_SYS, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

















