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
#include "Debug.h"

class ConfigEditSpO2MenuContentPrivate
{
public:
    enum MenuItem
    {
        ITEM_CBO_WAVE_SPEED,
        ITEM_CBO_AVERAGE_TIME,
        ITEM_CBO_SENSITIVITY,
        ITEM_CBO_FAST_SAT,
        ITEM_CBO_SMART_TONE,
        ITEM_CBO_BEAT_VOL,
        ITEM_CBO_NIBP_SAME_SIDE,
        ITEM_CBO_MAX,
    };

    explicit ConfigEditSpO2MenuContentPrivate(Config *const config);
    /**
     * @brief loadOptions
     */
    void loadOptions();

    QMap <MenuItem, ComboBox *> combos;
    Config *const config;
    SPO2ModuleType moduleType;

    QLabel *focusLable;  // 仅查看该菜单时,设置该label为可聚焦方式，便于旋转飞梭查看视图
};

ConfigEditSpO2MenuContentPrivate
::ConfigEditSpO2MenuContentPrivate(Config *const config)
    : config(config)
    , moduleType(MODULE_BLM_S5)
    , focusLable(NULL)
{
    combos.clear();
}

ConfigEditSpO2MenuContent::ConfigEditSpO2MenuContent(Config *const config):
    MenuContent(trs("SPO2Menu"),
                trs("SPO2MenuDesc")),
    d_ptr(new ConfigEditSpO2MenuContentPrivate(config))
{
    d_ptr->moduleType = spo2Param.getModuleType();
}

ConfigEditSpO2MenuContent::~ConfigEditSpO2MenuContent()
{
    delete d_ptr;
}

void ConfigEditSpO2MenuContentPrivate::loadOptions()
{
    int index;

    // wave speed
    index = 0;
    config->getNumValue("SPO2|SweepSpeed", index);
    combos[ITEM_CBO_WAVE_SPEED]->setCurrentIndex(index);

    if (moduleType == MODULE_MASIMO_SPO2 || moduleType == MODULE_RAINBOW_SPO2)
    {
        // average time
        index = 0;
        config->getNumValue("SPO2|AverageTime", index);
        combos[ITEM_CBO_AVERAGE_TIME]->setCurrentIndex(index);

        // fast sat
        index = 0;
        config->getNumValue("SPO2|FastSat", index);
        combos[ITEM_CBO_FAST_SAT]->setCurrentIndex(index);
    }

    // Sensitivity
    index = 0;
    config->getNumValue("SPO2|Sensitivity", index);
    combos[ITEM_CBO_SENSITIVITY]->setCurrentIndex(index);

    // Smart Pluse Tone
    index = 0;
    config->getNumValue("SPO2|SmartPluseTone", index);
    combos[ITEM_CBO_SMART_TONE]->setCurrentIndex(index);

    // Beat Volume
    index = 0;
    config->getNumValue("ECG|QRSVolume", index);
    combos[ITEM_CBO_BEAT_VOL]->setCurrentIndex(index);

    // NIBP Same Side
    index = 0;
    config->getNumValue("SPO2|NIBPSameSide", index);
    combos[ITEM_CBO_NIBP_SAME_SIDE]->setCurrentIndex(index);
}

void ConfigEditSpO2MenuContent::readyShow()
{
    d_ptr->loadOptions();
    bool isOnlyToRead = configManager.isReadOnly();

    for (int i = 0; i < ConfigEditSpO2MenuContentPrivate::ITEM_CBO_MAX; i++)
    {
        ConfigEditSpO2MenuContentPrivate::MenuItem item =
                static_cast<ConfigEditSpO2MenuContentPrivate::MenuItem>(i);
        if (d_ptr->combos[item])
        {
            d_ptr->combos[item]->setEnabled(!isOnlyToRead);
        }
    }

    // 模块类型不同时，参数设置菜单的长度也会有所改变;
    // 当参数设置菜单过长超过一屏时，加入可选择的聚焦点,
    // 便于使用飞梭查看所有菜单内部item
    switch (d_ptr->moduleType)
    {
        case MODULE_BLM_S5:
        default:
        break;
        case MODULE_MASIMO_SPO2:
        case MODULE_RAINBOW_SPO2:
        {
            if (d_ptr->focusLable)
            {
                if (isOnlyToRead)
                {
                    d_ptr->focusLable->setFocusPolicy(Qt::StrongFocus);
                }
                else
                {
                    d_ptr->focusLable->setFocusPolicy(Qt::NoFocus);
                }
            }
        }
        break;
    }
}

