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
#include <QGroupBox>
#include "IBPParam.h"

class ConfigEditIBPMenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_ENTITLE_1 = 0,
        ITEM_CBO_RULER_1,
        ITEM_CBO_ENTITLE_2,
        ITEM_CBO_RULER_2,
        ITEM_CBO_SWEEP_SPEED,
        ITEM_CBO_FILTER_MODE,
        ITEM_CBO_SENSITIVITY,
    };

    explicit ConfigEditIBPMenuContentPrivate(Config * const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
    QMap<MenuItem, Button *> buttons;
    IBPLabel ibp1;
};

ConfigEditIBPMenuContentPrivate::ConfigEditIBPMenuContentPrivate(Config *const config)
    :config(config), ibp1(IBP_LABEL_NR)
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
    int index = 0;
    // channel 1 entitle
    config->getNumValue("IBP|ChannelPressureEntitle1", index);
    combos[ITEM_CBO_ENTITLE_1]->setCurrentIndex(index);
    ibp1 = static_cast<IBPLabel> (index);

    // channel 2 entitle
    config->getNumValue("IBP|ChannelPressureEntitle2", index);
    combos[ITEM_CBO_ENTITLE_2]->setCurrentIndex(index);

    // ruler limit 1
    config->getNumValue("IBP|RulerLimit1", index);
    combos[ITEM_CBO_RULER_1]->setCurrentIndex(index);

    // ruler limit 2
    config->getNumValue("IBP|RulerLimit2", index);
    combos[ITEM_CBO_RULER_2]->setCurrentIndex(index);

    // IBP Sweep Speed
    config->getNumValue("IBP|SweepSpeed", index);
    combos[ITEM_CBO_SWEEP_SPEED]->setCurrentIndex(index);

    if (ibpParam.getMoudleType() == IBP_MODULE_WITLEAF)
    {
        // Filter Mode
        config->getNumValue("IBP|FilterMode", index);
        combos[ITEM_CBO_FILTER_MODE]->setCurrentIndex(index);
        // Sensitivity
        config->getNumValue("IBP|Sensitivity", index);
        combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(index);
    }
}

void ConfigEditIBPMenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();

    // set widget enable status
    d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_1]->setEnabled(!isOnlyToRead);
    d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_1]->setEnabled(!isOnlyToRead);
    d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_2]->setEnabled(!isOnlyToRead);
    d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_2]->setEnabled(!isOnlyToRead);

    d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED]->setEnabled(!isOnlyToRead);

    if (ibpParam.getMoudleType() == IBP_MODULE_WITLEAF)
    {
        d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_FILTER_MODE]->setEnabled(!isOnlyToRead);
        d_ptr->combos[ConfigEditIBPMenuContentPrivate::ITEM_CBO_SENSITIVITY]->setEnabled(!isOnlyToRead);
    }
}