void ConfigEditSpO2MenuContent::layoutExec()
{
    QGridLayout *layout = new QGridLayout(this);

    ComboBox *comboBox;
    QLabel *label;
    int itemID;

    // wave speed
    label = new QLabel(trs("SPO2SweepSpeed"));
    d_ptr->focusLable = label;
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_62D5)
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_125)
                       << SPO2Symbol::convert(SPO2_WAVE_VELOCITY_250));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    itemID = ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED;
    comboBox->setProperty("Item", itemID);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED, comboBox);

    // 平均时间
    if (d_ptr->moduleType == MODULE_MASIMO_SPO2 || d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        label = new QLabel(trs("AverageTime"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        for (int i = 0; i < SPO2_AVER_TIME_NR; i++)
        {
            comboBox->addItem(SPO2Symbol::convert((AverageTime)i));
        }
        itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME, comboBox);
    }

    // 灵敏度
    label = new QLabel(trs("Sensitivity"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
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

    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SENSITIVITY);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SENSITIVITY, comboBox);

    // 快速血氧
    if (d_ptr->moduleType == MODULE_MASIMO_SPO2 || d_ptr->moduleType == MODULE_RAINBOW_SPO2)
    {
        label = new QLabel(trs("FastSat"));
        layout->addWidget(label, d_ptr->combos.count(), 0);
        comboBox = new ComboBox();
        comboBox->addItems(QStringList()
                           << trs("Off")
                           << trs("On"));
        itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_FAST_SAT);
        comboBox->setProperty("Item",
                              qVariantFromValue(itemID));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
        layout->addWidget(comboBox, d_ptr->combos.count(), 1);
        d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_FAST_SAT, comboBox);
    }

    // 智能脉搏音
    label = new QLabel(trs("SPO2SmartPulseTone"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_OFF))
                       << trs(SPO2Symbol::convert(SPO2_SMART_PLUSE_TONE_ON)));
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_TONE);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_TONE, comboBox);

    // 音量
    label = new QLabel(trs("BeatVol"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItem(trs("Off"));

    // 设置声音触发方式
    connect(comboBox, SIGNAL(itemFocusChanged(int)),
            this, SLOT(onPopupListItemFocusChanged(int)));

    for (int i = SoundManager::VOLUME_LEV_1; i <= SoundManager::VOLUME_LEV_MAX; i++)
    {
        comboBox->addItem(QString::number(i));
    }
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL);
    comboBox->setProperty("Item",
                          qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(activated(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL, comboBox);

    // NIBP同侧功能
    label = new QLabel(trs("NIBPSimul"));
    layout->addWidget(label, d_ptr->combos.count(), 0);
    comboBox = new ComboBox();
    comboBox->addItems(QStringList()
                       << trs("Off")
                       << trs("On"));
    itemID = static_cast<int>(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE);
    comboBox->setProperty("Item", qVariantFromValue(itemID));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
    layout->addWidget(comboBox, d_ptr->combos.count(), 1);
    d_ptr->combos.insert(ConfigEditSpO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE, comboBox);

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
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SMART_TONE:
        str = "SmartPluseTone";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_SENSITIVITY:
        str = "Sensitivity";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_WAVE_SPEED:
        str = "SweepSpeed";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_AVERAGE_TIME:
        str = "AverageTime";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_FAST_SAT:
        str = "FastSat";
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL:
    {
        int vol = 0;
        currentConfig.getNumValue("ECG|QRSVolume", vol);
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(vol));
        d_ptr->config->setNumValue("ECG|QRSVolume", index);
    }
        break;
    case ConfigEditSpO2MenuContentPrivate::ITEM_CBO_NIBP_SAME_SIDE:
        str = "NIBPSameSide";
        break;
    default :
        qdebug("Invalid combo id.");
        break;
    }
    if (!str.isEmpty())
    {
        d_ptr->config->setNumValue(QString("SPO2|%1").arg(str), index);
    }
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

void ConfigEditSpO2MenuContent::onPopupListItemFocusChanged(int volume)
{
    ComboBox *w = qobject_cast<ComboBox*>(sender());
    if (w == d_ptr->combos[ConfigEditSpO2MenuContentPrivate::ITEM_CBO_BEAT_VOL])
    {
        soundManager.setVolume(SoundManager::SOUND_TYPE_HEARTBEAT , static_cast<SoundManager::VolumeLevel>(volume));
        soundManager.heartBeatTone();
    }
}