void ConfigEditIBPMenuContent::layoutExec()
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label;
    ComboBox *comboBox;
    int itemID;

    // channel 1 QGroupBox
    QGridLayout *gLayout = new QGridLayout();
    QGroupBox * groupBox = new QGroupBox("IBP1");
    groupBox->setLayout(gLayout);
    layout->addWidget(groupBox);

    // channel 1 pressure entitle
    label = new QLabel(trs("PressureEntitle"));
    gLayout->addWidget(label, 0, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_LABEL_ART))
                       << trs(IBPSymbol::convert(IBP_LABEL_PA))
                       << trs(IBPSymbol::convert(IBP_LABEL_CVP))
                       << trs(IBPSymbol::convert(IBP_LABEL_LAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_RAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_ICP))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP1))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP2)));
    gLayout->addWidget(comboBox, 0, 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_1, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_1;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // channel 1 ruler
    label = new QLabel(trs("Ruler"));
    gLayout->addWidget(label, 1, 0);
    comboBox = new ComboBox();
    for (int i = 0; i < ibpParam.ibpScaleList.count(); i++)
    {
        if (i == 0)
        {
            comboBox->addItem(trs("Auto"));
        }
        else if (i == ibpParam.ibpScaleList.count() - 1)
        {
            comboBox->addItem(trs("Manual"));
        }
        else
        {
            comboBox->addItem(QString::number(ibpParam.ibpScaleList.at(i).low) + "~" +
                              QString::number(ibpParam.ibpScaleList.at(i).high));
        }
    }
    itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_1;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 1, 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_1, comboBox);

    // channel 2 QGroupBoxd
    gLayout = new QGridLayout();
    groupBox = new QGroupBox("IBP2");
    groupBox->setLayout(gLayout);
    layout->addWidget(groupBox);

    // channel 2 pressure entitle
    label = new QLabel(trs("PressureEntitle"));
    gLayout->addWidget(label, 0, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(IBPSymbol::convert(IBP_LABEL_ART))
                       << trs(IBPSymbol::convert(IBP_LABEL_PA))
                       << trs(IBPSymbol::convert(IBP_LABEL_CVP))
                       << trs(IBPSymbol::convert(IBP_LABEL_LAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_RAP))
                       << trs(IBPSymbol::convert(IBP_LABEL_ICP))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP1))
                       << trs(IBPSymbol::convert(IBP_LABEL_AUXP2)));
    gLayout->addWidget(comboBox, 0, 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_2, comboBox);
    itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_2;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));

    // channel 2 ruler
    label = new QLabel(trs("Ruler"));
    gLayout->addWidget(label, 1, 0);
    comboBox = new ComboBox();
    for (int i = 0; i < ibpParam.ibpScaleList.count(); i++)
    {
        if (i == 0)
        {
            comboBox->addItem(trs("Auto"));
        }
        else if (i == ibpParam.ibpScaleList.count() - 1)
        {
            comboBox->addItem(trs("Manual"));
        }
        else
        {
            comboBox->addItem(QString::number(ibpParam.ibpScaleList.at(i).low) + "~" +
                              QString::number(ibpParam.ibpScaleList.at(i).high));
        }
    }
    itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_2;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, 1, 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_2, comboBox);

    // Sweep Speed
    int row = 0;
    gLayout = new QGridLayout();
    label = new QLabel(trs("IBPSweepSpeed"));
    gLayout->addWidget(label, row, 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_62_5)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_125)
                       << IBPSymbol::convert(IBP_SWEEP_SPEED_250));
    itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED;
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    gLayout->addWidget(comboBox, row, 1);
    d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED, comboBox);

    if (ibpParam.getMoudleType() == IBP_MODULE_WITLEAF)
    {
        // filter mode
        row++;
        label = new QLabel(trs("FilterMode"));
        gLayout->addWidget(label, row, 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << IBPSymbol::convert(IBP_FILTER_MODE_0)
                           << IBPSymbol::convert(IBP_FILTER_MODE_1));
        itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_FILTER_MODE;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        gLayout->addWidget(comboBox, row, 1);
        d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_FILTER_MODE, comboBox);

        // sensitivity
        row++;
        label = new QLabel(trs("Sensitivity"));
        gLayout->addWidget(label, row, 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << trs(IBPSymbol::convert(IBP_SENSITIVITY_HIGH))
                           << trs(IBPSymbol::convert(IBP_SENSITIVITY_MID))
                           << trs(IBPSymbol::convert(IBP_SENSITIVITY_LOW)));
        itemID = ConfigEditIBPMenuContentPrivate::ITEM_CBO_SENSITIVITY;
        comboBox->setProperty("Item", qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        gLayout->addWidget(comboBox, row, 1);
        d_ptr->combos.insert(ConfigEditIBPMenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);
    }

    row++;

    // 添加报警设置链接
    Button *btn = new Button(QString("%1%2").
                             arg(trs("AlarmSettingUp")).
                             arg(" >>"));
    btn->setButtonStyle(Button::ButtonTextOnly);
    gLayout->addWidget(btn, row, 1);
    connect(btn, SIGNAL(released()), this, SLOT(onAlarmBtnReleased()));
    gLayout->setRowStretch(d_ptr->combos.count() + d_ptr->buttons.count() + 1, 1);

    layout->addLayout(gLayout);
}

void ConfigEditIBPMenuContent::onComboBoxIndexChanged(int index)
{
    ComboBox *combo = qobject_cast<ComboBox *>(sender());
    int indexType = combo->property("Item").toInt();
    QString str;
    switch (indexType)
    {
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_1:
        str = "ChannelPressureEntitle1";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_ENTITLE_2:
        str = "ChannelPressureEntitle2";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_1:
        str = "RulerLimit1";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_RULER_2:
        str = "RulerLimit2";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_SWEEP_SPEED:
        str = "SweepSpeed";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_FILTER_MODE:
        str = "FilterMode";
        break;
    case ConfigEditIBPMenuContentPrivate::ITEM_CBO_SENSITIVITY:
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
    SubParamID chn1ParamId = ibpParam.getSubParamID(d_ptr->ibp1);
    QString subParamName = paramInfo.getSubParamName(chn1ParamId, true);
    if (w)
    {
        w->popup(trs("AlarmLimitMenu"), qVariantFromValue(subParamName));
    }
}

















